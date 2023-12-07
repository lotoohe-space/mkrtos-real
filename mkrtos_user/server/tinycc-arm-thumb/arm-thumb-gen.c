/*
 *  ARM Thumb 2 code generator for TCC
 *
 *  Copyright (c) 2020 Erlend J. Sveen
 *
 *  Based on the ARMv4 code generator (arm-gen.c) for TCC by
 *
 *  Copyright (c) 2003 Daniel Glöckner
 *  Copyright (c) 2012 Thomas Preud'homme
 *
 *  Based on i386-gen.c by Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef TARGET_DEFS_ONLY

/* error out if not EABI and VFP */
#if !defined (TCC_ARM_VFP)
# error "Support for FPA is deprecated"
#endif
#if !defined (TCC_ARM_EABI)
# error "Support for OABI is deprecated"
#endif

/* debug level: 0 = none, 1 = info, 2 = all */
#define DODBG 0

#if DODBG == 2
#define DBG1(a) a
#define DBG2(a) a
#elif DODBG == 1
#define DBG1(a) a
#define DBG2(a)
#else
#define DBG1(a)
#define DBG2(a)
#endif

/* number of available registers */
#define NB_REGS 13

#ifndef TCC_CPU_VERSION
# define TCC_CPU_VERSION 5
#endif

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#define RC_INT     0x0001 /* generic integer register */
#define RC_FLOAT   0x0002 /* generic float register */
#define RC_R0      0x0004
#define RC_R1      0x0008
#define RC_R2      0x0010
#define RC_R3      0x0020
#define RC_R12     0x0040
#define RC_F0      0x0080
#define RC_F1      0x0100
#define RC_F2      0x0200
#define RC_F3      0x0400
#define RC_F4      0x0800
#define RC_F5      0x1000
#define RC_F6      0x2000
#define RC_F7      0x4000
#define RC_IRET    RC_R0  /* function return: integer register */
#define RC_IRE2    RC_R1  /* function return: second integer register */
#define RC_FRET    RC_F0  /* function return: float register */

/* pretty names for the registers */
enum {
    TREG_R0 = 0,
    TREG_R1,
    TREG_R2,
    TREG_R3,
    TREG_R12,
    TREG_F0,
    TREG_F1,
    TREG_F2,
    TREG_F3,
    TREG_F4,
    TREG_F5,
    TREG_F6,
    TREG_F7,
    TREG_SP = 13,
    TREG_LR,
};

/* return registers for function */
#define REG_IRET TREG_R0 /* single word int return register */
#define REG_IRE2 TREG_R1 /* second word return register (for long long) */
#define REG_FRET TREG_F0 /* float return register */

#define TOK___divdi3 TOK___aeabi_ldivmod
#define TOK___moddi3 TOK___aeabi_ldivmod
#define TOK___udivdi3 TOK___aeabi_uldivmod
#define TOK___umoddi3 TOK___aeabi_uldivmod

/* defined if function parameters must be evaluated in reverse order */
#define INVERT_FUNC_PARAMS

/* pointer size, in bytes */
#define PTR_SIZE 4

/* long double size and alignment, in bytes */
#define LDOUBLE_SIZE  8
#define LDOUBLE_ALIGN 8

/* maximum alignment (for aligned attribute support) */
#define MAX_ALIGN     8

#define CHAR_IS_UNSIGNED

/******************************************************/
#else /* ! TARGET_DEFS_ONLY */
/******************************************************/
#define USING_GLOBALS
#include "tcc.h"

enum float_abi float_abi;

ST_DATA const int reg_classes[NB_REGS] = {
    /* r0 */ RC_INT | RC_R0,
    /* r1 */ RC_INT | RC_R1,
    /* r2 */ RC_INT | RC_R2,
    /* r3 */ RC_INT | RC_R3,
    /* r12 */ RC_INT | RC_R12,
    /* f0 */ RC_FLOAT | RC_F0,
    /* f1 */ RC_FLOAT | RC_F1,
    /* f2 */ RC_FLOAT | RC_F2,
    /* f3 */ RC_FLOAT | RC_F3,
 /* d4/s8 */ RC_FLOAT | RC_F4,
/* d5/s10 */ RC_FLOAT | RC_F5,
/* d6/s12 */ RC_FLOAT | RC_F6,
/* d7/s14 */ RC_FLOAT | RC_F7,
};

static int func_sub_sp_offset;
static int func_nregs;
static int leaffunc;

static CType float_type, double_type, func_float_type, func_double_type;

ST_FUNC void arm_init(struct TCCState *s)
{
    float_type.t = VT_FLOAT;
    double_type.t = VT_DOUBLE;
    func_float_type.t = VT_FUNC;
    func_float_type.ref = sym_push(SYM_FIELD, &float_type, FUNC_CDECL, FUNC_OLD);
    func_double_type.t = VT_FUNC;
    func_double_type.ref = sym_push(SYM_FIELD, &double_type, FUNC_CDECL, FUNC_OLD);

    float_abi = s->float_abi;
#ifndef TCC_ARM_HARDFLOAT
# warning "soft float ABI currently not supported: default to softfp"
#endif
}

static int two2mask(int a,int b) {
  return (reg_classes[a]|reg_classes[b])&~(RC_INT|RC_FLOAT);
}

static int regmask(int r) {
  return reg_classes[r]&~(RC_INT|RC_FLOAT);
}

/******************************************************/

#if !defined(CONFIG_TCC_ELFINTERP)
const char *default_elfinterp(struct TCCState *s)
{
    if (s->float_abi == ARM_HARD_FLOAT)
        return "/lib/ld-linux-armhf.so.3";
    else
        return "/lib/ld-linux.so.3";
}
#endif

/*
 * Write a two-byte thumb instruction. Always returns 2 regardless if code
 * is wanted or not, useful for code size estimation. It is assumed that
 * the current write position is already 16-bit aligned.
 */
int ot(uint16_t i)
{
  int ind1 = ind + 2;
  DBG2(printf ("  ot %.4x ind %x\n", i, ind));

  if (nocode_wanted)
    return 2;

  if (!cur_text_section)
    tcc_error("compiler error! This happens f.ex. if the compiler\n"
         "can't evaluate constant expressions outside of a function.");

  if (ind1 > cur_text_section->data_allocated)
    section_realloc(cur_text_section, ind1);

  cur_text_section->data[ind++] = i & 255;
  i >>= 8;
  cur_text_section->data[ind++] = i & 255;
  return 2;
}

/*
 * Write a four-byte thumb instruction. Always returns 4 regardless if code
 * is wanted or not, useful for code size estimation. It is assumed that
 * the current write position is already 16-bit aligned.
 */
void ott(uint32_t i)
{
  int ind1 = ind + 4;
  DBG2(printf ("  ott %.8x\n", i));

  if (nocode_wanted)
    return;

  if (!cur_text_section)
    tcc_error("compiler error! This happens f.ex. if the compiler\n"
         "can't evaluate constant expressions outside of a function.");

  if (ind1 > cur_text_section->data_allocated)
    section_realloc(cur_text_section, ind1);

  cur_text_section->data[ind++] = i & 255;
  i >>= 8;
  cur_text_section->data[ind++] = i & 255;
  i >>= 8;
  cur_text_section->data[ind++] = i & 255;
  i >>= 8;
  cur_text_section->data[ind++] = i;
}

