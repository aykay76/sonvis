// sonvis.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Drawing.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void Initialize(void);
BOOL Render(unsigned long *Video, int width, int height, int pitch, VisData* pVD);
BOOL SaveSettings(char* FileName);
BOOL OpenSettings(char* FileName);
void RotateAndZoom(unsigned long *oldbfr, unsigned long *newbfr, double angle, double zoom);

double cos_table[628];
#define sin_table cos_table
int rand_table[1024];
int rand_table_offset = 0;
int rand_table_max = 1024;

DWORD dwLast;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

static VisInfo vi;

DLLEXPORT VisInfo* QueryModule(void)
{
	vi.PluginName = "Alan's plugin";
	vi.lRequired = VI_WAVEFORM;//VI_SPECTRUM | VI_WAVEFORM;// | SONIQUEVISPROC;
	vi.Initialize = Initialize;
	vi.Render = Render;
	vi.SaveSettings = SaveSettings;
	vi.OpenSettings = OpenSettings;

	return &vi;
}


// Called some time before your plugin is asked to render for
// the first time
void Initialize(void)
{
	srand(time(0));

	for (int i = 0; i < 628; i++) {
		cos_table[i] = cos((double)i / 10.0);
	}

	for (int r = 0; r < 1024; r++) {
		rand_table[r] = 60 + ((double)rand() / (double)RAND_MAX * 30.0);
	}
}

// Called for each frame. Pitch is in pixels and can be negative.
// Render like this:
// for (y = 0; y < height; y++)
// {
//    for (x = 0; x < width; x++)
//       Video[x] = <pixel value>;
//	  Video += pitch;
// }
//				OR
// void PutPixel(int x, int y, unsigned long Pixel)
// {
//    _ASSERT( x >= 0 && x < width && y >= 0 && y < height );
//	  Video[y*pitch+x] = Pixel;
// }

unsigned long *buffer = NULL;
unsigned long *bufptr = NULL;
unsigned long *g_Video;
int g_width = 0;
int g_height = 0;
int g_pitch = 0;
VisData *g_pVD;

unsigned long waveColour = 0xff00ff00;
bool fIncWaveColour = true;
float fColour = 0.0;
int nColour = 16;

bool fRotateClock = true;
int nFrame = 0;

unsigned int fade = 150;  // fade out in xms
float fractionOfSecond = 0.0;

int table_offset = 0;

BOOL Render(unsigned long *Video, int width, int height, int pitch, VisData* pVD)
{
	float xstep = width / 128.F;
	float yscale = height / 2 / 256.F;
	DWORD dwFrameTime = GetTickCount();

	// if the dimensions have changed, start again with a new back buffer
	buffer = new unsigned long[abs(pitch) * height];
	if (!buffer)
		return FALSE;

	g_width = width;
	g_height = height;
	g_pitch = pitch;

	// set the pointer to the first pixel
	if (pitch < 0) {
		bufptr = &buffer[abs((height - 1) * pitch)];
	} else {
		bufptr = buffer;
	}

	// make a copy of what's on the screen now
	unsigned long *vp = Video;
	unsigned long *bp = bufptr;
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			*(bp + w) = *(vp + w);
		}
		vp += pitch;
		bp += pitch;
	}

	g_Video = Video;
	g_width = width;
	g_height = height;
	g_pitch = pitch;
	g_pVD = pVD;

	int midx = (width / 2), midy = (height / 2);

	// projection centre vars
	int px = midx, py = midy;

	// rotate the projection point
	px = midx;// + (int)(20 * cos_table[table_offset] - 20 * sin_table[(table_offset + 157) % 628]);
	py = midy;// + (int)(20 * sin_table[(table_offset + 157) % 628] + 20 * cos_table[table_offset]);
	table_offset = (table_offset + 1) % 628;
	// overplot the waveform in a different colour to mix it up a bit
	{
		int dx = rand_table[rand_table_offset];//width >> 3;
		int dy = rand_table[rand_table_offset];//height >> 3;
		rand_table_offset = (rand_table_offset + 1) % rand_table_max;
		{
			double theta = 0.0;

			int x = px + (dx * cos(theta)) + (((char)pVD->Waveform[1][0]) * cos(theta));
			int y = py + (dy * sin(theta)) + (((char)pVD->Waveform[1][0]) * sin(theta));

			for (int i = 0; i < 512; i++) {
				int x2 = px + (dx * cos(theta)) + (((char)pVD->Waveform[1][i]) * cos(theta));
				int y2 = py + (dy * sin(theta)) + (((char)pVD->Waveform[1][i]) * sin(theta));

				Line(bufptr, x, y, x2, y2, 0);

				x = x2;
				y = y2;
			
				theta += (3.141592652 * 2) / 512;
			}
		}
		{
			double theta = 0.0;

			int x = px + (dx * cos(theta)) + (((char)pVD->Waveform[0][0] >> 2) * cos(theta));
			int y = py + (dy * sin(theta)) + (((char)pVD->Waveform[0][0] >> 2) * sin(theta));

			for (int i = 0; i < 512; i++) {
				int x2 = px + (dx * cos(theta)) + (((char)pVD->Waveform[0][i] >> 2) * cos(theta));
				int y2 = py + (dy * sin(theta)) + (((char)pVD->Waveform[0][i] >> 2) * sin(theta));

				Line(bufptr, x, y, x2, y2, waveColour);

				x = x2;
				y = y2;
			
				theta += (3.141592652 * 2) / 512;
			}
		}
//		table_offset = (table_offset + 1) % 512;

		// now lets monkey with the colour
		unsigned char col = (waveColour >> nColour) & 0xff;
		bool fChange = false;
		if (fIncWaveColour) {
			fColour += 255 * fractionOfSecond / 2;
			if (fColour > 255.0) {
				fColour = 255.0;

				fChange = true;
			}
			col = (unsigned char)fColour;
		} else {
			fColour -= 255 * fractionOfSecond / 2;
			if (fColour < 0.0) {
				fColour = 0.0; 

				fChange = true;
			}
			col = (unsigned char)fColour;
		}
		unsigned long mask = 0x00000000;
		mask |= (0xff << nColour);
		waveColour &= ~mask;
		waveColour |= (col << nColour);
		if (fChange) {
			// time to choose whether we increase or decrease another colour!
			int n;
			while ((n = (rand() * 3 / RAND_MAX) * 8) == nColour) ;
			nColour = n;
			if (((waveColour >> nColour) & 0xff) == 0xff) {
				fIncWaveColour = false;
				fColour = 255.0;
			} else {
				fIncWaveColour = true;
				fColour = 0.0;
			}
		}
	}

	//// let's try rotating a bit of the screen and see what happens, and how quick it is!
