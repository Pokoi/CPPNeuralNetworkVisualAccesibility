#pragma once

#include <Layer.hpp>

class NeuralNetwork
{
private:

    Layer** layers;
    uint32_t layers_count;
    float learning_rate = 0.01f;
    float error = 1.f;

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
        initialize_layer(1, first_layer_neurons / 3, first_layer_neurons);
        // The third or output layer
        initialize_layer(2, first_layer_neurons, first_layer_neurons / 3);
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
    @brief Gets the error
    @return The error
    */
    float get_error() 
    { 
        return error;
    }

    /**
    @brief Calculates the value of each neuron by the feed_forward process. The output is stored in the given parameter.
    @param inputs A pointer to the first input
    @param output A pointer to the first output
    */
    void feed_forward(float* inputs, float* output);

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