#include "ARPong.h"

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

GLuint load_texture(const char *filename)
{
	struct jpeg_decompress_struct jpeg_info;
	struct jpeg_error_mgr jerr;
	
	unsigned int width, height;

	FILE *file = fopen(filename, "rb");
	if(file == NULL)
		return -1;

	jpeg_create_decompress(&jpeg_info);
	jpeg_info.err = jpeg_std_error(&jerr);
	jpeg_stdio_src(&jpeg_info, file);
	jpeg_read_header(&jpeg_info, TRUE);
	
	width = jpeg_info.image_width;
	height = jpeg_info.image_height;

	/* Explicit cast necessary in C++ only */
	unsigned char *data = (unsigned char*)malloc(width*height*3);
	jpeg_start_decompress(&jpeg_info);
	while(jpeg_info.output_scanline < jpeg_info.output_height)
	{
		unsigned char *line = data + 3*width*jpeg_info.output_scanline;
		jpeg_read_scanlines(&jpeg_info, &line, 1);
	}
	jpeg_finish_decompress(&jpeg_info);
	jpeg_destroy_decompress(&jpeg_info);

	GLuint texture;
	glGenTextures(1, &texture);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);
	free(data);

	return texture;
}
