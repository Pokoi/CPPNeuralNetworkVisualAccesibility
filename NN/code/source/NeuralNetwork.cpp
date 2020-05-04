#include <NeuralNetwork.hpp>


/**
@brief Creates a neural network with the data of a binary file
@param path The path of the file with the data
*/
NeuralNetwork::NeuralNetwork(std::string path)
{  
    std::ifstream stream;

    stream.open("../../assets/data/data.dat");
    std::string content;
    std::getline(stream, content); 
    stream.close();
      
    BinaryData* data = new BinaryData();
    data->read(content);

    this->layers = new Layer * [3];

    // The proposed method only has 3 layers connected in a particular way
    // The first or input layer
    initialize_layer(0, data->first_layer_neurons, 0);
    // The second or hidden layer
    initialize_layer(1, data->first_layer_neurons / 3, data->first_layer_neurons);
    // The third or output layer
    initialize_layer(2, data->first_layer_neurons, data->first_layer_neurons / 3);

    // Set the wa, wb, wc 
    Neuron** start = layers[1]->get_neurons();
    Neuron** end = start + (data->first_layer_neurons / 3);

    while (start < end)
    {
        (*start)->get_weights()[0] = data->wa;
        (*start)->get_weights()[1] = data->wb;
        (*start)->get_weights()[2] = data->wc;

        ++start;
    }

    // Set the wd, we, wf
    start = layers[2]->get_neurons();
    end = start + (data->first_layer_neurons);

    while (start < end)
    {
        (*start)->get_weights()[0] = data->wd;
        (*(start + 1))->get_weights()[0] = data->we;
        (*(start + 2))->get_weights()[0] = data->wf;

        start += 3;
    }


    delete data;
}

/**
@brief Export the neural network data to a binary file
@param path The path of the file where the data will be exported
*/
void NeuralNetwork::export_network(std::string path)
{
    BinaryData* data = new BinaryData();
    data->wa = layers[1]->get_neurons()[0]->get_weights()[0];
    data->wb = layers[1]->get_neurons()[0]->get_weights()[1];
    data->wc = layers[1]->get_neurons()[0]->get_weights()[2];
    data->wd = layers[2]->get_neurons()[0]->get_weights()[0];
    data->we = layers[2]->get_neurons()[1]->get_weights()[0];
    data->wf = layers[2]->get_neurons()[2]->get_weights()[0];

    std::ofstream stream("../../assets/data/data.dat");
    
    data->first_layer_neurons = layers[0]->get_neurons_size();
    std::cout<< data->to_string();

    stream << data->to_string();

    stream.close();      

    delete data;
}

