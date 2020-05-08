
#include <Image.hpp>


/**
@brief Exports the image to the given path
@param path The path where exports the image
*/
void Image::export_image(std::string path)
{
    QImage img = QImage(int(width), int(height), QImage::Format::Format_RGB32);
    QColor value;

    for (uint16_t i = 0; i < width; ++i)
    {
        for (uint16_t j = 0; j < height; ++j)
        {
            Pixel pixel = pixels[j * width + i];
            
            value = QColor(pixel.rgb_components.red * 255, pixel.rgb_components.green * 255, pixel.rgb_components.blue * 255);
            if (pixel.rgb_components.red < 0 || pixel.rgb_components.green < 0 || pixel.rgb_components.blue < 0)
            {
                int i = 0;
            }

            img.setPixelColor(i, j, value);
        }
    }
    
    for (uint16_t i = 0; i < width; ++i)
    {
        for (uint16_t j = 0; j < height; ++j)
        {
            QColor colour = img.pixel(i, j);
            pixels[j * width + i].rgb_components = Pixel::RGB(size_t(colour.red()), size_t(colour.green()), size_t(colour.blue()));
            pixels[j * width + i].convert_rgb_to_luv();
        }
    }   


    img.save(QString::fromStdString(path));
}

/**
@brief Loads an image data from a file path
#param path The path of the image
*/
Image::Image(std::string path) : pixels{500 * 500}
{
    QPixmap pm(QString(path.c_str()));
    QImage image = (pm.toImage());
    width = image.width();
    height = image.height();
    
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
@brief Blurs the image. 
*/
void Image::blur()
{  
    int iterator = 0;
    int end = 0;

    float red   = 0.f;
    float green = 0.f;
    float blue  = 0.f;


    // The critical rows are the first one and last one because they have no previous or next one.
    // They have to be calculated separatly

    // The first row of pixels

    // The firs pixel has no previous pixel. 
    //To prevent to use a conditional, the first pixel is calculated as follows
    add_components(red, green, blue, pixels[iterator]);
    add_components(red, green, blue, pixels[iterator + 1]);
    add_components(red, green, blue, pixels[iterator + width]);
    add_components(red, green, blue, pixels[iterator + width + 1]);

    pixels[iterator].rgb_components.red   = red   * 0.25f;
    pixels[iterator].rgb_components.green = green * 0.25f;
    pixels[iterator].rgb_components.blue  = blue  * 0.25f;

    ++iterator;   
   

    // The rest of the first row
    end = width;
    while (iterator < width)
    {
        red     = 0.f;
        green   = 0.f;
        blue    = 0.f;

        add_components(red, green, blue, pixels[iterator]);
        add_components(red, green, blue, pixels[iterator - 1 ]);
        add_components(red, green, blue, pixels[iterator + 1 ]);
        add_components(red, green, blue, pixels[iterator + width]);
        add_components(red, green, blue, pixels[iterator + width + 1]);
        add_components(red, green, blue, pixels[iterator + width - 1]);

        pixels[iterator].rgb_components.red   = red   * 0.16f;
        pixels[iterator].rgb_components.green = green * 0.16f;
        pixels[iterator].rgb_components.blue  = blue  * 0.16f;

        ++iterator;        
    }

    end = width * height - width;

    // The body of the image
    while (iterator < end)
    {
        red     = 0.f;
        green   = 0.f;
        blue    = 0.f;

        add_components(red, green, blue, pixels[iterator]);
        add_components(red, green, blue, pixels[iterator - 1]);
        add_components(red, green, blue, pixels[iterator + 1]);
        add_components(red, green, blue, pixels[iterator + width]);
        add_components(red, green, blue, pixels[iterator + width + 1]);
        add_components(red, green, blue, pixels[iterator + width - 1]);
        add_components(red, green, blue, pixels[iterator - width]);
        add_components(red, green, blue, pixels[iterator - width + 1]);
        add_components(red, green, blue, pixels[iterator - width - 1]);

        pixels[iterator].rgb_components.red = red     * 0.11f;
        pixels[iterator].rgb_components.green = green * 0.11f;
        pixels[iterator].rgb_components.blue = blue   * 0.11f;

        ++iterator;
    }

    // The last row of pixels
    end += width;
    while (iterator < end - 1)
    {
        red     = 0.f;
        green   = 0.f;
        blue    = 0.f;

        add_components(red, green, blue, pixels[iterator]);
        add_components(red, green, blue, pixels[iterator - 1]);
        add_components(red, green, blue, pixels[iterator + 1]);
        add_components(red, green, blue, pixels[iterator - width]);
        add_components(red, green, blue, pixels[iterator - width - 1]);
        add_components(red, green, blue, pixels[iterator - width + 1]);

        pixels[iterator].rgb_components.red   = red   * 0.16f;
        pixels[iterator].rgb_components.green = green * 0.16f;
        pixels[iterator].rgb_components.blue  = blue  * 0.16f;

        ++iterator;
    }

    // The last pixel
    red     = 0.f;
    green   = 0.f;
    blue    = 0.f;

    add_components(red, green, blue, pixels[iterator]);
    add_components(red, green, blue, pixels[iterator - 1]);
    add_components(red, green, blue, pixels[iterator - width]);
    add_components(red, green, blue, pixels[iterator - width - 1]);

    pixels[iterator].rgb_components.red   = red   * 0.25f;
    pixels[iterator].rgb_components.green = green * 0.25f;
    pixels[iterator].rgb_components.blue  = blue  * 0.25f;

}

/**
@brief Apply a sobel colour effect to the image
*/
void Image::sobel_colour()
{
    int end = 0;      
    int iterator = 0;

    // set the start in the second row and the end in the previous of the last row
    // This avoid extra conditional operations and the lost of information is not relevant
    iterator += width + 1;
    end -= (width+1);

    float* buffer = new float[width * height];
    float* writer = buffer;

    float min = 1.f;
    float max = 0.f;  
    
    
    while (iterator < end)
    {
        float gx =
            1.f * colour_difference(pixels[iterator - width - 1], pixels[iterator - width + 1]) +
            2.f * colour_difference(pixels[iterator - 1], pixels[iterator + 1]) +
            1.f * colour_difference(pixels[iterator - 1 + width], pixels[iterator + width + 1]);

        float gy =
            1.f * colour_difference(pixels[iterator - width - 1], pixels[iterator + width - 1]) +
            2.f * colour_difference(pixels[iterator - width], pixels[iterator + width]) +
            1.f * colour_difference(pixels[iterator + 1 - width], pixels[iterator + width + 1]);

        float val = pow(gx * gx + gy * gy, 0.5);
        *writer = val;         

        max = val > max ? val : max;
        min = val < min ? val : min;

        ++writer;
        ++iterator;
    }
        
    float* last_value = buffer + (width * height);
    writer = buffer;
    iterator = 0;

    while (writer < last_value)
    {
        //if(max - min != 0.f)
        //*writer = ((*writer) - min) / max - min;

        pixels[iterator].rgb_components.red    = (*writer);
        pixels[iterator].rgb_components.green  = (*writer);
        pixels[iterator].rgb_components.blue   = (*writer);
       
        ++writer;
        ++iterator;
        
    }
    
    // Free the memory
    delete [] buffer;
    
}
