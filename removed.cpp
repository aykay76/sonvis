// Line graph stylee
/*	{
		int x, y, x2;
		x = 0;
		y = height / 2 - (pVD->Spectrum[0][0] * yscale);
		for (i = 1; i < 128; i++) {
			x2 = x + xstep;
			y2 = height / 2 - (pVD->Spectrum[0][i] * yscale);

			Line((int)x, (int)y, (int)x2, (int)y2, 0x800000ff);

			x = x2;
			y = y2;
		}

		x = 0;
		y = height / 2 + 1 + (pVD->Spectrum[1][0] * yscale);
		for (i = 1; i < 128; i++) {
			x2 = x + xstep;
			y2 = height / 2 + 1 + (pVD->Spectrum[1][i] * yscale);
			Line((int)x, (int)y, (int)x2, (int)y2, 0x80ff0000);

			x = x2;
			y = y2;
		}
	}*/
	/// fade out the previous frame
/*	{
		int x, y;
		unsigned char dwOff = (unsigned char)((float)dwFrameTime / (float)fade * 256.0F);
		unsigned long *vp = Video;
		unsigned long *tp;

		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				tp = vp + x;
				if (dwFrameTime < fade) {
					unsigned char *rp = RedPtr(tp);
					unsigned char *gp = GreenPtr(tp);
					unsigned char *bp = BluePtr(tp);

					if (*rp > dwOff) *rp -= dwOff; else *rp = 0;
					if (*gp > dwOff) *gp -= dwOff; else *gp = 0;
					if (*bp > dwOff) *bp -= dwOff; else *bp = 0;
				} else {
					*tp = 0;
				}
			}

			vp += pitch;
		}
	}
*/

// some old rotation and zoom code - don't know why i'm keeping it really, nostalgia?
/*	{
		unsigned long *vp = Video;
		unsigned long *bp = bufptr;
		unsigned char dwOff = (unsigned char)((float)dwFrameTime / (float)fade * 256.0F);

		for (int y = 1; y < height - 1; y++) {
			for (int x = 0; x < width - 1; x++) {
				// for each pixel in the offscreen buffer we are going to manipulate
				// three pixels on the screen, the corresponding one and one either side
				// NOTE! Remember with these effects that each video pixel should only be processed once
				// otherwise it all goes loopy
				if (x && *bp) {
					unsigned char *vrp = RedPtr(vp + x);
					unsigned char *vgp = GreenPtr(vp + x);
					unsigned char *vbp = BluePtr(vp + x);

					unsigned char *brp = RedPtr(bp - pitch + x);
					unsigned char *bgp = GreenPtr(bp - pitch + x);
					unsigned char *bbp = BluePtr(bp - pitch + x);

					*vrp = (*brp >> 1);
					*vgp = (*bgp >> 1);
					*vbp = (*bbp >> 1);

					brp = RedPtr(bp + pitch + x);
					bgp = GreenPtr(bp + pitch + x);
					bbp = BluePtr(bp + pitch + x);

					*vrp += (*brp >> 1);
					*vgp += (*bgp >> 1);
					*vbp += (*bbp >> 1);
				}
			}

			vp += pitch;
			bp += pitch;
		}
	}*/

// If we want to change the angle of rotation dynamically, put this at the end of the frame
/*	if (fRotateClock) {
		angle += 2 * fractionOfSecond;
		if (angle > 5.0) fRotateClock = false;
	} else {
		angle -= 2 * fractionOfSecond;
		if (angle < -5.0) fRotateClock = true;
	}*/


// some more old rotation code
/*		for (y = -midy; y < midy; y++) {
			for (x = -midx; x < midx; x++) {
				x1 = (x * cosT - y * sinT) + midx;
				y1 = (x * sinT + y * cosT) + midy;

				xp = x + midx;
				yp = y + midy;

				// now we process nine pixels (3x3 grid) around the original pixel to soften and disperse the colour
				if (y1 >= 1 && y1 < g_height - 1 && x1 >= 1 && x1 < g_width) {
					unsigned long *bp = bufptr + y1 * pitch + x1;
					unsigned long t;
					unsigned char *tvp = (unsigned char *)(Video + yp * pitch + xp);
					for (int i = 0; i < 4; i++) {
						t = (
							((unsigned char *)(bp - pitch - 1))[i] + 
							((unsigned char *)(bp - pitch))[i] * 1 + 
							((unsigned char *)(bp - pitch + 1))[i] + 
							((unsigned char *)(bp - 1))[i] * 1 + 
							((unsigned char *)(bp))[i] * 8 + 
							((unsigned char *)(bp + 1))[i] * 1 + 
							((unsigned char *)(bp + pitch - 1))[i] + 
							((unsigned char *)(bp + pitch))[i] * 1 + 
							((unsigned char *)(bp + pitch + 1))[i]
						) >> 4;

						tvp[i] = (tvp[i] + (unsigned char)t) >> 1;
					}
				}

				fx += fZoom;
            }
			fy += fZoom;
			fx = sfx;
		}*/


