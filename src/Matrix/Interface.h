#pragma once

#include <memory>

namespace Matrix {

enum Type
{
    UDEFINED = 0,
    ORDINATY_MATRIX,
    SPARCE_MATRIX
};

template<typename T>
class IMatrix 
{
    public:

    virtual ~IMatrix () = default;

    virtual std::unique_ptr<IMatrix<T>> Clone() const = 0;

    virtual Type getType() const { return Type::UDEFINED; }

    virtual size_t getWidth()  const = 0;
    virtual size_t getHeight() const = 0;

    virtual       T& Access(size_t row, size_t col)       = 0;
    virtual const T& Access(size_t row, size_t col) const = 0;

    virtual T det() = 0;
};

} // namespace Matrix
