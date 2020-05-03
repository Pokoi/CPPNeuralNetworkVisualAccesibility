#pragma once

#include <QtCore/QCoreApplication>
#include <Image.hpp>
#include <NeuralNetwork.hpp>

class NeuralNetworkApplication : public QCoreApplication
{
private:

    float* neural_network_input = nullptr;
    float* neural_network_output = nullptr;
    float* neural_network_desired_output= nullptr;

public:

    /**
    @brief Creates an instance of the application
    */
    NeuralNetworkApplication(int& argc, char** argv) : QCoreApplication(argc, argv)
    {
       
    }

    /**
    @brief Train the neural network with images of the given size
    @param image_width The width of the image
    @param image_height The height of the image
    */
    void training(uint16_t image_width, uint16_t image_height)
    {
        uint16_t dataset_count = 100;
        uint16_t training_iterations = 10;
        std::string path = "../../assets/training_dataset/";

        uint32_t size = image_width * image_height;

        neural_network_input            = new float[size];
        neural_network_output           = new float[size];
        neural_network_desired_output   = new float[size];


        // Load data from parsed network
        NeuralNetwork net(size * 3);


        // Training
        for (uint16_t i = 0; i < training_iterations; ++i)
        {
            for (uint16_t j = 0; j < dataset_count; ++j)
            {
                Image img(path + std::to_string(j + 1) + ".jpg");

                extract_input_from_image(img);
                
                get_sobel_values(img, neural_network_desired_output);
                
                net.feed_forward(neural_network_input, neural_network_output);

                parse_output(img, neural_network_output);

                net.back_propagation(neural_network_output, neural_network_desired_output);
                
            }
        }

        // Save the neural network values in file


        // Frees the memory
        delete[] neural_network_input;
        delete[] neural_network_output;
        delete[] neural_network_desired_output;
    }

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
    void get_sobel_values(Image & original, float* values)
    {
        uint32_t size = original.get_height() * original.get_width();

        Image blurred (original.get_width(), original.get_height());
        Image sobeled (original.get_width(), original.get_height());

        original.blur(blurred);
        blurred.sobel_colour(sobeled);

        Pixel* pixels = sobeled.get_pixels();
        Pixel* last_pixel = pixels + size;

        float* start = values;

        copy_pixel_components_to_float(pixels, last_pixel, start);    
    }

    /**
    @brief Extract the color components values of a collecion of pixels 
    @param pixels The first pixel in the collection 
    @param last_pixel The last pixel in the collection
    @param start The collection where the values will be stored
    */
    void copy_pixel_components_to_float(Pixel* pixels, Pixel* last_pixel, float* start)
    {
        while (pixels < last_pixel)
        {
            *start       = pixels->rgb_components.red;
            *(start + 1) = pixels->rgb_components.green;
            *(start + 2) = pixels->rgb_components.blue;

            ++pixels;
            start += 3;
        }
    }


    void parse_output(Image & original, float* output);



};
