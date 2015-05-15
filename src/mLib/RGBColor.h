#ifndef CORE_GRAPHICS_RGBCOLOR_H_
#define CORE_GRAPHICS_RGBCOLOR_H_

struct RGBColor
{
    RGBColor() {}
    RGBColor(BYTE _r, BYTE _g, BYTE _b)
	{
		r = _r;
		g = _g;
		b = _b;
		a = 255;
	}
    RGBColor(BYTE _r, BYTE _g, BYTE _b, BYTE _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
	
    RGBColor flipBlueAndRed() const
    {
        return RGBColor(b, g, r, a);
    }

    RGBColor grayscale() const
    {
        BYTE avg = BYTE(((int)r + (int)g + (int)b) / 3);
        return RGBColor(avg, avg, avg, 255);
    }

    RGBColor inverse() const
    {
        return RGBColor(255 - r, 255 - g, 255 - b, 255 - a);
    }

	static UINT distL1(RGBColor a, RGBColor b)
	{
		return std::abs(int(a.r) - int(b.r)) +
			std::abs(int(a.g) - int(b.g)) +
			std::abs(int(a.b) - int(b.b));
	}

	static UINT distL2(RGBColor a, RGBColor b)
	{
		int DiffR = int(a.r) - int(b.r);
		int DiffG = int(a.g) - int(b.g);
		int DiffB = int(a.b) - int(b.b);
		return DiffR * DiffR + DiffG * DiffG + DiffB * DiffB;
	}

	static RGBColor randomColor()
	{
		return RGBColor(rand() & 255, rand() & 255, rand() & 255);
	}

    BYTE r, g, b, a;
};

//
// Exact comparison functions.  Does not match the alpha channel.
//
inline bool operator == (RGBColor left, RGBColor right)
{
    return ((left.r == right.r) && (left.g == right.g) && (left.b == right.b));
}

inline bool operator != (RGBColor left, RGBColor right)
{
    return ((left.r != right.r) || (left.g != right.g) || (left.b != right.b));
}

#endif  // CORE_GRAPHICS_RGBCOLOR_H_
