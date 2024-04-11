#pragma once

void cpu_start_to(int cpu_id, void *stack, void (*fn)(void));
void *get_cpu_stack(int cpu_inx);
