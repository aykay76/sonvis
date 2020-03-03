// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__4DDC3E8A_015B_4749_BB1A_D369BE9A4DFB__INCLUDED_)
#define AFX_STDAFX_H__4DDC3E8A_015B_4749_BB1A_D369BE9A4DFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "vis.h"

extern unsigned long *g_Video;
extern int g_width;
extern int g_height;
extern int g_pitch;
extern VisData *g_pVD;

#define RedPtr(x) ((unsigned char *)(x) + 2)
#define GreenPtr(x) ((unsigned char *)(x) + 1)
#define BluePtr(x) ((unsigned char *)(x))

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4DDC3E8A_015B_4749_BB1A_D369BE9A4DFB__INCLUDED_)