#include "arm-thumb-instructions.c"

/*
 * Output a symbol and patch all calls to it.
 * (modifies other branches to branch here)
 * t = address of branch to be modified (target)
 * a = branch target
 */
void gsym_addr(int t, int a)
{
  DBG1(printf (GRN"gsym_addr instruction %.8x branch target %.8x\n"CLR, t, a));

  while (t)
    t = th_patch_call (t, a);
}

static uint32_t vfpr(int r)
{
  if (r < TREG_F0 || r > TREG_F7)
    tcc_error("internal error: register %i is no vfp register",r);
  return r - TREG_F0;
}

// TODO: What is the point of this?
static uint32_t intr(int r)
{
  if (r == TREG_R12)
    return 12;

  if (r >= TREG_R0 && r <= TREG_R3)
    return r - TREG_R0;

  return r + (13 - TREG_SP);
}

static uint32_t mapcc(int cc)
{
  switch(cc)
  {
    case TOK_ULT:
      return 0x3; /* CC/LO */
    case TOK_UGE:
      return 0x2; /* CS/HS */
    case TOK_EQ:
      return 0x0; /* EQ */
    case TOK_NE:
      return 0x1; /* NE */
    case TOK_ULE:
      return 0x9; /* LS */
    case TOK_UGT:
      return 0x8; /* HI */
    case TOK_Nset:
      return 0x4; /* MI */
    case TOK_Nclear:
      return 0x5; /* PL */
    case TOK_LT:
      return 0xB; /* LT */
    case TOK_GE:
      return 0xA; /* GE */
    case TOK_LE:
      return 0xD; /* LE */
    case TOK_GT:
      return 0xC; /* GT */
  }
  tcc_error("internal error: unexpected condition code");
  return 0xE; /* AL */
}

static int negcc(int cc)
{
  switch(cc)
  {
    case TOK_ULT:
      return TOK_UGE;
    case TOK_UGE:
      return TOK_ULT;
    case TOK_EQ:
      return TOK_NE;
    case TOK_NE:
      return TOK_EQ;
    case TOK_ULE:
      return TOK_UGT;
    case TOK_UGT:
      return TOK_ULE;
    case TOK_Nset:
      return TOK_Nclear;
    case TOK_Nclear:
      return TOK_Nset;
    case TOK_LT:
      return TOK_GE;
    case TOK_GE:
      return TOK_LT;
    case TOK_LE:
      return TOK_GT;
    case TOK_GT:
      return TOK_LE;
  }
  tcc_error("internal error: unexpected condition code");
  return TOK_NE;
}

/* load 'r' from value 'sv' */
void load(int r, SValue *sv)
{
  int v, ft, fc, fr, sign;

  fr = sv->r;
  ft = sv->type.t;
  fc = sv->c.i;

  if(fc>=0)
    sign=0;
  else {
    sign=1;
    fc=-fc;
  }

  v = fr & VT_VALMASK;

  if (fr & VT_LVAL) {
    uint32_t base = 11; // TODO: Use r7 instead of fp?
    if(v == VT_LLOCAL) {
      SValue v1;
      v1.type.t = VT_PTR;
      v1.r = VT_LOCAL | VT_LVAL;
      v1.c.i = sv->c.i;
      load(base=14 /* lr */, &v1);
      fc=sign=0;
      v=VT_LOCAL;
    } else if(v == VT_CONST) {
      SValue v1;
      v1.type.t = VT_PTR;
      v1.r = fr&~VT_LVAL;
      v1.c.i = sv->c.i;
      v1.sym=sv->sym;
      load(base=14, &v1);
      fc=sign=0;
      v=VT_LOCAL;
    } else if(v < VT_CONST) {
      base=intr(v);
      fc=sign=0;
      v=VT_LOCAL;
    }
    if(v == VT_LOCAL)
      return load_vt_lval_vt_local (r, sv, ft, fc, sign, base);
  }
  else if (v == VT_CONST)
    return load_vt_const (r, sv);
  else if (v == VT_LOCAL)
    return load_vt_local (r, sv);
  else if(v == VT_CMP)
    return load_vt_cmp (r, sv);
  else if (v == VT_JMP || v == VT_JMPI)
    return load_vt_jmp_jmpi (r, sv);
  else if (v < VT_CONST) {
    if(is_float(ft))
      tcc_error("internal error: missing code in function load");
    else
    {
      DBG1(printf (GRN"mov r %i v %i\n"CLR, r, v));
      th_mov_reg (r, v);
      return;
    }
  }

  tcc_error("internal error: load unimplemented");
}

/* store register 'r' in lvalue 'v' */
void store(int r, SValue *sv)
{
  int v, ft, fc, fr, sign;

  fr = sv->r;
  ft = sv->type.t;
  fc = sv->c.i;

  if(fc>=0)
    sign=0;
  else {
    sign=1;
    fc=-fc;
  }

  v = fr & VT_VALMASK;

  if (fr & VT_LVAL || fr == VT_LOCAL) {
    uint32_t base = 11;
    if(v < VT_CONST) {
      base=intr(v);
      v=VT_LOCAL;
      fc=sign=0;
    } else if (v == VT_CONST) {
      SValue v1;
      v1.type.t = ft;
      v1.r = fr&~VT_LVAL;
      v1.c.i = sv->c.i;
      v1.sym=sv->sym;
      load(base=14, &v1);
      fc=sign=0;
      v=VT_LOCAL;
    }
    if (v == VT_LOCAL) {
      if (is_float(ft)) {
        if ((ft & VT_BTYPE) != VT_FLOAT)
          th_vstr (base, vfpr(r), !sign, 1, fc);
        else
          th_vstr (base, vfpr(r), !sign, 0, fc);
      } else if ((ft & VT_BTYPE) == VT_SHORT) {
        DBG1(printf (GRN "store short r %i\n" CLR, r));
        if (!th_strh_imm (r, base, fc, sign ? 4 : 6))
        {
          int rr = th_offset_to_reg (fc, sign);
          th_strh_reg (r, base, rr);
        }
      } else if ((ft & VT_BTYPE) == VT_BYTE || (ft & VT_BTYPE) == VT_BOOL) {
        DBG1(printf (GRN "store byte r %i base %i fc %i sign %i\n" CLR, r, base, fc, sign));
        if (!th_strb_imm (r, base, fc, sign ? 4 : 6))
        {
          int rr = th_offset_to_reg (fc, sign);
          th_strb_reg (r, base, rr);
        }
      } else {
        DBG1(printf (GRN "store sign %x r %x base %x fc %x\n" CLR, sign, r, base, fc));
        if (!th_str_imm (r, base, fc, sign ? 4 : 6))
        {
          int rr = th_offset_to_reg (fc, sign);
          th_str_reg (r, base, rr);
        }
      }
      return;
    }
  }
  tcc_error("internal error: store unimplemented");
}

