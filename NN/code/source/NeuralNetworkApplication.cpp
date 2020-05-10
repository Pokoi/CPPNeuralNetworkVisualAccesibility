
#include <NeuralNetwork.hpp>
#include "..\headers\NeuralNetworkApplication.hpp"
#include <limits>

/**
@brief Train the neural network with images of the given size
@param image_width The width of the image
@param image_height The height of the image
*/
void NeuralNetworkApplication::training(uint16_t image_width, uint16_t image_height)
{
    uint16_t dataset_count = 1049; //1049 max
    uint16_t training_iterations = 1;
    std::string path = "../../assets/training_dataset/";

    uint32_t size = image_width * image_height * 3;

    std::vector <float > neural_network_input (size);
    std::vector <float > neural_network_output (size);
    std::vector <float > neural_network_desired_output (size);    

    // Load data from parsed network
    NeuralNetwork net("../../assets/data/data.dat");
      

    // Training
   for (uint16_t i = 0; i < training_iterations; ++i)
    {
        for (uint16_t j = 0; j < dataset_count; ++j)
        {
            Image img(path + std::to_string(j) + ".png");
            Image output_img(img.get_width(), img.get_height());

            extract_input_from_image(img, neural_network_input);

            //get_sobel_values(img, neural_network_desired_output);

            net.feed_forward(neural_network_input, neural_network_output);
           
            // Create the generated image. This is for debug proporse only                    

            int iterator = 0;

            for (auto& pixel : output_img.get_pixels())
            {
                pixel.rgb_components.red = limit(neural_network_output[iterator], 0.f, 1.f);
                pixel.rgb_components.green = limit(neural_network_output[iterator + 1], 0.f, 1.f);
                pixel.rgb_components.blue = limit(neural_network_output[iterator + 2], 0.f, 1.f);

                iterator += 3;
            }            

            img.export_image("../../assets/data/original.png");
            output_img.export_image("../../assets/data/postFF.png");

           // parse_output(img, neural_network_output, output_img);                       

            lms_daltonization(img, neural_network_desired_output);            

            for (auto& f : neural_network_desired_output)
            {
                if (f < 0)
                {
                    int i = 0;
                }
            }

            net.back_propagation(neural_network_output, neural_network_desired_output);        

            //system("cls");
            std::cout << std::endl << std::to_string(i * dataset_count + j) << " of " << std::to_string(dataset_count * training_iterations)<<std::endl;
            
       }
    }

    // Save the neural network values in file
    net.export_network("/../../assets/data/data.dat");    
}

/**
@brief Train the network with genetic algorithm
*/
void NeuralNetworkApplication::genetic_training(uint16_t image_width, uint16_t image_height, std::string data_path)
{
    const uint8_t network_count = 20;
    const uint8_t genetic_generations = 100;
    const uint16_t dataset_count = 5; //1049 max
    const uint16_t training_iterations = 1;
    
    std::string path = "../../assets/training_dataset/";

    const uint32_t size = image_width * image_height * 3;

    std::vector <float > neural_network_input(size);    
    std::vector <float > neural_network_desired_output(size);
    
    uint8_t best_parent_index = 0;
    uint8_t second_best_parent_index = 0;

    // Create random networks
    std::vector<std::shared_ptr<NeuralNetwork>> networks(network_count);
    initialize_networks(networks, network_count - 1);

    networks[network_count - 1] = std::make_shared<NeuralNetwork>(data_path);

    // Do the training for each image and each training iteration
    for (uint16_t i = 0; i < training_iterations; ++i)
    {
        for (uint16_t j = 0; j < dataset_count; ++j)
        {
            // Extract input
            Image img(path + std::to_string(j) + ".png");
            Image output_img(img.get_width(), img.get_height());

            extract_input_from_image(img, neural_network_input);
            
            //img.export_image("../../assets/data/original.png");

            // Extract desired outputs
            if (evaluation == evaluation_type::LMS)
            {
                lms_daltonization(img, neural_network_desired_output);
            }
            else if (evaluation == evaluation_type::RGB)
            {
                rgb_daltonization(img, neural_network_desired_output);           
            }

            // For each genetic iteration
            for (uint8_t genetic_iteration = 0; genetic_iteration < genetic_generations; ++genetic_iteration)
            {
                int neural_network_index = 0;
                
                best_parent_index = 0;
                second_best_parent_index = 0;

                float best_delta = std::numeric_limits<float>::max();
                float second_best_delta = std::numeric_limits<float>::max();

                // For each neural network generated
                for (auto& net : networks)
                {
                    std::vector <float > neural_network_output(size);                    

                    net->feed_forward(neural_network_input, neural_network_output);

                    // Create the generated image. This is for debug proporse only                 
                    /*int iterator = 0;

                    for (auto& pixel : output_img.get_pixels())
                    {
                        pixel.luv_components.l  = limit(neural_network_output[iterator], 0.f, 1.f);
                        pixel.luv_components.u  = limit(neural_network_output[iterator + 1], 0.f, 1.f);
                        pixel.luv_components.v  = limit(neural_network_output[iterator + 2], 0.f, 1.f);

                        iterator += 3;
                    }*/
                    //output_img.export_image("../../assets/data/post_element_" + std::to_string(neural_network_index) + ".png");
            
                    // Calculate delta                   
                    int iterator = 0;
                    float delta = 0;

                    while (iterator < size)
                    {
                        Pixel first;
                        Pixel second;

                        first.luv_components  = Pixel::LUV(neural_network_output[iterator], neural_network_output[iterator + 1], neural_network_output[iterator + 2]);
                        second.luv_components = Pixel::LUV(neural_network_desired_output[iterator], neural_network_desired_output[iterator + 1], neural_network_desired_output[iterator + 2]);

                        delta += calculate_delta(first, second);                                        
                        iterator += 3;
                    }

                    if (delta < best_delta)
                    {
                        best_parent_index = neural_network_index;
                        best_delta = delta;
                    }
                    else if (delta < second_best_delta)
                    {
                        second_best_parent_index = neural_network_index;
                        second_best_delta = delta;
                    }

                    neural_network_index++;
                }             

                // Recombine
                recombine_networks(networks, best_parent_index, second_best_parent_index);

            system("cls");
            std::cout << std::endl << " Evaluation: " + data_path << std::endl
                                   << " Genetic iteration : " << std::to_string(genetic_iteration) << " / " << std::to_string(genetic_generations) << std::endl
                                   << " Training iteration: " << std::to_string(i * dataset_count + j) << " / " << std::to_string(dataset_count * training_iterations) << std::endl;                                 
            }

            // Export the data of the  best generated network
            networks[best_parent_index]->export_network(data_path);
        }
    } 
    
    // Export the data of the  best generated network
    networks[best_parent_index]->export_network(data_path);

}

