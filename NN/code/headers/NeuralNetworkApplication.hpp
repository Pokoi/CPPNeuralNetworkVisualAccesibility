#pragma once


#include <qguiapplication.h>
#include <Image.hpp>
#include <NeuralNetwork.hpp>
#include <iostream>
#include <memory>

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
        //NeuralNetwork net(500 * 500 * 3);
        //net.export_network("../../assets/data/data.dat");        

        // Train
        //training(500, 500);

        // Generate a test
        //transform("test.png");

        // Genetic training
        genetic_training(500, 500, "../../assets/data/data.dat");
        
        std::cout << std::endl << "Done! :)";
    }

    /**
    @brief Train the neural network with images of the given size
    @param image_width The width of the image
    @param image_height The height of the image
    */
    void training(uint16_t image_width, uint16_t image_height);

    /**
    @brief Train the network with genetic algorithm
    */
    void genetic_training(uint16_t image_width, uint16_t image_height, std::string data_path);


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

    /**
    @brief Calculates the values of rgb daltonization process
    @param original The original image
    @param output The collection where store the values
    */
    void rgb_daltonization(Image& original, std::vector<float>& output_values)
    {
        for (auto& pixel : original.get_pixels())
        {
            pixel.rgb_daltonization();
        }
        original.export_image("../../assets/data/rgb_daltonization.png");

        copy_pixel_components_to_float(original.get_pixels(), original.get_width() * original.get_height(), output_values);
    }

    private:

        /**
        @brief Creates random networks in the given collection
        @param networks The collection of networks
        @param amount The amount to networks to add
        */
        void initialize_networks(std::vector<std::shared_ptr<NeuralNetwork>>& networks, uint8_t amount)
        {
            int iterator = 0;
            while (iterator < amount)
            {
                networks[iterator] = std::make_shared<NeuralNetwork>(500 * 500 * 3);
                ++iterator;
            }
        }

        /**
        @brief Recombine the given network parents 
        @param parent_1_index The index of the firs parent
        @param parent_2_index The index of the second parent
        */
        void recombine_networks(std::vector<std::shared_ptr<NeuralNetwork>>& networks,  uint8_t parent_1_index, uint8_t parent_2_index)
        {          
            BinaryData parent_1_binary_data = networks[parent_1_index]->get_binary_data();
            BinaryData parent_2_binary_data = networks[parent_2_index]->get_binary_data();

            for (auto& net : networks)
            {
                BinaryData network_data = net->get_binary_data();

                //For each component                               
                recombine_weights(network_data.wa, parent_1_binary_data.wa, parent_2_binary_data.wa);
                recombine_weights(network_data.wb, parent_1_binary_data.wb, parent_2_binary_data.wb);
                recombine_weights(network_data.wc, parent_1_binary_data.wc, parent_2_binary_data.wc);
                recombine_weights(network_data.wd, parent_1_binary_data.wd, parent_2_binary_data.wd);
                recombine_weights(network_data.we, parent_1_binary_data.we, parent_2_binary_data.we);
                recombine_weights(network_data.wf, parent_1_binary_data.wf, parent_2_binary_data.wf);

                net->apply_binary_data(network_data);
            }           
        }

        /**
        @brief Recombine the given weights and mutate if needed
        @param original The param where the new value will be stored
        @param parent_1_value The value of the first parent
        @param parent_2_value The value of the second parent
        */
        void recombine_weights(float& original, float& parent_1_value, float& parent_2_value)
        {
            const float parent_1_prob = 0.45f;
            const float parent_2_prob = 0.45f;
            const float mutation_prob = 0.1f;

            float action = (float(rand()) / RAND_MAX);
            
            original = action < parent_1_prob                 ? parent_1_value :
                       action < parent_1_prob + parent_2_prob ? parent_2_value : 
                                                                (float(rand()) / RAND_MAX) * (5.f - (-5.f)) + (-5.f);
   
        }

        /**
        @brief Calculates delta color of two pixels
        @param first The first pixel
        @param second The second pixel
        @return The delta 
        */
        float calculate_delta(Pixel & first, Pixel & second)
        {
            first.convert_rgb_to_luv();
            second.convert_rgb_to_luv();

            return pow(
                pow(second.luv_components.l - first.luv_components.l, 2.f) +
                pow(second.luv_components.u - first.luv_components.u, 2.f) +
                pow(second.luv_components.v - first.luv_components.v, 2.f), 0.5f

                );
        
        }

};
