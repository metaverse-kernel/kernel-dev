#ifndef BOOTINFO_H
#define BOOTINFO_H 1

#include <types.h>
#include <utils.h>

#define BOOTINFO_TAG_REPEAT_MAX 32
typedef struct __bootinfo_t
{
    void *start;
    usize size;
    usize iter_index;
    void *map[24][BOOTINFO_TAG_REPEAT_MAX];
    usize map_counts[24];
} bootinfo_t;

typedef struct __bootinfo_tag
{
    u32 type;
    u32 size;
} DISALIGNED bootinfo_tag_t;

/** Basic memory information
 * ‘mem_lower’ and ‘mem_upper’ indicate the amount of lower and upper memory,
 * respectively, in kilobytes. Lower memory starts at address 0, and upper memory
 * starts at address 1 megabyte. The maximum possible value for lower memory
 * is 640 kilobytes. The value returned for upper memory is maximally the address
 * of the first upper memory hole minus 1 megabyte. It is not guaranteed to be this value.
 *
 * This tag may not be provided by some boot loaders on EFI platforms if EFI boot services
 * are enabled and available for the loaded image (EFI boot services not terminated tag
 * exists in Multiboot2 information structure).
 */
typedef struct __bootinfo_basic_memory_info_t
{
    u32 mem_lower;
    u32 mem_upper;
} DISALIGNED bootinfo_basic_memory_info_t;
#define bootinfo_basic_memory_info(addr) (bootinfo_basic_memory_info_t *)(addr)
#define BOOTINFO_BASIC_MEMORY_INFO_TYPE 4

/** BIOS Boot device
 * This tag indicates which BIOS disk device the boot loader loaded the OS image from.
 * If the OS image was not loaded from a BIOS disk, then this tag must not be present.
 * The operating system may use this field as a hint for determining its own root device,
 * but is not required to.
 *
 * The ‘biosdev’ contains the BIOS drive number as understood by the BIOS INT 0x13
 * low-level disk interface: e.g. 0x00 for the first floppy disk or 0x80 for the first hard disk.
 *
 * The three remaining bytes specify the boot partition. ‘partition’ specifies the top-level
 * partition number, ‘sub_partition’ specifies a sub-partition in the top-level partition,
 * etc. Partition numbers always start from zero. Unused partition bytes must be set to 0xFFFFFFFF.
 * For example, if the disk is partitioned using a simple one-level DOS partitioning scheme,
 * then ‘partition’ contains the DOS partition number, and ‘sub_partition’ if 0xFFFFFF.
 * As another example, if a disk is partitioned first into DOS partitions, and then one of
 * those DOS partitions is subdivided into several BSD partitions using BSD’s disklabel strategy,
 * then ‘partition’ contains the DOS partition number and ‘sub_partition’ contains
 * the BSD sub-partition within that DOS partition.
 *
 * DOS extended partitions are indicated as partition numbers starting from 4 and increasing,
 * rather than as nested sub-partitions, even though the underlying disk layout of
 * extended partitions is hierarchical in nature. For example, if the boot loader boots from
 * the second extended partition on a disk partitioned in conventional DOS style, then ‘partition’
 * will be 5, and ‘sub_partiton’ will be 0xFFFFFFFF.
 */
typedef struct __bootinfo_bios_boot_device_t
{
    u32 biosdev;
    u32 partition;
    u32 sub_partition;
} DISALIGNED bootinfo_bios_boot_device_t;
#define bootinfo_bios_boot_device(addr) (bootinfo_bios_boot_device_t *)(addr)
#define BOOTINFO_BIOS_BOOT_DEVICE_TYPE 5

/** Boot command line
 * ‘string’ contains command line. The command line is a normal C-style zero-terminated UTF-8 string.
 */
typedef struct __bootinfo_boot_command_line_t
{
    u32 size;
    u8 string[0];
} DISALIGNED bootinfo_boot_command_line_t;
#define bootinfo_boot_command_line(addr) (bootinfo_boot_command_line_t *)((usize)(addr) - sizeof(u32))
#define BOOTINFO_BOOT_COMMAND_LINE_TYPE 1

/** Modules
 * This tag indicates to the kernel what boot module was loaded along with the kernel image,
 * and where it can be found.
 *
 * The ‘mod_start’ and ‘mod_end’ contain the start and end physical addresses of the boot module
 * itself. The ‘string’ field provides an arbitrary string to be associated with that particular
 * boot module; it is a zero-terminated UTF-8 string, just like the kernel command line.
 * Typically the string might be a command line (e.g. if the operating system treats boot modules
 * as executable programs), or a pathname (e.g. if the operating system treats boot modules
 * as files in a file system), but its exact use is specific to the operating system.
 *
 * One tag appears per module. This tag type may appear multiple times.
 */
