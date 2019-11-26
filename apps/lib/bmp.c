#include <bmp.h>
#include <stdio.h>
#include <malloc.h>
#include <klika_os.h>

void bmp_from_file(char *filename, bmp_image_t *bmp_out) {
	FILE *file = fopen(filename, "r");
	DEBUG("Open BMP %s", filename);
	if (file == NULL) {
		DEBUG("File not found");
		return;
	}
	DEBUG("File found %X", file);
	uint32_t size = fsize(file);


	void *buffer = malloc(size + 512);
	DEBUG("Buffer allocated %X", buffer);

	fread(buffer, size, 1, file);

	DEBUG("Buffer read %X", buffer);

	bmp_out->buffer = buffer;
	bmp_out->header = (bmp_header_t*)bmp_out->buffer;
	bmp_out->data = (uint32_t*)(((uint8_t*)buffer) + bmp_out->header->offset);
}

void bmp_close(bmp_image_t *bmp_image) {
	free(bmp_image->buffer);
}