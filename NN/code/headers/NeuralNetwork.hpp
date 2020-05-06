#pragma once

#include <Layer.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

struct BinaryData
{
    float wa;
    float wb;
    float wc;
    float wd;
    float we;
    float wf;

    uint32_t first_layer_neurons;

    std::string to_string()
    {
        return  std::to_string(wa) + "&" +
                std::to_string(wb) + "&" +
                std::to_string(wc) + "&" +
                std::to_string(wd) + "&" +
                std::to_string(we) + "&" +
                std::to_string(wf) + "&" +
                std::to_string(first_layer_neurons);
                            
    }

    void read(std::string s)
    {
        std::stringstream test(s);
        std::string segment;
        std::vector<std::string> collection;

        while (std::getline(test, segment, '&'))
        {
            collection.push_back(segment);
        }

        wa = std::stof(collection[0]);
        wb = std::stof(collection[1]);
        wc = std::stof(collection[2]);
        wd = std::stof(collection[3]);
        we = std::stof(collection[4]);
        wf = std::stof(collection[5]);
        first_layer_neurons = std::stoul(collection[6]);        
    }
};

class NeuralNetwork
{
private:

    Layer** layers;
    uint32_t layers_count;
    float learning_rate = 0.01f;
       

public:

    /**
    @brief Creates a neural network with the proposed structure
    @param first_layer_neurons The amount of neurons in the input layer
    @param layers_count The amount of layers. By default it's value is 3
    */
    NeuralNetwork( uint32_t first_layer_neurons, uint32_t layers_count = 3 ) : layers_count{layers_count}
    {
        this->layers = new Layer * [layers_count];
        
        // The proposed method only has 3 layers connected in a particular way
        // The first or input layer
        initialize_layer(0, first_layer_neurons, 0);
        // The second or hidden layer
        initialize_layer(1, uint32_t (first_layer_neurons / 3), first_layer_neurons);
        // The third or output layer
        initialize_layer(2, first_layer_neurons, uint32_t (first_layer_neurons / 3));
    }

    /**
    @brief Creates a neural network with the data of a binary file
    @param path The path of the file with the data
    */
    NeuralNetwork(std::string path);

    /**
    @brief Export the neural network data to a binary file
    @param path The path of the file where the data will be exported
    */
    void export_network(std::string path);
       
    Layer** get_layers()
    {
        return layers;
    }

    /**
    @brief Releases the dynamic memory
    */
    ~NeuralNetwork()
    {
        for (uint32_t i = 0; i < layers_count; ++i)
        {
            delete layers[i];
        }

        delete[] layers;
    }
    

    /**
    @brief Calculates the value of each neuron by the feed_forward process. The output is stored in the given parameter.
    @param inputs A pointer to the first input
    @param output A pointer to the first output
    */
    void feed_forward(std::vector<float>& inputs, std::vector<float>& output);

    /**
    @brief Calculates the backpropagation
    @param output The output values of the information proccess of the feed_forward output
    @param desired The desired values
    */
    void back_propagation(std::vector<float>& output, std::vector<float>& desired);

    /**
    @brief Training with genetic algorithim
    @param output The feed forward output values
    @param desired The desired output values
    */
    void genetic_training(std::vector<float>& output, std::vector<float>& desired);

private:

    /**
    @brief Layer initialization
    @param index The index of the layer to initialize
    @param neurons_count The amount of neurons on this layer
    */
    void initialize_layer(uint32_t index, uint32_t neurons_count, uint32_t previous_layer_neurons_count)
    {
        layers[index] = index > 0 ? 
                                    new Layer (neurons_count, layers[index-1]->get_neurons_size()) :
                                    new Layer (neurons_count, 0);
    }

    /**
    @brief Calculate the activation value 
    @param value The value to use in the calculation
    @param activation The activation function type to apply
    @return The activated value
    */
    float activate(float value, NNActivations::activations activation)
    {
        switch (activation)
        {
            case NNActivations::SIGMOID: return NNActivations::get().sigmoid(value);
            case NNActivations::TANH: return NNActivations::get().tanh(value);
            case NNActivations::RELU: return NNActivations::get().relu(value);
            case NNActivations::LEAKYRELU: return NNActivations::get().leakyrelu(value);
            default: return 0.0f;
        }
    }

};

