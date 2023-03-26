#pragma once

#include <stdexcept>
#include <string>

namespace Matrix {

struct MatrixError : public std::logic_error
{
    MatrixError (const std::string& what) : logic_error(what) {}
    MatrixError (const char* what) : logic_error(what) {}
};

} // namespce Matrix
