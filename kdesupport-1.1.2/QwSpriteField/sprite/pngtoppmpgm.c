/*  Author:  Warwick Allison                         */#include <stdio.h>
/*    Date:  04 Jan 1997                             */static char*cmd;void
/*   eMail:  warwick@cs.uq.edu.au                    */Usage(){fprintf(stderr,"

Usage:  %s [-c] [-h | -H x y] filename.png

  Convert a 4-channel PNG file into a PPM file and a PGM file.

  Reads filename.png then writes filename.ppm and filename.pgm
     filename.ppm is RGB channels
     filename.pgm is alpha channel

  -c      auto-cropping to non-transparent rectangle
  -h      HOTSPOT comment in PPM file, center of original
  -H x y  HOTSPOT comment in PPM file, given position

"/*********************************************************************/,cmd);}

#include <png.h>
#include <ppm.h>

main(int argc, char** argv)
{
	char* pngfile=0;
	char* ppmfile;
	char* pgmfile;
	char* ext;
	char header[8];
	FILE *png;
	FILE *ppm;
	FILE *pgm;
	png_structp png_ptr;
	png_infop info_ptr;
	png_infop end_info;
	png_bytep* row_pointers;
	int h;
	int crop=0;
	int hotspot=0;
	int hotx,hoty;
	int arg;

	cmd=argv[0];

	for (arg=1; arg<argc; arg++) {
		if (0==strcmp(argv[arg],"-c")) {
			crop=1;
		} else if (0==strcmp(argv[arg],"-h")) {
			hotspot=1;
		} else if (0==strcmp(argv[arg],"-H")) {
			hotx=atoi(argv[++arg]);
			hoty=atoi(argv[++arg]);
			hotspot=2;
		} else {
			if (!pngfile) {
				pngfile=argv[arg];
			} else {
				Usage();
				exit(1);
			}
		}
	}

	ppmfile=strdup(pngfile);
	ext=strstr(ppmfile,".png");
	if (!ext) {
		fprintf(stderr,"%s:  PNG file must have extension .png\n");
		exit(1);
	}
	strcpy(ext,".ppm");
	pgmfile=strdup(pngfile);
	ext=strstr(pgmfile,".png");
	strcpy(ext,".pgm");

	ppm = fopen(ppmfile, "wb");
	if (!ppm) { perror(ppmfile); exit(1); }
	pgm = fopen(pgmfile, "wb");
	if (!pgm) { perror(pgmfile); exit(1); }

	/* Boilerplater PNG access code [gee I hate C] */

	png = fopen(pngfile, "rb");
	if (!png) { perror(pngfile); exit(1); }
	fread(header, 1, 8, png);
	if (!png_check_sig(header, 8)) {
		fprintf(stderr,"%s:  %s is not a valid PNG file\n",cmd,pngfile);
		exit(1);
	}
	fseek(png,0,SEEK_SET);

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	if (!png_ptr) exit(1);

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) exit(1);

	end_info = png_create_info_struct(png_ptr);
	if (!end_info) exit(1);

	png_init_io(png_ptr, png);

	png_read_info(png_ptr, info_ptr);

	if (info_ptr->channels!=4) {
		fprintf(stderr,"%s:  found %d channels, expected 4\n",cmd,info_ptr->channels);
		exit(1);
	}

	if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE &&
		info_ptr->bit_depth < 8)
	png_set_expand(png_ptr);

	if (info_ptr->valid & PNG_INFO_tRNS)
	png_set_expand(png_ptr);

	if (info_ptr->bit_depth == 16)
		png_set_strip_16(png_ptr);

	if (info_ptr->bit_depth < 8)
		png_set_packing(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	row_pointers=(png_bytep*)malloc(sizeof(png_bytep)*info_ptr->height);
	{
		int y;
		for (y=0; y<info_ptr->height; y++) {
			row_pointers[y]=(unsigned char*)malloc(info_ptr->rowbytes);
		}
	}
	png_read_image(png_ptr, row_pointers);

	{
		int top,bottom;
		int left,right;
		int opaque=0;
		int y;

		if (crop) {
			for (left=0; left<info_ptr->width*4-4 && !opaque; left+=4) {
				for (y=0; y<info_ptr->height && !opaque; y++) {
					if (row_pointers[y][left+3]!=0) opaque=1;
				}
			}
			left-=4;
			opaque=0;
			for (right=info_ptr->width*4-4; right>=0 && !opaque; right-=4) {
				for (y=0; y<info_ptr->height && !opaque; y++) {
					if (row_pointers[y][right+3]!=0) opaque=1;
				}
			}
			right+=4;
			opaque=0;
			for (top=0; top<info_ptr->height && !opaque; top++) {
				int x;
				for (x=left; x<=right && !opaque; x+=4) {
					if (row_pointers[top][x+3]!=0) opaque=1;
				}
			}
			top--;
			opaque=0;
			for (bottom=info_ptr->height-1; bottom>=0 && !opaque; bottom--) {
				int x;
				for (x=left; x<=right && !opaque; x+=4) {
					if (row_pointers[bottom][x+3]!=0) opaque=1;
				}
			}
			bottom++;
		} else {
			top=left=0;
			right=info_ptr->width*4-4;
			bottom=info_ptr->height-1;
		}

		if (hotspot==1)
			fprintf(ppm,"P6\n# HOTSPOT %d %d\n%d %d\n%d\n",
				info_ptr->width/2-left/4,info_ptr->height/2-top,
				right/4-left/4+1,bottom-top+1,255);
		else if (hotspot==2)
			fprintf(ppm,"P6\n# HOTSPOT %d %d\n%d %d\n%d\n",
				hotx-left/4,hoty-top,
				right/4-left/4+1,bottom-top+1,255);
		else
			fprintf(ppm,"P6\n%d %d\n%d\n",
				right/4-left/4+1,bottom-top+1,255);
		fprintf(pgm,"P5\n%d %d\n%d\n",right/4-left/4+1,bottom-top+1,255);

		for (y=top; y<=bottom; y++) {
			int x;
			for (x=left; x<=right; x+=4) {
				fwrite(row_pointers[y]+x,1,3,ppm);
				fwrite(row_pointers[y]+x+3,1,1,pgm);
			}
		}
	}

	fclose(ppm);
	fclose(pgm);

	png_read_end(png_ptr, end_info);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
}
