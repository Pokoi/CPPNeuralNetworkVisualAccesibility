#include <QtCore/QCoreApplication>
#include <ctime>
#include <random>

int main(int argc, char *argv[])
{
    srand(time(NULL));
    QCoreApplication a(argc, argv);

    return a.exec();
}
