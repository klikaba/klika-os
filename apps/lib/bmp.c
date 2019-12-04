#include <bmp.h>
#include <stdio.h>
#include <malloc.h>
#include <klika_os.h>
#include <stdlib.h>
#include <gfx.h>

void bmp_from_file(char *filename, bmp_image_t *bmp_out) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		return;
	}
	uint32_t size = fsize(file);


	void *buffer = malloc(size + 512);

	fread(buffer, size, 1, file);

	bmp_out->buffer = buffer;
	bmp_out->header = (bmp_header_t*)bmp_out->buffer;
	bmp_out->data = (uint32_t*)(((uint8_t*)buffer) + bmp_out->header->offset);
	// Usually header is negative (means that image is stored : top - to -bottom)
	// bmp_out->header->height_px = abs(bmp_out->header->height_px);

}

void bmp_close(bmp_image_t *bmp_image) {
	free(bmp_image->buffer);
}

void bmp_blit(context_t* context, bmp_image_t *bmp, int x, int y) {
	int height = bmp->header->height_px;
	int width = bmp->header->width_px;
	// If negative : stored TOP -> BOTTOM
	if (height < 0) {
		gfx_blit(context, x, y, width, -height, bmp->data);
	}
	else {
		// If positive : stored BOTTOM -> TOP ... make inverted blit (height - i)
		uint32_t *src = bmp->data;
		for (int i=0; i<height; i++) {
			for (int j=0; j<width; j++) {
				CONTEXT_32[FIRST_PIXEL(x+j, y+(height-i))] = *(src + (j + i * width));
			}
		}		
	}
}