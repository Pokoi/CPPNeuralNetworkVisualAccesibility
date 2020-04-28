#pragma once

#include <QtCore/QCoreApplication>

class NeuralNetworkApplication : public QCoreApplication
{

public:

    NeuralNetworkApplication(int& argc, char** argv) : QCoreApplication(argc, argv)
    {}


};
