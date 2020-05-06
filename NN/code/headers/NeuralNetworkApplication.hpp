#pragma once


#include <qguiapplication.h>
#include <Image.hpp>
#include <NeuralNetwork.hpp>
#include <iostream>

class NeuralNetworkApplication : public QGuiApplication
{
private:
       
    enum impairment_types {DEUTERANOPIA, PROTANOPIA, TRITANOPIA};
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
        NeuralNetwork net(500 * 500 * 3);
        net.export_network("../../assets/data/data.dat");        

        // Train
        training(500, 500);

        // Generate a test
        transform("test.png");
        
        std::cout << std::endl << "Done! :)";
    }

    /**
    @brief Train the neural network with images of the given size
    @param image_width The width of the image
    @param image_height The height of the image
    */
    void training(uint16_t image_width, uint16_t image_height);


    //void 


    /**
    @brief Transform a given image 
    @param filename The name of the image to transform
    */
    void transform(std::string filename);

    /**
    @brief Extract the input for the neural network from a image data
    @param img The image with the data
    @param input The collection where store the input values 
    */
    void extract_input_from_image(Image & img, std::vector<float> & input)
    {
        uint32_t size = img.get_height() * img.get_width();       
        
        copy_pixel_components_to_float(img.get_pixels(), size, input);
    }

    /**
    @brief Get the sobel values of a given image and store them in the given collection
    @param original The image to sobel
    @param values The collection where store the values. Each pixel generates 3 values (one for each channel)
    */
    void get_sobel_values(Image& original, std::vector<float>& values);

    /**
    @brief Extract the color components values of a collecion of pixels 
    @param pixels The first pixel in the collection 
    @param last_pixel The last pixel index in the collection
    @param start The collection where the values will be stored
    */
    void copy_pixel_components_to_float(std::vector<Pixel> & pixels, int last_pixel, std::vector<float>& start)
    {       
        int iterator = 0;
        int pixel_iterator = 0;

        while (pixel_iterator < last_pixel)
        {
            start [iterator]     = pixels[pixel_iterator].rgb_components.red;
            start [iterator + 1] = pixels[pixel_iterator].rgb_components.green;
            start [iterator + 2] = pixels[pixel_iterator].rgb_components.blue;

            if (start[iterator] < 0 || start[iterator + 1] < 0 || start[iterator + 2] < 0)
            {
                int i = 0;
            }

            ++pixel_iterator;
            iterator += 3;
        }
    }

    /**
    @brief Apply the transformations to the output needed to the output
    @param original The original image
    @param output The output of the neural network. The values will be overriden with the new ones 
    */
    void parse_output(Image& original, std::vector<float> & output, Image& output_image);

    /**
    @brief Limit the value to the given limits
    @param original The value to limit
    @param min The min value
    @param max The max value
    @return The limited value
    */
    float limit(float original, float min, float max)
    {
        return original < min ? min : original > max ? max : original;
    }

    /**
    @brief Calculates the values of lms daltonization process
    @param original The original image
    @param output The collection where store the values
    */
    void lms_daltonization(Image& original, std::vector<float> & output);
   


};
