#pragma once

#include <math.h>

/**
 * @brief Class that manages the neuronal network activation functions and it's derivates
*/
class NNActivations
{

public:

    /** 
     * @brief The different activation function types 
    */
    enum activations {NONE, SIGMOID, TANH, RELU, LEAKYRELU};  

private:

    NNActivations() {}

public:

    /** 
     * @brief Gets a global accesible instance of this class. 
    */
    NNActivations & get()
    {
        static NNActivations instance  = NNActivations();
        return instance;
    }

    float sigmoid (float x)
    {
        float k = exp(x);
        return k / (1.f + k);
    }

    float tanh (float x)
    {
        return tanhf(x);
    }

    float relu (float x)
    {
        return 0 >= x ? 0.f : x;
    }

    float leakyrelu (float x)
    {
        return 0 >= x ? 0.01f * x : x;
    }

    float sigmoid_derivate (float x)
    {
        return x * (1 - x);
    }

    float tanh_derivate (float x)
    {
        return 1 - x * x;
    }

    float relu_derivate (float x)
    {
        return 0 >= x ? 0.f : 1;
    }

    float leakyrelu_derivate (float x)
    {
        return 0 >= x ? 0.01f : 1;
    }

};


