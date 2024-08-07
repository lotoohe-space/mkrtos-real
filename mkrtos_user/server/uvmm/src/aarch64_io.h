#pragma once
#include "u_types.h"
#define rev16(val) ({ uint16_t rval; asm volatile(\
				" rev16   %0, %1\n\t" : "=r" (rval) : \
				"r" (val) : "memory", "cc"); rval; })

#define rev32(val) ({ uint32_t rval; asm volatile(\
				" rev32    %0, %1\n\t" : "=r" (rval) : \
				"r" (val) : "memory", "cc"); rval; })

#define rev64(val) ({ uint32_t d1, d2; \
				d1 = (uint32_t)((uint64_t)val >> 32); d2 = (uint32_t)val; \
				d1 = rev32(d1); d2 = rev32(d2); \
				(((uint64_t)d2 << 32) | ((uint64_t)d1)); })
/*
 * Endianness primitives
 * ------------------------
 */
#define arch_cpu_to_le16(v) (v)

#define arch_le16_to_cpu(v) (v)

#define arch_cpu_to_be16(v) rev16(v)

#define arch_be16_to_cpu(v) rev16(v)

#define arch_cpu_to_le32(v) (v)

#define arch_le32_to_cpu(v) (v)

#define arch_cpu_to_be32(v) rev32(v)

#define arch_be32_to_cpu(v) rev32(v)

#define arch_cpu_to_le64(v) (v)

#define arch_le64_to_cpu(v) (v)

#define arch_cpu_to_be64(v) rev64(v)

#define arch_be64_to_cpu(v) rev64(v)

/** Endianness related helper macros */
#define vmm_cpu_to_le16(data) arch_cpu_to_le16(data)

#define vmm_le16_to_cpu(data) arch_le16_to_cpu(data)

#define vmm_cpu_to_be16(data) arch_cpu_to_be16(data)

#define vmm_be16_to_cpu(data) arch_be16_to_cpu(data)

#define vmm_cpu_to_le32(data) arch_cpu_to_le32(data)

#define vmm_le32_to_cpu(data) arch_le32_to_cpu(data)

#define vmm_cpu_to_be32(data) arch_cpu_to_be32(data)

#define vmm_be32_to_cpu(data) arch_be32_to_cpu(data)

#define vmm_cpu_to_le64(data) arch_cpu_to_le64(data)

#define vmm_le64_to_cpu(data) arch_le64_to_cpu(data)

#define vmm_cpu_to_be64(data) arch_cpu_to_be64(data)

#define vmm_be64_to_cpu(data) arch_be64_to_cpu(data)