typedef struct __bootinfo_modules_t
{
    u32 size;
    u32 mod_start;
    u32 mod_end;
    u8 string[0];
} DISALIGNED bootinfo_modules_t;
#define bootinfo_modules(addr) (bootinfo_modules_t *)((usize)(addr) - sizeof(u32))
#define BOOTINFO_MODULES_TYPE 3

/** ELF-Symbols
 * This tag contains section header table from an ELF kernel, the size of each entry, number
 * of entries, and the string table used as the index of names. They correspond to the
 * ‘shdr_*’ entries (‘shdr_num’, etc.) in the Executable and Linkable Format (ELF) specification
 * in the program header. All sections are loaded, and the physical address fields of the
 * ELF section header then refer to where the sections are in memory (refer to the i386 ELF
 * documentation for details as to how to read the section header(s)).
 */
typedef struct __bootinfo_elf_symbols_t
{
    u16 num;
    u16 entsize;
    u16 shndx;
    u16 reserved;
    u8 section_headers[0];
} DISALIGNED bootinfo_elf_symbols_t;
#define bootinfo_elf_symbols(addr) (bootinfo_elf_symbols_t *)(addr)
#define BOOTINFO_ELF_SYMBOLS_TYPE 9

/** Memory map
 * This tag provides memory map.
 *
 * ‘entry_size’ contains the size of one entry so that in future new fields may be added to it.
 * It’s guaranteed to be a multiple of 8. ‘entry_version’ is currently set at ‘0’. Future versions
 * will increment this field. Future version are guranteed to be backward compatible with
 * older format. Each entry has the following structure: bootinfo_memory_map_entry.
 *
 * 'size' contains the size of current entry including this field itself. It may be bigger
 * than 24 bytes in future versions but is guaranteed to be ‘base_addr’ is the starting
 * physical address.
 * ‘length’ is the size of the memory region in bytes.
 * ‘type’ is the variety of address range represented, where a value of 1 indicates available RAM,
 * value of 3 indicates usable memory holding ACPI information, value of 4 indicates
 * reserved memory which needs to be preserved on hibernation, value of 5 indicates a memory
 * which is occupied by defective RAM modules and all other values currently indicated
 * a reserved area.
 * ‘reserved’ is set to ‘0’ by bootloader and must be ignored by the OS image.
 *
 * The map provided is guaranteed to list all standard RAM that should be available
 * for normal use. This type however includes the regions occupied by kernel, mbi, segments
 * and modules. Kernel must take care not to overwrite these regions.
 *
 * This tag may not be provided by some boot loaders on EFI platforms if EFI boot services
 * are enabled and available for the loaded image (EFI boot services not terminated tag
 * exists in Multiboot2 information structure).
 */
typedef struct __bootinfo_memory_map_t
{
    u32 size;
    u32 entry_size;
    u32 entry_version;
    struct __bootinfo_memory_map_entry_t
    {
        u64 base_addr;
        u64 length;
        u32 type;
        u32 reserved;
    } DISALIGNED entries[0];
} DISALIGNED bootinfo_memory_map_t;
#define bootinfo_memory_map(addr) (bootinfo_memory_map_t *)((usize)(addr) - sizeof(u32))
#define bootinfo_memory_map_end(addr) ((void *)(addr) + (addr)->size - sizeof(u32))
#define BOOTINFO_MEMORY_MAP_TYPE 6
typedef struct __bootinfo_memory_map_entry_t bootinfo_memory_map_entry_t;

/** Boot loader name
 * ‘string’ contains the name of a boot loader booting the kernel. The name is a normal
 * C-style UTF-8 zero-terminated string.
 */
typedef struct __bootinfo_bootloader_name_t
{
    u32 size;
    u8 string[0];
} DISALIGNED bootinfo_bootloader_name_t;
#define bootinfo_bootloader_name(addr) (bootinfo_bootloader_name_t *)((usize)(addr) - sizeof(u32))
#define BOOTINFO_BOOTLOADER_NAME_TYPE 2

