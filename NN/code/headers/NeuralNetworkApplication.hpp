#pragma once


#include <qguiapplication.h>
#include <Image.hpp>
#include <NeuralNetwork.hpp>
#include <iostream>

class NeuralNetworkApplication : public QGuiApplication
{
private:

    float* neural_network_input = nullptr;
    float* neural_network_output = nullptr;
    float* neural_network_desired_output= nullptr;

    enum impairment_types {DEUTERANOPIA, PROTANOPIA};
    impairment_types type;

    bool exporting = false;
    std::string export_path;

public:

    /**
    @brief Creates an instance of the application
    */
    NeuralNetworkApplication(int& argc, char** argv) : QGuiApplication(argc, argv)
    {
        type = impairment_types::DEUTERANOPIA;       
       
        // Generate a random values
        /*
        NeuralNetwork net(500 * 500 * 3);
        net.export_network("../../assets/data/data.dat");
        */

        transform("test.png");

        //training(500, 500);
        
        std::cout << std::endl << "Done! :)";
    }

    /**
    @brief Train the neural network with images of the given size
    @param image_width The width of the image
    @param image_height The height of the image
    */
    void training(uint16_t image_width, uint16_t image_height);

    /**
    @brief Transform a given image 
    @param filename The name of the image to transform
    */
    void transform(std::string filename);


    /**
    @brief Extract the input for the neural network from a image data
    @param img The image with the data
    */
    void extract_input_from_image(Image & img)
    {
        uint32_t size = img.get_height() * img.get_width();
        
        Pixel* pixels = img.get_pixels();
        Pixel* last_pixel = pixels + size;

        float* start = neural_network_input;

        copy_pixel_components_to_float(pixels, last_pixel, start);
    }

    /**
    @brief Get the sobel values of a given image and store them in the given collection
    @param original The image to sobel
    @param values The collection where store the values. Each pixel generates 3 values (one for each channel)
    */
    void get_sobel_values(Image& original, float* values);

    /**
    @brief Extract the color components values of a collecion of pixels 
    @param pixels The first pixel in the collection 
    @param last_pixel The last pixel in the collection
    @param start The collection where the values will be stored
    */
    void copy_pixel_components_to_float(Pixel* pixels, Pixel* last_pixel, float* start)
    {
        int count = 0;
        while (pixels < last_pixel)
        {
            *start       = pixels->rgb_components.red;
            *(start + 1) = pixels->rgb_components.green;
            *(start + 2) = pixels->rgb_components.blue;            

            ++pixels;
            start += 3;
            count++;
        }
    }

    /**
    @brief Apply the transformations to the output needed to the output
    @param original The original image
    @param output The output of the neural network. The values will be overriden with the new ones 
    */
    void parse_output(Image& original, float* output);

    /**
    @brief Limit the value to the given limits
    @param original The value to limit
    @param min The min value
    @param max The max value
    @return The limited value
    */
    float limit(float& original, float min, float max)
    {
        return original < min ? min : original > max ? max : original;
    }

   


};
