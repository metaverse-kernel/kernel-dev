.SUFFIXES:

################################
# C语言环境变量

CC = gcc
CCFLAGS = -m64 -mcmodel=large -I ../../include \
			-fno-stack-protector -fno-exceptions \
			-fno-builtin -nostdinc -nostdlib
ifdef release
	CCFLAGS := ${CCFLAGS} -O2
endif

C_SRCS = bootinfo.c lst.c
C_OBJS = ${C_SRCS:.c=.c.o}

################################

################################
# 汇编语言环境变量

ifndef ASM
	ASM = as
endif

ASMFLAGS := ${ASMFLAGS}

S_SRCS = memset.s memcpy.s strlen.s
S_OBJS = ${S_SRCS:.s=.s.o}

################################

OBJS = ${S_OBJS} ${C_OBJS}

STRIP_SECS = -R .note.GNU-stack

OBJCOPY_FLAGS = ${STRIP_SECS}

# 子目录
VPATH = multiboot2/ string/

%.c.o: %.c
	@echo -e "\e[1m\e[33m${CC}\e[0m \e[32m$<\e[0m \e[34m-->\e[0m \e[1m\e[32m$@\e[0m"
	@${CC} -c ${CCFLAGS} $< -o $@

%.s.o: arch/${ARCH}/%.s
	@echo -e "\e[1m\e[33m${ASM}\e[0m \e[32m$<\e[0m \e[34m-->\e[0m \e[1m\e[32m$@\e[0m"
	@${ASM} ${ASMFLAGS} -o $@ $< 2>&1 | "${SOURCE}/colorize" "warning:=pink" "error:=red"

libk.o: ${OBJS}
	@echo -e "\e[1m\e[33mld\e[0m \e[1m\e[32mlibk.o\e[0m \e[34m<--\e[0m \e[32m${OBJS}\e[0m"
	@ld -r ${OBJS} -o libk.o -Map=libk.map -unresolved-symbols=ignore-all 2>&1 \
		| "${SOURCE}/colorize" "warning:=pink" "error:=red" "ld=lyellow"
	@objcopy ${OBJCOPY_FLAGS} libk.o libk.o

.PHONY: all postproc postproc_x86_64

all: postproc libk.o

postproc: postproc_${ARCH}

postproc_x86_64:
	$(eval ASM = nasm)
	$(eval ASMFLAGS = -f elf64)

clear:
	@-rm ${OBJS} libk.o libk.map
