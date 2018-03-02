#include "world.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <png.h>
#include <cstdlib>

using namespace std;

void save_image(unsigned char *pixels, int width, int height)
{
	int i,d;
	png_bytep row = NULL;

	FILE *fp = fopen("snap.png", "wb");
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info = png_create_info_struct(png);
	if (setjmp(png_jmpbuf(png))){
		cout << "Error taking snapshot" << endl;
	}
	png_init_io(png, fp);
	png_set_IHDR(
		png,
		info,
		width, height,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);
	
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));
	for (i=0; i<height; i++) {
		for (d=0;d<width*3;d++) {
			row[d]=pixels[i*width*3+d];
		}
		png_write_row(png, row);
	}
	free(row);
	png_write_end(png, NULL);
	fclose(fp);
}

int main(int argc,char* argv[])
{
	int width = 500;
	int height = 500;

	//int width = 1500;
	//int height = 2668;
	//int width = 750;
	//int height = 1334;

	//float dist = toca_triangulo(glm::vec3(0,0,-1),glm::vec3(0,0,1),glm::vec3(-1,-1,0),glm::vec3(1,-1,0),glm::vec3(0,1,0));
	//cout << dist << endl;
	world_init();
	unsigned char *pixels = world_display(width,height);

	save_image(pixels,width,height);
	free(pixels);

	return 0;
}
