.PHONY: all clear run debug

all:
	@make -C src all --no-print-directory

clear:
	@make -C src clear --no-print-directory

run:
	@make -C qemu run --no-print-directory

debug:
	@make -C qemu debug --no-print-directory
