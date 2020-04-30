#pragma once

#include <cstdint>
#include <random>

/** 
 * @brief Class that manages the data of a neuron 
*/
class Neuron
{
private:

    struct 
    {
        float * values;
        uint8_t size;
    } weights;

    float value;
    float delta;

public: 

    /**
     * @brief Creates a neuron with the given amount of weights
     * @param weights_size The amount of weights (correspond to the amount of neurons in the previous layer)
     */
    Neuron(uint8_t weigths_size) 
    {
        weights.size    = weigths_size;
        weights.values  = new float [weigths_size];

        float * start   = weights.values;
        float * end     = start + weights.size;

        while (start < end)
        {
            (*start) = (float (rand()) / RAND_MAX) * (5.f - (-5.f)) + (-5.f);
            ++start;
        }
    }

    /**
     * @brief Frees the memory
    */
    ~Neuron()
    {
        delete [] weights.values;
    }

    /**
     * @brief Gets the delta value 
     * @return The delta value
    */
    float get_delta () const 
    { 
        return delta;
    }
    
    /**
     * @brief Gets the value of the neuron
     * @return The value of the neuron
    */
    float get_value () const 
    { 
        return value;
    }

    /**
     * @brief Gets a pointer to the weights values
     * @return The pointer to the weights values
    */
    float * get_weights() 
    { 
        return weights.values; 
    }

    /**
     * @brief Gets the amount of weights of this neuron
     * @return The amount of weights of this neuron
    */
    uint8_t get_weights_size() 
    {
        return weights.size;
    }

    /** 
     * @brief Set the delta of this neuron
     * @param delta The delta to set
    */
    void set_delta (float delta) 
    {
        this->delta = delta;
    }
    
    /**
     * @brief Set the value of this neuron
     * @param value The value of this neuron
    */
    void set_value (float value)
    {
        this->value = value;
    }

};