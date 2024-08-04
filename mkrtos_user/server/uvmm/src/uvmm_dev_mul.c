#include "u_types.h"
#include "uvmm_dev_mul.h"
#include "guest_os.h"
#include "aarch64_io.h"

#include <errno.h>
#include <stdio.h>
#include <errno.h>

static int devemu_doread(vmm_dev_t *edev,
                         paddr_t offset,
                         void *dst, uint32_t dst_len,
                         enum vmm_devemu_endianness dst_endian)
{
    int rc;
    uint16_t data16;
    uint32_t data32;
    uint64_t data64;
    enum vmm_devemu_endianness data_endian;

    if (!edev ||
        (dst_endian <= VMM_DEVEMU_UNKNOWN_ENDIAN) ||
        (VMM_DEVEMU_MAX_ENDIAN <= dst_endian))
    {
        return -EFAULT;
    }

    switch (dst_len)
    {
    case 1:
        if (edev->dev_operations->read_u8)
        {
            rc = edev->dev_operations->read_u8(edev, offset, dst);
            // debug_read(edev, offset, sizeof(uint8_t), *((uint8_t *)dst));
        }
        else
        {
            printf("%s: edev=%s does not have read_u8()\n",
                   __func__, edev->dev_operations->name);
            rc = -ENOENT;
        }
        break;
    case 2:
        if (edev->dev_operations->read_u16)
        {
            rc = edev->dev_operations->read_u16(edev, offset, &data16);
            // debug_read(edev, offset, sizeof(uint16_t), data16);
        }
        else
        {
            printf("%s: edev=%s does not have read_u16()\n",
                   __func__, edev->dev_operations->name);
            rc = -ENOENT;
        }
        if (!rc)
        {
            // switch (edev->endian)TODO:
            // {
            // case VMM_DEVEMU_LITTLE_ENDIAN:
            data16 = vmm_cpu_to_le16(data16);
            data_endian = VMM_DEVEMU_LITTLE_ENDIAN;
            //     break;
            // case VMM_DEVEMU_BIG_ENDIAN:
            //     data16 = vmm_cpu_to_be16(data16);
            //     data_endian = VMM_DEVEMU_BIG_ENDIAN;
            //     break;
            // default:
            //     data_endian = VMM_DEVEMU_NATIVE_ENDIAN;
            //     break;
            // };
            if (data_endian != dst_endian)
            {
                switch (dst_endian)
                {
                case VMM_DEVEMU_LITTLE_ENDIAN:
                    data16 = vmm_cpu_to_le16(data16);
                    break;
                case VMM_DEVEMU_BIG_ENDIAN:
                    data16 = vmm_cpu_to_be16(data16);
                    break;
                default:
                    break;
                };
            }
            *(uint16_t *)dst = data16;
        }
        break;
    case 4:
        if (edev->dev_operations->read_u32)
        {
            rc = edev->dev_operations->read_u32(edev, offset, &data32);
            // debug_read(edev, offset, sizeof(uint32_t), data32);
        }
        else
        {
            printf("%s: edev=%s does not have read_u32()\n",
                   __func__, edev->dev_operations->name);
            rc = -ENOENT;
        }
        if (!rc)
        {
            // switch (edev->endian)
            // {
            // case VMM_DEVEMU_LITTLE_ENDIAN:
            data32 = vmm_cpu_to_le32(data32);
            data_endian = VMM_DEVEMU_LITTLE_ENDIAN;
            //     break;
            // case VMM_DEVEMU_BIG_ENDIAN:
            //     data32 = vmm_cpu_to_be32(data32);
            //     data_endian = VMM_DEVEMU_BIG_ENDIAN;
            //     break;
            // default:
            //     data_endian = VMM_DEVEMU_NATIVE_ENDIAN;
            //     break;
            // };
            if (data_endian != dst_endian)
            {
                switch (dst_endian)
                {
                case VMM_DEVEMU_LITTLE_ENDIAN:
                    data32 = vmm_cpu_to_le32(data32);
                    break;
                case VMM_DEVEMU_BIG_ENDIAN:
                    data32 = vmm_cpu_to_be32(data32);
                    break;
                default:
                    break;
                };
            }
            *(uint32_t *)dst = data32;
        }
        break;
    case 8:
        if (edev->dev_operations->read_u64)
        {
            rc = edev->dev_operations->read_u64(edev, offset, &data64);
            // debug_read(edev, offset, sizeof(uint64_t), data64);
        }
        else
        {
            printf("%s: edev=%s does not have read_u64()\n",
                   __func__, edev->dev_operations->name);
            rc = -ENOENT;
        }
        if (!rc)
        {
            // switch (edev->endian)
            // {
            // case VMM_DEVEMU_LITTLE_ENDIAN:
            data64 = vmm_cpu_to_le64(data64);
            data_endian = VMM_DEVEMU_LITTLE_ENDIAN;
            //     break;
            // case VMM_DEVEMU_BIG_ENDIAN:
            //     data64 = vmm_cpu_to_be64(data64);
            //     data_endian = VMM_DEVEMU_BIG_ENDIAN;
            //     break;
            // default:
            //     data_endian = VMM_DEVEMU_NATIVE_ENDIAN;
            //     break;
            // };
            if (data_endian != dst_endian)
            {
                switch (dst_endian)
                {
                case VMM_DEVEMU_LITTLE_ENDIAN:
                    data64 = vmm_cpu_to_le64(data64);
                    break;
                case VMM_DEVEMU_BIG_ENDIAN:
                    data64 = vmm_cpu_to_be32(data64);
                    break;
                default:
                    break;
                };
            }
            *(uint64_t *)dst = data64;
        }
        break;
    default:
        printf("%s: edev=%s invalid len=%d\n",
               __func__, edev->dev_operations->name, dst_len);
        rc = -EINVAL;
        break;
    };

