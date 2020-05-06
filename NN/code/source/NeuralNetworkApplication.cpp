
#include <NeuralNetwork.hpp>
#include "..\headers\NeuralNetworkApplication.hpp"

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
@brief Transform a given image
@param filename The name of the image to transform
*/
void NeuralNetworkApplication::transform(std::string filename)
{
    /*std::string path = "../../assets/sample/" + filename;
    exporting = true;

    // Load the image
    Image img(path);

    // Load the neural network values
    std::ifstream stream;

    stream.open("../../assets/data/data.dat");
    std::string content;
    std::getline(stream, content);
    stream.close();

    BinaryData data;
    data.read(content);

    // Export original
    export_path = "../../assets/generated/original.png";
    img.export_image(export_path);

    // Export original sobeled
    export_path = "../../assets/generated/original_sobeled.png";
    std::vector<float> values (img.get_width() * img.get_height() * 3);
    get_sobel_values(img, values);

    // Export original simulated
    Pixel* start = img.get_pixels();
    Pixel* end = start + (img.get_width() * img.get_height());

    while (start < end)
    {
        start->simulate_deuteranopia();
        ++start;
    }

    export_path = "../../assets/generated/original_deuteranopia_simulation.png";
    img.export_image(export_path);

    // Export original simulated sobeled
    export_path = "../../assets/generated/original_deuteranopia_sobeled.png";    
    get_sobel_values(img, values);

    // Calculate transformed
    img = Image(path);
    start = img.get_pixels();
    end = start + (img.get_width() * img.get_height());

    float w1 = data.wa * data.wd;
    float w2 = data.wb * data.wd;
    float w3 = data.wc * data.wd;
    float w4 = data.wa * data.we;
    float w5 = data.wb * data.we;
    float w6 = data.wc * data.we;
    float w7 = data.wa * data.wf;
    float w8 = data.wb * data.wf;
    float w9 = data.wc * data.wf;


    while (start < end)
    {
        start->rgb_components.red = start->rgb_components.red * w1 +
            start->rgb_components.green * w2 +
            start->rgb_components.blue * w3;

        start->rgb_components.green = start->rgb_components.red * w4 +
            start->rgb_components.green * w5 +
            start->rgb_components.blue * w6;

        start->rgb_components.blue = start->rgb_components.red * w7 +
            start->rgb_components.green * w8 +
            start->rgb_components.blue * w9;
        ++start;
    }

    export_path = "../../assets/generated/transformed.png";
    img.export_image(export_path);

    // Calculate transformed simulation
    start = img.get_pixels();
    while (start < end)
    {
        start->simulate_deuteranopia();
        ++start;
    }

    export_path = "../../assets/generated/transformed_deuteranopia_simulation.png";
    img.export_image(export_path);

    // Export transformed sobeled
    export_path = "../../assets/generated/transformed_deuteranopia_sobeled.png";    
    get_sobel_values(img, values);
    */
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
        original.export_image("../../assets/data/deuteranopia_LMS.png");
        break;

    case PROTANOPIA:

        for (auto& pixel : original.get_pixels())
        {
            pixel.lms_protanopia();
        }
        original.export_image("../../assets/data/protanopia_LMS.png");
        break;

    case TRITANOPIA:

        for (auto& pixel : original.get_pixels())
        {
            pixel.lms_tritanopia();
        }
        original.export_image("../../assets/data/tritanopia_LMS.png");
        break;
    }    

   copy_pixel_components_to_float(original.get_pixels(), original.get_width() * original.get_height(), output_values);
}