static void gadd_sp(int val)
{
  DBG1(printf (GRN"  gadd_sp, %i\n"CLR, val));

  if (val > 0)
    th_add_sp_imm (13, val);
  else if (val < 0)
    th_sub_sp_imm (13, -val);
}

/* 'is_jmp' is '1' if it is a jump */
static void gcall_or_jmp(int is_jmp)
{
  if ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST)
  {
    uint32_t x = th_encbranch(ind, ind + vtop->c.i);

    if (x)
    {
      DBG1(printf (GRN"gcall_or_jmp i %x x 0x%x\n"CLR, (int)vtop->c.i, x));
      if (vtop->r & VT_SYM)
        greloc(cur_text_section, vtop->sym, ind, R_ARM_THM_JUMP24);
      else
        printf(RED"put_elf_reloc(symtab_section, cur_text_section, ind, R_ARM_THM_JUMP24, 0);\n"CLR);
      th_bl_t1(x);
    }
    else
      tcc_error ("internal error: gcall_or_jmp longjump not implemented");
  }
  else
  {
    int r = gv(RC_INT);

    /* Indirect call (basically function calls and such) */
    DBG1(printf (GRN"gcall_or_jmp indirect call\n"CLR));

    // TODO: Find the proper place to do this. LSB Must be 1 because thumb.
    th_orr_imm (r, r, 1);

    if (!is_jmp)
      th_blx_reg (intr(r));
    else
      th_bx_reg (intr(r));
  }
}

static int unalias_ldbl(int btype)
{
    if (btype == VT_LDOUBLE)
      btype = VT_DOUBLE;

    return btype;
}

/* Return whether a structure is an homogeneous float aggregate or not.
   The answer is true if all the elements of the structure are of the same
   primitive float type and there is less than 4 elements.

   type: the type corresponding to the structure to be tested */
static int is_hgen_float_aggr(CType *type)
{
  if ((type->t & VT_BTYPE) == VT_STRUCT) {
    struct Sym *ref;
    int btype, nb_fields = 0;

    ref = type->ref->next;
    btype = unalias_ldbl(ref->type.t & VT_BTYPE);
    if (btype == VT_FLOAT || btype == VT_DOUBLE) {
      for(; ref && btype == unalias_ldbl(ref->type.t & VT_BTYPE); ref = ref->next, nb_fields++);
      return !ref && nb_fields <= 4;
    }
  }
  return 0;
}

struct avail_regs {
  signed char avail[3]; /* 3 holes max with only float and double alignments */
  int first_hole; /* first available hole */
  int last_hole; /* last available hole (none if equal to first_hole) */
  int first_free_reg; /* next free register in the sequence, hole excluded */
};

#define AVAIL_REGS_INITIALIZER (struct avail_regs) { { 0, 0, 0}, 0, 0, 0 }

/* Find suitable registers for a VFP Co-Processor Register Candidate (VFP CPRC
   param) according to the rules described in the procedure call standard for
   the ARM architecture (AAPCS). If found, the registers are assigned to this
   VFP CPRC parameter. Registers are allocated in sequence unless a hole exists
   and the parameter is a single float.

   avregs: opaque structure to keep track of available VFP co-processor regs
   align: alignment constraints for the param, as returned by type_size()
   size: size of the parameter, as returned by type_size() */
int assign_vfpreg(struct avail_regs *avregs, int align, int size)
{
  int first_reg = 0;

  if (avregs->first_free_reg == -1)
    return -1;
  if (align >> 3) { /* double alignment */
    first_reg = avregs->first_free_reg;
    /* alignment constraint not respected so use next reg and record hole */
    if (first_reg & 1)
      avregs->avail[avregs->last_hole++] = first_reg++;
  } else { /* no special alignment (float or array of float) */
    /* if single float and a hole is available, assign the param to it */
    if (size == 4 && avregs->first_hole != avregs->last_hole)
      return avregs->avail[avregs->first_hole++];
    else
      first_reg = avregs->first_free_reg;
  }
  if (first_reg + size / 4 <= 16) {
    avregs->first_free_reg = first_reg + size / 4;
    return first_reg;
  }
  avregs->first_free_reg = -1;
  return -1;
}

/* Returns whether all params need to be passed in core registers or not.
   This is the case for function part of the runtime ABI. */
int floats_in_core_regs(SValue *sval)
{
  if (!sval->sym)
    return 0;

  switch (sval->sym->v) {
    case TOK___floatundisf:
    case TOK___floatundidf:
    case TOK___fixunssfdi:
    case TOK___fixunsdfdi:
    case TOK___floatdisf:
    case TOK___floatdidf:
    case TOK___fixsfdi:
    case TOK___fixdfdi:
      return 1;

    default:
      return 0;
  }
}

/* Return the number of registers needed to return the struct, or 0 if
   returning via struct pointer. */
ST_FUNC int gfunc_sret(CType *vt, int variadic, CType *ret, int *ret_align, int *regsize) {
    int size, align;
    size = type_size(vt, &align);
    if (float_abi == ARM_HARD_FLOAT && !variadic &&
        (is_float(vt->t) || is_hgen_float_aggr(vt))) {
        *ret_align = 8;
        *regsize = 8;
        ret->ref = NULL;
        ret->t = VT_DOUBLE;
        return (size + 7) >> 3;
    } else if (size <= 4) {
        *ret_align = 4;
        *regsize = 4;
        ret->ref = NULL;
        ret->t = VT_INT;
        return 1;
    } else
        return 0;
}

/* Parameters are classified according to how they are copied to their final
   destination for the function call. Because the copying is performed class
   after class according to the order in the union below, it is important that
   some constraints about the order of the members of this union are respected:
   - CORE_STRUCT_CLASS must come after STACK_CLASS;
   - CORE_CLASS must come after STACK_CLASS, CORE_STRUCT_CLASS and
     VFP_STRUCT_CLASS;
   - VFP_STRUCT_CLASS must come after VFP_CLASS.
   See the comment for the main loop in copy_params() for the reason. */
enum reg_class {
  STACK_CLASS = 0,
  CORE_STRUCT_CLASS,
  VFP_CLASS,
  VFP_STRUCT_CLASS,
  CORE_CLASS,
  NB_CLASSES
};

struct param_plan {
    int start; /* first reg or addr used depending on the class */
    int end; /* last reg used or next free addr depending on the class */
    SValue *sval; /* pointer to SValue on the value stack */
    struct param_plan *prev; /*  previous element in this class */
};

struct plan {
    struct param_plan *pplans; /* array of all the param plans */
    struct param_plan *clsplans[NB_CLASSES]; /* per class lists of param plans */
};

