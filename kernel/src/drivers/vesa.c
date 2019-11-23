#include <kernel.h>
#include <stdbool.h>
#include <string.h>
#include <vesa.h>
#include <x86.h>

struct video_info_struct vesa_video_info;

static void extract_framebuffer_info(multiboot_info_t *mbi) {
  multiboot_tag_t *tag;
  for (
    tag = (multiboot_tag_t *) mbi->tags;
    tag->type != MULTIBOOT_TAG_TYPE_END;
    tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7))
    ) {
      if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
        struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
        void *fb = (void *) (unsigned long) tagfb->common.framebuffer_addr;
            
        vesa_video_info.linear_addr = (uint64_t)fb;
        vesa_video_info.addr = (uint64_t)KERNEL_VIDEO_MEMORY;
        vesa_video_info.width = tagfb->common.framebuffer_width;
        vesa_video_info.height = tagfb->common.framebuffer_height;
        vesa_video_info.bits = tagfb->common.framebuffer_bpp;
        vesa_video_info.pitch = tagfb->common.framebuffer_pitch;
        vesa_video_info.type = tagfb->common.framebuffer_type;
        DEBUG("VESA: Frame buffer addr: 0x%X\n", vesa_video_info.addr);
        DEBUG("VESA: Frame buffer linear addr: 0x%X\n", vesa_video_info.linear_addr);
        DEBUG("VESA: Frame buffer info: %i x %i : %ibpp\n", vesa_video_info.width, vesa_video_info.height, vesa_video_info.bits);
        DEBUG("VESA: Frame buffer pitch: %i\n", vesa_video_info.pitch);
        DEBUG("VESA: Frame buffer type: %i\n", vesa_video_info.type);
      }
  }
}

void init_kernel_vesa(multiboot_info_t *mbi) {
    extract_framebuffer_info(mbi);
}

