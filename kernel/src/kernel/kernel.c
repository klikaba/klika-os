#include <multiboot.h>
#include <stdint.h>
#include <string.h>
#include <kernel.h>
#include <memory.h>
#include <elf.h>
#include <x86.h>
#include <stdarg.h>
#include <_printf.h>
#include <isr.h>
#include <timer.h>
#include <keyboard.h>
#include <timer.h>
#include <process.h>
#include <serial.h>
#include <pic.h>
#include <mmu_pagging.h>
#include <mmu_heap.h>
#include <mouse.h>
#include <assert.h>
#include <vesa.h>
#include <window_manager.h>
#include <pci.h>
#include <ata.h>

#define GDT_NULL        0x00
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20
#define GDT_TSS         0x28 

int serial_printf_help(unsigned c, void *ptr __UNUSED__) {
  serial_write_com(1, c);
  return 0;
}

void HALT_AND_CATCH_FIRE(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);
  x86_hlt(); 
  while(1) {}
}

void _kdebug(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);
}

#include <../../../apps/desktop/build/desktop_byte.c>

void idle() {
  while(1) {
    x86_hlt();
  }
}

typedef struct {
  uint32_t  table_size_32;
  uint16_t  extended_flags;
  uint16_t  fat_version;
  uint32_t  root_cluster;
  uint16_t  fat_info;
  uint16_t  backup_BS_sector;
  uint8_t   reserved_0[12];
  uint8_t   drive_number;
  uint8_t   reserved_1;
  uint8_t   boot_signature;
  uint32_t  volume_id;
  uint8_t   volume_label[11];
  uint8_t   fat_type_label[8];
} __attribute__((packed)) boot_sector_ext32_t;
 
typedef struct {
  uint8_t   bootjmp[3];
  uint8_t   oem_name[8];
  uint16_t  bytes_per_sector;
  uint8_t   sectors_per_cluster;
  uint16_t  reserved_sector_count;
  uint8_t   table_count;
  uint16_t  root_entry_count;
  uint16_t  total_sectors_16;
  uint8_t   media_type;
  uint16_t  table_size_16;
  uint16_t  sectors_per_track;
  uint16_t  head_side_count;
  uint32_t  hidden_sector_count;
  uint32_t  total_sectors_32;
 
  boot_sector_ext32_t  boot_sector_ext32;
} __attribute__((packed)) boot_sector_t;

typedef struct {
  uint32_t total_sectors;    // Total sectors in volume (including VBR): 
  uint32_t fat_size;         // FAT size in sectors: 
  uint32_t root_dir_sectors; // The size of the root directory (unless you have FAT32, in which case the size will be 0):
  uint32_t first_data_sector; //The first data sector (that is, the first sector in which directories and files may be stored):
  uint32_t first_fat_sector; // The first sector in the File Allocation Table:
  uint32_t data_sectors;     // The total number of data sectors:
  uint32_t total_clusters;   // The total number of clusters:
  uint32_t root_cluster_32;
  uint32_t first_sector_of_cluster;
} fat32_descriptor_t;


typedef struct {
  char      short_name[11];
  uint8_t   attributes;
  uint8_t   reserved;
  uint8_t   created_tenths;
  uint16_t  time_created; // hour:5 minutes:6 seconds:5 (sec = sec * 2)
  uint16_t  date_created; // year:7 month:4 day:5
  uint16_t  date_accessed;// year:7 month:4 day:5
  uint16_t  cluster_high_16;
  uint16_t  time_modified;// hour:5 minutes:6 seconds:5 (sec = sec * 2)
  uint16_t  date_modified;// year:7 month:4 day:5
  uint16_t  cluster_low_16;
  uint32_t  file_size;
} fat32_directory_t;


boot_sector_t *boot_sector;
fat32_descriptor_t fat32_descriptor;


