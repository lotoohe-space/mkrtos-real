# 使用 ubuntu:20.04 镜像作为基础镜像
FROM ubuntu:20.04

# 设置环境变量
ENV DEBIAN_FRONTEND=noninteractive

# 切换到 home 目录
WORKDIR /home

# 备份原源列表
RUN cp /etc/apt/sources.list /etc/apt/sources.list.bak

# 使用清华大学的 apt 源
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal main restricted universe multiverse" > /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-security main restricted universe multiverse" >> /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-updates main restricted universe multiverse" >> /etc/apt/sources.list
RUN echo "deb http://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-backports main restricted universe multiverse" >> /etc/apt/sources.list

# 更新源
RUN apt-get update && \
	apt-get install -y python3 cmake ninja-build cpio srecord libc6-i386 git qemu-system-arm && \
	git clone https://gitee.com/IsYourGod/mkrtos-tools.git && \
	cat mkrtos-tools/gcc/gcc-arm-none-eabi-5_4.tar.gz_* | tar -zxpf - -C /home/ && \
	cat mkrtos-tools/qemu8.1/qemu8.1.tar.gz_* | tar -zxpf - -C /home/

# 默认命令，如果需要的话可以在这里添加
CMD ["bash"]
