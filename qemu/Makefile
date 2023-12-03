TARGET = ../src/metaverse.elf

.PHONY: run debug load mount unmount

BIOS = bios/OVMF_CODE.fd

run:
	@sudo modprobe nbd
	@make load
	@qemu-system-x86_64 -m 4G metaverse.img -enable-kvm -bios ${BIOS}

debug:
	@sudo modprobe nbd
	@make load
	@qemu-system-x86_64 -m 4G metaverse.img -enable-kvm -bios ${BIOS} -s -S

create:
	@qemu-img create -f qcow2 metaverse.img 512M
	@sudo modprobe nbd
	@sudo qemu-nbd --connect=/dev/nbd0 metaverse.img
	@sudo partprobe /dev/nbd0
	@echo "你需要为这个虚拟磁盘写入一个GPT分区表并分出efi分区和根分区。"
	@sudo fdisk /dev/nbd0
	@sudo mkfs.fat -F 32 /dev/nbd0p1
	@sudo mkfs.fat -F 32 /dev/nbd0p2
	@sudo mount /dev/nbd0p2 mnt/
	@sudo mount -m /dev/nbd0p1 mnt/boot/
	@sudo mkdir mnt/boot/EFI/
	@make unmount

load:
	@echo loading kernel ...
	@make mount
	@sudo cp ${TARGET} mnt/boot/
	@make unmount

mount:
	@sudo modprobe nbd
	@sudo qemu-nbd --connect=/dev/nbd0 metaverse.img
	@sudo partprobe /dev/nbd0
	@sudo mount /dev/nbd0p2 mnt/
	@sudo mount /dev/nbd0p1 mnt/boot/

unmount:
	@sync
	@sudo umount /dev/nbd0p1
	@sudo umount /dev/nbd0p2
	@sudo qemu-nbd --disconnect /dev/nbd0
