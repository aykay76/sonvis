// Drawing.h: interface for the Drawing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWING_H__793CB83F_E661_472B_B954_4227C713F3EF__INCLUDED_)
#define AFX_DRAWING_H__793CB83F_E661_472B_B954_4227C713F3EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

void Dot(unsigned long *buffer, int x, int y, DWORD pixel);
void Line(unsigned long *buffer, int x, int y, int x2, int y2, DWORD pixel);
void Line(unsigned long *buffer, int x, int y, int x2, int y2, DWORD start, DWORD end);

#endif // !defined(AFX_DRAWING_H__793CB83F_E661_472B_B954_4227C713F3EF__INCLUDED_)