/**
@brief Calculates the value of each neuron by the feed_forward process. The output is stored in the given parameter.
@param inputs A pointer to the first input
@param output A pointer to the first output
*/
void NeuralNetwork::feed_forward(float* inputs, float* output)
{
    // Set the input
    Neuron** start = layers[0]->get_neurons();
    Neuron** end = start + layers[0]->get_neurons_size();

    while (start < end)
    {
        (*start)->set_value(*inputs);
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
                   /                                  \          /                                       \
    Pixel 1 ------- G  --->  Input 2  --- weight 2.A  ---- A ----  weight A.2 --- Output 2  ------->  G' ----- Pixel 1'
                   \                                  /          \                                       /
                    B  --->  Input 3  --- weight 3.A               weight A.3 --- Output 3  ------->  B'

                    R  --->  Input 4  --- weight 4.B               weight B.4 --- Output 4  ------->  R'
                   /                                  \          /                                       \
    Pixel 2 ------- G  --->  Input 5  --- weight 5.B  ---- B ----  weight B.5 --- Output 5  ------->  G' ---- Pixel 2'
                   \                                  /          \                                       /
                    B  --->  Input 6  --- weight 6.B               weight B.6 --- Output 6  ------->  B'

    ...                      ...                         ...                       ...                         ...

    */
    start = layers[1]->get_neurons();
    end = start + layers[1]->get_neurons_size();

    Neuron* previous_layer_start = layers[0]->get_neurons()[0];
    Neuron* previous_layer_end = previous_layer_start + layers[0]->get_neurons_size();

    int count = 0;
    
    while (previous_layer_start < previous_layer_end)
    {
        float value = previous_layer_start->get_value()  * (*start)->get_weights()[0];
        value += (previous_layer_start + 1)->get_value() * (*start)->get_weights()[1];
        value += (previous_layer_start + 2)->get_value() * (*start)->get_weights()[2];

        (*start)->set_value(activate(value, layers[1]->get_activation()));

        ++start;
        previous_layer_start += 3;
        count++;

    }


    // Calculate the output layer
    start = layers[2]->get_neurons();
    end = start + layers[2]->get_neurons_size();

    previous_layer_start = layers[1]->get_neurons()[0];

    while (start < end)
    {
        // This process can be in a loop (each last hidden layer neuron generates 3 output neurons)
        // Here the loop is omitted to prevent the conditional process

        float value = previous_layer_start->get_value() * (*start)->get_weights()[0];
        value = activate(value, layers[2]->get_activation());
        (*start)->set_value(value);
        *output = value;
        ++start;
        ++output;

        value = previous_layer_start->get_value() * (*start)->get_weights()[1];
        value = activate(value, layers[2]->get_activation());
        (*start)->set_value(value);
        *output = value;
        ++start;
        ++output;

        value = previous_layer_start->get_value() * (*start)->get_weights()[2];
        value = activate(value, layers[2]->get_activation());
        (*start)->set_value(value);
        *output = value;
        ++start;
        ++output;

        ++previous_layer_start;
    }

}


/**
@brief Calculates the backpropagation
@param output The output values of the information proccess of the feed_forward output
@param desired The desired values
*/
void NeuralNetwork::back_propagation(float* output, float* desired)
{
    error = 0.f;
    float wa = 0.f, wb = 0.f, wc = 0.f, wd = 0.f, we = 0.f, wf = 0.f;

    Neuron** start = layers[layers_count - 1]->get_neurons();
    Neuron** end = start + layers[layers_count - 1]->get_neurons_size();
    float value = 0.f;

    // Output adjustment
    // In the proposed method, all the triplets of weights between output layer and last hidden layer must
    // be adjusted like there are the same weights. For this reason, the adjustment has to be acumulated and 
    // applied after an average calculation. 
    // The nomenclature follows this structure:

    /*

     ___________________   ______________   _______________   ______________   _________________________
    |     INPUT IMAGE   | | INPUT LAYER  | |  HIDDEN LAYER | | OUTPUT LAYER | |     OUPUT IMAGE         |
    |___________________| |______________| |_______________| |______________| |_________________________|

                    R  --->  Input 1  --- WA               WD --- Output 1  ------->  R'
                   /                         \          /                               \
    Pixel 1 ------- G  --->  Input 2  --- WB  ---- A ----  WE --- Output 2  ------->  G' ----- Pixel 1'
                   \                         /          \                               /
                    B  --->  Input 3  --- WC               WF --- Output 3  ------->  B'

                    R  --->  Input 4  --- WA               WD --- Output 4  ------->  R'
                   /                         \          /                               \
    Pixel 2 ------- G  --->  Input 5  --- WB  ---- B ----  WE --- Output 5  ------->  G' ---- Pixel 2'
                   \                         /          \                               /
                    B  --->  Input 6  --- WC               WF --- Output 6  ------->  B'

    ...                      ...                  ...              ...                         ...


    */

    while (start < end)
    {
        value = (*output) - (*desired);

        (*start)->set_delta(value);
        (*(start + 1))->set_delta(value);
        (*(start + 2))->set_delta(value);

        wd += (*start)->get_weights()[0] - (value * learning_rate * (*start)->get_value());
        we += (*start + 1)->get_weights()[0] - (value * learning_rate * (*start + 1)->get_value());
        wf += (*start + 2)->get_weights()[0] - (value * learning_rate * (*start + 2)->get_value());

        error += std::abs(value);

        start   += 3;
        output  += 3;
        desired += 3;
    }

    float divider = 1.f / float(layers[layers_count - 1]->get_neurons_size());

    start = layers[layers_count - 1]->get_neurons();
    wd *= divider;
    we *= divider;
    wf *= divider;

    while (start < end)
    {
        (*start)->get_weights()[0] = wd;
        (*(start + 1))->get_weights()[0] = we;
        (*(start + 2))->get_weights()[0] = wf;

        start += 3;
    }

    // Error calculation
    error = (error * 3.f) / layers[layers_count - 1]->get_neurons_size();

    // Hidden layers
    // In the proposed method there is only one hidden layer. If this is not your case, you have
    // to add this into a for loop

    start = layers[1]->get_neurons();
    end = start + layers[1]->get_neurons_size();
    Neuron** next_layer_start = layers[2]->get_neurons();

    while (start < end)
    {
        value = (*next_layer_start)->get_delta() * wd;
        value += (*(next_layer_start + 1))->get_delta() * we;
        value += (*(next_layer_start + 2))->get_delta() * wf;

        value = (*start)->get_delta() - value;

        (*start)->set_delta(value);

        wa += (*start)->get_weights()[0] - (value * learning_rate * (*start)->get_value());
        wb += (*start)->get_weights()[1] - (value * learning_rate * (*start + 1)->get_value());
        wc += (*start)->get_weights()[2] - (value * learning_rate * (*start + 2)->get_value());

        ++start;
    }

    divider = 1.f / float(layers[1]->get_neurons_size());

    start = layers[1]->get_neurons();
    wa *= divider;
    wb *= divider;
    wc *= divider;

    while (start < end)
    {
        (*start)->get_weights()[0] = wa;
        (*start)->get_weights()[1] = wb;
        (*start)->get_weights()[2] = wc;

        ++start;
    }

}