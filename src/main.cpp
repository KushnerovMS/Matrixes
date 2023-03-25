#include <iostream>
#include <stdexcept>

#include "Matrix/Container.h"

int main () try
{
    Matrix::Matrix m = Matrix::Matrix<int>::Eye(10);

    m.Print();

    std::cout << m[1][10] << std::endl;

    return 0;
}
catch (std::out_of_range& err)
{
    std::cout << "Out of range error: " << err.what() << std::endl;
}
catch (std::logic_error& err)
{
    std::cout << "Logical error: " << err.what() << std::endl;
}
catch (std::exception& err)
{
    std::cout << "Standart error: " << err.what() << std::endl;
}
catch(...)
{
    std::cout << "Unknown error\n";
}
