#pragma once

#include <Neuron.hpp>
#include <NNActivations.hpp>

/** 
 * Class that manages the data of a layer
*/
class Layer
{

private:

    NNActivations::activations activation;
    Neuron ** neurons;
    uint neurons_count;

public:

    /** 
     * @brief Creates a layer of the given amount of neurons
     * @param neurons_count The amount of neurons of this layer
     * @param neurons_previous_layer The amount of neurons in the previous layer
     * @param activation The activation function type to apply to this layer
    */
    Layer   (
                uint neurons_count,
                uint neurons_previous_layer,
                NNActivations::activations activation
            ) 
            : 
            neurons_count {neurons_count},
            activation {activation}
    {
        neurons = new Neuron * [neurons_count];

        Neuron ** start = neurons;
        Neuron ** end = start + neurons_count;

        while (start < end)
        {
            (*start) = new Neuron(neurons_previous_layer);
            ++start;
        }
    }

    /** 
     * @brief Gets the activation function type of the layer
     * @return The activation type
    */
    NNActivations::activations get_activation () 
    {
        return activation;
    }

    /** 
     * @brief Gets the neurons of the layer
     * @return The neurons of this layer
    */
    Neuron ** get_neurons()
    {
        return neurons;
    }

    /** 
     * @brief Gets the amount of neurons on this layer 
     * @return The amount of neurons on this layer
    */
    uint get_neurons_size()
    {
        return neurons_count;
    }

    /**
     * @brief Frees the memory
    */
    ~Layer () 
    {
        for (uint i = 0; i < neurons_count; ++i)
        {
            delete neurons[i];
        }

        delete [] neurons;
    }

};