/*
 *  ARM Thumb 2 instruction functions for TCC
 *
 *  Copyright (c) 2020 Erlend J. Sveen
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

/*
 * This file contains the th_* functions called by arm-thumb-gen.c. They fit
 * into 4 main categories:
 *
 * 1. Branch and immediate field encoding
 * 2. $t, $a and $d symbol functions. These symbols indicates if data in .text
 *    is a thumb instruction, ARM instruction or data. Needed by objdump and
 *    ld, among others.
 * 3. General instruction functions. They validate the arguments/operands and
 *    output the smallest encoding of that instruction. Some return the size
 *    of the generated instruction. If the return code is null there was no
 *    possible encoding.
 * 4. Load, store and branch helper functions.
 */

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define RED "\x1B[31;1merror "
#define GRN "\x1B[32;1m"
#define YEL "\x1B[33;1m"
#define BLU "\x1B[34;1m"
#define CLR "\x1B[0m"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

/*
 * Use this flag to temporarily disable .s (set flags) instructions,
 * for those that support it.
 */
uint32_t g_setflags = 1;

///////////////////////////////////////////////////////////////////////////////
// Encoding for branches and immediate fields
///////////////////////////////////////////////////////////////////////////////

/*
 *
 */
static uint32_t th_encbranch (int pos, int addr)
{
	DBG2(printf ("  th_encbranch pos %x addr %x\n", pos, addr));

	return addr - pos - 4;
}

/*
 * Returns true if addr is within reach for b (branch) type 1
 */
static uint32_t th_encbranch_8_check (int pos, int addr)
{
	DBG2(printf ("  th_encbranch_8_check pos %x addr %x\n", pos, addr));

	addr = (addr - pos - 4) / 2;

	if (addr >= 127 || addr < -128)
		return 0;
	else
		return 1;
}

/*
 * Encode branch address for b type 1
 */
static uint32_t th_encbranch_8 (int pos, int addr)
{
	DBG2(printf ("  th_encbranch_8 pos %x addr %x\n", pos, addr));

	addr = (addr - pos - 4) / 2;

	if (addr >= 127 || addr < -128)
	{
		tcc_error("internal error: th_encbranch_8 called for branch that is too far: %i", addr);
		return 0;
	}

	return addr & 0x0ff;
}

/*
 * Encode branch address for b type 2
 */
static uint32_t th_encbranch_11 (int pos, int addr)
{
	DBG1(printf ("  th_encbranch_11 pos %x addr %x\n", pos, addr));

	addr = (addr - pos - 4) / 2;

	if (addr >= 1023 || addr < -1024)
	{
		tcc_error("internal error: th_encbranch_11 called for branch that is too far");
		return 0;
	}

	return addr & 0x07ff;
}

/*
 * Encode branch address for b type 3
 */
static uint32_t th_encbranch_20 (int pos, int addr)
{
	DBG2(printf ("  th_encbranch_20 pos %x addr %x\n", pos, addr));
	return (addr - pos - 4) / 2;
}

/*
 * Encode branch instruction for b type 3
 */
static uint32_t th_encbranch_b_t3 (uint32_t imm)
{
	uint32_t s = (imm >> 19) & 1;
	uint32_t imm6 = (imm >> 11) & 0x3f;
	uint32_t imm11 = (imm & 0x7ff);
	uint32_t j2 = (imm >> 18) & 1;
	uint32_t j1 = (imm >> 17) & 1;

	uint32_t a = (s << 10) | imm6;
	uint32_t b = (j1 << 13) | (j2 << 11) | imm11;
	return (a << 16) | b;
}

/*
 * Pack immediate, type 10 11
 */
static uint32_t th_packimm_10_11_0 (uint32_t imm)
{
	uint32_t imm11 = (imm >> 1) & 0x000007FF;
	uint32_t imm10 = (imm >> 12) & 0x000003FF;
	uint32_t s = (imm >> 24) & 1;
	uint32_t j1 = ~((imm >> 23) ^ s) & 1;
	uint32_t j2 = ~((imm >> 22) ^ s) & 1;
	DBG2(printf ("  th_packimm_10_11_0 %i %i %i %x\n", s, j1, j2, imm));
	return (s << 26) | (imm10 << 16) | (j1 << 13) | (j2 << 11) | imm11;
}

/*
 * See A6.3.2: Modified immediate constants in Thumb instructions
 */
