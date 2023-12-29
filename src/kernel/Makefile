################################
# C语言环境变量

CC = gcc
CCFLAGS = -m64 -mcmodel=large -I ../include \
			-fno-stack-protector -fno-exceptions \
			-fno-builtin -nostdinc -nostdlib
ifdef release
	CCFLAGS := ${CCFLAGS} -O2
endif

C_SRCS = main.c tty.c klog.c font.c memm.c memm_${ARCH}.c raw.c
C_OBJS = ${C_SRCS:.c=.o}

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
ASMFLAGS32 = -f elf32

S_OBJS = entry32.32.o entry.o memm_${ARCH}_s.o

################################

OBJS = ${S_OBJS} ${C_OBJS} ${RS_OBJS}

STRIP_SECS = -R .comment -R .note.GNU-stack

OBJCOPY_FLAGS = ${STRIP_SECS}

# 子目录
VPATH = memm/ memm/allocator tty/ klog/ arch/${ARCH}

%.o: %.c
	@echo -e "\e[1m\e[33m${CC}\e[0m \e[32m$<\e[0m \e[34m-->\e[0m \e[1m\e[32m$@\e[0m"
	@${CC} -c ${CCFLAGS} $< -o $@

%.o: %.rs
	@echo -e "\e[1m\e[33mrustc\e[0m \e[32m$<\e[0m \e[34m-->\e[0m \e[1m\e[32m$@\e[0m"
	@rustc ${RSCFLAGS} $< -o $@

%.32.o: arch/${ARCH}/%.s
	@echo -e "\e[1m\e[33m${ASM}\e[0m \e[32m$<\e[0m \e[34m-->\e[0m \e[1m\e[32m$@\e[0m"
	@${ASM} ${ASMFLAGS32} -o $@ $< 2>&1 \
		| "${SOURCE}/colorize" "warning:=pink" "error:=red"
	@objcopy -I elf32-i386 -O elf64-x86-64 $@ $@ 2>&1 \
		| "${SOURCE}/colorize" "warning:=pink" "error:=red" "objcopy=lyellow"

%.o: arch/${ARCH}/%.s
	@echo -e "\e[1m\e[33m${ASM}\e[0m \e[32m$<\e[0m \e[34m-->\e[0m \e[1m\e[32m$@\e[0m"
	@${ASM} ${ASMFLAGS} -o $@ $< 2>&1 | "${SOURCE}/colorize" "warning:=pink" "error:=red"

kernel.o: ${OBJS}
	@echo -e "\e[1m\e[33mld\e[0m \e[1m\e[32mkernel.o\e[0m \e[34m<--\e[0m \e[32m${OBJS}\e[0m"
	@ld -r ${OBJS} -o kernel.o -Map=kernel.map -unresolved-symbols=ignore-all 2>&1 \
		| "${SOURCE}/colorize" "warning:=pink" "error:=red" "ld=lyellow"
	@objcopy ${OBJCOPY_FLAGS} kernel.o kernel.o

.PHONY: all clear

all: kernel.o

clear:
	@-rm ${OBJS} kernel.o kernel.map
