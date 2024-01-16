ARCH := $(shell uname -m)
PWD := $(shell pwd)
SOURCE := ${PWD}/scripts
ifeq (${ARCH},x86_64)
	ASM = nasm
	ASMFLAGS = -f elf64
endif

ifdef release
	release = 1
endif

SUBOBJS = kernel/kernel.o libk/libk.o

DEFINES = ARCH="${ARCH}" ASM="${ASM}" ASMFLAGS="${ASMFLAGS}" SOURCE="${SOURCE}" PWD="${PWD}"
ifdef release
	DEFINES := ${DEFINES} release=1
endif

metaverse.elf: kernel libk metaverse.lds
	@echo -e "\e[1;33mld\e[0m \e[1;32m$@\e[0m \e[34m<--\e[0m \e[32m${SUBOBJS}\e[0m"
	@ld -T metaverse.lds -Map=metaverse.map -o $@ ${SUBOBJS} 2>&1 | "${SOURCE}/colorize" "warning:=yellow" "error:=red" "ld=lyellow"

.PHONY: kernel libk all clear postproc

all: postproc metaverse.elf

postproc:
	@echo -n -e "\e[36m"
	@if [ "${release}" = "1" ]; then \
		echo -n -e "[Building on ${ARCH} as release]"; \
	else \
		echo -n -e "[Building on ${ARCH}]"; \
	fi
	@echo -e "\e[0m"

kernel:
	@echo -e "\e[33m__\e[0m \e[1;35m[Building kernel]\e[0m \e[33m____\e[0m"
	@make -C kernel all --no-print-directory ${DEFINES}
	@echo -e "\e[33m-------------------------\e[0m"

libk:
	@echo -e "\e[33m__\e[0m \e[1;35m[Building libk]\e[0m \e[33m______\e[0m"
	@make -C libk all --no-print-directory ${DEFINES}
	@echo -e "\e[33m-------------------------\e[0m"

clear:
	@make -C kernel clear --no-print-directory ${DEFINES}
	@make -C libk clear --no-print-directory ${DEFINES}
	@-rm metaverse.elf metaverse.map
