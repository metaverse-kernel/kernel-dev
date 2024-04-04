SOURCE := $(shell pwd)/scripts

.PHONY: all clear run debug disass

all: metaverse_kernel
	@make -C src all --no-print-directory

clear: metaverse_kernel
	@make -C src clear --no-print-directory

run: metaverse_kernel
	@make -C test run --no-print-directory

debug: metaverse_kernel
	@make -C test debug --no-print-directory

metaverse_kernel:
	@if [ "${shell uname -s}" != "Linux" ]; then \
		echo -e "\e[1;33mMetaverse\e[0m must build under \e[1;35mLinux\e[0m or itself."; \
		exit -1; \
	fi
	@if [ -f "metaverse_kernel" ]; then \
		echo; \
	else \
		"${SOURCE}/depcheck"; \
		if [ $$? != 0 ]; then \
			exit $$?; \
		fi; \
		touch metaverse_kernel; \
	fi
	@if [ $$? != 0 ]; then \
		exit -1; \
	fi

disass:
	objdump -D src/metaverse.elf > kerndisass.txt

install-grub:
	@cp src/metaverse.elf /boot/
	@if ! grep -q "Metaverse" /boot/grub/grub.cfg; then \
		echo -e 'menuentry "Metaverse" {\n    set root=(hd0,gpt1)\n    multiboot2 /metaverse.elf\n}\n' >> \
			/boot/grub/grub.cfg; \
		echo -e 'Added Metaverse into grub entries.'; \
	else \
		echo -e 'Metaverse entry exists.'; \
	fi

remove-grub:
	@-rm /boot/metaverse.elf