// old floating point blur, made this much more efficient now
	// bit of a blur effect coming in
/*	{
		unsigned long *vp = Video;
		unsigned long *bp = bufptr;
		float factor[9] = { 0.5, 1.0, 0.5,
							1.0, 2.0, 1.0,
							0.5, 1.0, 0.5 };

		for (int y = 1; y < height - 1; y++) {
			for (int x = 1; x < width - 1; x++) {
				unsigned long t;
				unsigned char *pix[9] = {
					(unsigned char *)(bp + x - pitch - 1),
					(unsigned char *)(bp + x - pitch),
					(unsigned char *)(bp + x - pitch + 1),
					(unsigned char *)(bp + x - 1),
					(unsigned char *)(bp + x),
					(unsigned char *)(bp + x + 1),
					(unsigned char *)(bp + x + pitch - 1),
					(unsigned char *)(bp + x + pitch),
					(unsigned char *)(bp + x + pitch + 1)
				};

				unsigned char *tvp = (unsigned char *)(vp + x);
				for (int i = 0; i < 4; i++) {
					t = 0;
					for (int p = 0; p < 9; p++) {
						t += (unsigned long)(pix[p][i] * factor[p]);
					}
					t = t >> 3;
					if (t > 5) t-=5;

					tvp[i] = (unsigned char)t;
				}
			}

			vp += pitch;
			bp += pitch;
		}
	}*/

// Bar chart stylee
/*	for (i = 0; i < 128; i++) {
		if (pVD->Spectrum[0][i] > 1) {
			y2 = height / 2 - (pVD->Spectrum[0][i] * yscale);
			for (int x = 0; x < xstep; x++) {
				Line(i * xstep + x, midy, i * xstep + x, y2, 0xffff0000, 0xff000000);
			}
		}
	}
	for (i = 0; i < 128; i++) {
		if (pVD->Spectrum[1][i] > 1) {
			y2 = height / 2 + 1 + (pVD->Spectrum[0][i] * yscale);
			for (int x = 0; x < xstep; x++) {
				Line(i * xstep + x, midy + 1, i * xstep + x, y2, 0xff0000ff, 0xff000000);
			}
		}
	}*/

// draw the waveform data as dots
/*		{
			double theta = 0.0;

			for (int i = 0; i < 512; i++) {
				int x = midx + ((char)pVD->Waveform[1][i] / 256.0 * width * cos(theta));
				int y = midy + ((char)pVD->Waveform[1][i] / 256.0 * width * sin(theta));

				Dot(x, y, ~waveColour);
			
				theta += (3.141592652 * 2) / 512;
			}
		}*/

// draw a waveform across the width of the sreen
	// draw one channel across the width
/*	{
		int xinc = 512 / width;
		int max = min(width, 512);
		int x = 0;
		int y = midy + (char)pVD->Waveform[1][0];
		for (i = 0; i < max; i++) {
			int y2 = midy + (char)pVD->Waveform[1][i];
			Line(x, y, x + xinc, y2, 0xffffffff);
			y = y2;
			x += xinc;
		}
	}*/


// thought this would be faster without casting to char's but doesn't seem to be  :(
/*
				unsigned long *tvp = newbfr + ((int)(y + midy) * g_pitch) + (int)(x + midx);
				t = *tvp;
				unsigned long b1, b2, b3, b4, b5;
				b1 = *(bp - g_pitch);
				b2 = *(bp - 1);
				b3 = *bp;
				b4 = *(bp + 1);
				b5 = *(bp + g_pitch);

				unsigned long red, green, blue, alpha;
				alpha = (
						(AlphaByte(b1)) * 2 + 
						(AlphaByte(b2)) * 2 + 
						(AlphaByte(b3)) * 2 + 
						(AlphaByte(b4)) * 2 + 
						(AlphaByte(b5)) * 2) / 10;

				red = (
						(RedByte(b1)) * 2 + 
						(RedByte(b2)) * 2 + 
						(RedByte(b3)) * 2 + 
						(RedByte(b4)) * 2 +
						(RedByte(b5)) * 2) / 10; 
				green = (
						(GreenByte(b1)) * 2 + 
						(GreenByte(b2)) * 2 + 
						(GreenByte(b3)) * 2 + 
						(GreenByte(b4)) * 2 + 
						(GreenByte(b5)) * 2) / 10;
				blue = (
						(BlueByte(b1)) * 2 + 
						(BlueByte(b2)) * 2 + 
						(BlueByte(b3)) * 2 + 
						(BlueByte(b4)) * 2 + 
						(BlueByte(b5)) * 2) / 10;

				alpha = (alpha + AlphaByte(t)) >> 1;
				red = (red + RedByte(t)) >> 1;
				green = (green + GreenByte(t)) >> 1;
				blue = (blue + BlueByte(t)) >> 1;

				*tvp = (alpha << 24) | (red << 16) | (green << 8) | blue;
*/