#include <iostream>
#include <stdexcept>
#include <vector>
#include <ctime>

#include "Matrix/Container.h"
#include "Matrix/Errors.h"

int main () try
{
    size_t dim = 200;
    double range = 1;
    std::vector<double> vec(dim * dim);
    std::srand(std::time(nullptr));
    for (auto& item : vec)
        item = range - 2 * range * std::rand() / RAND_MAX;
    Matrix::Matrix<double> m (dim, dim, vec.begin());

    /*
    Matrix::Matrix<float> m = {{1, 2, 3, 1},
                             {4, 8, 6, 2},
                             {7, 8, 9, 3},
                             {2, 3, 4, 5}};
                             */


    //m.Print();
    
    std::cout << m.det() << std::endl;

    clock_t beg_t = std::clock();
    for (size_t i = 0; i < 100; ++ i)
        m.det();
    clock_t end_t = std::clock();
    std::cout << 1000 * (end_t - beg_t) / CLOCKS_PER_SEC << " ms\n";

    return 0;
}
catch (Matrix::MatrixError& err)
{
    std::cout << "Matrix error: " << err.what() << std::endl;
}
catch (std::exception& err)
{
    std::cout << "Standart error: " << err.what() << std::endl;
}
catch(...)
{
    std::cout << "Unknown error\n";
}
