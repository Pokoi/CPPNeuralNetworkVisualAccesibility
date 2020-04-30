
#include <Image.hpp>

/**
@brief Blurs the image. New image is stored in the given image container
@param output The image where store the new image
*/
void Image::blur(Image& output)
{
    output = Image(width, height);

    Pixel* start    = output.get_pixels();
    Pixel* end      = start + output.get_width() * output.get_height();
    Pixel* original_iterator = pixels;

    float red   = 0.f;
    float green = 0.f;
    float blue  = 0.f;

    // The critical rows are the first one and last one because they have no previous or next one.
    // They have to be calculated separatly

    // The first row of pixels

    // The firs pixel has no previous pixel. 
    //To prevent to use a conditional, the first pixel is calculated as follows
    add_components(red, green, blue, original_iterator);
    add_components(red, green, blue, original_iterator + 1);
    add_components(red, green, blue, original_iterator + width);
    add_components(red, green, blue, original_iterator + width + 1);

    start->rgb_components.red   = red   * 0.25f;
    start->rgb_components.green = green * 0.25f;
    start->rgb_components.blue  = blue  * 0.25f;

    ++original_iterator;
    ++start;

    // The rest of the first row
    while (start < start + width)
    {
        red     = 0.f;
        green   = 0.f;
        blue    = 0.f;

        add_components(red, green, blue, original_iterator);
        add_components(red, green, blue, original_iterator - 1);
        add_components(red, green, blue, original_iterator + 1);
        add_components(red, green, blue, original_iterator + width);
        add_components(red, green, blue, original_iterator + width + 1);
        add_components(red, green, blue, original_iterator + width - 1);

        start->rgb_components.red   = red   * 0.16f;
        start->rgb_components.green = green * 0.16f;
        start->rgb_components.blue  = blue  * 0.16f;

        ++original_iterator;
        ++start;
    }

    // The body of the image
    while (start < end - width)
    {
        red     = 0.f;
        green   = 0.f;
        blue    = 0.f;

        add_components(red, green, blue, original_iterator);
        add_components(red, green, blue, original_iterator - 1);
        add_components(red, green, blue, original_iterator + 1);
        add_components(red, green, blue, original_iterator + width);
        add_components(red, green, blue, original_iterator + width + 1);
        add_components(red, green, blue, original_iterator + width - 1);
        add_components(red, green, blue, original_iterator - width);
        add_components(red, green, blue, original_iterator - width + 1);
        add_components(red, green, blue, original_iterator - width - 1);

        start->rgb_components.red = red     * 0.11f;
        start->rgb_components.green = green * 0.11f;
        start->rgb_components.blue = blue   * 0.11f;

        ++original_iterator;
        ++start;
    }

    // The last row of pixels
    while (start < end - 1)
    {
        red     = 0.f;
        green   = 0.f;
        blue    = 0.f;

        add_components(red, green, blue, original_iterator);
        add_components(red, green, blue, original_iterator - 1);
        add_components(red, green, blue, original_iterator + 1);
        add_components(red, green, blue, original_iterator - width);
        add_components(red, green, blue, original_iterator - width - 1);
        add_components(red, green, blue, original_iterator - width + 1);

        start->rgb_components.red   = red   * 0.16f;
        start->rgb_components.green = green * 0.16f;
        start->rgb_components.blue  = blue  * 0.16f;

        ++original_iterator;
        ++start;
    }

    // The last pixel
    red     = 0.f;
    green   = 0.f;
    blue    = 0.f;

    add_components(red, green, blue, original_iterator);
    add_components(red, green, blue, original_iterator - 1);
    add_components(red, green, blue, original_iterator - width);
    add_components(red, green, blue, original_iterator - width - 1);

    start->rgb_components.red   = red   * 0.25f;
    start->rgb_components.green = green * 0.25f;
    start->rgb_components.blue  = blue  * 0.25f;

}

/**
@brief Apply a sobel colour effect to the image and store the result in the given image
@param output The image where store the new image
*/

void Image::sobel_colour(Image& output)
{
    output = Image(width, height);

    Pixel* start = pixels;
    Pixel* end = start + width * height;

    // set the start in the second row and the end in the previous of the last row
    // This avoid extra conditional operations and the lost of information is not relevant
    start += width;
    end -= width;

    float* buffer = new float[width * height];

    float min = 1.f;
    float max = 0.f;

    int x, y;

    while (start < end)
    {
        float gx =
            1.f * colour_difference(*(start - width - 1), *(start - width + 1)) +
            2.f * colour_difference(*(start - 1), *(start + 1)) +
            1.f * colour_difference(*(start - 1 + width), *(start + width + 1));

        float gy =
            1.f * colour_difference(*(start - width - 1), *(start + width - 1)) +
            2.f * colour_difference(*(start - width), *(start + width)) +
            1.f * colour_difference(*(start + 1 - width), *(start + width + 1));

        float val = pow(gx * gx + gy * gy, 0.5);
        *buffer = val;

        max = val > max ? val : max;
        min = val < min ? val : min;

        ++buffer;
        ++start;
    }

    Pixel* iterator = output.get_pixels() + width * height - 1;

    while (buffer)
    {
        *buffer = ((*buffer) - min) / max - min;

        iterator->rgb_components.red = *buffer;
        iterator->rgb_components.green = *buffer;
        iterator->rgb_components.blue = *buffer;

        --buffer;
        --iterator;
    }

    // Free the memory
    delete buffer;

}
