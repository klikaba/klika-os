// Based on LevOS ata driver - https://github.com/levex/osdev/blob/master/drivers/ata.c

#include <ata.h>
#include <kernel.h>
#include <mmu_heap.h>
#include <string.h>
#include <x86.h>
#include <isr.h>

uint8_t ata_pm = 0; /* Primary master exists? */
uint8_t ide_buf[512];
uint8_t ata_drive;
char ata_device_name[40];

void ide_select_drive(uint8_t bus, uint8_t i) {
  if(bus == ATA_PRIMARY)
    if(i == ATA_MASTER)
      outp(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
    else outp(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
  else
    if(i == ATA_MASTER)
      outp(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
    else outp(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
}

void ide_primary_irq() {
//  DEBUG("ATA: primary IRQ\n");
  pic_acknowledge(ATA_PRIMARY_IRQ);
}

void ide_secondary_irq() {
//  DEBUG("ATA: secondary IRQ\n");
  pic_acknowledge(ATA_SECONDARY_IRQ);
}

bool ide_identify(uint8_t bus, uint8_t drive) {
  ide_select_drive(bus, drive);

  uint16_t io = bus == ATA_PRIMARY ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

  /* ATA specs say these values must be zero before sending IDENTIFY */
  outp(io + ATA_REG_SECCOUNT0, 0);
  outp(io + ATA_REG_LBA0, 0);
  outp(io + ATA_REG_LBA1, 0);
  outp(io + ATA_REG_LBA2, 0);

  /* Now, send IDENTIFY */
  outp(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
//  DEBUG("ATA: Sent IDENTIFY\n");

  /* Now, read status port */
  uint8_t status = inp(io + ATA_REG_STATUS);
  if (status) {
//    DEBUG("ATA: Satus %i\n", status);

    /* Now, poll untill BSY is clear. */
    while((inp(io + ATA_REG_STATUS) & ATA_SR_BSY) != 0);
pm_stat_read:   
//    DEBUG("ATA: Busy clear\n");
    
    status = inp(io + ATA_REG_STATUS);
    if (status & ATA_SR_ERR) {
//      DEBUG("ATA: %s%s has ERR set. Disabled.\n", bus == ATA_PRIMARY ? "Primary" : "Secondary", drive == ATA_PRIMARY ? " master" : " slave");
      return false;
    }

    while(!(status & ATA_SR_DRQ)) {
      goto pm_stat_read;
    }
//    DEBUG("ATA: %s%s is online.\n", bus == ATA_PRIMARY ? "Primary" : "Secondary", drive == ATA_PRIMARY ? " master" : " slave");
    
    /* Now, actually read the data */
//    DEBUG("ATA: Reading ide_buffer ...\n");
    for(int i = 0; i<256; i++) {
      *(uint16_t *)(ide_buf + i*2) = inpw(io + ATA_REG_DATA);
      DEBUG("%02X %02X ", *(uint16_t *)(ide_buf + i*2) >> 8, *(uint16_t *)(ide_buf + i*2) &0xFF);
    }
    DEBUG("\n");
    return true;
  }
  else {
//    DEBUG("ATA: Status 0\n");
  }
  return false;
}

void ide_400ns_delay(uint16_t io) {
  for(int i = 0;i < 4; i++) {
    inp(io + ATA_REG_ALTSTATUS);
  }
}

void ide_poll(uint16_t io) {
  for(int i=0; i< 4; i++) {
    inp(io + ATA_REG_ALTSTATUS);
  }
retry:;
  uint8_t status = inp(io + ATA_REG_STATUS);
//  DEBUG("ATA: testing for BSY\n");

  if(status & ATA_SR_BSY) {
    goto retry;
  }
//  DEBUG("ATA: BSY cleared\n");
retry2: 
  status = inp(io + ATA_REG_STATUS);
  if(status & ATA_SR_ERR) {
    HALT_AND_CATCH_FIRE("ERR set, device failure!\n");
  }

//  DEBUG("ATA: testing for DRQ\n");
  
  if (!(status & ATA_SR_DRQ)) {
    goto retry2;
  }
//  DEBUG("ATA: DRQ set, ready to PIO!\n");
  return;
}

uint8_t ata_write_one(uint8_t *buf, uint32_t lba) {
  /* We only support 28bit LBA so far */
  uint16_t io = 0;
  switch(ata_drive) {
    case (ATA_PRIMARY << 1 | ATA_MASTER):
      io = ATA_PRIMARY_IO;
      ata_drive = ATA_MASTER;
      break;
    case (ATA_PRIMARY << 1 | ATA_SLAVE):
      io = ATA_PRIMARY_IO;
      ata_drive = ATA_SLAVE;
      break;
    case (ATA_SECONDARY << 1 | ATA_MASTER):
      io = ATA_SECONDARY_IO;
      ata_drive = ATA_MASTER;
      break;
    case (ATA_SECONDARY << 1 | ATA_SLAVE):
      io = ATA_SECONDARY_IO;
      ata_drive = ATA_SLAVE;
      break;
    default:
      return 0;
  }

  uint8_t cmd = (ata_drive == ATA_MASTER ? 0xE0 : 0xF0);

  outp(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((lba >> 24 & 0x0F))));
  outp(io + 1, 0x00);

  // Single sector write
  outp(io + ATA_REG_SECCOUNT0, 1); 

  // Select LBA
  outp(io + ATA_REG_LBA0, (uint8_t)((lba)));
  outp(io + ATA_REG_LBA1, (uint8_t)((lba) >> 8));
  outp(io + ATA_REG_LBA2, (uint8_t)((lba) >> 16));

  // Select write command
  outp(io + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

  // Wait until ready
  ide_poll(io);

  for(int i = 0; i < 256; i++) {
    outpw(io + ATA_REG_DATA, *(uint16_t *)(buf + i * 2));
  }
  ide_400ns_delay(io);
  return 1;
}

uint8_t ata_read_one(uint8_t *buf, uint32_t lba) {
  /* We only support 28bit LBA so far */
  uint16_t io = 0;
  switch(ata_drive) {
    case (ATA_PRIMARY << 1 | ATA_MASTER):
      io = ATA_PRIMARY_IO;
      ata_drive = ATA_MASTER;
      break;
    case (ATA_PRIMARY << 1 | ATA_SLAVE):
      io = ATA_PRIMARY_IO;
      ata_drive = ATA_SLAVE;
      break;
    case (ATA_SECONDARY << 1 | ATA_MASTER):
      io = ATA_SECONDARY_IO;
      ata_drive = ATA_MASTER;
      break;
    case (ATA_SECONDARY << 1 | ATA_SLAVE):
      io = ATA_SECONDARY_IO;
      ata_drive = ATA_SLAVE;
      break;
    default:
      return 0;
  }

  uint8_t cmd = (ata_drive == ATA_MASTER ? 0xE0 : 0xF0);

  outp(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((lba >> 24 & 0x0F))));
  outp(io + 1, 0x00);

  // Single sector read
  outp(io + ATA_REG_SECCOUNT0, 1);

  // Select LBA
  outp(io + ATA_REG_LBA0, (uint8_t)((lba)));
  outp(io + ATA_REG_LBA1, (uint8_t)((lba) >> 8));
  outp(io + ATA_REG_LBA2, (uint8_t)((lba) >> 16));

  // Select read command
  outp(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  // Wait until ready
  ide_poll(io);

  for(int i = 0; i < 256; i++) {
    *(uint16_t *)(buf + i * 2) = inpw(io + ATA_REG_DATA);
  }
  ide_400ns_delay(io);
  return 1;
}

void ata_probe() {
  if(ide_identify(ATA_PRIMARY, ATA_MASTER)) {
//    DEBUG("ATA: YESSS\n");
    ata_pm = 1;
    /* Now, process the IDENTIFY data */
    /* Model goes from W#27 to W#46 */    
    memset(ata_device_name, 0 , 40);
    for(int i = 0; i < 40; i += 2) {
      ata_device_name[i] = ide_buf[ATA_IDENT_MODEL + i + 1];
      ata_device_name[i + 1] = ide_buf[ATA_IDENT_MODEL + i];
    }
//    DEBUG("ATA: Device: %s\n", ata_device_name);
    ata_drive = (ATA_PRIMARY << 1) | ATA_MASTER;
  }
  ide_identify(ATA_PRIMARY, ATA_SLAVE);
}

void init_kernel_ata() {
//  DEBUG("ATA: Checking for ATA drives\n");

  register_interrupt_handler(ISR_IRQE, ide_primary_irq);
  irq_enable(PIC_IRQE);

  register_interrupt_handler(ISR_IRQF, ide_secondary_irq);
  irq_enable(PIC_IRQF);  

  ata_probe();
}