/** APM table
 * The fields ‘version’, ‘cseg’, ‘offset’, ‘cseg_16’, ‘dseg’, ‘flags’, ‘cseg_len’, ‘cseg_16_len’,
 * ‘dseg_len’ indicate the version number, the protected mode 32-bit code segment, the offset
 * of the entry point, the protected mode 16-bit code segment, the protected mode 16-bit
 * data segment, the flags, the length of the protected mode 32-bit code segment, the length of
 * the protected mode 16-bit code segment, and the length of the protected mode 16-bit data segment,
 * respectively. Only the field ‘offset’ is 4 bytes, and the others are 2 bytes.
 * See [Advanced Power Management (APM) BIOS Interface Specification]
 * (http://www.microsoft.com/hwdev/busbios/amp_12.htm), for more information.
 */
typedef struct __bootinfo_apm_table_t
{
    u16 version;
    u16 cseg;
    u32 offset;
    u16 cseg_16;
    u16 dseg;
    u16 flags;
    u16 cseg_len;
    u16 cseg_16_len;
    u16 dseg_len;
} DISALIGNED bootinfo_apm_table_t;
#define bootinfo_apm_table(addr) (bootinfo_apm_table_t *)(addr)
#define BOOTINFO_APM_TABLE_TYPE 10

/** VBE info
 * The fields ‘vbe_control_info’ and ‘vbe_mode_info’ contain VBE control information returned by
 * the VBE Function 00h and VBE mode information returned by the VBE Function 01h, respectively.
 *
 * The field ‘vbe_mode’ indicates current video mode in the format specified in VBE 3.0.
 *
 * The rest fields ‘vbe_interface_seg’, ‘vbe_interface_off’, and ‘vbe_interface_len’ contain
 * the table of a protected mode interface defined in VBE 2.0+. If this information is not
 * available, those fields contain zero. Note that VBE 3.0 defines another protected mode
 * interface which is incompatible with the old one. If you want to use the new protected mode
 * interface, you will have to find the table yourself.
 */
typedef struct __bootinfo_vbe_info_t
{
    u16 vbe_mode;
    u16 vbe_interface_seg;
    u16 vbe_interface_off;
    u16 vbe_interface_len;
    u8 vbe_control_info[512];
    u8 vbe_mode_info[256];
} DISALIGNED bootinfo_vbe_info_t;
#define bootinfo_vbe_info(addr) (bootinfo_vbe_info_t *)(addr)
#define BOOTINFO_VBE_INFO_TYPE 7

/** Framebuffer info
 * The field ‘framebuffer_addr’ contains framebuffer physical address. This field is 64-bit wide
 * but bootloader should set it under 4GiB if possible for compatibility with payloads
 * which aren’t aware of PAE or amd64.
 * The field ‘framebuffer_pitch’ contains pitch in bytes.
 * The fields ‘framebuffer_width’, ‘framebuffer_height’ contain framebuffer dimensions in pixels.
 * The field ‘framebuffer_bpp’ contains number of bits per pixel.
 * ‘reserved’ always contains 0 in current version of specification and must be ignored by OS image.
 */
typedef struct __bootinfo_framebuffer_info_t
{
    u32 size;
    u64 framebuffer_addr;
    u32 framebuffer_pitch;
    u32 framebuffer_width;
    u32 framebuffer_height;
    u8 framebuffer_bpp;
    u8 framebuffer_type;
    u8 color_info[0];
} DISALIGNED bootinfo_framebuffer_info_t;
#define bootinfo_framebuffer_info(addr) (bootinfo_framebuffer_info_t *)((usize)(addr) - sizeof(u32))
#define BOOTINFO_FRAMEBUFFER_INFO_TYPE 8

/**
 * If ‘framebuffer_type’ is set to 0 it means indexed color. In this case color_info
 * is defined as follows:
 *
 * ‘framebuffer_palette’ is an array of colour descriptors. Each colour descriptor
 * has following structure: bootinfo_framebuffer_info_framebuffer_palette_t.
 */
typedef struct __bootinfo_framebuffer_info_color_info_0_t
{
    u32 framebuffer_palette_num_colors;
    struct bootinfo_framebuffer_info_framebuffer_palette_t
    {
        u8 red, green, blue;
    } DISALIGNED palettes[0];
} DISALIGNED bootinfo_framebuffer_info_color_info_0_t;

/**
 * If ‘framebuffer_type’ is set to ‘1’ it means direct RGB color. Then color_type
 * is defined as follows:
 *
 * If ‘framebuffer_type’ is set to ‘2’ it means EGA text. In this case ‘framebuffer_width’
 * and ‘framebuffer_height’ are expressed in characters and not in pixels.
 * ‘framebuffer_bpp’ is equal 16 (16 bits per character) and ‘framebuffer_pitch’ is expressed
 * in bytes per text line. All further values of ‘framebuffer_type’ are reserved for
 * future expansion.
 */
