TARGET = ../src/metaverse.elf
ARCH := $(shell uname -m)

.PHONY: run debug load mount unmount

BIOS = bios/${ARCH}/OVMF_CODE.fd

run:
	@doas modprobe nbd
	@make load
	@qemu-system-${ARCH} -m 4G metaverse.img -bios ${BIOS}

debug:
	@echo "在gdb中连接远程目标'localhost:1234'即可"
	@doas modprobe nbd
	@make load
	@qemu-system-${ARCH} -m 4G metaverse.img -bios ${BIOS} -s -S

create:
	@qemu-img create -f qcow2 metaverse.img 512M
	@doas modprobe nbd
	@doas qemu-nbd --connect=/dev/nbd0 metaverse.img
	@doas partprobe /dev/nbd0
	@echo "你需要为这个虚拟磁盘写入一个GPT分区表并分出efi分区和根分区。"
	@doas fdisk /dev/nbd0
	@doas mkfs.fat -F 32 /dev/nbd0p1
	@doas mkfs.fat -F 32 /dev/nbd0p2
	@doas mount /dev/nbd0p2 mnt/
	@doas mount -m /dev/nbd0p1 mnt/boot/
	@doas mkdir mnt/boot/EFI/
	@make unmount

load:
	@echo loading kernel ...
	@make mount
	@doas cp ${TARGET} mnt/boot/
	@make unmount

mount:
	@doas modprobe nbd
	@doas qemu-nbd --connect=/dev/nbd0 metaverse.img
	@doas partprobe /dev/nbd0
	@doas mount /dev/nbd0p2 mnt/
	@doas mount /dev/nbd0p1 mnt/boot/

unmount:
	@sync
	@doas umount /dev/nbd0p1
	@doas umount /dev/nbd0p2
	@doas qemu-nbd --disconnect /dev/nbd0
