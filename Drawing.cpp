// Drawing.cpp: implementation of the Drawing class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Drawing.h"

void Dot(unsigned long *buffer, int x, int y, DWORD pixel)
{
	buffer[y * g_pitch + x] = pixel;
}

void Line(unsigned long *buffer, int x, int y, int x2, int y2, DWORD pixel)
{
	int dx, dy, long_d, short_d;
	int d, add_dh, add_dl;
	int inc_xh, inc_yh, inc_xl, inc_yl;
	int i;

	dx = x2 - x;
	dy = y2 - y;

	if (dx < 0) {
		dx = -dx;
		inc_xh = -1;
		inc_xl = -1;
	} else {
		inc_xh = 1;
		inc_xl = 1;
	}

	if (dy < 0) {
		dy = -dy;
		inc_yh = -1;
		inc_yl = -1;
	} else {
		inc_yh = 1;
		inc_yl = 1;
	}

	if (dx > dy) {
		long_d = dx;
		short_d = dy;
		inc_yl = 0;
	} else {
		long_d = dy;
		short_d = dx;
		inc_xl = 0;
	}

	d = 2 * short_d - long_d;
	add_dl = 2 * short_d;
	add_dh = 2 * short_d - 2 * long_d;

	for (i = 0; i <= long_d; i++) {
		if (y >= 0 && y < g_height && x >= 0 && x < g_width) {
			buffer[y * g_pitch + x] = pixel;
		}

		if (d >= 0) {
			x += inc_xh;
			y += inc_yh;
			d += add_dh;
		} else {
			x += inc_xl;
			y += inc_yl;
			d += add_dl;
		}
	}
}

void Line(unsigned long *buffer, int x, int y, int x2, int y2, DWORD start, DWORD end)
{
	int dx, dy, long_d, short_d;
	int d, add_dh, add_dl;
	int inc_xh, inc_yh, inc_xl, inc_yl;
	int i;
	DWORD colour = start;
	unsigned char radj, gadj, badj;

	dx = x2 - x;
	dy = y2 - y;

	if (!dx && !dy) return;

	if (dx < 0) {
		dx = -dx;
		inc_xh = -1;
		inc_xl = -1;
	} else {
		inc_xh = 1;
		inc_xl = 1;
	}

	if (dy < 0) {
		dy = -dy;
		inc_yh = -1;
		inc_yl = -1;
	} else {
		inc_yh = 1;
		inc_yl = 1;
	}

	if (dx > dy) {
		long_d = dx;
		short_d = dy;
		inc_yl = 0;
	} else {
		long_d = dy;
		short_d = dx;
		inc_xl = 0;
	}

	d = 2 * short_d - long_d;
	add_dl = 2 * short_d;
	add_dh = 2 * short_d - 2 * long_d;

	// calculate the colour adjustments based on the line length
	radj = (RedByte(end) - RedByte(start)) / long_d;
	gadj = (GreenByte(end) - GreenByte(start)) / long_d;
	badj = (BlueByte(end) - BlueByte(start)) / long_d;

	for (i = 0; i <= long_d; i++) {
		if (y >= 0 && y < g_height && x >= 0 && x < g_width) {
			buffer[y * g_pitch + x] = colour;
		}

		if (d >= 0) {
			x += inc_xh;
			y += inc_yh;
			d += add_dh;
		} else {
			x += inc_xl;
			y += inc_yl;
			d += add_dl;
		}

		// adjust the colour
		unsigned char r = RedByte(colour) + radj;
		unsigned char g = GreenByte(colour) + gadj;
		unsigned char b = BlueByte(colour) + badj;

		colour = ((0xff << 24) | (r << 16) | (g << 8) | b);
	}
}