void fat32_precalc(boot_sector_t *boot_sector, fat32_descriptor_t *desc) {
  desc->total_sectors = boot_sector->total_sectors_32;
  desc->fat_size = boot_sector->boot_sector_ext32.table_size_32;
  desc->first_data_sector = boot_sector->reserved_sector_count + 
    (boot_sector->table_count * desc->fat_size) + desc->root_dir_sectors;
  desc->first_fat_sector = boot_sector->reserved_sector_count; 
  desc->data_sectors = desc->total_sectors - 
    (boot_sector->reserved_sector_count + (boot_sector->table_count * desc->fat_size) + 
    desc->root_dir_sectors);
  desc->total_clusters = desc->data_sectors / boot_sector->sectors_per_cluster;
  desc->root_cluster_32 = boot_sector->boot_sector_ext32.root_cluster;
}

void fat32_dump_directory(fat32_directory_t *dir) {
  DEBUG("FAT32: Dir short_name = [0]=%02X %s\n", dir->short_name[0], dir->short_name);
  DEBUG("FAT32: Dir attributes = %02X\n", dir->attributes);
  DEBUG("FAT32: Dir reserved = %i\n", dir->reserved);
  DEBUG("FAT32: Dir created_tenths = %i\n", dir->created_tenths);
  DEBUG("FAT32: Dir time_created; = %i\n", dir->time_created); 
  DEBUG("FAT32: Dir date_created; = %i\n", dir->date_created) ;
  DEBUG("FAT32: Dir date_accessed = %i\n", dir->date_accessed);
  DEBUG("FAT32: Dir cluster_high_16 = %i\n", dir->cluster_high_16);
  DEBUG("FAT32: Dir time_modified = %i\n", dir->time_modified);
  DEBUG("FAT32: Dir date_modified = %i\n", dir->date_modified);
  DEBUG("FAT32: Dir cluster_low_16 = %i\n", dir->cluster_low_16);
  DEBUG("FAT32: Dir file_size = %i\n", dir->file_size);
}

uint32_t fat32_cluster_to_lba(uint32_t cluster) {
  // return fat32_descriptor.first_data_sector + cluster * boot_sector->sectors_per_cluster - (2 * boot_sector->sectors_per_cluster);
  return ((cluster - 2) * boot_sector->sectors_per_cluster) + fat32_descriptor.first_data_sector;
}