    if (rc)
    {
        printf("%s: edev=%s offset=0x%p dst_len=%d "
               "failed (error %d)\n",
               __func__,
               edev->dev_operations->name, offset, dst_len, rc);
    }

    return rc;
}

static int devemu_dowrite(vmm_dev_t *edev,
                          paddr_t offset,
                          void *src, uint32_t src_len,
                          enum vmm_devemu_endianness src_endian)
{
    int rc;
    uint16_t data16;
    uint32_t data32;
    uint64_t data64;

    if (!edev ||
        (src_endian <= VMM_DEVEMU_UNKNOWN_ENDIAN) ||
        (VMM_DEVEMU_MAX_ENDIAN <= src_endian))
    {
        return -EFAULT;
    }

    switch (src_len)
    {
    case 1:
        if (edev->dev_operations->write_u8)
        {
            rc = edev->dev_operations->write_u8(edev, offset, *((uint8_t *)src));
            // debug_write(edev, offset, sizeof(uint8_t), *((uint8_t *)src));
        }
        else
        {
            printf("%s: edev=%s does not have write_u8()\n",
                   __func__, edev->dev_operations->name);
            rc = -ENOENT;
        }
        break;
    case 2:
        data16 = *(uint16_t *)src;
        switch (src_endian)
        {
        case VMM_DEVEMU_LITTLE_ENDIAN:
            data16 = vmm_le16_to_cpu(data16);
            break;
        case VMM_DEVEMU_BIG_ENDIAN:
            data16 = vmm_be16_to_cpu(data16);
            break;
        default:
            break;
        };
        // switch (edev->endian)
        // {
        // case VMM_DEVEMU_LITTLE_ENDIAN:
        data16 = vmm_cpu_to_le16(data16);
        // break;
        // case VMM_DEVEMU_BIG_ENDIAN:
        //     data16 = vmm_cpu_to_be16(data16);
        //     break;
        // default:
        //     break;
        // };
        if (edev->dev_operations->write_u16)
        {
            rc = edev->dev_operations->write_u16(edev, offset, data16);
            // debug_write(edev, offset, sizeof(uint16_t), data16);
        }
        else
        {
            printf("%s: edev=%s does not have write_u16()\n",
                   __func__, edev->dev_operations->name);
            rc = -ENOENT;
        }
        break;
    case 4:
        data32 = *(uint32_t *)src;
        switch (src_endian)
        {
        case VMM_DEVEMU_LITTLE_ENDIAN:
            data32 = vmm_le32_to_cpu(data32);
            break;
        case VMM_DEVEMU_BIG_ENDIAN:
            data32 = vmm_be32_to_cpu(data32);
            break;
        default:
            break;
        };
        // switch (edev->endian)
        // {
        // case VMM_DEVEMU_LITTLE_ENDIAN:
        data32 = vmm_cpu_to_le32(data32);
        // break;
        // case VMM_DEVEMU_BIG_ENDIAN:
        //     data32 = vmm_cpu_to_be32(data32);
        //     break;
        // default:
        //     break;
        // };
        if (edev->dev_operations->write_u32)
        {
            rc = edev->dev_operations->write_u32(edev, offset, data32);
            // debug_write(edev, offset, sizeof(uint32_t), data32);
        }
        else
        {
            printf("%s: edev=%s does not have write_u32()\n",
                   __func__, edev->dev_operations->name);
            rc = -ENOENT;
        }
        break;
    case 8:
        data64 = *(uint64_t *)src;
        switch (src_endian)
        {
        case VMM_DEVEMU_LITTLE_ENDIAN:
            data64 = vmm_le64_to_cpu(data64);
            break;
        case VMM_DEVEMU_BIG_ENDIAN:
            data64 = vmm_be64_to_cpu(data64);
            break;
        default:
            break;
        };
        // switch (edev->endian)
        // {
        // case VMM_DEVEMU_LITTLE_ENDIAN:
        data64 = vmm_cpu_to_le64(data64);
        // break;
        // case VMM_DEVEMU_BIG_ENDIAN:
        //     data64 = vmm_cpu_to_be64(data64);
        //     break;
        // default:
        //     break;
        // };
        if (edev->dev_operations->write_u64)
        {
            rc = edev->dev_operations->write_u64(edev, offset, data64);
            // debug_write(edev, offset, sizeof(uint64_t), data64);
        }
        else
        {
            printf("%s: edev=%s does not have write_u64()\n",
                   __func__, edev->dev_operations->name);
            rc = -ENOENT;
        }
        break;
    default:
        printf("%s: edev=%s invalid len=%d\n",
               __func__, edev->dev_operations->name, src_len);
        rc = -EINVAL;
        break;
    };

