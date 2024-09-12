.global os_bin
.global os_bin_end
.global uvmm_dev_json
.global uvmm_dev_json_end
.align 12
os_bin:
.incbin "/Users/zhangzheng/mkrtos-real/benos/arch/arm64/boot/benos.bin"
os_bin_end:
.quad 0
uvmm_dev_json:
.incbin "/Users/zhangzheng/mkrtos-real/mkrtos_user/server/uvmm/src/vm_benos.json"
uvmm_dev_json_end:
.quad 0
