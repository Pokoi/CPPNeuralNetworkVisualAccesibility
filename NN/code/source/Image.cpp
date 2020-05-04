
#include <Image.hpp>


/**
@brief Exports the image to the given path
@param path The path where exports the image
*/
void Image::export_image(std::string path)
{
    QImage img = QImage(int(width), int(height), QImage::Format::Format_RGB32);
    QRgb value;

    for (uint16_t i = 0; i < width; ++i)
    {
        for (uint16_t j = 0; j < height; ++j)
        {
            Pixel& pixel = pixels[j * width + i];
            value = qRgb(pixel.rgb_components.red * 255, pixel.rgb_components.green * 255, pixel.rgb_components.blue * 255);
            img.setPixel(i, j, value);
        }
    }

    img.save(QString::fromStdString(path));
}

/**
@brief Loads an image data from a file path
#param path The path of the image
*/
Image::Image(std::string path)
{
    QPixmap pm(QString(path.c_str()));
    QImage image = (pm.toImage());
    width = image.width();
    height = image.height();
    pixels = new Pixel[width * height];

    for (uint16_t i = 0; i < width; ++i)
    {
        for (uint16_t j = 0; j < height; ++j)
        {
            QColor colour = image.pixel(i, j);
            pixels[j * width + i].rgb_components = Pixel::RGB(size_t(colour.red()), size_t(colour.green()), size_t(colour.blue()));
        }
    }

}

/**
@brief Blurs the image. New image is stored in the given image container
@param output The image where store the new image
*/
void Image::blur(Image& output)
{
   
    Pixel* start    = output.get_pixels();
    Pixel* end;
    Pixel* original_iterator = pixels;

    float red   = 0.f;
    float green = 0.f;
    float blue  = 0.f;

    uint32_t count = 0;

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
    ++count;

    end = start + width;
    // The rest of the first row
    while (start < end)
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

    end = output.get_pixels() + output.get_width() * output.get_height() - width;

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

void Image::sobel_colour(Image & output)
{
   // output = Image(width, height);

    Pixel* start = pixels;
    Pixel* end = start + width * height - 1;
      

    // set the start in the second row and the end in the previous of the last row
    // This avoid extra conditional operations and the lost of information is not relevant
    start += width;
    end -= (width+1);

    float* buffer = new float[width * height];
    float* writer = buffer;

    float min = 1.f;
    float max = 0.f;  
    
    
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
        *writer = val;

        if (isinf(val))
        {
            val = val;
        }

        max = val > max ? val : max;
        min = val < min ? val : min;

        ++writer;
        ++start;
    }
    Pixel* iterator = output.get_pixels();
    
    int count = 0;
    float* last_value = buffer + (width * height);
    writer = buffer;

    while (writer < last_value)
    {
        *writer = ((*writer) - min) / max - min;

        iterator->rgb_components.red    = (*writer);
        iterator->rgb_components.green  = (*writer);
        iterator->rgb_components.blue   = (*writer);

        ++writer;
        ++iterator;
        ++count;
    }
    
    // Free the memory
    delete [] buffer;
    
}
