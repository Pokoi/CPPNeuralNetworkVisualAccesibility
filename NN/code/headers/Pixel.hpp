#pragma once

#include <math.h>

/**
@brief Class that stores the pixel information and allow pixel color space conversions.
*/
class Pixel
{
public:
    
    struct RGB
    {
        float red;
        float green;
        float blue;

        RGB ()
            : 
                red  (0.f),
                green(0.f),
                blue (0.f)
        {}

        RGB ( 
                float r,
                float g,
                float b
            ) :
                red   (r),
                green (g),
                blue  (b)
        {}

        RGB (
                size_t r,
                size_t g,
                size_t b
            ) : 
                red   (((float) r ) / 255.f),
                green (((float) g ) / 255.f),
                blue  (((float) b ) / 255.f)
        {}

    } rgb_components;

    struct LUV
    {
        float l;
        float u;
        float v;

        LUV ()
            : 
                l (0.f),
                u (0.f),
                v (0.f)
        {}

        LUV (
                float l,
                float u,
                float v
            ) :
                l (l),
                u (u),
                v (v)
        {}

    } luv_components;

    Pixel()
    {}

    /**
    @brief Generates the luv components in base of rgb values
    */
    void convert_rgb_to_luv()
    {
        if (rgb_components.red == 0 && rgb_components.green == 0 && rgb_components.blue == 0)
        {
            luv_components.l = luv_components.u = luv_components.v = 0.f;
        }

        else
        {
            float buffer[3];
            convert_rgb_to_xyz(buffer);
            convert_xyz_to_luv(buffer);
        }
    }
    
    /**
    @brief Converts the rgb components into a protanopia simulation
    */
    void simulate_protanopia()
    {
        float buffer[3];
        convert_rgb_to_xyz  (buffer);
        convert_xyz_to_xyzp (buffer);
        convert_xyz_to_rgb  (buffer);
    }
    
    /**
    @brief Converts the rgb components into a deuteranopia simulation
    */
    void simulate_deuteranopia()
    {
        float buffer[3];
        convert_rgb_to_xyz  (buffer);
        convert_xyz_to_xyzd (buffer);
        convert_xyz_to_rgb  (buffer);
    }

    /**
    @brief Converts the rgb componentes into a xyz color space. New values are stored in the given buffer
    @param buffer The buffer where store the converted values. Must have size 3 or greater.
    */
    void convert_rgb_to_xyz(float * buffer)
    {       
        buffer[0] = (0.430574f) * rgb_components.red + (0.341550f) * rgb_components.green + (0.178325f) * rgb_components.blue;
        buffer[1] = (0.222015f) * rgb_components.red + (0.706655f) * rgb_components.green + (0.071330f) * rgb_components.blue;
        buffer[2] = (0.020183f) * rgb_components.red + (0.129553f) * rgb_components.green + (0.939180f) * rgb_components.blue;     
    }

    /**
    @brief Converts the given xyz values into a xyz protanopia simulation values. New values are stored in the given buffer
    @param buffer The buffer where store the converted values. Must have size 3 or greater.
    */
    void convert_xyz_to_xyzp(float * buffer)
    {
        float original_0 = buffer[0];
        float original_1 = buffer[1];
        float original_2 = buffer[2];

        buffer[0] = (-0.3813f) * original_0 + (1.1228f) * original_1 + (0.1730f) * original_2;
        buffer[1] = (-0.4691f) * original_0 + (1.3813f) * original_1 + (0.0587f) * original_2;
        buffer[2] = /*(0.0f)   * original_0 + (0.0f)    * original_1 + */(1.0f)  * original_2;
    }

    /**
    @brief Converts the given xyz values into a xyz deuteranopia simulation values. New values are stored in the given buffer
    @param buffer The buffer where store the converted values. Must have size 3 or greater.
    */
    void convert_xyz_to_xyzd(float* buffer)
    {
        float original_0 = buffer[0];
        float original_1 = buffer[1];
        float original_2 = buffer[2];

        buffer[0] = (0.1884f) * original_0 + (0.6597f) * original_1 + (0.1016f)  * original_2;
        buffer[1] = (0.2318f) * original_0 + (0.8116f) * original_1 + (-0.0290f) * original_2;
        buffer[2] = /*(0.0f)  * original_0 + (0.0f)    * original_1 + */(1.0f)   * original_2;
    }

    /**
    @brief Converts the given xyz values into a rgb color space. New values are stored in the rgb_components field
    @param buffer The buffer where store the converted values. Must have size 3 or greater.
    */
    void convert_xyz_to_rgb(float* buffer)
    {
        rgb_components.red   = (3.063218f)  * buffer[0] + (-1.393325f) * buffer[1] + (-0.475802f) * buffer[2];
        rgb_components.green = (-0.969243f) * buffer[0] + (1.875966f)  * buffer[1] + (0.041555f)  * buffer[2];
        rgb_components.blue  = (0.067871f)  * buffer[0] + (-0.228834f) * buffer[1] + (1.069251f)  * buffer[2];
    }