    if (rc)
    {
        printf("%s: edev=%s offset=0x%p src_len=%d "
               "failed (error %d)\n",
               __func__,
               edev->dev_operations->name, offset, src_len, rc);
    }

    return rc;
}

int vmm_devemu_emulate_read(guest_vcpu_t *vcpu,
                            paddr_t gphys_addr,
                            void *dst, uint32_t dst_len,
                            enum vmm_devemu_endianness dst_endian)
{
    int rc = -1;
    vmm_dev_t *vdev;

    if (!vcpu || !vcpu->guest)
    {
        return -EFAULT;
    }
    /*找到regs区域是否注册*/
    vdev = guest_os_find_region(vcpu->guest, gphys_addr);

    if (!vdev)
    {
        rc = -ENOENT;
        goto skip;
    }
    /*调用读取接口进行读取*/
    rc = devemu_doread(vdev,
                       gphys_addr - vdev->devtree_node->addr,
                       dst, dst_len, dst_endian);
skip:
    if (rc)
    {
        printf("%s: vcpu=%s gphys=0x%p dst_len=%d "
               "failed (error %d)\n",
               __func__,
               vcpu->name, gphys_addr, dst_len, rc);
        // vmm_manager_vcpu_halt(vcpu);
    }

    return rc;
}

int vmm_devemu_emulate_write(guest_vcpu_t *vcpu,
                             paddr_t gphys_addr,
                             void *src, uint32_t src_len,
                             enum vmm_devemu_endianness src_endian)
{
    int rc = -1;
    vmm_dev_t *vdev;

    if (!vcpu || !vcpu->guest)
    {
        return -EFAULT;
    }
    vdev = guest_os_find_region(vcpu->guest, gphys_addr);

    if (!vdev)
    {
        rc = -ENOENT;
        goto skip;
    }
    rc = devemu_dowrite(vdev,
                        gphys_addr - vdev->devtree_node->addr,
                        src, src_len, src_endian);
skip:
    if (rc)
    {
        printf("%s: vcpu=%s gphys=0x%p src_len=%d "
               "failed (error %d)\n",
               __func__,
               vcpu->name, gphys_addr, src_len, rc);
        // vmm_manager_vcpu_halt(vcpu);
    }

    return rc;
}
