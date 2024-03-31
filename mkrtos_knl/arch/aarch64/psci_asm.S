
#include <complier.h>

.section ".text"

BEGIN_FUNC(arm_smc)
    smc #0
    ret
END_FUNC(arm_smc)