/**
@brief Transform a given image
@param filename The name of the image to transform
*/
void NeuralNetworkApplication::transform(std::string filename, std::string data_path)
{
    std::string path = "../../assets/sample/" + filename;
    //std::string path = "../../assets/training_dataset/" + filename;
    exporting = true;

    // Load the image
    Image img(path);

    // Load the neural network values
    std::ifstream stream;

    stream.open(data_path);
    std::string content;
    std::getline(stream, content);
    stream.close();

    BinaryData data;
    data.read(content);

    // Export original
    export_path = "../../assets/generated/original.png";
    img.export_image(export_path);
    
    // Calculate transformed simulation
    for (auto& pixel : img.get_pixels())
    {
        pixel.simulate_deuteranopia();
    }
    export_path = "../../assets/generated/original_deuteranopia_simulation.png";
    img.export_image(export_path);


    // Export original simulated
    img = Image(path);
    for (auto& pixel : img.get_pixels())
    {        
        pixel.lms_deuteranopia();
    }
    export_path = "../../assets/generated/lms_simulation.png";
    img.export_image(export_path);

    for (auto& pixel : img.get_pixels())
    {
        pixel.simulate_deuteranopia();
    }
    export_path = "../../assets/generated/lms_simulation_colorblind_simulate.png";
    img.export_image(export_path);

    // Export rgb daltonization simulated
    img = Image(path);
    for (auto& pixel : img.get_pixels())
    {
        pixel.rgb_daltonization();
    }
    export_path = "../../assets/generated/rgb_simulation.png";
    img.export_image(export_path);

    for (auto& pixel : img.get_pixels())
    {
        pixel.simulate_deuteranopia();
    }
    export_path = "../../assets/generated/rgb_simulation_colorblind_simulate.png";
    img.export_image(export_path);


    float w1 = data.wa * data.wd;
    float w2 = data.wb * data.wd;
    float w3 = data.wc * data.wd;
    float w4 = data.wa * data.we;
    float w5 = data.wb * data.we;
    float w6 = data.wc * data.we;
    float w7 = data.wa * data.wf;
    float w8 = data.wb * data.wf;
    float w9 = data.wc * data.wf;

    // Calculate transformed    
    
    img = Image(path);       
    
    for (auto& pixel : img.get_pixels())
    {        
        pixel.convert_rgb_to_luv();

        float r = pixel.rgb_components.red;
        float g = pixel.rgb_components.green;
        float b = pixel.rgb_components.blue;

        float l = pixel.luv_components.l;
        float u = pixel.luv_components.u;
        float v = pixel.luv_components.v;

        float Li = l * w1 + u * w2 + v * w3;
        float Ui = l * w4 + u * w5 + v * w6;
        float Vi = l * w7 + u * w8 + v * w9;

        pixel.luv_components.l    = Li;
        pixel.luv_components.u    = Ui;
        pixel.luv_components.v    = Vi;
                
        pixel.convert_luv_to_rgb();  
        
        float Ri = pixel.rgb_components.red;
        float Gi = pixel.rgb_components.green;
        float Bi = pixel.rgb_components.blue;

        /*
        float Dr = r - Ri;
        float Dg = g - Gi;
        float Db = b - Bi;
        float Rm = (0.f  * Dr + 0.f  * Dg + 0.f * Db) + r ;
        float Gm = (0.7f * Dr + 1.f  * Dg + 0.f * Db) + g ;
        float Bm = (0.7f * Dr + 0.7f * Dg + 1.f * Db) + b ;
        */
        
        ///*
        float Rm = (Ri + r) ;
        float Gm = (Gi + g) ;
        float Bm = (Bi + b) + Ri;

        pixel.rgb_components.red = Rm < 0.f ? 0.f : Rm > 1.f ? 1.f : Rm;
        pixel.rgb_components.green = Gm < 0.f ? 0.f : Gm > 1.f ? 1.f : Gm;
        pixel.rgb_components.blue = Bm < 0.f ? 0.f : Bm > 1.f ? 1.f: Bm;
    }
    /*
    for (auto& pixel : img.get_pixels())
    {
        pixel.rgb_daltonization();
    }*/

    export_path = "../../assets/generated/transformed.png";
    img.export_image(export_path);

    // Calculate transformed simulation
    for (auto& pixel : img.get_pixels())
    {
        pixel.simulate_deuteranopia();
    }
    export_path = "../../assets/generated/transformed_deuteranopia_simulation.png";
    img.export_image(export_path);     
    
}

