
#include <NeuralNetworkApplication.hpp>
#include <ctime>
#include <random>

int main(int argc, char *argv[])
{
    srand(time(NULL));    

    NeuralNetworkApplication a(argc, argv);
    return a.exec();
}