    /**
    @brief Converts the given xyz values into a xyz protanopia simulation values. New values are stored in the given buffer
    @param buffer The buffer where store the converted values. Must have size 3 or greater.
    */
    void convert_xyz_to_luv(float* buffer)
    {
        float Xr = 0.33f;
        float Yr = 0.33f;
        float Zr = 0.33f;
        float yr;
        float us;
        float vs;
        float usr;
        float vsr;
        float e;
        float k;        

        yr = buffer[1] * 3.f;                                               //< It's the same that divide buffer[1] / Yr 
        us = 4 * buffer[0] / (buffer[0] + 15 * buffer[1] + 3 * buffer[2]);
        vs = 9 * buffer[1] / (buffer[0] + 15 * buffer[1] + 3 * buffer[2]);
        usr = 4 * Xr / (Xr + 15 * Yr + 2 * Zr);
        vsr = 9 * Yr / (Xr + 15 * Yr + 3 * Zr);

        e = 216.0f / 24389.f;
        k = 24389.0f / 27.f;

        luv_components.l = yr > e ? 
                                    luv_components.l = 116.f * pow(yr, 0.33) - 16 
                                  : k * yr;             

        luv_components.u = 13.0f * luv_components.l * (us - usr);
        luv_components.v = 13.0f * luv_components.l * (vs - vsr);      

    }

    void lms_protanopia()
    {
        float L = 17.8824f * rgb_components.red + 43.5161f * rgb_components.green + 4.11935f * rgb_components.blue;
        float M = 3.45565f * rgb_components.red + 27.1554f * rgb_components.green + 3.86714f * rgb_components.blue;
        float S = 0.0299566f * rgb_components.red + 0.184309f * rgb_components.green + 1.46709f * rgb_components.blue;

        float Lp = 0.f * L + 2.02344f * M + -2.52581f * S;
        float Mp = 0.f * L + 1.f * M + 0.f * S;
        float Sp = 0.f * L + 0.f * M + 1.f * S;

        float Ri = 0.0809444479f * Lp + -0.130504409f * Mp + 0.116721066f * Sp;
        float Gi = 0.113614708f * Lp + -0.0102485335f * Mp + 0.0540193266f * Sp;
        float Bi = -0.000365296938f * Lp + -0.00412161469f * Mp + 0.693511405 * Sp;

        float Dr = rgb_components.red - Ri;
        float Dg = rgb_components.green - Gi;
        float Db = rgb_components.blue - Bi;

        float Rm = 0.f * Dr + 0.f * Dg + 0.f * Db;
        float Gm = 0.7f * Dr + 1.f * Dg + 0.f * Db;
        float Bm = 0.7f * Dr + 0.f * Dg + 1.f * Db;

        rgb_components.red += Rm;
        rgb_components.green += Gm;
        rgb_components.blue += Bm;        
    }

    void lms_deuteranopia()
    {
        float L = 17.8824f * rgb_components.red + 43.5161f * rgb_components.green + 4.11935f * rgb_components.blue;
        float M = 3.45565f * rgb_components.red + 27.1554f * rgb_components.green + 3.86714f * rgb_components.blue;
        float S = 0.0299566f * rgb_components.red + 0.184309f * rgb_components.green + 1.46709f * rgb_components.blue;

        float Ld = 1.f * L + 0.f * M + 0.f * S;
        float Md = 0.49421f * L + 0.f * M + 1.24827f * S;
        float Sd = 0.f * L + 0.f * M + 1.f * S;

        float Ri = 0.0809444479f * Ld + -0.130504409f * Md + 0.116721066f * Sd;
        float Gi = 0.113614708f * Ld + -0.0102485335f * Md + 0.0540193266f * Sd;
        float Bi = -0.000365296938f * Ld + -0.00412161469f * Md + 0.693511405 * Sd;

        float Dr = rgb_components.red - Ri;
        float Dg = rgb_components.green - Gi;
        float Db = rgb_components.blue - Bi;

        float Rm = 1.f * Dr + 0.7f * Dg + 0.f * Db;
        float Gm = 0.0f * Dr + 0.f * Dg + 0.f * Db;
        float Bm = 0.f * Dr + 0.7f * Dg + 1.f * Db;

        rgb_components.red += Rm;
        rgb_components.green += Gm;
        rgb_components.blue += Bm;        
    }

    void lms_tritanopia()
    {
        float L = 17.8824f * rgb_components.red + 43.5161f * rgb_components.green + 4.11935f * rgb_components.blue;
        float M = 3.45565f * rgb_components.red + 27.1554f * rgb_components.green + 3.86714f * rgb_components.blue;
        float S = 0.0299566f * rgb_components.red + 0.184309f * rgb_components.green + 1.46709f * rgb_components.blue;

        float Lt = 1.f * L + 0.f * M + 0.f * S;
        float Mt = 0.f * L + 1.f * M + 0.f * S;
        float St = -0.395913f * L + 0.801109f * M + 0.f * S;

        float Ri = 0.0809444479f * Lt + -0.130504409f * Mt + 0.116721066f * St;
        float Gi = 0.113614708f * Lt + -0.0102485335f * Mt + 0.0540193266f * St;
        float Bi = -0.000365296938f * Lt + -0.00412161469f * Mt + 0.693511405 * St;

        float Dr = rgb_components.red - Ri;
        float Dg = rgb_components.green - Gi;
        float Db = rgb_components.blue - Bi;

        float Rm = 1.f * Dr + 0.f * Dg + 0.7f * Db;
        float Gm = 0.0f * Dr + 1.f * Dg + 0.7f * Db;
        float Bm = 0.f * Dr + 0.f * Dg + 0.f * Db;

        rgb_components.red += Rm;
        rgb_components.green += Gm;
        rgb_components.blue += Bm;
    }
       

};