static uint32_t th_packimm_thumb (uint32_t imm)
{
	if (!(imm & 0xFFFFFF00))
		return (0 << 12) | imm;
	else if (!(imm & 0xFF00FF00) && (imm >> 16) == (imm & 0xFF))
		return (1 << 12) | (imm & 0xFF);
	else if (!(imm & 0x00FF00FF) && (imm >> 16) == (imm & 0xFF00))
		return (2 << 12) | ((imm >> 8) & 0xFF);
	else if ((imm & 0x0FF) == (imm >> 24) && (imm & 0x0FFFF) == (imm >> 16))
		return (3 << 12) | ((imm >> 8) & 0xFF);
	else
	{
		for (uint32_t i = 8, j = 0; i <= 0x1F; i++, j++)
		{
			uint32_t mask = 0xFF000000 >> j;
			uint32_t one = 0x80000000 >> j;

			if ((imm & one) == one && (imm & ~mask) == 0)
			{
				uint32_t _i = i >> 4;
				uint32_t imm3 = (i >> 1) & 7;
				uint32_t a = i & 1;
				uint32_t bcdefgh = (imm >> (24-j)) & 0x7f;

				return (_i << 26) | (imm3 << 12) | (a << 7) |
					bcdefgh;
			}
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Functions for emitting $t, $a and $d symbols
///////////////////////////////////////////////////////////////////////////////
static void th_sym_t (void)
{
	int sym_type = STT_NOTYPE;
	int sym_bind = STB_LOCAL;
	int info = ELFW(ST_INFO)(sym_bind, sym_type);
	set_elf_sym(symtab_section, ind, 0, info, 0, 1, "$t");
}

static void th_sym_a (void)
{
	int sym_type = STT_NOTYPE;
	int sym_bind = STB_LOCAL;
	int info = ELFW(ST_INFO)(sym_bind, sym_type);
	set_elf_sym(symtab_section, ind, 0, info, 0, 1, "$a");
}

static void th_sym_d (void)
{
	int sym_type = STT_NOTYPE;
	int sym_bind = STB_LOCAL;
	int info = ELFW(ST_INFO)(sym_bind, sym_type);
	set_elf_sym(symtab_section, ind, 0, info, 0, 1, "$d");
}

///////////////////////////////////////////////////////////////////////////////
// Instruction functions that do not write instructions
///////////////////////////////////////////////////////////////////////////////
static uint32_t thg_sub_imm2 (uint32_t imm)
{
	uint32_t x = 0;

	DBG1(printf ("  thg_sub_imm2 imm %i\n", imm));

	/*if (imm <= 0x01fc && !(imm & 3))
		x = 0xb080 | (imm >> 2); // sub_sp_imm Encoding T1
	else*/ if (imm <= 0x0fff)
	{
		uint32_t i = (imm >> 11) & 1;
		uint32_t imm3 = (imm >> 8) & 7;
		x = (0xf2ad | (i << 10));		// Encoding T3
		x |= ((imm3 << 12) | (13 << 8) | (imm & 0x0ff)) << 16;
	}
	else
	{
		uint32_t im = th_packimm_thumb (imm);

		if (im || imm == 0)
		{
			uint32_t a = im >> 16;
			uint32_t b = im & 0x0000ffff;
			x = (0xf1a0 | 13 | a) << 16;	// Encoding T2
			x |= ((13 << 8) | b);
		}
	}

	return x;
}

///////////////////////////////////////////////////////////////////////////////
// Instruction functions that write instructions
///////////////////////////////////////////////////////////////////////////////
static void th_nop ()
{
	ot(0xbf00);
}

static void th_mov_reg (uint32_t rd, uint32_t rm)
{
	uint32_t d = (rd >> 3) & 1;
	ot(0x4600 | (d << 7) | (rm << 3) | (rd & 7));
}

static int th_mov_imm (uint32_t rd, uint32_t imm, uint32_t setflags)
{
	if (rd <= 7 && imm <= 0x0ff && (setflags == 1 || setflags == 2))
		return ot(0x2000 | (rd << 8) | imm);	// Encoding T1
	else if (rd != 13 && rd != 15 && imm <= 0x0ffff && (!setflags || setflags == 2))
	{
		uint32_t imm4 = (imm >> 12) & 0xf;
		uint32_t i = (imm >> 11) & 1;
		uint32_t imm3 = (imm >> 8) & 7;
		ot(0xf240 | (i << 10) | imm4);		// Encoding T3
		ot((imm3 << 12) | (rd << 8) | (imm & 0x0ff));
		return 4;
	}
	/*else if (rd != 13 && rd != 15)		// Actually uses carry flag, can't use it?
	{
		uint32_t enc = th_packimm_thumb (imm);

		if (enc || imm == 0)
		{
			uint32_t a = enc >> 16;
			uint32_t b = enc & 0x0000ffff;
			ot(0xf04f | (setflags << 4) | a);// Encoding T2
			ot(b | (rd << 8));
			return 4;
		}
	}*/

	return 0;
}

/*
 * Only has one encoding
 */
static int th_mvn_imm (uint32_t rd, uint32_t imm, uint32_t setflags)
{
	if (rd != 13 && rd != 15 && (!setflags || setflags == 2))
	{
	}

	return 0;
}

static void th_sub_sp_imm (uint32_t rd, uint32_t imm)
{
	if (rd == 13 && imm <= 0x03fc)
		ot(0xb080 | (imm >> 2));		// Encoding T1
	else if (rd != 15 && imm <= 0x0fff)
	{
		uint32_t i = (imm >> 11) & 1;
		uint32_t imm3 = (imm >> 8) & 7;
		ot(0xf2ad | (i << 10));			// Encoding T3
		ot((imm3 << 12) | (rd << 8) | (imm & 0x0ff));
	}
	else if (rd != 15)
	{
		uint32_t enc = th_packimm_thumb (imm);

		if (enc || imm == 0)
		{
			uint32_t a = enc >> 16;
			uint32_t b = enc & 0x0000ffff;
			ot(0xf1a0 | a | 13);		// Encoding T2
			ot(b | (rd << 8));
		}
		else
			tcc_error ("internal error: th_sub_sp_imm pack failed\n");
	}
	else
		tcc_error ("internal error: th_sub_sp_imm invalid parameters\n");
}

static int th_adc_imm (uint32_t rd, uint32_t rn, uint32_t imm)
{
	if (rn != 13 && rn != 15 && rd != 13 && rd != 15)
	{
		uint32_t enc = th_packimm_thumb (imm);

		if (enc || imm == 0)
		{
			uint32_t a = enc >> 16;
			uint32_t b = enc & 0x0000ffff;
			ot(0xf140 | a | rn);		// Encoding T1
			ot(b | (rd << 8));
			return 4;
		}
	}

	return 0;
}

static void th_adc_reg (uint32_t rd, uint32_t rn, uint32_t rm)
{
	if (rd == rn && rd <= 7 && rm <= 7)
		ot(0x4140 | (rm << 3) | rd);		// Encoding T1
	else if (rd != 13 && rd != 15 && rn != 13 && rn != 15 && rm != 13 && rm != 15)
	{
		ot(0xeb40 | rn);			// Encoding T2
		ot((rd << 8) | rm);
	}
	else
		tcc_error ("internal error: th_adc_reg invalid parameters\n");
}

static int th_add_imm (uint32_t rd, uint32_t rn, uint32_t imm)
{
	if (rd == rn && rd <= 7 && imm <= 0x00ff)
		return ot(0x3000 | (rd << 8) | imm); // add immediate t2
	else if (rd != 13 && rn != 15)
	{
		uint32_t enc = th_packimm_thumb (imm);

		if (enc || imm == 0)
		{
			uint32_t a = enc >> 16;
			uint32_t b = enc & 0x0000ffff;
			ot(0xf100 | a | rn);		// Encoding T3
			ot(b | (rd << 8));
			return 4;
		}
	}
	else if (rn != 13 && rn != 15 && rd != 13 && rd != 15 && imm <= 0x0fff)
	{
		uint32_t i = (imm >> 11) & 1;
		uint32_t imm3 = (imm >> 8) & 7;
		ot(0xf200 | (i << 10) | rn);		// Encoding T4
		ot((imm3 << 12) | (rd << 8) | (imm & 0x0ff));
		return 4;
	}

	return 0;
}

static void th_add_reg (uint32_t rd, uint32_t rn, uint32_t rm)
{
	if (rd <= 7 && rn <= 7 && rm <= 7)
		ot(0x1800 | (rm << 6) | (rn << 3) | rd);	// Encoding T1
	else if (rd == rn && rd <= 7)
		ot(0x4400 | (rm << 3) | rd);			// Encoding T2
	else
	{
		ot(0xeb00 | rn);				// Encoding T3
		ot((rd << 8) | rm);
	}
}

static void th_add_sp_imm (uint32_t rd, uint32_t imm)
{
	if (rd <= 7 && imm <= 0x03fc)
		ot(0xa800 | (rd << 8) | (imm >> 2));	// Encoding T1
	else if (rd == 13 && imm <= 0x001fc)
		ot(0xb000 | (imm >> 2));		// Encoding T2
	else if (rd != 15 && imm <= 0x0fff)
	{
		uint32_t i = (imm >> 11) & 1;
		uint32_t imm3 = (imm >> 8) & 7;
		ot(0xf20d | (i << 10));		// Encoding T4
		ot((imm3 << 12) | (rd << 8) | (imm & 0x0ff));
	}
	else if (rd != 15)
	{
		uint32_t enc = th_packimm_thumb (imm);

		if (enc || imm == 0)
		{
			uint32_t a = enc >> 16;
			uint32_t b = enc & 0x0000ffff;
			ot(0xf100 | a | 13);	// Encoding T3
			ot(b | (rd << 8));
		}
		else
			tcc_error ("internal error: th_add_sp_imm pack failed\n");
	}
	else
		tcc_error ("internal error: th_add_sp_imm invalid parameters\n");
}

static int th_bic_imm (uint32_t rd, uint32_t rn, uint32_t imm)
{
	if (rd != 13 && rd != 15 && rn != 13 && rn != 15)
	{
		uint32_t enc = th_packimm_thumb (imm);

		if (enc || imm == 0)
		{
			uint32_t a = enc >> 16;
			uint32_t b = enc & 0x0000ffff;
			ot(0xf020 | rn | a);
			ot((rd << 8) | b);
			return 4;
		}
	}

	return 0;
}

static void th_sub_reg (uint32_t rd, uint32_t rn, uint32_t rm)
{
	if (rd <= 7 && rn <= 7 && rm <= 7)
		ot(0x1a00 | (rm << 6) | (rn << 3) | rd);	// Encoding t1
	else if (rd != 13 && rd != 15 && rn != 15 && rm != 13 && rm != 15)
	{
		ot(0xeba0 | rn);				// Encoding t2
		ot((rd << 8) | rm);
	}
	else
		tcc_error ("internal error: th_sub_reg invalid parameters\n");
}

static int th_sub_imm (uint32_t rd, uint32_t rn, uint32_t imm)
{
	if (rd <= 7 && rn <= 7 && imm <= 7)
		return ot(0x1e00 | (imm << 6) | (rn << 3) | rd);	// Encoding t1
	else if (rd == rn && rd <= 7 && imm <= 0x0ff)
		return ot(0x3800 | (rd << 8) | imm);			// Encoding t2
	else if (imm <= 0x0fff && rd != 13 && rd != 15)
	{
		uint32_t imm8 = imm & 0x0ff;
		uint32_t imm3 = (imm >> 8) & 7;
		uint32_t i = (imm >> 11) & 1;
		ot(0xf2a0 | (i << 10) | rn);
		ot((imm3 << 12) | (rd << 8) | imm8);		// Encoding t4
		return 4;
	}
	else if (rd != 13 && rd != 15)
	{
		uint32_t im = th_packimm_thumb (imm);

		if (im || imm == 0)
		{
			uint32_t a = im >> 16;
			uint32_t b = im & 0x0000ffff;
			ot(0xf1a0 | rn | a);			// Encoding t1
			ot((rd << 8) | b);
			return 4;
		}
	}

	return 0;
}

/*
 * For imm = 0, it is a neg instruction
 */
static int th_rsb_imm (uint32_t rd, uint32_t rn, uint32_t imm, uint32_t setflags)
{
	if (rd <= 7 && rn <= 7 && imm == 0 && setflags)
		return ot(0x4240 | (rn << 3) | rd);		// Encoding t1
	else if (rd != 13 && rd != 15 && rn != 13 && rn != 15)
	{
		uint32_t im = th_packimm_thumb (imm);

		if (im || imm == 0)
		{
			uint32_t a = im >> 16;
			uint32_t b = im & 0x0000ffff;
			ot(0xf1c0 | (setflags << 4) | rn | a);	// Encoding t2
			ot((rd << 8) | b);
			return 4;
		}
	}
	else
		tcc_error ("internal error: th_rsb_imm invalid parameters\n");

	return 0;
}

/*
 * Only has one encoding
 */
static void th_rsb_reg (uint32_t rd, uint32_t rn, uint32_t rm)
{
	if (rd != 13 && rd != 15 && rn != 13 && rn != 15 && rm != 13 && rm != 15)
	{
		ot(0xebc0 | rn);				// Encoding t1
		ot((rd << 8) | rm);
	}
	else
		tcc_error ("internal error: th_rsb_reg invalid parameters\n");
}

/*
 * Only has one encoding
 */
static int th_sbc_imm (uint32_t rd, uint32_t rn, uint32_t imm)
{
	if (rd != 13 && rd != 15 && rn != 13 && rn != 15)
	{
		uint32_t im = th_packimm_thumb (imm);

		if (im || imm == 0)
		{
			uint32_t a = im >> 16;
			uint32_t b = im & 0x0000ffff;
			ot(0xf160 | rn | a);		// Encoding t1
			ot((rd << 8) | b);
			return 4;
		}
	}
	else
		tcc_error ("internal error: th_sbc_imm invalid parameters\n");

	return 0;
}

static void th_sbc_reg (uint32_t rd, uint32_t rn, uint32_t rm)
{
	if (rd == rn && rd <= 7 && rn <= 7 && rm <= 7)
		ot(0x4180 | (rm << 3) | rd);	// Encoding t1
	else
	{
		ot(0xeb70 | rn);		// Encoding t2
		ot((rd << 8) | rm);
	}
}

static void th_and_reg (uint32_t rd, uint32_t rn, uint32_t rm)
{
	if (rd == rn && rm <= 7 && rn <= 7)
		ot(0x4000 | (rm << 3) | rd);	// Encoding t1
	else if (rd != 13 && rn != 13 && rn != 15 && rm != 13 && rm != 15)
	{
		ot(0xea00 | rn);
		ot((rd << 8) | rm);		// Encoding t2
	}
	else
		tcc_error ("internal error: th_and_reg invalid parameters\n");
}

/*
 * Only has one encoding
 */
static int th_and_imm (uint32_t rd, uint32_t rn, uint32_t imm)
{
	uint32_t im = th_packimm_thumb (imm);

	if (im || imm == 0)
	{
		uint32_t a = im >> 16;
		uint32_t b = im & 0x0000ffff;
		ot(0xf000 | rn | a);		// Encoding t1
		ot((rd << 8) | b);
		return 4;
	}
	else
		return th_bic_imm (rd, rn, ~imm);
}

static void th_xor_reg (uint32_t rd, uint32_t rn, uint32_t rm)
{
	// Also called EOR
	if (rd == rn && rm <= 7 && rn <= 7)
		ot(0x4040 | (rm << 3) | rd);	// Encoding t1
	else if (rd != 13 && rd != 15 && rn != 13 && rn != 15)
	{
		ot(0xea80 | rn);		// Encoding T2
		ot((rd << 8) | rm);
	}
	else
		tcc_error ("internal error: th_xor_reg invalid parameters\n");
}

/*
 * Only has one encoding.
 * Instruction is called EOR.
 */
static int th_xor_imm (uint32_t rd, uint32_t rn, uint32_t imm)
{
	uint32_t im = th_packimm_thumb (imm);

	if (im || imm == 0)
	{
		uint32_t a = im >> 16;
		uint32_t b = im & 0x0000ffff;
		ot(0xf080 | rn | a);		// Encoding t1
		ot((rd << 8) | b);
		return 4;
	}

	return 0;
}

static void th_orr_reg (uint32_t rd, uint32_t rn, uint32_t rm)
{
	if (rd == rn && rm <= 7 && rn <= 7)
		ot(0x4300 | (rm << 3) | rd);	// Encoding t1
	else if (rd != 13 && rd != 15 && rn != 13 && rm != 13 && rm != 15)
	{
		ot(0xea40 | rn);		// Encoding t2
		ot((rd << 8) | rm);
	}
	else
		tcc_error ("internal error: th_orr_reg invalid parameters\n");
}

/*
 * Only has one encoding
 */
static int th_orr_imm (uint32_t rd, uint32_t rn, uint32_t imm)
{
	if (rd != 13 && rd != 15 && rn != 13)
	{
		uint32_t enc = th_packimm_thumb (imm);

		if (enc || imm == 0)
		{
			ot(0xf040 | rn | (enc >> 16));	// Encoding t1
			ot((rd << 8) | enc);
			return 4;
		}
	}

	return 0;
}

static void th_cmp_reg (uint32_t rn, uint32_t rm)
{
	if (rm <= 7 && rn <= 7)
		ot(0x4280 | (rm << 3) | rn);	// Encoding t1
	else if (!(rn <= 7 && rm <= 7))
		ot(0x4500 | ((rn & 0x8) << 4) | (rm << 3) | (rn & 7)); // Enc T2
	else
		tcc_error ("internal error: th_cmp_reg invalid parameters\n");
}

static int th_cmp_imm (uint32_t rn, uint32_t imm)
{
	if (rn <= 7 && imm <= 0x00ff)
	{
		ot(0x2800 | (rn << 8) | imm); // cmp immediate enc t1
		return 2;
	}
	else
	{
		uint32_t enc = th_packimm_thumb (imm);

		if (enc || imm == 0)
		{
			uint32_t a = enc >> 16;
			uint32_t b = enc & 0x0000ffff;
			ot(0xf1b0 | rn | a);
			ot(0x0f00 | b);
			return 4;
		}
	}

	return 0;
}

static void th_lsl_reg (uint32_t rd, uint32_t rn, uint32_t rm)
{
	if (rd == rn && rm <= 7 && rn <= 7)
		ot(0x4080 | (rm << 3) | rd);			// Encoding t1
	else if (rd != 13 && rd != 15 && rn != 13 && rn != 15 && rm != 13 && rm != 15)
	{
		ot(0xfa00 | rn);
		ot(0xf000 | (rd << 8) | rm);			// Encoding t2
	}
	else
		tcc_error ("internal error: th_lsl_reg invalid parameters\n");
}

static void th_lsl_imm (uint32_t rd, uint32_t rm, uint32_t imm)
{
	if (rm <= 7 && rd <= 7)
		ot(0x0000 | (imm << 6) | (rm << 3) | rd);	// Encoding t1
	else
	{
		uint32_t imm3 = (imm >> 2) & 7;			// Encoding t2
		ot(0xea4f);
		ot((imm3 << 12) | (rd << 8) | ((imm & 3) << 6) | rm);
	}
}

static void th_lsr_imm (uint32_t rd, uint32_t rm, uint32_t imm5)
{
	if (rm <= 7 && rd <= 7)
		ot(0x0800 | (imm5 << 6) | (rm << 3) | rd);	// Encoding t1
	else
		tcc_error ("internal error: th_lsr_imm invalid parameters\n");
}

static void th_asr_reg (uint32_t rd, uint32_t rn, uint32_t rm)
{
	if (rd == rn && rm <= 7 && rn <= 7)
		ot(0x4100 | (rm << 3) | rd);			// Encoding t1
	else if (rd != 13 && rd != 15 && rn != 13 && rn != 15 && rm != 13 && rm != 15)
	{
		ot(0xfa40 | rn);
		ot(0xf000 | (rd << 8) | rm);			// Encoding t2
	}
	else
		tcc_error ("internal error: th_asr_reg invalid parameters\n");
}

static void th_asr_imm (uint32_t rd, uint32_t rm, uint32_t imm)
{
	if (rm <= 7 && rd <= 7)
		ot(0x1000 | (imm << 6) | (rm << 3) | rd);	// Encoding t1
	else
	{
		uint32_t imm3 = (imm >> 2) & 7;			// Encoding t2
		ot(0xea4f);
		ot((imm3 << 12) | (rd << 8) | ((imm & 3) << 6) | (2 << 4) | rm);
	}
}

static void th_mul (uint32_t rd, uint32_t rn, uint32_t rm)
{
	if (rd == rm && rd <= 7 && rn <= 7)
		ot(0x4340 | (rn << 3) | rm); // mul encoding t1
	else
	{
		ot(0xfb00 | rn);
		ot(0xf000 | (rd << 8) | rm); // mul encoding t2
	}
}

static void th_umull (uint32_t rdlo, uint32_t rdhi, uint32_t rn, uint32_t rm)
{
	ot(0xfba0 | rn);
	ot((rdlo << 12) | (rdhi << 8) | rm);
}

static void th_b_t1 (uint32_t op, uint32_t imm)
{
	ot(0xd000 | (op << 8) | imm);
}

static void th_b_t2 (int32_t imm)
{
	int32_t i = imm >> 1;

	if (i < 1023 && i > -1024 && !(imm&1))
		ot(0xe000 | (i & 0x7ff));
	else
		tcc_error ("internal error: th_b_t2 branch is out of range %i", imm);
}

/*
 * Branch instruction, possibly conditional.
 *
 *      10 9876543210 9876543210 9876543210
 * IMM:             s 21bbbbbbaa aaaaaaaaa0
 * IMM:               s21bbbbbba aaaaaaaaaa
 */
static void th_b_t3 (uint32_t op, uint32_t imm)
{
	uint32_t enc = th_encbranch_b_t3 (imm);

	ot(0xf000 | (op << 6) | (enc >> 16));
	ot(0x8000 | enc);
}

// Uncoditional
static void th_b_t4 (int32_t imm)
{
	uint32_t packed = 0;

	if (imm > 16777215 || imm < -16777215)
		tcc_error ("internal error: th_b_t4 branch is out of range");

	packed = th_packimm_10_11_0 (imm) | 0xF0009000;
	ot(packed >> 16);
	ot(packed);
}

static void th_bl_t1 (uint32_t imm)
{
	uint32_t packed = th_packimm_10_11_0 (imm) | 0xF000D000;
	ot(packed >> 16);
	ot(packed);
}

/*
 * Only has one encoding
 */
static void th_blx_reg (uint32_t rm)
{
	ot(0x4780 | (rm << 3));
}

/*
 * Only has one encoding
 */
static void th_bx_reg (uint32_t rm)
{
	ot(0x4700 | (rm << 3));
}

static int th_str_imm (uint32_t rt, uint32_t rn, uint32_t imm, uint32_t puw)
{
	if (puw == 6 && rn <= 7 && rt <= 7 && imm <= 0x1f)
		return ot(0x6000 | (imm << 6) | (rn << 3) | rt);// Encoding T1
	else if (puw == 6 && imm <= 0x0fff)
	{
		ot(0xf8c0 | rn);				// Encoding T3
		ot((rt << 12) | imm);
		return 4;
	}
	else if (imm <= 0x0ff)
	{
		ot(0xf840 | rn);				// Encoding T4
		ot(0x0800 | (rt << 12) | (puw << 8) | imm);
		return 4;
	}
	else
		return 0;
}

static int th_str_reg (uint32_t rt, uint32_t rn, uint32_t rm)
{
	if (rm <= 7 && rn <= 7 && rt <= 7)
		return ot(0x5000 | (rm << 6) | (rn << 3) | rt);	// Encoding T1
	else if (rm != 13 && rm != 15)
	{
		ot(0xf840 | rn);
		ot((rt << 12) | rm);				// Encoding T2
		return 4;
	}
	else
		return 0;
}

static int th_strb_imm (uint32_t rt, uint32_t rn, uint32_t imm, uint32_t puw)
{
	if (puw == 6 && rn <= 7 && rt <= 7 && imm <= 0x1f)
		return ot(0x7000 | (imm << 6) | (rn << 3) | rt);// Encoding T1
	else if (puw == 6 && imm <= 0x0fff)
	{
		ot(0xf880 | rn);				// Encoding T2
		ot((rt << 12) | imm);
		return 4;
	}
	else if (imm <= 0x0ff)
	{
		ot(0xf800 | rn);				// Encoding T3
		ot(0x0800 | (rt << 12) | (puw << 8) | imm);
		return 4;
	}
	else
		return 0;
}

static int th_strb_reg (uint32_t rt, uint32_t rn, uint32_t rm)
{
	if (rm <= 7 && rn <= 7 && rt <= 7)
		return ot(0x5400 | (rm << 6) | (rn << 3) | rt);	// Encoding T1
	else if (rm != 13 && rm != 15)
	{
		ot(0xf800 | rn);
		ot((rt << 12) | rm);				// Encoding T2
		return 4;
	}
	else
		return 0;
}

static int th_strh_imm (uint32_t rt, uint32_t rn, uint32_t imm, uint32_t puw)
{
	if (puw == 6 && rn <= 7 && rt <= 7 && imm <= 0x1f)
		return ot(0x8000 | (imm << 6) | (rn << 3) | rt);// Encoding T1
	else if (puw == 6 && imm <= 0x0fff)
	{
		ot(0xf8a0 | rn);				// Encoding T2
		ot((rt << 12) | imm);
		return 4;
	}
	else if (imm <= 0x0ff)
	{
		ot(0xf820 | rn);				// Encoding T3
		ot(0x0800 | (rt << 12) | (puw << 8) | imm);
		return 4;
	}
	else
		return 0;
}

static int th_strh_reg (uint32_t rt, uint32_t rn, uint32_t rm)
{
	if (rm <= 7 && rn <= 7 && rt <= 7)
		return ot(0x5200 | (rm << 6) | (rn << 3) | rt);	// Encoding T1
	else if (rm != 13 && rm != 15)
	{
		ot(0xf820 | rn);
		ot((rt << 12) | rm);				// Encoding T2
		return 4;
	}
	else
		return 0;
}

static int th_ldr_imm (uint32_t rt, uint32_t rn, uint32_t imm, uint32_t puw)
{
	if (puw == 0x6 && rn <= 7 && rt <= 7 && !(imm & 0xffffff83))
		return ot(0x6800 | (imm << 4) | (rn << 3) | rt);// Encoding T1
	else if (puw == 0x6 && rn == 13 && rt <= 7 && !(imm & 0xfffffc03))
		return ot(0x9800 | (rt << 8) | (imm >> 2));	// Encoding T2
	else if (puw == 0x6 && imm <= 0x0fff)
	{
		ot(0xf8d0 | rn);
		ot((rt << 12) | imm);				// Encoding T3
		return 4;
	}
	else if (imm <= 0x0ff)
	{
		ot(0xf850 | rn);
		ot(0x0800 | (rt << 12) | (puw << 8) | imm);	// Encoding T4
		return 4;
	}

	return 0;
}

static void th_ldr_literal (uint32_t rt, uint32_t imm)
{
	if (rt <= 7 && !(imm & 3) && imm <= 0x3ff)
	{
		imm = (imm >> 2) & 0x0ff;		// Encoding T1
		ot(0x4800 | (rt << 8) | imm);
	}
	else if (imm <= 0x0fff)
	{
		ot(0xf8df);
		ot((rt << 12) | imm);
	}
	else
		tcc_error ("internal error: th_ldr_literal invalid parameters\n");
}

/*
 * Estimate if size of generated instruction will be 4, 2 or not possible.
 */
static int th_ldr_literal_estimate (uint32_t rt, uint32_t imm)
{
	if (rt <= 7 && !(imm & 3) && imm <= 0x3ff)
		return 2;
	else if (imm <= 0x0fff)
		return 4;
	else
		return 0;
}

static int th_ldr_reg (uint32_t rt, uint32_t rn, uint32_t rm)
{
	if (rm <= 7 && rn <= 7 && rt <= 7)
		return ot(0x5800 | (rm << 6) | (rn << 3) | rt);	// Encoding T1
	else if (rm != 13 && rm != 15)
	{
		ot(0xf850 | rn);
		ot((rt << 12) | rm);				// Encoding T2
		return 4;
	}
	else
		return 0;
}

static int th_ldrb_imm (uint32_t rt, uint32_t rn, uint32_t imm8, uint32_t puw)
{
	if (puw == 6 && rn <= 7 && rt <= 7 && imm8 <= 0x1f)
	{
		// Encoding T1
		return ot(0x7800 | (imm8 << 6) | (rn << 3) | rt);
	}
	else if (puw == 6 && rt != 13 && imm8 <= 0xfff)
	{
		// Encoding T2
		ot(0xf890 | rn);
		ot((rt << 12) | imm8);
		return 4;
	}
	else if (rt != 13 && imm8 <= 0x0ff)
	{
		// Encoding T3
		ot(0xf810 | rn);
		ot(0x0800 | (rt << 12) | (puw << 8) | imm8);
		return 4;
	}
	else
		return 0;
}

static int th_ldrb_reg (uint32_t rt, uint32_t rn, uint32_t rm)
{
	if (rt <= 7 && rn <= 7 && rm <= 7)
		return ot(0x5c00 | (rm << 6) | (rn << 3) | rt);	// Encoding t1
	else if (rt != 13 && rm != 13 && rm != 15)
	{
		ot(0xf810 | rn);
		ot((rt << 12) | rm);				// Encoding t2
		return 4;
	}
	else
		return 0;
}

static int th_ldrsb_imm (uint32_t rt, uint32_t rn, uint32_t imm12, uint32_t puw)
{
	if (puw == 6 && rt != 13 && imm12 <= 0x0fff)
	{
		// Encoding T1
		ot(0xf990 | rn);
		ot(0x0000 | (rt << 12) | imm12);
		return 4;
	}
	else if (rt != 13 && imm12 <= 0x0ff)
	{
		// Encoding T2
		ot(0xf910 | rn);
		ot(0x0800 | (rt << 12) | (puw << 8) | imm12);
		return 4;
	}
	else
		return 0;
}

static int th_ldrsb_reg (uint32_t rt, uint32_t rn, uint32_t rm)
{
	if (rt <= 7 && rn <= 7 && rm <= 7)
		return ot(0x5600 | (rm << 6) | (rn << 3) | rt);	// Encoding t1
	else if (rt != 13 && rm != 13 && rm != 15)
	{
		ot(0xf910 | rn);
		ot((rt << 12) | rm);				// Encoding t2
		return 4;
	}
	else
		return 0;
}

static int th_ldrsh_imm (uint32_t rt, uint32_t rn, uint32_t imm12, uint32_t puw)
{
	if (puw == 6 && rt != 13 && imm12 <= 0x0fff)
	{
		ot(0xf9b0 | rn);	// Encoding t1
		ot((rt << 12) | imm12);
		return 4;
	}
	else if (rt != 13 && imm12 <= 0x0ff)
	{
		ot(0xf930 | rn);	// Encoding t2
		ot(0x0800 | (rt << 12) | (puw << 8) | imm12);
		return 4;
	}
}

static int th_ldrsh_reg (uint32_t rt, uint32_t rn, uint32_t rm)
{
	if (rt <= 7 && rn <= 7 && rm <= 7)
		return ot(0x5e00 | (rm << 6) | (rn << 3) | rt);	// Encoding t1
	else if (rt != 13 && rm != 13 && rm != 15)
	{
		ot(0xf930 | rn);
		ot((rt << 12) | rm);				// Encoding t2
		return 4;
	}
	else
		return 0;
}

static int th_ldrh_imm (uint32_t rt, uint32_t rn, uint32_t imm12, uint32_t puw)
{
	if (puw == 6 && rn <= 7 && rt <= 7 && imm12 <= 0x3f && !(imm12 & 1))
		return ot(0x8800 | (imm12 << 5) | (rn << 3) | rt);// Encoding t1
	else if (puw == 6 && rt != 13)
	{
		ot(0xf8b0 | rn);	// Encoding t2
		ot((rt << 12) | imm12);
		return 4;
	}
	else if (rt != 13)
	{
		ot(0xf830 | rn);	// Encoding t3
		ot(0x0800 | (rt << 12) | (puw << 8) | imm12);
		return 4;
	}
	else
		return 0;
}

static int th_ldrh_reg (uint32_t rt, uint32_t rn, uint32_t rm)
{
	if (rt <= 7 && rn <= 7 && rm <= 7)
		return ot(0x5a00 | (rm << 6) | (rn << 3) | rt);	// Encoding t1
	else if (rt != 13 && rm != 13 && rm != 15)
	{
		ot(0xf830 | rn);
		ot((rt << 12) | rm);				// Encoding t2
		return 4;
	}
	else
		return 0;
}

static void th_push (uint32_t regs)
{
	if (!(regs & 0xffffff00))
		ot(0xb400 | regs);				// Encoding t1
	else
	{
		ot(0xe92d);						// Encoding t2
		ot(regs);
	}
}

static void th_pop (uint32_t regs)
{
	if (!(regs & 0xffffff00))
		ot(0xbc00 | regs);				// Encoding t1
	else
	{
		ot(0xe8bd);						// Encoding t2
		ot(regs);
	}
}

static void th_sdiv (uint32_t rd, uint32_t rn, uint32_t rm)
{
	ot (0xfb90 | rn);
	ot (0xf0f0 | (rd << 8) | rm);
}

static void th_udiv (uint32_t rd, uint32_t rn, uint32_t rm)
{
	ot (0xfbb0 | rn);
	ot (0xf0f0 | (rd << 8) | rm);
}

///////////////////////////////////////////////////////////////////////////////
// Fractions used in load, store, branch, operations etc.
///////////////////////////////////////////////////////////////////////////////
static uint32_t intr (int r);
static uint32_t mapcc (int cc);
void load_vt_lval_vt_local_float (int r, SValue *sv, int ft, int fc, int sign, uint32_t base);

/*
 * Used when a literal must be stored as a full 32 bit word. If sym is not
 * null it will emit a relocation.
 */
void load_full_const (int r, uint32_t imm, struct Sym *sym)
{
	int est = 0;

	DBG1(printf (GRN"load_full_const\n"CLR));

	// Estimate the alignment of the constant: space for load and branch instruction
	est = th_ldr_literal_estimate (r, 4);
	est += 4;
	est += ind;

	// Large literal, constant has to be 4-byte aligned
	if (est & 3)
		ot(0xbf00);

	th_ldr_literal (r, 4);
	th_b_t4 (4);

	// Write data with $d symbol
	if (sym)
		greloc(cur_text_section, sym, ind, R_ARM_ABS32);

	th_sym_d ();
	ott (imm);
	th_sym_t ();
}

/*
 * In some cases the offset is out of range for ldr/str instructions, so the
 * offset must be loaded into a register instead.
 */
static int th_offset_to_reg (int off, int sign)
{
	// Get a new register
	int rr = get_reg (RC_INT);

	// Try mov first
	int ok = th_mov_imm (rr, off, 2);

	if (!ok)
	{
		//tcc_error ("internal error: th_offset_to_reg failed for mov (off %i sign %i)",
		//	off, sign);
		load_full_const (rr, sign ? -off : off, 0);
		return rr;
	}

	// No PUW flags for thumb. Add rsb instruction (0 - whatever)
	if (sign)
		th_rsb_imm (rr, rr, 0, 1);

	return rr;
}

void load_vt_lval_vt_local (int r, SValue *sv, int ft, int fc, int sign, uint32_t base)
{
	int ok = 0;
	int btype = ft & VT_BTYPE;
	int ir = intr (r);

	if (is_float(ft))
	{
		return load_vt_lval_vt_local_float (r, sv, ft, fc, sign, base);
	}
	else if (btype == VT_SHORT)
	{
		DBG1(printf (GRN"load short sign %i r %i base %i fc %i\n"CLR,
			sign, ir, base, fc));

		if (!(ft & VT_UNSIGNED))
			ok = th_ldrsh_imm (ir, base, fc, sign ? 4 : 6);
		else
			ok = th_ldrh_imm (ir, base, fc, sign ? 4 : 6);
	}
	else if (btype == VT_BYTE || btype == VT_BOOL)
	{
		DBG1(printf (GRN"load byte/bool sign %i r %i base %i fc %i\n"CLR,
			sign, ir, base, fc));

		if (!(ft & VT_UNSIGNED))
			ok = th_ldrsb_imm (ir, base, fc, sign ? 4 : 6);
		else
			ok = th_ldrb_imm (ir, base, fc, sign ? 4 : 6);
	}
	else
	{
		DBG1(printf (GRN"load int/byte/bool sign %i r %i base %i fc %i\n"CLR,
			sign, ir, base, fc));

		ok = th_ldr_imm (ir, base, fc, sign ? 4 : 6);
	}

	if (!ok)
	{
		int rr = th_offset_to_reg (fc, sign);

		if (btype == VT_SHORT)
		{
			if (ft & VT_UNSIGNED)
				ok = th_ldrh_reg (ir, base, rr);
			else
				ok = th_ldrsh_reg (ir, base, rr);
		}
		else if (btype == VT_BYTE || btype == VT_BOOL)
		{
			if (ft & VT_UNSIGNED)
				ok = th_ldrb_reg (ir, base, rr);
			else
				ok = th_ldrsb_reg (ir, base, rr);
		}
		else
		{
			ok = th_ldr_reg (ir, base, rr);
		}

		if (!ok)
			tcc_error ("internal error: bad ldr instruction attempted");
	}
}

void load_vt_const (int r, SValue *sv)
{
	int est = 0;
	r = intr (r);

	DBG1(printf (GRN"load_vt_const r %i const %i sym %i\n"CLR,
		r, (int)sv->c.i, (sv->r & VT_SYM) == VT_SYM));

	if (sv->r & VT_SYM)
	{
		load_full_const (r, sv->c.i, sv->sym);
		return;
	}
	else
	{
		int ok = th_mov_imm (r, sv->c.i, g_setflags ? 2 : 0);

		// Fallback for when const cant fit in an instruction
		if (!ok)
			load_full_const (r, sv->c.i, 0);
	}
}

/*
 * Subtract to some offset on the stack.
 */
void load_vt_local (int r, SValue *sv)
{
	//r = intr(r);

	// If its a symbol or cant fit in the usual sub_imm instruction
	if (sv->r & VT_SYM || (-sv->c.i) >= 0x0fff)
	{
		// Load the constant using 32-bit word and add to fp
		DBG1(printf (GRN"load_vt_local\n"CLR));
		load_full_const (r, sv->c.i, sv->r & VT_SYM ? sv->sym : 0);
		th_add_reg (r, 11, r);
	}
	else
	{
		// fp (r11) is hardcoded
		DBG1(printf (GRN"load_vt_local sub %i\n"CLR, (int)-sv->c.i));

		if (!th_sub_imm (r, 11, -sv->c.i))
			tcc_error ("internal error: load_vt_local th_sub_imm th_packimm_thumb failed\n");
	}
}

void load_vt_cmp (int r, SValue *sv)
{
	uint32_t firstcond = 0;
	uint32_t rr = intr (r);

	DBG1(printf (GRN"load_vt_cmp\n"CLR));

	if (rr == 13 || rr == 15)
		tcc_error ("internal error: load_vt_cmp register out of range (%i)", rr);

	// An it block with two mov
	firstcond = mapcc (sv->c.i);
	ot(0xbf00 | (firstcond <<  4) | 0x4 | ((~firstcond & 1) << 3));

	if (rr <= 7)
	{
		// Encoding T1
		ot(0x2001 | (rr << 8));
		ot(0x2000 | (rr << 8));
	}
	else
	{
		// Encoding T3
		ot(0xf240);
		ot(0x0001 | (rr << 8));

		ot(0xf240);
		ot(0x0000 | (rr << 8));
	}
}

/*
 * From the description in tcc.h:
 * VT_JMP: Value is the consequence of jmp true
 * VT_JMPI: Value is the consequence of jmp false
 *
 */
void load_vt_jmp_jmpi (int r, SValue *sv)
{
	DBG1(printf (GRN"load_vt_jmp_jmpi\n"CLR));
	th_mov_imm (intr(r), sv->r & 1, 2);
	th_b_t4 (2);			// Should branch over one instruction
	gsym(sv->c.i);
	th_mov_imm (intr(r), (sv->r^1) & 1, 2);
}

// Operation with two registers
void gen_opi_regular_reg (int opc, int c)
{
	int oldflags = g_setflags;
	int fr = 0;
	int r = 0;

	// Can't mangle carry flag when emulating rsc
	if (opc == 0xe)
		g_setflags = 0;

	fr = intr(gv(RC_INT));
	r = intr(vtop[-1].r=get_reg_ex(RC_INT,two2mask(vtop->r,vtop[-1].r)));
	g_setflags = oldflags;

	if (opc == 8 || opc == 9) // 9 = carry generation
		th_add_reg (r, c, fr);
	else if (opc == 0xa)
		th_adc_reg (r, c, fr);
	else if (opc == 4 || opc == 5) // 5 = carry generation
		th_sub_reg (r, c, fr);
	else if (opc == 6 || opc == 7)
		th_rsb_reg (r, c, fr);
	else if (opc == 0xc)
		th_sbc_reg (r, c, fr);
	else if (opc == 0)
		th_and_reg (r, c, fr);
	else if (opc == 2)
		th_xor_reg (r, c, fr);
	else if (opc == 0x18)
		th_orr_reg (r, c, fr);
	else if (opc == 0xe)
	{
		// GCC Emulates rsc with a neg and sbc
		th_sbc_reg (r, fr, c);
	}
	else if (opc == 0x15)
		th_cmp_reg (c, fr);
	else
		tcc_error ("internal error: gen_opi_regular unhandled case opc %i c %i r %i fr %i\n", opc, c, r, fr);
}

void gen_opi_regular (int opc, int c)
{
	// Operation with register and const
	if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
	{
		int ok = 0;
		int r = intr(vtop[-1].r=get_reg_ex(RC_INT,regmask(vtop[-1].r)));

		DBG1(printf ("  gen_opi_regular r %i c %i val %x (0x%x) opc %x\n",
			r, c, (int)vtop->c.i, (int)vtop->c.i, opc));

		if (opc != 0x15 && r != c)
			tcc_error ("internal error: check order of r and c");


		if (opc == 0x15)
			ok = th_cmp_imm (c, vtop->c.i);
		else if (opc == 8 || opc == 9) // 9 = carry generation
			ok = th_add_imm (r, r, vtop->c.i);
		else if (opc == 0xa)
			ok = th_adc_imm (r, r, vtop->c.i);
		else if (opc == 4 || opc == 5) // 5 = carry generation
			ok = th_sub_imm (r, r, vtop->c.i);
		else if (opc == 6 || opc == 7)
			ok = th_rsb_imm (r, r, vtop->c.i, 1);
		else if (opc == 0xc)
			ok = th_sbc_imm (r, r, vtop->c.i);
		else if (opc == 0)
			ok = th_and_imm (r, r, vtop->c.i);
		else if (opc == 2)
			ok = th_xor_imm (r, r, vtop->c.i);
		else if (opc == 0x18)
			ok = th_orr_imm (r, r, vtop->c.i);
		else if (opc == 0xe)
		{
			// Fall through to operation with registers below
		}
		else
			tcc_error ("internal error: gen_opi_regular unhandled case opc 0x%x\n", opc);

		if (ok)
			return;
	}

	return gen_opi_regular_reg (opc, c);
}

/*
 * Decode branch for instruction at pos.
 * Returns where it would branch to.
 */
int decbranch (int pos)
{
	int xa = *(uint16_t *)(cur_text_section->data + pos);
	int xb = *(uint16_t *)(cur_text_section->data + pos + 2);

	DBG1(printf ("  decbranch ins at pos 0x%.8x, target inst 0x%x\n", pos, xa));

	if ((xa & 0xf000) == 0xd000)
	{
		// Branch encoding t1
		xa &= 0x00ff;
		if (xa & 0x0080) xa -= 0x100;
		xa = (xa*2) + pos + 4;
	}
	else if ((xa & 0xf800) == 0xe000)
	{
		// Branch encoding t2
		xa &= 0x7ff;
		if(xa & 0x400) xa -= 0x800;
		xa = (xa*2) + pos + 4;
	}
	else if ((xa & 0xf800) == 0xf000 && (xb & 0xd000) == 0x8000)
	{
		// Branch encoding t3
		uint32_t s = (xa >> 10) & 1;
		uint32_t imm6 = (xa & 0x3f);
		uint32_t j1 = (xb >> 13) & 1;
		uint32_t j2 = (xb >> 11) & 1;
		uint32_t imm11 = xb & 0x7ff;

		//      10 9876543210 9876543210 9876543210
		// IMM:             s 21bbbbbbaa aaaaaaaaa0
		// IMM:               s21bbbbbba aaaaaaaaaa
		uint32_t ret = (j2 << 19) | (j1 << 18) | (imm6 << 12) | (imm11 << 1);
		if (s) ret |= 0xfff00000;

		xa = ret + pos + 4;
	}
	else if ((xa & 0xf800) == 0xf000 && (xb & 0xd000) == 0x9000)
	{
		// Branch encoding t4
		uint32_t s = (xa >> 10) & 1;
		uint32_t imm10 = (xa & 0x3ff);
		uint32_t j1 = (xb >> 13) & 1;
		uint32_t j2 = (xb >> 11) & 1;
		uint32_t imm11 = xb & 0x7ff;

		uint32_t i1 = ~(j1 ^ s) & 1;
		uint32_t i2 = ~(j2 ^ s) & 1;

		//      10 9876543210 9876543210 9876543210
		// IMM:         s21bb bbbbbbbbaa aaaaaaaaa0
		uint32_t ret = (i2 << 23) | (i1 << 22) | (imm10 << 12) | (imm11 << 1);
		if (s) ret |= 0xff000000;

		xa = ret + pos + 4;
	}
	else
	{
		tcc_error ("internal error: decbranch unknown encoding pos 0x%x\n", pos);
		return 0;
	}

	DBG1(printf ("  decbranch ins at pos 0x%.8x, target 0x%x\n", pos, xa));
	return xa;
}

int th_patch_call (int t, int a)
{
	uint16_t *x = (uint16_t *)(cur_text_section->data + t);
	int lt = t;
	t = decbranch(t);

	if(a == lt+2)
		*x = 0xbf00;
	else if ((*x & 0xf000) == 0xd000)
	{
		// Branch encoding t1
		*x &= 0xff00;
		*x |= th_encbranch_8 (lt, a);
	}
	else if ((*x & 0xf800) == 0xe000)
	{
		// Branch encoding t2
		*x &= 0xf800;
		*x |= th_encbranch_11 (lt, a);
	}
	else if ((x[0] & 0xf800) == 0xf000 && (x[1] & 0xd000) == 0x8000)
	{
		// Branch encoding t3
		uint32_t enc = 0;
		x[0] &= 0xfbc0;
		x[1] &= 0xd000;
		enc = th_encbranch_b_t3 (th_encbranch_20 (lt, a));
		x[0] |= enc >> 16;
		x[1] |= enc;
	}
	else if ((x[0] & 0xf800) == 0xf000 && (x[1] & 0xd000) == 0x9000)
	{
		// Branch encoding t4
		uint32_t enc = 0;
		x[0] &= 0xf800;
		x[1] &= 0xd000;
		enc = th_packimm_10_11_0 (th_encbranch_20 (lt, a) << 1);
		x[0] |= enc >> 16;
		x[1] |= enc;
	}
	else
		tcc_error ("internal error: unhandled branch type in th_patch_call");

	return t;
}

///////////////////////////////////////////////////////////////////////////////
// Float stuff
///////////////////////////////////////////////////////////////////////////////
static uint32_t vfpr(int r);

//
void th_vpush (uint32_t regs)
{
	ot(0xed2d);
	ot(0x0a00 | regs);
}

void th_vpop (uint32_t regs)
{
	ot(0xecbd | (regs >> 16));
	ot(0x0a00 | regs);
}

void th_vldr (uint32_t rn, uint32_t vd, uint32_t add, uint32_t dbl, uint32_t imm)
{
	if (dbl)
	{
		ot(0xed10 | (add << 7) | rn);
		ot(0x0b00 | (vd << 12) | (imm >> 2));
	}
	else
	{
		ot(0xed10 | (add << 7) | rn);
		ot(0x0a00 | (vd << 12) | (imm >> 2));
	}
}

void load_vt_lval_vt_local_float (int r, SValue *sv, int ft, int fc, int sign, uint32_t base)
{
	// TODO: Check imm range
	if ((ft & VT_BTYPE) != VT_FLOAT)
		th_vldr (base, vfpr(r), !sign, 1, fc);
	else
		th_vldr (base, vfpr(r), !sign, 0, fc);
}

int th_vstr (uint32_t rn, uint32_t vd, uint32_t add, uint32_t dbl, uint32_t imm)
{
	// TODO: Check imm range
	if (dbl)
	{
		ot(0xed00 | (add << 7) | rn);
		ot(0x0b00 | (vd << 12) | (imm >> 2));
		return 4;
	}
	else
	{
		ot(0xed00 | (add << 7) | rn);
		ot(0x0a00 | (vd << 12) | (imm >> 2));
		return 4;
	}
}

void th_vcvt_float_to_double (uint32_t vd, uint32_t vm)
{
	ot(0xeeb7);
	ot(0x0ac0 | (vd << 12) | vm);
}

void th_vcvt_double_to_float (uint32_t vd, uint32_t vm)
{
	ot(0xeeb7);
	ot(0x0bc0 | (vd << 12) | vm);
}

void th_vcvt_fp_int (uint32_t vd, uint32_t vm, uint32_t opc, uint32_t sz, uint32_t op)
{
	ot(0xeeb8 | opc);
	ot(0x0a40 | (vd << 12) | (sz << 8) | (op << 7) | vm);
}

void th_vmov_armcore_sp (uint32_t rt, uint32_t sn, uint32_t to_arm_register)
{
	ot(0xee00 | (to_arm_register << 4) | (sn & 0x0f));
	ot(0x0a10 | (rt << 12) | ((sn&0x10) << 3));
}

void th_vmov_armcore_scalar (uint32_t rt, uint32_t vd, uint32_t opc)
{
	ot(0xee00 | ((opc & 0xc) << 3) | (vd & 0x0f));
	ot(0x0b10 | (rt << 12) | ((vd & 0x10) << 3) | ((opc & 0x3) << 5));
}

void th_vmov_scalar_armcore (uint32_t rt, uint32_t vd, uint32_t opc)
{
	ot(0xee10 | ((opc & 0xc) << 3) | (vd & 0x0f));
	ot(0x0b10 | (rt << 12) | ((vd & 0x10) << 3) | ((opc & 0x3) << 5));
}

void th_vmov_register (uint32_t vd, uint32_t vm, uint32_t sz)
{
	if (!sz)
	{
		uint32_t d = vd & 1;
		uint32_t m = vm & 1;
		vd = vd >> 1;
		vm = vm >> 1;
		ot(0xeeb0 | (d << 6));
		ot(0x0a40 | (vd << 12) | (sz << 8) | (m << 5) | vm);
	}
	else
		tcc_error ("internal error: th_vmov_register\n");
}

void th_vmrs (uint32_t rt)
{
	ot(0xeef1);
	ot(0x0a10 | (rt << 12));
}

/* vim: set ts=4 sw=4 sts=4 tw=80 :*/
