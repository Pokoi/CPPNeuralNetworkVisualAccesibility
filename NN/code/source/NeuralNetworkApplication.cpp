
#include <NeuralNetwork.hpp>
#include "..\headers\NeuralNetworkApplication.hpp"

/**
@brief Train the neural network with images of the given size
@param image_width The width of the image
@param image_height The height of the image
*/
void NeuralNetworkApplication::training(uint16_t image_width, uint16_t image_height)
{
    uint16_t dataset_count = 5;
    uint16_t training_iterations = 1;
    std::string path = "../../assets/training_dataset/";

    uint32_t size = image_width * image_height * 3;

    neural_network_input = new float[size];
    neural_network_output = new float[size];
    neural_network_desired_output = new float[size];


    // Load data from parsed network
    NeuralNetwork net("../../assets/data/data.dat");

    // Training
    for (uint16_t i = 0; i < training_iterations; ++i)
    {
        for (uint16_t j = 0; j < dataset_count; ++j)
        {
            Image img(path + std::to_string(j) + ".png");

            extract_input_from_image(img);

            get_sobel_values(img, neural_network_desired_output);

            net.feed_forward(neural_network_input, neural_network_output);

            parse_output(img, neural_network_output);

            net.back_propagation(neural_network_output, neural_network_desired_output);

        }
    }

    // Save the neural network values in file

    net.export_network("/../../assets/data/data.dat");

    // Frees the memory
    delete[] neural_network_input;
    delete[] neural_network_output;
    delete[] neural_network_desired_output;
}

/**
@brief Transform a given image
@param filename The name of the image to transform
*/
void NeuralNetworkApplication::transform(std::string filename)
{
    std::string path = "../../assets/sample/" + filename;
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
    float* values = new float[img.get_width() * img.get_height() * 3];
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
    values = new float[img.get_width() * img.get_height() * 3];
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
    values = new float[img.get_width() * img.get_height() * 3];
    get_sobel_values(img, values);


}

/**
@brief Get the sobel values of a given image and store them in the given collection
@param original The image to sobel
@param values The collection where store the values. Each pixel generates 3 values (one for each channel)
*/

void NeuralNetworkApplication::get_sobel_values(Image& original, float* values)
{
    uint32_t size = original.get_height() * original.get_width();

    Image blurred(original.get_width(), original.get_height());
    Image sobeled(original.get_width(), original.get_height());

    original.blur(blurred);
    blurred.sobel_colour(sobeled);

    if (exporting)
    {
        sobeled.export_image(export_path);
    }

    Pixel* pixels = sobeled.get_pixels();
    Pixel* last_pixel = pixels + size;

    float* start = values;

    copy_pixel_components_to_float(pixels, last_pixel, start);
}

/**
@brief Apply the transformations to the output needed to the output
@param original The original image
@param output The output of the neural network. The values will be overriden with the new ones
*/

void NeuralNetworkApplication::parse_output(Image& original, float* output)
{
    // Create an image with the neural network output data
    uint32_t size = original.get_height() * original.get_width();

    Image output_img(original.get_width(), original.get_height());

    Pixel* start = output_img.get_pixels();
    Pixel* last_pixel = start + size;

    float* pixel_components = output;

    while (start < last_pixel)
    {
        start->rgb_components.red = limit(*(pixel_components), 0.f, 1.f);
        start->rgb_components.green = limit(*(pixel_components + 1), 0.f, 1.f);
        start->rgb_components.blue = limit(*(pixel_components + 2), 0.f, 1.f);

        ++start;
        pixel_components += 3;
    }

    // Impaired simulation
    switch (type)
    {
    case DEUTERANOPIA:

        start = output_img.get_pixels();

        while (start < last_pixel)
        {
            start->simulate_deuteranopia();
            ++start;
        }

        break;

    case PROTANOPIA:

        start = output_img.get_pixels();

        while (start < last_pixel)
        {
            start->simulate_protanopia();
            ++start;
        }

        break;
    }

    // Sobel the image
    get_sobel_values(original, output);
}
