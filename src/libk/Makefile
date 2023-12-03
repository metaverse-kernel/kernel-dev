ARCH := $(shell uname -m)
ifdef release
	release = 1
endif

################################
# C语言环境变量

CC = gcc
CCFLAGS = -m64 -mcmodel=large -I ../include -fno-stack-protector -fno-exceptions -fno-builtin -nostdinc -nostdlib
ifdef release
	CCFLAGS := ${CCFLAGS} -O2
endif

C_OBJS = bootinfo.o

################################

################################
# rust语音环境变量

RSCFLAGS = --emit=obj --target x86_64-unknown-linux-gnu --crate-type=bin

RS_OBJS =

################################

################################
# 汇编语言环境变量

ifndef ASM
	ASM = as
endif

ASMFLAGS := ${ASMFLAGS}

S_OBJS = memset.o memcpy.o

################################

OBJS = ${S_OBJS} ${C_OBJS} ${RS_OBJS}

STRIP_SECS = -R .comment -R .note.GNU-stack

OBJCOPY_FLAGS = ${STRIP_SECS}

# 子目录
VPATH = multiboot2/ string/

%.o: %.c
	@echo -e "\e[1m\e[33m${CC}\e[0m \e[32m$<\e[0m \e[34m-->\e[0m \e[1m\e[32m$@\e[0m"
	@${CC} -c ${CCFLAGS} $< -o $@

%.o: %.rs
	@echo -e "\e[1m\e[33mrustc\e[0m \e[32m$<\e[0m \e[34m-->\e[0m \e[1m\e[32m$@\e[0m"
	@rustc ${RSCFLAGS} $< -o $@

%.o: arch/${ARCH}/%.s
	@echo -e "\e[1m\e[33m${ASM}\e[0m \e[32m$<\e[0m \e[34m-->\e[0m \e[1m\e[32m$@\e[0m"
	@${ASM} ${ASMFLAGS} -o $@ $< 2>&1 | "${SOURCE}/colorize" "warning:=yellow" "error:=red"

libk.o: ${OBJS}
	@echo -e "\e[1m\e[33mld\e[0m \e[1m\e[32mlibk.o\e[0m \e[34m<--\e[0m \e[32m${OBJS}\e[0m"
	@ld -r ${OBJS} -o libk.o -Map=libk.map 2>&1 | "${SOURCE}/colorize" "warning:=yellow" "error:=red" "ld=lyellow"
	@objcopy ${OBJCOPY_FLAGS} libk.o libk.o

.PHONY: all postproc postproc_x86_64

all: postproc libk.o

postproc: postproc_${ARCH}

postproc_x86_64:
	$(eval ASM = nasm)
	$(eval ASMFLAGS = -f elf64)

clear:
	@-rm ${OBJS} libk.o libk.map
