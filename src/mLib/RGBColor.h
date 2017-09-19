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
    RGBColor(const vec3f &v)
    {
        r = util::boundToByte(v.r * 255.0f);
        g = util::boundToByte(v.g * 255.0f);
        b = util::boundToByte(v.b * 255.0f);
        a = 255;
    }
    RGBColor(const vec4f &v)
    {
        r = util::boundToByte(v.r * 255.0f);
        g = util::boundToByte(v.g * 255.0f);
        b = util::boundToByte(v.b * 255.0f);
        a = util::boundToByte(v.a * 255.0f);
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

    operator vec4f() const
    {
        static const float rcp = 1.0f / 255.0f;
        return vec4f(r * rcp, g * rcp, b * rcp, a * rcp);
    }

    vec4f toVec4f() const
    {
        static const float rcp = 1.0f / 255.0f;
        return vec4f(r * rcp, g * rcp, b * rcp, a * rcp);
    }

    vec3f toVec3f() const
    {
        static const float rcp = 1.0f / 255.0f;
        return vec3f(r * rcp, g * rcp, b * rcp);
    }

    BYTE r, g, b, a;
};

namespace Colors
{
    static RGBColor Magenta()  { return RGBColor(255, 0, 255); }
    static RGBColor Cyan()	   { return RGBColor(0, 255, 255); }
    static RGBColor Gray()	   { return RGBColor(128, 128, 128); }
    static RGBColor White()	   { return RGBColor(255, 255, 255); }
    static RGBColor Black()	   { return RGBColor(0, 0, 0); }
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
