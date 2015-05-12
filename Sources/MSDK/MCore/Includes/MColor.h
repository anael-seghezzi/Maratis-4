/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MColor.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#ifndef _M_COLOR_H
#define _M_COLOR_H


class M_CORE_EXPORT MColor
{
public:

	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

public:

	MColor(void):	
		r(0), 
		g(0), 
		b(0),
		a(0)
	{}

	MColor(unsigned char newR, unsigned char newG, unsigned char newB, unsigned char newA):	
		r(newR), 
		g(newG), 
		b(newB),
		a(newA)
	{}

	MColor(const MColor & color):	
		r(color.r), 
		g(color.g), 
		b(color.b),
		a(color.a)
	{}

	MColor(const MVector3 & color):	
		r(CLAMP((int)(color.x*255), 0, 255)), 
		g(CLAMP((int)(color.y*255), 0, 255)), 
		b(CLAMP((int)(color.z*255), 0, 255)),
		a(255)
	{}

	MColor(const MVector4 & color):	
		r(CLAMP((int)(color.x*255), 0, 255)), 
		g(CLAMP((int)(color.y*255), 0, 255)), 
		b(CLAMP((int)(color.z*255), 0, 255)),
		a(CLAMP((int)(color.w*255), 0, 255))
	{}

	~MColor() 
	{}

public:

	inline MColor operator + (const MColor & color) const
	{	
		int R = r + color.r; if(R > 255) R = 255;
		int G = g + color.g; if(G > 255) G = 255;
		int B = b + color.b; if(B > 255) B = 255;
		int A = a + color.a; if(A > 255) A = 255;

		return MColor(R, G, B, A);
	}
	
	inline MColor operator - (const MColor & color) const
	{	
		int R = r - color.r; if(R < 0) R = 0;
		int G = g - color.g; if(G < 0) G = 0;
		int B = b - color.b; if(B < 0) B = 0;
		int A = a - color.a; if(A < 0) A = 0;

		return MColor(R, G, B, A);
	}

	inline MColor operator + (const int value) const
	{	
		int R = CLAMP((int)(r + value), 0, 255);
		int G = CLAMP((int)(g + value), 0, 255);
		int B = CLAMP((int)(b + value), 0, 255);
		int A = CLAMP((int)(a + value), 0, 255);

		return MColor(R, G, B, A);
	}

	inline MColor operator - (const int value) const
	{	
		int R = CLAMP((int)(r - value), 0, 255);
		int G = CLAMP((int)(g - value), 0, 255);
		int B = CLAMP((int)(b - value), 0, 255);
		int A = CLAMP((int)(a - value), 0, 255);

		return MColor(R, G, B, A);
	}

	inline MColor operator * (const float value) const
	{	
		int R = CLAMP((int)(r * value), 0, 255);
		int G = CLAMP((int)(g * value), 0, 255);
		int B = CLAMP((int)(b * value), 0, 255);
		int A = CLAMP((int)(a * value), 0, 255);

		return MColor(R, G, B, A);
	}

	inline MColor operator / (const float value) const
	{	
		int R = CLAMP((int)(r / value), 0, 255);
		int G = CLAMP((int)(g / value), 0, 255);
		int B = CLAMP((int)(b / value), 0, 255);
		int A = CLAMP((int)(a / value), 0, 255);

		return MColor(R, G, B, A);
	}

	inline bool operator == (const MColor & color) const
	{
		if(r == color.r && g == color.g && b == color.b && a == color.a)
			return true;

		return false;
	}

	inline bool operator != (const MColor & color) const
	{	
		return !((*this) == color);
	}

	inline operator unsigned char * () const 
	{
		return (unsigned char *) this;
	}

	inline operator const unsigned char * () const 
	{
		return (const unsigned char *) this;
	}

public:

	inline void set(unsigned char newR, unsigned char newG, unsigned char newB, unsigned char newA)
	{	
		r = newR;	
		g = newG;	
		b = newB;
		a = newA;
	}
};

#endif