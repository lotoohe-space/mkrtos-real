#pragma once
#include "u_types.h"
#include "guest_os.h"
enum vmm_devemu_endianness
{
    VMM_DEVEMU_UNKNOWN_ENDIAN = 0,
    VMM_DEVEMU_NATIVE_ENDIAN = 1,
    VMM_DEVEMU_LITTLE_ENDIAN = 2,
    VMM_DEVEMU_BIG_ENDIAN = 3,
    VMM_DEVEMU_MAX_ENDIAN = 4,
};
int vmm_devemu_emulate_read(guest_vcpu_t *vcpu,
                            paddr_t gphys_addr,
                            void *dst, uint32_t dst_len,
                            enum vmm_devemu_endianness dst_endian);

int vmm_devemu_emulate_write(guest_vcpu_t *vcpu,
                             paddr_t gphys_addr,
                             void *src, uint32_t src_len,
                             enum vmm_devemu_endianness src_endian);