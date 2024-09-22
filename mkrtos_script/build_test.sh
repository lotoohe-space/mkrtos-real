#!/bin/bash
set -e

# 定义超时时间(秒)
TIMEOUT=30

# 定义成功标志
SUCCESS_MSG="MKRTOS test ok."

# 运行命令并检查输出的函数
run_and_check() {
    local cmd="$1"
    local timeout="$2"
    
    echo "Running: $cmd"
    
    # 使用管道和 grep 来监听输出
    timeout --preserve-status --foreground $timeout $cmd | while IFS= read -r line
    do
        echo "$line"
        if [[ "$line" == *"$SUCCESS_MSG"* ]]; then
            echo "Success message detected. Exiting..."
            kill -INT $$
            return 0;
        fi
    done
    # 检查 timeout 命令的退出状态
    if [ $? -eq 124 ]; then
        echo "Test timed out: $cmd"
        exit -1
    fi
    
    return 0
}

# 清理
./mkrtos_script/clean.sh
./mkrtos_script/build_qemu_aarch64.sh test
run_and_check "./mkrtos_script/run_aarch64_qemu.sh" "$TIMEOUT"# 运行测试

./mkrtos_script/clean.sh
./mkrtos_script/build_f2.sh test
run_and_check "./mkrtos_script/run_m3.sh" "$TIMEOUT"

./mkrtos_script/clean.sh
./mkrtos_script/build_f4.sh test
run_and_check "./mkrtos_script/run_m4.sh" "$TIMEOUT"

./mkrtos_script/clean.sh
./mkrtos_script/build_swm34s.sh test
# run_and_check "./mkrtos_script/build_swm34s.sh test" "$TIMEOUT"

echo "All tests completed."