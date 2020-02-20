#include <fat.h>
#include <dosfs.h>
#include <kernel.h>
#include <ata.h>

VOLINFO dosfs_volume_info;

// Implementation needed for 'dosfs'
uint32_t DFS_ReadSector(uint8_t unit __UNUSED__, uint8_t *buffer, uint32_t sector, uint32_t count) {
  for (uint32_t i=0; i<count; i++) {
    if (ata_read_one(buffer, sector) == 0) {
      return 1; // error
    }
    buffer += SECTOR_SIZE;
  }
  return 0; // success
}

// Implementation needed for 'dosfs'
uint32_t DFS_WriteSector(uint8_t unit __UNUSED__, uint8_t *buffer, uint32_t sector, uint32_t count) {
  for (uint32_t i=0; i<count; i++) {
    if (ata_write_one(buffer, sector) == 0) {
      return 1; // error
    }
    buffer += SECTOR_SIZE;
  }
  return 0; // success
}

void init_kernel_fat() {
  uint8_t scratch_sector[SECTOR_SIZE];
  uint32_t pstart, psize;
  uint8_t pactive, ptype;

  pstart = DFS_GetPtnStart(0, scratch_sector, 0, &pactive, &ptype, &psize);
  if (pstart == 0xffffffff) {
    DEBUG("DOSFS: Cannot find first partition\n");
    return ;
  }
  DEBUG("DOSFS: Partition 0 start sector 0x%-08.8lX active %-02.2hX type %-02.2hX size %-08.8lX\n", pstart, pactive, ptype, psize);

  if (DFS_GetVolInfo(0, scratch_sector, pstart, &dosfs_volume_info)) {
    DEBUG("DOSFS: Error getting volume information\n");
    return;
  }
  DEBUG("DOSFS: Volume label '%s'\n", dosfs_volume_info.label);
  DEBUG("DOSFS: %d sector/s per cluster, %d reserved sector/s, volume total %d sectors.\n", dosfs_volume_info.secperclus, dosfs_volume_info.reservedsecs, dosfs_volume_info.numsecs);
  DEBUG("DOSFS: %d sectors per FAT, first FAT at sector #%d, root dir at #%d.\n",dosfs_volume_info.secperfat,dosfs_volume_info.fat1,dosfs_volume_info.rootdir);
  DEBUG("DOSFS: (For FAT32, the root dir is a CLUSTER number, FAT12/16 it is a SECTOR number)\n");
  DEBUG("DOSFS: %d root dir entries, data area commences at sector #%d.\n",dosfs_volume_info.rootentries,dosfs_volume_info.dataarea);
  DEBUG("DOSFS: %d clusters (%d bytes) in data area, filesystem IDd as ", dosfs_volume_info.numclusters, dosfs_volume_info.numclusters * dosfs_volume_info.secperclus * SECTOR_SIZE);
  if (dosfs_volume_info.filesystem == FAT12) {
    DEBUG("DOSFS: FAT12.\n");
  }
  else if (dosfs_volume_info.filesystem == FAT16) {
    DEBUG("DOSFS: FAT16.\n");
  }
  else if (dosfs_volume_info.filesystem == FAT32) {
    DEBUG("DOSFS: FAT32.\n");
  }
  else {
    DEBUG("DOSFS: [unknown]\n");
  }
}