void init_kernel_fat32() {
  // read boot sector
  uint8_t *buffer = calloc(512, 1);
  ata_read_one(buffer, 0);

  boot_sector = (boot_sector_t*)buffer;
  DEBUG("FAT32: BOOT bootjmp = %02X %02X %02X\n", boot_sector->bootjmp[0], boot_sector->bootjmp[1], boot_sector->bootjmp[2]);
  DEBUG("FAT32: BOOT oem_name = %8s\n", boot_sector->oem_name);
  DEBUG("FAT32: BOOT bytes_per_sector = %i\n", boot_sector->bytes_per_sector);
  DEBUG("FAT32: BOOT sectors_per_cluster = %i\n", boot_sector->sectors_per_cluster);
  DEBUG("FAT32: BOOT reserved_sector_count = %i\n", boot_sector->reserved_sector_count);
  DEBUG("FAT32: BOOT table_count = %i\n", boot_sector->table_count);
  DEBUG("FAT32: BOOT root_entry_count = %i\n", boot_sector->root_entry_count);
  DEBUG("FAT32: BOOT total_sectors_16 = %i\n", boot_sector->total_sectors_16);
  DEBUG("FAT32: BOOT media_type = %i\n", boot_sector->media_type);
  DEBUG("FAT32: BOOT table_size_16 = %i\n", boot_sector->table_size_16);
  DEBUG("FAT32: BOOT sectors_per_track = %i\n", boot_sector->sectors_per_track);
  DEBUG("FAT32: BOOT head_side_count = %i\n", boot_sector->head_side_count);
  DEBUG("FAT32: BOOT hidden_sector_count = %i\n", boot_sector->hidden_sector_count);
  DEBUG("FAT32: BOOT total_sectors_32 = %i\n", boot_sector->total_sectors_32);
  
  assert(boot_sector->total_sectors_16 == 0); // Not supporting FAT12 or FAT16
  assert(boot_sector->total_sectors_32 != 0); // FAST32
  assert(boot_sector->sectors_per_cluster == 1); // For now we are only supporting 1:1 (secotr : cluster)

  boot_sector_ext32_t *boot_ext32 = &boot_sector->boot_sector_ext32;
  DEBUG("FAT32: BOOT-Ext table_size_32 = %i\n", boot_ext32->table_size_32);
  DEBUG("FAT32: BOOT-Ext extended_flags = %i\n", boot_ext32->extended_flags);
  DEBUG("FAT32: BOOT-Ext fat_version = %i\n", boot_ext32->fat_version);
  DEBUG("FAT32: BOOT-Ext root_cluster = %i\n", boot_ext32->root_cluster);
  DEBUG("FAT32: BOOT-Ext fat_info = %i\n", boot_ext32->fat_info);
  DEBUG("FAT32: BOOT-Ext backup_BS_sector = %i\n", boot_ext32->backup_BS_sector);
  DEBUG("FAT32: BOOT-Ext drive_number = %i\n", boot_ext32->drive_number);
  DEBUG("FAT32: BOOT-Ext boot_signature = %i\n", boot_ext32->boot_signature);
  DEBUG("FAT32: BOOT-Ext volume_id = %i\n", boot_ext32->volume_id);
  DEBUG("FAT32: BOOT-Ext volume_label = %11s\n", boot_ext32->volume_label);
  DEBUG("FAT32: BOOT-Ext fat_type_label = %8s\n", boot_ext32->fat_type_label);  

  fat32_precalc(boot_sector, &fat32_descriptor);
  DEBUG("FAT32: Precalc total_sectors = %i\n", fat32_descriptor.total_sectors);
  DEBUG("FAT32: Precalc fat_size = %i\n", fat32_descriptor.fat_size);
  DEBUG("FAT32: Precalc root_dir_sectors = %i\n", fat32_descriptor.root_dir_sectors);
  DEBUG("FAT32: Precalc first_data_sector = %i\n", fat32_descriptor.first_data_sector);
  DEBUG("FAT32: Precalc first_fat_sector = %i\n", fat32_descriptor.first_fat_sector);
  DEBUG("FAT32: Precalc data_sectors = %i\n", fat32_descriptor.data_sectors);
  DEBUG("FAT32: Precalc total_clusters = %i\n", fat32_descriptor.total_clusters);
  DEBUG("FAT32: Precalc root_cluster_32 = %i\n", fat32_descriptor.root_cluster_32);
  DEBUG("FAT32: Precalc first_sector_of_cluster = %i\n", fat32_descriptor.first_sector_of_cluster);  

  uint8_t dir_cluster[512];
  memset(dir_cluster, 0, 512);
  ata_read_one(dir_cluster, fat32_cluster_to_lba(fat32_descriptor.root_cluster_32));


  fat32_directory_t *dir = (fat32_directory_t*)dir_cluster;
  for (int i=0; i<512/32; i++) {
    DEBUG("---------------------------------------\n");
    fat32_dump_directory(dir);
    dir++;
  }

}


void kmain(unsigned long magic __UNUSED__, multiboot_info_t* mbi_phys) {
  init_kernel_serial();
  init_kernel_vesa(TO_VMA_PTR(multiboot_info_t *, mbi_phys));
  init_kernel_pagging();
  init_kernel_window_manager();

  init_kernel_pic();
  init_kernel_isr();
  init_kernel_timer();
  init_kernel_keyboard();
  init_kernel_mouse();
  //init_kernel_pci();

  init_kernel_ata();
  init_kernel_fat32();

  create_kernel_process((void*)idle);
  create_user_process(desktop);

  do_first_task_jump();
  while(1) { }
}