/**
@brief Get the sobel values of a given image and store them in the given collection
@param original The image to sobel
@param values The collection where store the values. Each pixel generates 3 values (one for each channel)
*/

void NeuralNetworkApplication::get_sobel_values(Image& original, std::vector<float>& values)
{
    uint32_t size = original.get_height() * original.get_width();

    original.blur();
    original.sobel_colour();   

    if (exporting)
    {
        original.export_image(export_path);
    }

    copy_pixel_components_to_float(original.get_pixels(), size, values);  

}

/**
@brief Apply the transformations to the output needed to the output
@param original The original image
@param output The output of the neural network. The values will be overriden with the new ones
*/
void NeuralNetworkApplication::parse_output(Image& original, std::vector<float> & output, Image& output_img)
{
    // Create an image with the neural network output data
    uint32_t size = original.get_height() * original.get_width();  

    int iterator = 0;
    
    for (auto& pixel : output_img.get_pixels())
    {
        pixel.rgb_components.red = limit(output[iterator], 0.f, 1.f);
        pixel.rgb_components.green = limit(output[iterator + 1], 0.f, 1.f);
        pixel.rgb_components.blue = limit(output[iterator + 2], 0.f, 1.f);

        iterator += 3;
    }
   
    original.export_image("../../assets/data/original.png");
    output_img.export_image("../../assets/data/postFF.png");

    // Impaired simulation
    switch (type)
    {
    case DEUTERANOPIA:                

        for (auto& pixel : output_img.get_pixels())
        {
            pixel.simulate_deuteranopia();           
        }
        output_img.export_image("../../assets/data/deut.png");
        break;

    case PROTANOPIA:

        for (auto& pixel : output_img.get_pixels())
        {
            pixel.simulate_protanopia();
        }
        break;
    }    

    // Sobel the image
    get_sobel_values(output_img, output);
}

/**
@brief Calculates the values of lms daltonization process
@param original The original image
@param output_values The collection where store the values
*/
void NeuralNetworkApplication::lms_daltonization(Image& original, std::vector<float>& output_values)
{
   switch (type)
    {
    case DEUTERANOPIA:

        for (auto& pixel : original.get_pixels())
        {
            pixel.lms_deuteranopia();
        }
        //original.export_image("../../assets/data/deuteranopia_LMS.png");
        break;

    case PROTANOPIA:

        for (auto& pixel : original.get_pixels())
        {
            pixel.lms_protanopia();
        }
        //original.export_image("../../assets/data/protanopia_LMS.png");
        break;

    case TRITANOPIA:

        for (auto& pixel : original.get_pixels())
        {
            pixel.lms_tritanopia();
        }
        //original.export_image("../../assets/data/tritanopia_LMS.png");
        break;
    }    

   copy_pixel_components_to_float(original.get_pixels(), original.get_width() * original.get_height(), output_values);
}