#define add_param_plan(plan,pplan,class)                        \
    do {                                                        \
        pplan.prev = plan->clsplans[class];                     \
        plan->pplans[plan ## _nb] = pplan;                      \
        plan->clsplans[class] = &plan->pplans[plan ## _nb++];   \
    } while(0)

/* Assign parameters to registers and stack with alignment according to the
   rules in the procedure call standard for the ARM architecture (AAPCS).
   The overall assignment is recorded in an array of per parameter structures
   called parameter plans. The parameter plans are also further organized in a
   number of linked lists, one per class of parameter (see the comment for the
   definition of union reg_class).

   nb_args: number of parameters of the function for which a call is generated
   float_abi: float ABI in use for this function call
   plan: the structure where the overall assignment is recorded
   todo: a bitmap that record which core registers hold a parameter

   Returns the amount of stack space needed for parameter passing

   Note: this function allocated an array in plan->pplans with tcc_malloc. It
   is the responsibility of the caller to free this array once used (ie not
   before copy_params). */
static int assign_regs(int nb_args, int float_abi, struct plan *plan, int *todo)
{
  int i, size, align;
  int ncrn /* next core register number */, nsaa /* next stacked argument address*/;
  int plan_nb = 0;
  struct param_plan pplan;
  struct avail_regs avregs = AVAIL_REGS_INITIALIZER;

  ncrn = nsaa = 0;
  *todo = 0;
  plan->pplans = tcc_malloc(nb_args * sizeof(*plan->pplans));
  memset(plan->clsplans, 0, sizeof(plan->clsplans));
  for(i = nb_args; i-- ;) {
    int j, start_vfpreg = 0;
    CType type = vtop[-i].type;
    type.t &= ~VT_ARRAY;
    size = type_size(&type, &align);
    size = (size + 3) & ~3;
    align = (align + 3) & ~3;
    switch(vtop[-i].type.t & VT_BTYPE) {
      case VT_STRUCT:
      case VT_FLOAT:
      case VT_DOUBLE:
      case VT_LDOUBLE:
      if (float_abi == ARM_HARD_FLOAT) {
        int is_hfa = 0; /* Homogeneous float aggregate */

        if (is_float(vtop[-i].type.t)
            || (is_hfa = is_hgen_float_aggr(&vtop[-i].type))) {
          int end_vfpreg;

          start_vfpreg = assign_vfpreg(&avregs, align, size);
          end_vfpreg = start_vfpreg + ((size - 1) >> 2);
          if (start_vfpreg >= 0) {
            pplan = (struct param_plan) {start_vfpreg, end_vfpreg, &vtop[-i]};
            if (is_hfa)
              add_param_plan(plan, pplan, VFP_STRUCT_CLASS);
            else
              add_param_plan(plan, pplan, VFP_CLASS);
            continue;
          } else
            break;
        }
      }
      ncrn = (ncrn + (align-1)/4) & ~((align/4) - 1);
      if (ncrn + size/4 <= 4 || (ncrn < 4 && start_vfpreg != -1)) {
        /* The parameter is allocated both in core register and on stack. As
         * such, it can be of either class: it would either be the last of
         * CORE_STRUCT_CLASS or the first of STACK_CLASS. */
        for (j = ncrn; j < 4 && j < ncrn + size / 4; j++)
          *todo|=(1<<j);
        pplan = (struct param_plan) {ncrn, j, &vtop[-i]};
        add_param_plan(plan, pplan, CORE_STRUCT_CLASS);
        ncrn += size/4;
        if (ncrn > 4)
          nsaa = (ncrn - 4) * 4;
      } else {
        ncrn = 4;
        break;
      }
      continue;
      default:
      if (ncrn < 4) {
        int is_long = (vtop[-i].type.t & VT_BTYPE) == VT_LLONG;

        if (is_long) {
          ncrn = (ncrn + 1) & -2;
          if (ncrn == 4)
            break;
        }
        pplan = (struct param_plan) {ncrn, ncrn, &vtop[-i]};
        ncrn++;
        if (is_long)
          pplan.end = ncrn++;
        add_param_plan(plan, pplan, CORE_CLASS);
        continue;
      }
    }
    nsaa = (nsaa + (align - 1)) & ~(align - 1);
    pplan = (struct param_plan) {nsaa, nsaa + size, &vtop[-i]};
    add_param_plan(plan, pplan, STACK_CLASS);
    nsaa += size; /* size already rounded up before */
  }
  return nsaa;
}

#undef add_param_plan

/* Copy parameters to their final destination (core reg, VFP reg or stack) for
   function call.

   nb_args: number of parameters the function take
   plan: the overall assignment plan for parameters
   todo: a bitmap indicating what core reg will hold a parameter

   Returns the number of SValue added by this function on the value stack */
static int copy_params(int nb_args, struct plan *plan, int todo)
{
  int size, align, r, i, nb_extra_sval = 0;
  struct param_plan *pplan;
  int pass = 0;

   /* Several constraints require parameters to be copied in a specific order:
      - structures are copied to the stack before being loaded in a reg;
      - floats loaded to an odd numbered VFP reg are first copied to the
        preceding even numbered VFP reg and then moved to the next VFP reg.

      It is thus important that:
      - structures assigned to core regs must be copied after parameters
        assigned to the stack but before structures assigned to VFP regs because
        a structure can lie partly in core registers and partly on the stack;
      - parameters assigned to the stack and all structures be copied before
        parameters assigned to a core reg since copying a parameter to the stack
        require using a core reg;
      - parameters assigned to VFP regs be copied before structures assigned to
        VFP regs as the copy might use an even numbered VFP reg that already
        holds part of a structure. */
again:
  for(i = 0; i < NB_CLASSES; i++) {
    for(pplan = plan->clsplans[i]; pplan; pplan = pplan->prev) {

      if (pass
          && (i != CORE_CLASS || pplan->sval->r < VT_CONST))
        continue;

      vpushv(pplan->sval);
      pplan->sval->r = pplan->sval->r2 = VT_CONST; /* disable entry */
      switch(i) {
        case STACK_CLASS:
        case CORE_STRUCT_CLASS:
        case VFP_STRUCT_CLASS:
          if ((pplan->sval->type.t & VT_BTYPE) == VT_STRUCT) {
            int padding = 0;
            size = type_size(&pplan->sval->type, &align);
            /* align to stack align size */
            size = (size + 3) & ~3;
            if (i == STACK_CLASS && pplan->prev)
              padding = pplan->start - pplan->prev->end;
            size += padding; /* Add padding if any */
            /* allocate the necessary size on stack */
            gadd_sp(-size);
            /* generate structure store */
            r = get_reg(RC_INT);
            th_add_sp_imm (intr(r), padding); /* add r, sp, padding */
            vset(&vtop->type, r | VT_LVAL, 0);
            vswap();
            vstore(); /* memcpy to current sp + potential padding */

            /* Homogeneous float aggregate are loaded to VFP registers
               immediately since there is no way of loading data in multiple
               non consecutive VFP registers as what is done for other
               structures (see the use of todo). */
            if (i == VFP_STRUCT_CLASS) {
              int first = pplan->start, nb = pplan->end - first + 1;
              /* vpop.32 {pplan->start, ..., pplan->end} */
              DBG1(printf (GRN"  copy_params b VFP\n"CLR));
              th_vpop ((first & 1) << 22 | (first >> 1) << 12 | nb);
              /* No need to write the register used to a SValue since VFP regs
                 cannot be used for gcall_or_jmp */
            }
          } else {
            if (is_float(pplan->sval->type.t)) {
              r = vfpr(gv(RC_FLOAT)) << 12;
              if ((pplan->sval->type.t & VT_BTYPE) == VT_FLOAT)
                size = 4;
              else {
                size = 8;
                r |= 0x101; /* vpush.32 -> vpush.64 */
              }
              DBG1(printf (GRN"  copy_params c VFP\n"CLR));
              th_vpush (1 + r);
            } else {
              /* simple type (currently always same size) */
              /* XXX: implicit cast ? */
              size=4;
              if ((pplan->sval->type.t & VT_BTYPE) == VT_LLONG) {
                lexpand();
                size = 8;
                r = gv(RC_INT);
                DBG1(printf (GRN"  copy_params e\n"CLR));
                th_push (1 << intr(r));
                vtop--;
              }
              r = gv(RC_INT);
              th_push (1 << intr(r));
            }
            if (i == STACK_CLASS && pplan->prev)
              gadd_sp(pplan->prev->end - pplan->start); /* Add padding if any */
          }
          break;

        case VFP_CLASS:
          gv(regmask(TREG_F0 + (pplan->start >> 1)));

          if (pplan->start & 1) { /* Must be in upper part of double register */
            DBG1(printf (GRN"  copy_params g VFP %i\n"CLR, pplan->start));
            th_vmov_register (pplan->start, pplan->start - 1, 0); /* vmov.f32 s(n+1), sn */
            vtop->r = VT_CONST; /* avoid being saved on stack by gv for next float */
          }
          break;

        case CORE_CLASS:
          if ((pplan->sval->type.t & VT_BTYPE) == VT_LLONG) {
            lexpand();
            gv(regmask(pplan->end));
            pplan->sval->r2 = vtop->r;
            vtop--;
          }
          gv(regmask(pplan->start));
          /* Mark register as used so that gcall_or_jmp use another one
             (regs >=4 are free as never used to pass parameters) */
          pplan->sval->r = vtop->r;
          break;
      }
      vtop--;
    }
  }

  /* second pass to restore registers that were saved on stack by accident.
     Maybe redundant after the "lvalue_save" patch in tccgen.c:gv() */
  if (++pass < 2)
    goto again;

  /* Manually free remaining registers since next parameters are loaded
   * manually, without the help of gv(int). */
  save_regs(nb_args);

  if (todo) {
    DBG1(printf (GRN"  copy_params h 0x%x\n"CLR, todo));
    th_pop (todo);
    for(pplan = plan->clsplans[CORE_STRUCT_CLASS]; pplan; pplan = pplan->prev) {
      int r;
      pplan->sval->r = pplan->start;
      /* An SValue can only pin 2 registers at best (r and r2) but a structure
         can occupy more than 2 registers. Thus, we need to push on the value
         stack some fake parameter to have on SValue for each registers used
         by a structure (r2 is not used). */
      for (r = pplan->start + 1; r <= pplan->end; r++) {
        if (todo & (1 << r)) {
          nb_extra_sval++;
          vpushi(0);
          vtop->r = r;
        }
      }
    }
  }
  return nb_extra_sval;
}

/* Generate function call. The function address is pushed first, then
   all the parameters in call order. This functions pops all the
   parameters and the function address. */
void gfunc_call(int nb_args)
{
  int r, args_size;
  int def_float_abi = float_abi;
  int todo;
  struct plan plan;

  int variadic;

  if (float_abi == ARM_HARD_FLOAT) {
    variadic = (vtop[-nb_args].type.ref->f.func_type == FUNC_ELLIPSIS);
    if (variadic || floats_in_core_regs(&vtop[-nb_args]))
      float_abi = ARM_SOFTFP_FLOAT;
  }

  /* cannot let cpu flags if other instruction are generated. Also avoid leaving
     VT_JMP anywhere except on the top of the stack because it would complicate
     the code generator. */
  r = vtop->r & VT_VALMASK;
  if (r == VT_CMP || (r & ~1) == VT_JMP)
    gv(RC_INT);

  args_size = assign_regs(nb_args, float_abi, &plan, &todo);

  if (args_size & 7) { /* Stack must be 8 byte aligned at fct call for EABI */
    DBG1(printf (GRN"gfunc_call adjusting alignment\n"CLR));
    args_size = (args_size + 7) & ~7;
    th_sub_sp_imm (13, 4); /* sub sp, sp, #4 */
  }

  nb_args += copy_params(nb_args, &plan, todo);
  tcc_free(plan.pplans);

  /* Move fct SValue on top as required by gcall_or_jmp */
  vrotb(nb_args + 1);
  gcall_or_jmp(0);
  if (args_size)
      gadd_sp(args_size); /* pop all parameters passed on the stack */
  if(float_abi == ARM_SOFTFP_FLOAT && is_float(vtop->type.ref->type.t)) {
    if((vtop->type.ref->type.t & VT_BTYPE) == VT_FLOAT) {
      th_vmov_armcore_sp (0, 0, 0); /* vmov s0, r0 */
    } else {
      th_vmov_armcore_scalar (0, 0, 0); /* vmov.32 d0[0], r0 */
      th_vmov_armcore_scalar (1, 0, 4); /* vmov.32 d0[1], r1 */
    }
  }
  vtop -= nb_args + 1; /* Pop all params and fct address from value stack */
  leaffunc = 0; /* we are calling a function, so we aren't in a leaf function */
  float_abi = def_float_abi;
}

/* generate function prolog of type 't' */
void gfunc_prolog(Sym *func_sym)
{
  CType *func_type = &func_sym->type;
  Sym *sym,*sym2;
  int n, nf, size, align, rs, struct_ret = 0;
  int addr, pn, sn; /* pn=core, sn=stack */
  CType ret_type;

  struct avail_regs avregs = AVAIL_REGS_INITIALIZER;

  DBG1(printf (BLU "########## gfunc_prolog ##########\n" CLR));

  sym = func_type->ref;
  func_vt = sym->type;
  func_var = (func_type->ref->f.func_type == FUNC_ELLIPSIS);

  n = nf = 0;
  if ((func_vt.t & VT_BTYPE) == VT_STRUCT &&
      !gfunc_sret(&func_vt, func_var, &ret_type, &align, &rs))
  {
    n++;
    struct_ret = 1;
    func_vc = 12; /* Offset from fp of the place to store the result */
  }
  for(sym2 = sym->next; sym2 && (n < 4 || nf < 16); sym2 = sym2->next) {
    size = type_size(&sym2->type, &align);
    if (float_abi == ARM_HARD_FLOAT && !func_var &&
        (is_float(sym2->type.t) || is_hgen_float_aggr(&sym2->type))) {
      int tmpnf = assign_vfpreg(&avregs, align, size);
      tmpnf += (size + 3) / 4;
      nf = (tmpnf > nf) ? tmpnf : nf;
    } else
    if (n < 4)
      n += (size + 3) / 4;
  }
  th_sym_t ();
  if (func_var)
    n=4;

  if (n) {
    if(n>4)
      n=4;
    n=(n+1)&-2;
    func_nregs = n;
    DBG1(printf (GRN"  save r0-r4 on stack, n %i\n"CLR, n));
    th_push ((1<<n)-1);
  }
  else
    func_nregs = 0;

  if (nf) {
    if (nf>16)
      nf=16;
    nf=(nf+1)&-2; /* nf => HARDFLOAT => EABI */
    DBG1(printf (GRN"  save s0-s15 on stack if needed\n"CLR));
    th_vpush (nf);
    func_nregs += nf;
  }

  th_push (0x5800);    // push {fp, ip, lr} (r11, r12, r14)
  th_mov_reg (11, 13); // mov fp, sp
  func_sub_sp_offset = ind;
  th_nop();   /* leave space for stack adjustment in epilog */
  th_nop();

  if (float_abi == ARM_HARD_FLOAT) {
    func_vc += nf * 4;
    avregs = AVAIL_REGS_INITIALIZER;
  }

  pn = struct_ret, sn = 0;
  while ((sym = sym->next)) {
    CType *type;
    type = &sym->type;
    size = type_size(type, &align);
    size = (size + 3) >> 2;
    align = (align + 3) & ~3;

    if (float_abi == ARM_HARD_FLOAT && !func_var && (is_float(sym->type.t)
        || is_hgen_float_aggr(&sym->type))) {
      int fpn = assign_vfpreg(&avregs, align, size << 2);
      if (fpn >= 0)
        addr = fpn * 4;
      else
        goto from_stack;
    } else if (pn < 4) {
      pn = (pn + (align-1)/4) & -(align/4);
      addr = (nf + pn) * 4;
      pn += size;
      if (!sn && pn > 4)
        sn = (pn - 4);
    } else {
from_stack:
        sn = (sn + (align-1)/4) & -(align/4);
      addr = (n + nf + sn) * 4;
      sn += size;
    }
    sym_push(sym->v & ~SYM_FIELD, type, VT_LOCAL | VT_LVAL,
             addr + 12);
  }
  leaffunc = 1;
  loc = 0;
}

/* generate function epilog */
void gfunc_epilog(void)
{
  uint32_t x;
  int diff;
  DBG1(printf (GRN"gfunc_epilog\n"CLR));
  /* Copy float return value to core register if base standard is used and
     float computation is made with VFP */
  if ((float_abi == ARM_SOFTFP_FLOAT || func_var) && is_float(func_vt.t)) {
    if((func_vt.t & VT_BTYPE) == VT_FLOAT) {
      th_vmov_armcore_sp (0, 0, 1); /* vmov r0, s0 */
    } else {
      th_vmov_scalar_armcore (0, 0, 0); /* vmov.32 r0, d0[0] */
      th_vmov_scalar_armcore (1, 0, 4); /* vmov.32 r1, d0[1] */
    }
  }

  diff = (-loc + 3) & -4;
  if(!leaffunc)
    diff = ((diff + 11) & -8) - 4;
  if (diff > 0)
    th_add_sp_imm (13, diff);

  th_pop(0x5800); /* pop {fp, ip, lr} */

  if(diff > 0) {
    // Try generating stack adjustment, sub sp, sp, #diff
    x = thg_sub_imm2 (diff);

    if(x) {
      *(uint32_t *)(cur_text_section->data + func_sub_sp_offset) = x;
    } else {
      tcc_error ("internal error: gfunc_epilog failed to generate stack adjustment\n");
    }
  }

  // If the prolog stacked arguments, do not unstack but skip over instead
  if (func_nregs)
    th_add_sp_imm (13, func_nregs << 2);

  // bx lr, and optional nop padding
  th_bx_reg (14);

  if (ind & 3)
    th_nop();
}

ST_FUNC void gen_fill_nops(int bytes)
{
    if (bytes & 1)
      tcc_error("alignment of code section not multiple of 2");
    while (bytes > 0) {
      th_nop();
      bytes -= 2;
    }
}

/* generate a jump to a label */
ST_FUNC int gjmp(int t)
{
  int r = ind;
  int val = ((t-r)>>1) - 2;
  DBG1(printf (GRN"gjmp t 0x%x\n"CLR, t));
  if (nocode_wanted)
    return t;
  if (val < -1024 || val > 1023)
    th_b_t4 (val << 1);
  else
    th_b_t2 (val << 1);
  return r;
}

/* generate a jump to a fixed address */
ST_FUNC void gjmp_addr(int a)
{
  gjmp(a);
}

ST_FUNC int gjmp_cond(int op, int t)
{
  int r = ind;

  DBG1(printf (GRN"gjmp_cond op 0x%x  target 0x%x\n"CLR, op, t));

  if (nocode_wanted)
    return t;

  op = mapcc(op);

  // Branches are adjusted later causing t1 to fail, and it cannot be replaced
  // since tcc is single-pass.

  //if (th_encbranch_8_check (r, t))
  //  th_b_t1 (op, th_encbranch_8 (r,t));
  //else
    th_b_t3 (op, th_encbranch_20 (r,t));

  return r;
}

ST_FUNC int gjmp_append(int n, int t)
{
  int p,lp;
  if(n) {
    DBG1(printf (GRN"gjmp_append n 0x%x t 0x%x\n"CLR, n, t));
    p = n;
    do {
      p = decbranch(lp=p);
    } while(p);
    th_patch_call (lp, t);
    t = n;
  }
  return t;
}

void gen_opi_notshift (int op, int opc)
{
  int c = 0;

  if ((vtop[-1].r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
  {
    if (opc == 4 || opc == 5 || opc == 0xc)
    {
      // sub -> rsb
      // subs -> rsbs
      // sbc -> rsc
      vswap();
      opc |= 2;
    }
  }

  if ((vtop->r & VT_VALMASK) == VT_CMP ||
      (vtop->r & (VT_VALMASK & ~1)) == VT_JMP)
  {
    // TODO: How to test this?
    printf (RED"  VT_CMP/CT_JMP\n"CLR);
    gv(RC_INT);
  }

  vswap();
  c = intr(gv(RC_INT));
  vswap();

  gen_opi_regular (opc, c);

  vtop--;

  if (op >= TOK_ULT && op <= TOK_GT)
    vset_VT_CMP (op);
}

void gen_opi_shift (int opc)
{
  int r = 0;

  if ((vtop->r & VT_VALMASK) == VT_CMP ||
      (vtop->r & (VT_VALMASK & ~1)) == VT_JMP)
    gv(RC_INT);

  vswap();
  r = intr(gv(RC_INT));
  vswap();

  if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
  {
    int fr = intr(vtop[-1].r=get_reg_ex(RC_INT,regmask(vtop[-1].r)));
    int c = vtop->c.i & 0x1f;

    if (opc == 0)
      th_lsl_imm (r, fr, c);
    else if (opc == 1)
      th_lsr_imm (r, fr, c);
    else if (opc == 2)
      th_asr_imm (r, fr, c);
  }
  else
  {
    int fr = intr(gv(RC_INT));
    int c = intr(vtop[-1].r=get_reg_ex(RC_INT,two2mask(vtop->r,vtop[-1].r)));

    if (opc == 0)
      th_lsl_reg (c, r, fr);
    else if (opc == 2)
      th_asr_reg (c, r, fr);
    else
      tcc_error ("internal error: gen_opi_shift unhandled case");
  }

  vtop--;
}

/* generate an integer binary operation */
void gen_opi(int op)
{
  uint32_t r, fr;

  switch(op) {
    case '+':
      DBG1(printf (GRN"gen_opi op +\n"CLR));
      return gen_opi_notshift (op, 0x8);
    case TOK_ADDC1:
      DBG1(printf (GRN"gen_opi op TOK_ADDC1\n"CLR)); /* add with carry generation */
      return gen_opi_notshift (op, 0x9);
    case '-':
      DBG1(printf (GRN "  gen_opi op -\n" CLR));
      return gen_opi_notshift (op, 0x4);
    case TOK_SUBC1:
      DBG1(printf (GRN"gen_opi op TOK_SUBC1\n"CLR)); /* sub with carry generation */
      return gen_opi_notshift (op, 0x5);
    case TOK_ADDC2:
      DBG1(printf (GRN"gen_opi op TOK_ADDC2\n"CLR)); /* add with carry use */
      return gen_opi_notshift (op, 0xA);
    case TOK_SUBC2:
      DBG1(printf (GRN"gen_opi op TOK_SUBC2\n"CLR)); /* sub with carry use */
      return gen_opi_notshift (op, 0xC);
    case '&':
      DBG1(printf (GRN"gen_opi op &\n"CLR));
      return gen_opi_notshift (op, 0x0);
    case '^':
      DBG1(printf (GRN"gen_opi op ^\n"CLR));
      return gen_opi_notshift (op, 0x2);
    case '|':
      DBG1(printf (GRN"gen_opi op |\n"CLR));
      return gen_opi_notshift (op, 0x18);
    case '*':
      DBG1(printf (GRN"gen_opi op *\n"CLR));
      gv2(RC_INT, RC_INT);
      r = vtop[-1].r;
      fr = vtop[0].r;
      vtop--;
      th_mul (intr(r), intr(fr), intr(r));
      return;
    case TOK_SHL:
      DBG1(printf (GRN"gen_opi op TOK_SHL\n"CLR));
      return gen_opi_shift (0);
    case TOK_SHR:
      DBG1(printf (GRN"gen_opi op TOK_SHR\n"CLR));
      return gen_opi_shift (1);
    case TOK_SAR:
      DBG1(printf (GRN"gen_opi op TOK_SAR\n"CLR));
      return gen_opi_shift (2);
    case '/':
      DBG1(printf (GRN"gen_opi op /\n"CLR));
    case TOK_PDIV:
      DBG1(printf (GRN"gen_opi op TOK_PDIV\n"CLR));
      gv2(RC_INT, RC_INT);
      r = vtop[-1].r;
      fr = vtop[0].r;
      th_sdiv (intr(r), intr(r), intr(fr));
      vtop--;
      return;
    case TOK_UDIV:
      DBG1(printf (GRN"gen_opi op TOK_UDIV\n"CLR));
      gv2(RC_INT, RC_INT);
      r = vtop[-1].r;
      fr = vtop[0].r;
      th_udiv (intr(r), intr(r), intr(fr));
      vtop--;
      return;
    case '%':
      DBG1(printf (GRN"gen_opi op %%\n"CLR));
      {
        uint32_t rr = 0;

        gv2(RC_INT, RC_INT);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        r = intr(r);
        fr = intr(fr);

        // Allocation of temporary register
        for (int i = 0; i < 5; i++)
          if (rr == r || rr == fr)
            rr++;
          else break;

        th_push (1 << rr);
        th_sdiv (rr, r, fr);
        th_mul (fr, fr, rr);
        th_sub_reg (r, r, fr);
        th_pop (1 << rr);
      }
      return;
    case TOK_UMOD:
      DBG1(printf(GRN"gen_opi op TOK_UMOD\n"CLR));
      {
        uint32_t rr = 0;

        gv2(RC_INT, RC_INT);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        r = intr(r);
        fr = intr(fr);

        // Allocation of temporary register
        for (int i = 0; i < 5; i++)
          if (rr == r || rr == fr)
            rr++;
          else break;

        th_push (1 << rr);
        th_udiv (rr, r, fr);
        th_mul (fr, fr, rr);
        th_sub_reg (r, r, fr);
        th_pop (1 << rr);
      }
      return;
    case TOK_UMULL:
      DBG1(printf (GRN"gen_opi op TOK_UMULL\n"CLR));
      gv2(RC_INT, RC_INT);
      r = intr(vtop[-1].r2=get_reg(RC_INT));
      fr = vtop[-1].r;
      vtop[-1].r = get_reg_ex(RC_INT,regmask(fr));
      vtop--;
      th_umull (intr(vtop->r), r, intr(vtop[1].r), intr(fr));
      return;
    default:
      DBG1(printf (GRN"gen_opi op def (cmp imm)\n"CLR));
      return gen_opi_notshift (op, 0x15);
  }
  tcc_error("internal error: gen_opi %i unimplemented", op);
}

static int is_zero (int i)
{
  if((vtop[i].r & (VT_VALMASK | VT_LVAL | VT_SYM)) != VT_CONST)
    return 0;
  if (vtop[i].type.t == VT_FLOAT)
    return (vtop[i].c.f == 0.f);
  else if (vtop[i].type.t == VT_DOUBLE)
    return (vtop[i].c.d == 0.0);
  return (vtop[i].c.ld == 0.l);
}

#define T2CPR(t) (((t) & VT_BTYPE) != VT_FLOAT ? 0x100 : 0)

void gen_opf_regular (uint32_t opc, int fneg)
{
  uint32_t inst = 0;
  int r = gv(RC_FLOAT);
  opc |= 0xee000a00 | vfpr(r);
  r = regmask(r);
  if(!fneg) {
    int r2;
    vswap();
    r2 = gv(RC_FLOAT);
    opc |= vfpr(r2) << 16;
    r |= regmask(r2);
  }
  vtop->r = get_reg_ex(RC_FLOAT,r);
  if(!fneg)
    vtop--;
  inst = opc | (vfpr(vtop->r) << 12);
  ot(inst >> 16);
  ot(inst);
}

void gen_opf_cmp (uint32_t opc, uint32_t op)
{
  uint32_t inst = 0;
  opc |= 0xeeb40a40; /* vcmp */

  if(op != TOK_EQ && op != TOK_NE)
    opc |= 0x80; /* vcmp -> vcmpe */

  if(is_zero(0)) {
    vtop--;
    inst = opc | 0x10000 | (vfpr(gv(RC_FLOAT)) << 12); /* vcmp(e) -> vcmp(e)z */
  } else {
    opc |= vfpr(gv(RC_FLOAT));
    vswap();
    inst = opc | (vfpr(gv(RC_FLOAT)) << 12);
    vtop--;
  }

  ot(inst >> 16);
  ot(inst);
  th_vmrs (15); // vmrs APSR_nzcv, fpscr: Move FPU status flags
}

/* generate a floating point operation 'v = t1 op t2' instruction. The
   two operands are guaranteed to have the same floating point type */
void gen_opf(int op)
{
  uint32_t isdouble = ((vtop->type.t & VT_BTYPE) != VT_FLOAT) ? 0x100 : 0;

  switch(op) {
    case '+':
      if(is_zero(-1))
        vswap();
      if(is_zero(0)) {
        vtop--;
        return;
      }
      return gen_opf_regular (isdouble | 0x00300000, 0); // vadd
    case '-':
      if(is_zero(0)) {
        vtop--;
        return;
      }
      if(is_zero(-1)) {
        vswap();
        vtop--;
        return gen_opf_regular (isdouble | 0x00b10040, 1); // vneg
      }
      else
        return gen_opf_regular (isdouble | 0x00300040, 0); // vsub
    case '*':
      return gen_opf_regular (isdouble | 0x00200000, 0); // vmul
    case '/':
      return gen_opf_regular (isdouble | 0x00800000, 0); // vdiv
    default:
      if (op < TOK_ULT || op > TOK_GT) {
        tcc_error("unknown fp op %x!", op);
        return;
      }
      if (is_zero(-1)) {
        vswap();
        switch(op) {
          case TOK_LT: op = TOK_GT; break;
          case TOK_GE: op = TOK_ULE; break;
          case TOK_LE: op = TOK_GE; break;
          case TOK_GT: op = TOK_ULT; break;
        }
      }

      gen_opf_cmp (isdouble, op);

      switch (op) {
        case TOK_LE: op = TOK_ULE; break;
        case TOK_LT: op = TOK_ULT; break;
        case TOK_UGE: op = TOK_GE; break;
        case TOK_UGT: op = TOK_GT; break;
      }
      vset_VT_CMP(op);
      return;
  }
}

/* convert integers to fp 't' type. Must handle 'int', 'unsigned int'
   and 'long long' cases. */
ST_FUNC void gen_cvt_itof(int t)
{
  int bt = vtop->type.t & VT_BTYPE;

  if (bt == VT_INT || bt == VT_SHORT || bt == VT_BYTE) {
    uint32_t r = intr(gv(RC_INT));
    uint32_t r2 = vfpr(vtop->r=get_reg(RC_FLOAT));
    uint32_t op = (vtop->type.t & VT_UNSIGNED) ? 0 : 1;

    th_vmov_armcore_sp (r, r2, 0);
    th_vcvt_fp_int (r2, r2, 0, (t & VT_BTYPE) != VT_FLOAT, op);
    return;
  } else if(bt == VT_LLONG) {
    int func;
    CType *func_type = 0;
    if((t & VT_BTYPE) == VT_FLOAT) {
      func_type = &func_float_type;
      if(vtop->type.t & VT_UNSIGNED)
        func=TOK___floatundisf;
      else
        func=TOK___floatdisf;
    } else if((t & VT_BTYPE) == VT_DOUBLE || (t & VT_BTYPE) == VT_LDOUBLE) {
      func_type = &func_double_type;
      if(vtop->type.t & VT_UNSIGNED)
        func=TOK___floatundidf;
      else
        func=TOK___floatdidf;
    }
    if(func_type) {
      vpush_global_sym(func_type, func);
      vswap();
      gfunc_call(1);
      vpushi(0);
      vtop->r=TREG_F0;
      return;
    }
  }
  tcc_error("unimplemented gen_cvt_itof %x!",vtop->type.t);
}

/* convert fp to int 't' type */
void gen_cvt_ftoi(int t)
{
  uint32_t r2 = vtop->type.t & VT_BTYPE;
  int u = t & VT_UNSIGNED;
  t &= VT_BTYPE;

  if (t == VT_INT) {
    uint32_t opc = u ? 0x4 : 0x5;
    uint32_t r = vfpr(gv(RC_FLOAT));
    uint32_t rr = intr(vtop->r = get_reg(RC_INT));
    th_vcvt_fp_int (rr, r, opc, (r2 & VT_BTYPE) != VT_FLOAT, 1);
    th_vmov_armcore_sp (rr, rr, 1);
    return;
  } else if (t == VT_LLONG) { // unsigned handled in gen_cvt_ftoi1
    int func = 0;
    if (r2 == VT_FLOAT)
      func = TOK___fixsfdi;
    else if (r2 == VT_LDOUBLE || r2 == VT_DOUBLE)
      func = TOK___fixdfdi;

    if (func) {
      vpush_global_sym(&func_old_type, func);
      vswap();
      gfunc_call(1);
      vpushi(0);
      if (t == VT_LLONG)
        vtop->r2 = REG_IRE2;
      vtop->r = REG_IRET;
      return;
    }
  }
  tcc_error("unimplemented gen_cvt_ftoi!");
}

/* convert from one floating point type to another */
void gen_cvt_ftof(int t)
{
  if (((vtop->type.t & VT_BTYPE) == VT_FLOAT) != ((t & VT_BTYPE) == VT_FLOAT))
  {
    uint32_t r = vfpr(gv(RC_FLOAT));

    if ((t & VT_BTYPE) != VT_FLOAT)
      th_vcvt_float_to_double (r, r);
    else
      th_vcvt_double_to_float (r, r);
  }
}

/* computed goto support */
void ggoto(void)
{
  gcall_or_jmp (1);
  vtop--;
}

/* Save the stack pointer onto the stack and return the location of its address */
ST_FUNC void gen_vla_sp_save(int addr) {
    SValue v;
    v.type.t = VT_PTR;
    v.r = VT_LOCAL | VT_LVAL;
    v.c.i = addr;
    store(TREG_SP, &v);
}

/* Restore the SP from a location on the stack */
ST_FUNC void gen_vla_sp_restore(int addr) {
    SValue v;
    v.type.t = VT_PTR;
    v.r = VT_LOCAL | VT_LVAL;
    v.c.i = addr;
    load(TREG_SP, &v);
}

/* Subtract from the stack pointer, and push the resulting value onto the stack */
ST_FUNC void gen_vla_alloc(CType *type, int align) {
    int r = intr(gv(RC_INT));
    th_sub_reg (r, 13, r);
    if (align < 8)
        align = 8;
    if (align & (align - 1))
        tcc_error("alignment is not a power of 2: %i", align);
    /* bic sp, r, #align-1 */
    if(!th_bic_imm (r, r, align - 1))
      tcc_error ("internal error: gen_vla_alloc th_bic_imm failure");
    th_mov_reg (13, r);
    vpop();
}

/* end of thumb code generator */
/*************************************************************/
#endif
/*************************************************************/

/* vim: expandtab set ts=2 sw=2 sts=2 tw=80 :*/