typedef struct __bootinfo_framebuffer_info_color_info_1_2_t
{
    u8 framebuffer_red_field_position;
    u8 framebuffer_red_mask_size;
    u8 framebuffer_green_field_position;
    u8 framebuffer_green_mask_size;
    u8 framebuffer_blue_field_position;
    u8 framebuffer_blue_mask_size;
} DISALIGNED bootinfo_framebuffer_info_color_info_1_2_t;

/** EFI 64-bit system table pointer
 * This tag contains pointer to amd64 EFI system table.
 */
typedef struct __bootinfo_efi_system_table_pointer_t
{
    u64 pointer;
} DISALIGNED bootinfo_efi_system_table_pointer_t;
#define bootinfo_efi_system_table_pointer(addr) (bootinfo_efi_system_table_pointer_t *)(addr)
#define BOOTINFO_EFI_SYSTEM_TABLE_POINTER_TYPE 12

/** ACPI v2 RSDP
 * This tag contains a copy of RSDP as defined per ACPI 2.0 or later specification.
 */
typedef struct __bootinfo_acpi_rsdp_t
{
    u32 size;
    u8 acpi_data[0];
} DISALIGNED bootinfo_acpi_rsdp_t;
#define bootinfo_acpi_rsdp(addr) (bootinfo_acpi_rsdp_t *)((usize)(addr) - sizeof(u32))
#define BOOTINFO_ACPI_RSDP_TYPE 15

/** Networking information
 * This tag contains network information in the format specified as DHCP. It may be either
 * a real DHCP reply or just the configuration info in the same format.
 * This tag appears once per card.
 */
typedef struct __bootinfo_networking_info_t
{
    u32 size;
    u8 dhcp_ack[0];
} DISALIGNED bootinfo_networking_info_t;
#define bootinfo_networking_info(addr) (bootinfo_networking_info_t *)((usize)(addr) - sizeof(u32))
#define BOOTINFO_NETWORKING_INFO_TYPE 16

/** EFI memory map
 * This tag contains EFI memory map as per EFI specification.
 *
 * This tag may not be provided by some boot loaders on EFI platforms if EFI boot services
 * are enabled and available for the loaded image (EFI boot services not terminated tag
 * exists in Multiboot2 information structure).
 */
typedef struct __bootinfo_efi_memory_map_t
{
    u32 discriptor_size;
    u32 discriptor_version;
    u8 efi_memory_map[0];
} DISALIGNED bootinfo_efi_memory_map_t;
#define bootinfo_efi_memory_map(addr) (bootinfo_efi_memory_map_t *)(addr)
#define BOOTINFO_EFI_MEMORY_MAP_TYPE 17

/** EFI boot services not terminated
 * This tag indicates ExitBootServices wasn’t called.
 */
#define BOOTINFO_EFI_BOOTSERVICE_NOT_TERMINATED_TYPE 18

/** EFI 64-bit image handle pointer
 * This tag contains pointer to EFI amd64 image handle. Usually it is boot loader image handle.
 */
typedef struct __bootinfo_efi_image_handle_pointer_t
{
    u64 pointer;
} DISALIGNED bootinfo_efi_image_handle_pointer_t;
#define bootinfo_efi_image_handle_pointer(addr) (bootinfo_efi_image_handle_pointer_t *)(addr)
#define BOOTINFO_EFI_IMAGE_HANDLE_POINTER_TYPE 20

/** Image load base physical address
 * This tag contains image load base physical address. It is provided only if image has
 * relocatable header tag.
 */
typedef struct __bootinfo_image_load_base_address_t
{
    u32 load_base_addr;
} DISALIGNED bootinfo_image_load_base_address_t;
#define bootinfo_image_load_base_address(addr) (bootinfo_image_load_base_address_t *)(addr)
#define BOOTINFO_IMAGE_LOAD_BASE_ADDRESS_TYPE 21

/**
 * @brief 初始化bootinfo结构
 *
 * @param bootinfo
 * @param bootinfo_addr
 */
void bootinfo_new(bootinfo_t *bootinfo, void *bootinfo_addr);

/**
 * @brief 获取bootinfo中的tag
 *
 * @param bootinfo
 * @param type
 * @param tags 返回的tag的指针数组
 * @return usize tag的总数
 */
usize bootinfo_get_tag(bootinfo_t *bootinfo, usize type, void ***tags);

#endif
