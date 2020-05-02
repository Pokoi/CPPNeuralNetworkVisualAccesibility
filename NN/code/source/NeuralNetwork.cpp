#include <NeuralNetwork.hpp>

/**
@brief Calculates the value of each neuron by the feed_forward process. The output is stored in the given parameter.
@param inputs A pointer to the first input
@param output A pointer to the first output
*/
void NeuralNetwork::feed_forward(float* inputs, float* output)
{
    // Set the input
    Neuron* start = layers[0]->get_neurons()[0];
    Neuron* end = start + layers[0]->get_neurons_size();

    while (start < end)
    {
        start->set_value(*inputs);
        ++inputs;
        ++start;
    }

    // Calculate the hidden layers
    // In the proposed method there is only 1 hidden layer. 
    // If there will be more than 1, this must be into a for loop from index 1 to index layers_count - 1

    // The proposed method structure is as follows:
    /*

    ___________________   ______________            _______________           ______________        _________________________
    |     INPUT IMAGE   | | INPUT LAYER  |          |  HIDDEN LAYER |         | OUTPUT LAYER |      |     OUPUT IMAGE         |
    |___________________| |______________|          |_______________|         |______________|      |_________________________|

    R  --->  Input 1  --- weight 1.A               weight A.1 --- Output 1  ------->  R'
    /                                  \           /                                       \
    Pixel 1 ------- G  --->  Input 2  --- weight 2.A  ---- A ----  weight A.2 --- Output 2  ------->  G' ----- Pixel 1'
    \                                  /           \                                       /
    B  --->  Input 3  --- weight 3.A               weight A.3 --- Output 3  ------->  B'

    R  --->  Input 4  --- weight 4.B               weight B.4 --- Output 4  ------->  R'
    /                                  \           /                                       \
    Pixel 2 ------- G  --->  Input 5  --- weight 5.B  ---- B ----  weight B.5 --- Output 5  ------->  G' ---- Pixel 2'
    \                                  /           \                                       /
    B  --->  Input 6  --- weight 6.B               weight B.6 --- Output 6  ------->  B'

    ...                      ...                         ...                       ...                         ...

    */
    start = layers[1]->get_neurons()[0];
    end = start + layers[1]->get_neurons_size();

    Neuron* previous_layer_start = layers[0]->get_neurons()[0];

    while (start < end)
    {
        float value = previous_layer_start->get_value() * start->get_weights()[0];
        value += (previous_layer_start + 1)->get_value() * start->get_weights()[1];
        value += (previous_layer_start + 2)->get_value() * start->get_weights()[2];

        start->set_value(activate(value, layers[1]->get_activation()));

        ++start;
        previous_layer_start += 3;
    }


    // Calculate the output layer
    start = layers[2]->get_neurons()[0];
    end = start + layers[2]->get_neurons_size();

    Neuron* previous_layer_start = layers[1]->get_neurons()[0];

    while (start < end)
    {
        // This process can be in a loop (each last hidden layer neuron generates 3 output neurons)
        // Here the loop is omitted to prevent the conditional process

        float value = previous_layer_start->get_value() * start->get_weights()[0];
        value = activate(value, layers[2]->get_activation());
        start->set_value(value);
        *output = value;
        ++start;
        ++output;

        value = previous_layer_start->get_value() * start->get_weights()[1];
        value = activate(value, layers[2]->get_activation());
        start->set_value(value);
        *output = value;
        ++start;
        ++output;

        value = previous_layer_start->get_value() * start->get_weights()[2];
        value = activate(value, layers[2]->get_activation());
        start->set_value(value);
        *output = value;
        ++start;
        ++output;

        ++previous_layer_start;
    }

}
