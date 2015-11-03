/*
 * utility
 *
 *  Created on: 2014年12月23日
 *      Author: jugo
 */

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <qrencode.h>
#include <png.h>

using namespace std;

template<class T>
string ConvertToString(T value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

inline int spliteData(char *pData, const char * delim, vector<string> &vData)
{
	char * pch;

	pch = strtok( pData, delim );
	while ( pch != NULL )
	{
		vData.push_back( string( pch ) );
		pch = strtok( NULL, delim );
	}

	return vData.size();
}

inline bool mkdirp(string strPath)
{
	size_t found = strPath.find_last_of( "/\\" );
	string strDir = strPath.substr( 0, found );

	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

	// const cast for hack
	char* p = const_cast<char*>( strDir.c_str() );

	// Do mkdir for each slash until end of string or error
	while ( *p != '\0' )
	{
		// Skip first character
		++p;

		// Find first slash or end
		while ( *p != '\0' && *p != '/' )
			++p;

		// Remember value from p
		char v = *p;

		// Write end of string at p
		*p = '\0';

		// Create folder from path to '\0' inserted at p
		if ( mkdir( strDir.c_str(), mode ) == -1 && errno != EEXIST )
		{
			*p = v;
			return false;
		}

		// Restore path to it's former glory
		*p = v;
	}

	return true;
}

inline const string currentDateTime()
{
	time_t now = time( 0 );
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime( &now );
	strftime( buf, sizeof(buf), "%Y-%m-%d %X", &tstruct );

	return buf;
}

#define INCHES_PER_METER (100.0/2.54)
unsigned int fg_color[4] = { 0, 0, 0, 255 };
unsigned int bg_color[4] = { 255, 255, 255, 255 };

int writePNG(QRcode *qrcode, const char *outfile)
{
	static FILE *fp; // avoid clobbering by setjmp.
	png_structp png_ptr;
	png_infop info_ptr;
	png_colorp palette;
	png_byte alpha_values[2];
	unsigned char *row, *p, *q;
	int x, y, xx, yy, bit;
	int realwidth;
	int margin = 4;
	int size = 5;
	int dpi = 72;

	realwidth = (qrcode->width + margin * 2) * size;
	row = (unsigned char *) malloc( (realwidth + 7) / 8 );
	if ( row == NULL )
	{
		fprintf( stderr, "Failed to allocate memory.\n" );
		exit (EXIT_FAILURE);
	}

	if ( outfile[0] == '-' && outfile[1] == '\0' )
	{
		fp = stdout;
	}
	else
	{
		fp = fopen( outfile, "wb" );
		if ( fp == NULL )
		{
			fprintf( stderr, "Failed to create file: %s\n", outfile );
			perror( NULL );
			exit (EXIT_FAILURE);
		}
	}

	png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if ( png_ptr == NULL )
	{
		fprintf( stderr, "Failed to initialize PNG writer.\n" );
		exit (EXIT_FAILURE);
	}

	info_ptr = png_create_info_struct( png_ptr );
	if ( info_ptr == NULL )
	{
		fprintf( stderr, "Failed to initialize PNG write.\n" );
		exit (EXIT_FAILURE);
	}

	if ( setjmp( png_jmpbuf(png_ptr) ) )
	{
		png_destroy_write_struct( &png_ptr, &info_ptr );
		fprintf( stderr, "Failed to write PNG image.\n" );
		exit (EXIT_FAILURE);
	}

	palette = (png_colorp) malloc( sizeof(png_color) * 2 );
	if ( palette == NULL )
	{
		fprintf( stderr, "Failed to allocate memory.\n" );
		exit (EXIT_FAILURE);
	}
	palette[0].red = fg_color[0];
	palette[0].green = fg_color[1];
	palette[0].blue = fg_color[2];
	palette[1].red = bg_color[0];
	palette[1].green = bg_color[1];
	palette[1].blue = bg_color[2];
	alpha_values[0] = fg_color[3];
	alpha_values[1] = bg_color[3];
	png_set_PLTE( png_ptr, info_ptr, palette, 2 );
	png_set_tRNS( png_ptr, info_ptr, alpha_values, 2, NULL );

	png_init_io( png_ptr, fp );
	png_set_IHDR( png_ptr, info_ptr, realwidth, realwidth, 1,
	PNG_COLOR_TYPE_PALETTE,
	PNG_INTERLACE_NONE,
	PNG_COMPRESSION_TYPE_DEFAULT,
	PNG_FILTER_TYPE_DEFAULT );
	png_set_pHYs( png_ptr, info_ptr, dpi * INCHES_PER_METER, dpi * INCHES_PER_METER,
	PNG_RESOLUTION_METER );
	png_write_info( png_ptr, info_ptr );

	/* top margin */
	memset( row, 0xff, (realwidth + 7) / 8 );
	for ( y = 0; y < margin * size ; y++ )
	{
		png_write_row( png_ptr, row );
	}

	/* data */
	p = qrcode->data;
	for ( y = 0; y < qrcode->width ; y++ )
	{
		bit = 7;
		memset( row, 0xff, (realwidth + 7) / 8 );
		q = row;
		q += margin * size / 8;
		bit = 7 - (margin * size % 8);
		for ( x = 0; x < qrcode->width ; x++ )
		{
			for ( xx = 0; xx < size ; xx++ )
			{
				*q ^= (*p & 1) << bit;
				bit--;
				if ( bit < 0 )
				{
					q++;
					bit = 7;
				}
			}
			p++;
		}
		for ( yy = 0; yy < size ; yy++ )
		{
			png_write_row( png_ptr, row );
		}
	}
	/* bottom margin */
	memset( row, 0xff, (realwidth + 7) / 8 );
	for ( y = 0; y < margin * size ; y++ )
	{
		png_write_row( png_ptr, row );
	}

	png_write_end( png_ptr, info_ptr );
	png_destroy_write_struct( &png_ptr, &info_ptr );

	fclose( fp );
	free( row );
	free( palette );

	return 0;
}

QRcode *encode(const unsigned char *intext, int length)
{
	QRcode *code;

	code = QRcode_encodeString( (char *) intext, 0, QR_ECLEVEL_L, QR_MODE_8, 1 );

	return code;
}

inline void qrencode(const unsigned char *intext, int length, const char *outfile)
{
	QRcode *qrcode;

	qrcode = encode( intext, length );
	if ( qrcode == NULL )
	{
		printf( "Failed to encode the input data" );
		return;
	}

	writePNG( qrcode, outfile );

	QRcode_free( qrcode );
}
