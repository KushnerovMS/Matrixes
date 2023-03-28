#pragma once

#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

#include "../Interface.h"
#include "../Errors.h"

namespace Matrix {

template<typename T>
class OrdinaryMatrix : public IMatrix<T>
{
    struct Data
    {
        T** data = nullptr;
        size_t width = 0, height = 0;

        Data() : width(0), height(0), data(nullptr) {}
        Data(size_t w, size_t h) : width(w), height(h)
        {
            data = new T* [height];
            for (size_t i = 0; i < height; i ++)
                data[i] = new T [width];
        }

        Data(const Data& other) : width(other.width), height(other.height)
        {
            data = new T*[height];
            for (size_t i = 0; i < height; ++ i)
            {
                data[i] = new T[width];
                std::copy(other.data[i], other.data[i] + width, data[i]);
            }
        }
        Data(Data&& other) noexcept: width(other.width), height(other.height), data(other.data)
        {
            other.data = nullptr;
            other.width = 0;
            other.height = 0;
        }

        Data& operator=(const Data& rhs)
        {
            if (&rhs == this)
                return *this;

            if (rhs.width == width)
            {
                if (rhs.height == height)
                    ;
                else if (height < rhs.height)
                {
                    T** newData = new T*[rhs.height];
                    std::copy(data, data + height, newData);
                    for (size_t i = height; i < rhs.height; ++ i)
                        newData[i] = new T[width];

                    delete[] data;
                    data = newData;

                    height = rhs.height;
                }
                else if (height > rhs.height)
                {
                    T** newData = new T*[rhs.height];
                    std::copy(data, data + rhs.height, newData);

                    for (size_t i = rhs.height; i < height; ++ i)
                        delete[] data[i];
                    delete[] data;
                    data = newData;

                    height = rhs.height;
                }
            }
            else
            {
                for (size_t i = 0; i < height; ++ i)
                    delete[] data[i];
                delete[] data;

                width = rhs.width;
                height = rhs.height;

                data = new T*[height];
                for (size_t i = 0; i < height; ++ i)
                    data[i] = new T[width];
            }

            for (size_t i = 0; i < height; i ++)
                std::copy (rhs.data[i], rhs.data[i] + width, data[i]);
            return *this;
        }
        Data& operator=(Data& rhs) noexcept
        {
            std::swap(data, rhs.data);
            std::swap(width, rhs.width);
            std::swap(height, rhs.height);
        }

        ~Data()
        {
            for (size_t i = 0; i < height; i ++)
                delete[] data[i];
            delete[] data;
        }

        inline T*& operator[](size_t i) { return data[i]; }
    } data_;

    public:
    OrdinaryMatrix() = delete;
    OrdinaryMatrix(size_t cols, size_t rows, const T& val = T(0))
    {
        if (cols == 0) throw Matrix::MatrixError("Matrix with 0 columns can not be created");
        if (rows == 0) throw Matrix::MatrixError("Matrix with 0 rows can not be created");

        data_ = Data(cols, rows);

        for (size_t row = 0; row < data_.height; ++ row)
            std::fill(data_[row], data_[row] + cols, val);
    }
    template <typename Iterator>
    OrdinaryMatrix (size_t cols, size_t rows, Iterator start)
    {
        if (cols == 0) throw Matrix::MatrixError("Matrix with 0 columns can not be created");
        if (rows == 0) throw Matrix::MatrixError("Matrix with 0 rows can not be created");

        data_ = Data(cols, rows);

        for (size_t row = 0; row < data_.height; ++ row)
            for (size_t col = 0; col < data_.width; ++ col, ++ start)
                data_.data[row][col] = *start;
    }

    std::unique_ptr<IMatrix<T>> Clone() const
    { return std::unique_ptr<IMatrix<T>>(new OrdinaryMatrix<T>(*this)); }

    Type getType() const noexcept { return Type::ORDINATY_MATRIX; }

    size_t getWidth()  const noexcept { return data_.width; }
    size_t getHeight() const noexcept { return data_.height; }

          T& Access(size_t row, size_t col)
    {
        if (row >= data_.height) throw Matrix::MatrixError("Row " + std::to_string(row) + " in matrix with height " + std::to_string(data_.height) + " can not be accessed");
        if (col >= data_.height) throw Matrix::MatrixError("Column " + std::to_string(col) + " in matrix with width " + std::to_string(data_.width) + " can not be accessed");
        return data_.data[row][col];
    }
    const T& Access(size_t row, size_t col) const
    {
        if (row >= data_.height) throw Matrix::MatrixError("Row " + std::to_string(row) + " in matrix with height " + std::to_string(data_.height) + " can not be accessed");
        if (col >= data_.height) throw Matrix::MatrixError("Column " + std::to_string(col) + " in matrix with width " + std::to_string(data_.width) + " can not be accessed");
        return data_.data[row][col];
    }

    T det() // Bareiss algorithm
    {
        if (data_.width != data_.height) throw Matrix::MatrixError("Can not find the discriminant of non-square matrix");

        Data copy = data_;
        size_t dim = data_.width;

        int sign = 1;

        const T nullItem = T(0);

        T ak_1k_1 = T(1);
        for (size_t k = 0; k < dim - 1; ++ k)
        {
            if (copy[k][k] == nullItem)
            {
                for (size_t i = k + 1; i < dim; ++ i)
                    if (copy[i][k] != nullItem)
                    {
                        std::swap(copy[k], copy[i]);
                        sign *= -1;
                        break;
                    }
                if (copy[k][k] == nullItem)
                    return nullItem;
            }

            T* rowk = copy[k];
            T akk = rowk[k];
            for (size_t i = k + 1; i < dim; ++ i)
            {
                T* rowi = copy[i];
                T aik = rowi[k];
                for (size_t j = k + 1; j < dim; ++ j)
                    rowi[j] = (rowi[j] * akk - rowk[j] * aik) / ak_1k_1;
            }

            ak_1k_1 = copy[k][k];
        }
        return sign * copy[dim - 1][dim - 1];
    }
};

}
