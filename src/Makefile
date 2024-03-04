.SUFFIXES:

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

ALLOCATOR_MAGIC = $(shell doas hwclock | sha512sum | head -c 128 | md5sum | head -c 8)

SUBOBJS = kernel/kernel.o libk/libk.o rust.o

DEFINES = ARCH="${ARCH}" ASM="${ASM}" ASMFLAGS="${ASMFLAGS}" SOURCE="${SOURCE}" PWD="${PWD}" ALLOCATOR_MAGIC="${ALLOCATOR_MAGIC}"
ifdef release
	DEFINES := ${DEFINES} release=1
endif

################################
# rust语言环境变量

RSCFLAGS = --emit obj --crate-type staticlib --verbose \
	--crate-name=metaverse \
	--edition 2021 \
	-L crate="${PWD}/../rustlib/${ARCH}/src/" \
	-C code-model=large \
	-C relocation-model=static \
	-C embed-bitcode=no

ifeq (${ARCH},x86_64)
	RSCFLAGS := ${RSCFLAGS} --target x86_64-unknown-none
endif

ifdef release
	RSCFLAGS := ${RSCFLAGS} -O
endif

ifeq (${ARCH},x86_64)
	RSCFLAGS := ${RSCFLAGS} -C target-feature=-sse
endif

RUSTLIB_PATH = ../rustlib/${ARCH}/lib
RUST_LIBS = "${RUSTLIB_PATH}/liballoc.rlib" "${RUSTLIB_PATH}/libcompiler_builtins.rlib" \
	"${RUSTLIB_PATH}/libcore.rlib" "${RUSTLIB_PATH}/librustc_std_workspace_core.rlib"

################################

metaverse.elf: kernel libk rust metaverse.lds
	@echo -e "\e[1;33mld\e[0m \e[1;32m$@\e[0m \e[34m<--\e[0m \e[32m${SUBOBJS}\e[0m"
	@ld -T metaverse.lds -Map=metaverse.map -unresolved-symbols=ignore-all -o $@ ${SUBOBJS} ${RUST_LIBS} \
		2>&1 | "${SOURCE}/colorize" "warning:=yellow" "error:=red" "ld=lyellow"

.PHONY: kernel libk all clear postproc rust

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
	@if [ ! ${release} ]; then \
		echo -e "\e[33m__\e[0m \e[1;35m[Building kernel]\e[0m \e[33m____\e[0m"; \
	fi
	@make -C kernel all --no-print-directory ${DEFINES}
	@if [ ! ${release} ]; then \
		echo -e "\e[33m-------------------------\e[0m"; \
	fi

libk:
	@if [ ! ${release} ]; then \
		echo -e "\e[33m__\e[0m \e[1;35m[Building libk]\e[0m \e[33m______\e[0m"; \
	fi
	@make -C libk all --no-print-directory ${DEFINES}
	@if [ ! ${release} ]; then \
		echo -e "\e[33m-------------------------\e[0m"; \
	fi

rust: postproc
	@echo -e "\e[1m\e[33mrustc\e[0m \e[34m-->\e[0m \e[1m\e[32m$@.o\e[0m"
	@rustc ${RSCFLAGS} lib.rs -o rust.o

clear:
	@make -C kernel clear --no-print-directory ${DEFINES}
	@make -C libk clear --no-print-directory ${DEFINES}
	@-rm metaverse.elf metaverse.map rust.o