//	if (nFrame == 1) {
		RotateAndZoom(bufptr, Video, -0.25, 0.95);
//	}

//	nFrame++;
	fractionOfSecond = (GetTickCount() - dwFrameTime) / 1000.0;
	dwLast = GetTickCount();

	char szText[128];
	sprintf(szText, "%d\n", dwLast - dwFrameTime);
	OutputDebugString(szText);

	if (buffer) delete [] buffer;

	return TRUE;
}

// Use WritePrivateProfileString to save settings when this is called
// Example:
// WritePrivateProfileString("my plugin", "brightness", "3", FileName);
BOOL SaveSettings(char* FileName)
{
	return TRUE;
}

// Use GetPrivateProfileString similarly:
// char BrightnessBuffer[256];
// GetPrivateProfileString("my plugin", "brightness", "3", BrightnessBuffer, sizeof(BrightnessBuffer), FileName);
BOOL OpenSettings(char* FileName)
{
	return TRUE;
}

void RotateAndZoom(unsigned long *oldbfr, unsigned long *newbfr, double angle, double zoom)
{
	// rotation about 0,0
    double cosT, sinT;
	double x, y;
	int xp, yp;
	double midx = (g_width / 2), midy = (g_height / 2);
	
    cosT = cos(angle * 3.141592 / 180.0) * zoom;
    sinT = sin(angle * 3.141592 / 180.0) * zoom;//0.0;

	double xcosT;
	double xsinT;
	double ycosT;
	double ysinT;

	ycosT = -midy * cosT;
	ysinT = -midy * sinT;
	for (y = -midy; y < midy; y++ ) {
		// precalculate x.cosT for the start of each line
		xcosT = -midx * cosT;
		xsinT = -midx * sinT;
		for (x = -midx; x < midx; x++) {
			int xo, yo;

			xo = (int)(xcosT - ysinT + midx);
			yo = (int)(xsinT + ycosT + midy);

			if ((xo > 0) && (xo < g_width) && (yo > 0) && (yo < g_height)) {
				unsigned long t;
				// get a pointer into the previous frame buffer
				unsigned long *bp = oldbfr + yo * g_pitch + xo;
				// get a pointer into the video buffer for this frame


				// go through each colour channel and take a weighted average of the surrounding eight pixels
				unsigned char *tvp = (unsigned char *)(newbfr + ((int)(y + midy) * g_pitch) + (int)(x + midx));

				for (int i = 0; i < 4; i++) {
					t = (
//						((unsigned char *)(bp - g_pitch - 1))[i] + 
						((unsigned char *)(bp - g_pitch))[i] * 2 + 
//						((unsigned char *)(bp - g_pitch + 1))[i] + 
						((unsigned char *)(bp - 1))[i] * 2 + 
						((unsigned char *)(bp))[i] * 2 + 
						((unsigned char *)(bp + 1))[i] * 2 + 
//						((unsigned char *)(bp + g_pitch - 1))[i] + 
						((unsigned char *)(bp + g_pitch))[i] * 2
//						((unsigned char *)(bp + g_pitch + 1))[i]
					) / 10;

					tvp[i] = (tvp[i] + (unsigned char)t) >> 1;
				}
			}
			// increment xcosT
			xcosT += cosT;
			xsinT += sinT;
		} // x loop

		ycosT += cosT;
		ysinT += sinT;
	} // y loop
}