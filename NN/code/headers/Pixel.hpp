#pragma once


class Pixel
{
public:
    
    struct RGB
    {
        float red;
        float green;
        float blue;

        RGB() : 
                red  (0.f),
                green(0.f),
                blue (0.f)
        {}

        RGB(float r, float g, float b) :
                                        red     (r),
                                        green   (g),
                                        blue    (b)
        {}

        RGB(size_t r, size_t g, size_t b) : 
                                            red     (((float) r ) / 255.f),
                                            green   (((float) g ) / 255.f),
                                            blue    (((float) b ) / 255.f)
        {}

    } rgb_components;

    struct LUV
    {
        float l;
        float u;
        float v;

        LUV() : 
                l (0.f),
                u (0.f),
                v (0.f)
        {}

        LUV(float l, float u, float v) :
                                            l (l),
                                            u (u),
                                            v (v)
        {}

    } luv_components;

    void convert_rgb_to_luv();
    void simulate_protanopia();
    void simulate_deuteranopia();

    [] float convert_RGB_to_XYZ();

};