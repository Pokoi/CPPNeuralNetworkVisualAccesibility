#pragma once

#include "Pixel.hpp"
#include <cstdint>
#include <string>

class Image
{
private: 
    std::uint16_t width;        // uint16_t allows numbers from 0 to 65 635 (2 bytes of memory). 
                                // This makes uint16_t better than size_t (It's very strange to have images greater than 65k pixels width)
    std::uint16_t height;       // uint16_t allows numbers from 0 to 65 635 (2 bytes of memory). 
                                // This makes uint16_t better than size_t (It's very strange to have images greater than 65k pixels height)

    Pixel * pixels;

public:

    Image() = delete;
    
    /**
    @brief Creates an empty image with the given size
    @param width The width of the image in pixels
    @param height The height of the image in pixels
    */
    Image (
            std::uint16_t width,
            std::uint16_t height
          ) : 
            width(width),
            height(height),
            pixels(new Pixel[width * height])
    {}

    /**
    @brief Loads an image data from a file path
    #param path The path of the image
    */
    Image (std::string path)
    {
        width = 0;
        height = 0;
        pixels = new Pixel[2];
    }

    /**
    @brief Gets the width of the image
    @return The width of the image
    */
    std::uint16_t get_width () { return width;  }
    
    /**
    @brief Gets the height of the image
    @return The height of the image
    */
    std::uint16_t get_height() { return height; }

    /**
    @brief Gets the pixels of the image
    @return The pixel pointer
    */
    Pixel* get_pixels() { return pixels; }

    /**
    @brief Gets a reference to the pixel at a certain index
    @param index The index of the pixel
    */
    Pixel & get_pixel(uint16_t index) 
    { 
        return pixels[index];
    }

    /**
    @brief Gets a reference to the pixel at a certain position
    @param x The x coordinate of the pixel
    @param y The y coordinate of the pixel
    */
    Pixel & get_pixel(uint16_t x, uint16_t y) 
    { 
        return pixels[y * width + x];
    }

    /**
    @brief Sets a certain pixel in a given position    
    @param x The x coordinate of the pixel
    @param y The y coordinate of the pixel
    @param pixel The pixel data to apply
    */
    void set_pixel(uint16_t x, uint16_t y, Pixel& pixel)
    {
        pixels[y * width + x] = pixel;
    }

    /**
    @brief Fills the entire image with the given pixel data
    @param pixel The pixel data to apply to all the image pixels
    */
    void fill_image(Pixel& pixel)
    {
        Pixel   * start = pixels;
        Pixel   * end   = pixels + width * height;

        while (start < end)
        {
            start->rgb_components = pixel.rgb_components;
            ++start;
        }        
    }

    /**
    @brief Frees memory
    */
    ~Image()
    {
        delete[] pixels;
    }

    /**
    @brief Exports the image to the given path
    @param path The path where exports the image
    */
    void export (std::string path)
    {}

    /**
    @brief Blurs the image. New image is stored in the given image container
    @param output The image where store the new image
    */
    void blur(Image& output);

    /**
    @brief Adds the component values of a given pixel to each given variable. The addition is stored in the given variables
    @param red The red component container
    @param green The green component container
    @param blue The blue component container
    @param original The pixel with the data
    */
    void add_components(float& red, float& green, float& blue, Pixel* original)
    {
        red     += (original)->rgb_components.red;
        green   += (original)->rgb_components.green;
        blue    += (original)->rgb_components.blue;
    }

    /**
    @brief Apply a sobel colour effect to the image and store the result in the given image
    @param output The image where store the new image
    */
    void sobel_colour(Image& output);

    float colour_difference(Pixel& first, Pixel& second)
    {
        first.convert_rgb_to_luv();
        second.convert_rgb_to_luv();

        return pow  (
                        pow (second.luv_components.l - first.luv_components.l, 2.f) + 
                        pow (second.luv_components.u - first.luv_components.u, 2.f) +
                        pow (second.luv_components.v - first.luv_components.v, 2.f), 0.5f

                    );
    }
};