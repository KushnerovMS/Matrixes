#pragma once

#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
namespace Matrix {

template<typename T>
class OrdinaryMatrix : public IMatrix<T>
{
    struct Data
    {
        T** data = nullptr;
        size_t width = 0, height = 0;

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
                if (rhs.height = height)
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

        T* operator[](size_t i) { return data[i]; }
    } data_;

    public:
    OrdinaryMatrix() = delete;
    OrdinaryMatrix(size_t cols, size_t rows, const T& val = T(0)) : data_(cols, rows)
    {
        if (cols == 0) throw std::logic_error("Matrix with 0 columns can not be created");
        if (rows == 0) throw std::logic_error("Matrix with 0 rows can not be created");

        for (size_t row = 0; row < data_.height; ++ row)
            std::fill(data_[row], data_[row] + cols, val);
    }
    template <typename Iterator>
    OrdinaryMatrix (size_t cols, size_t rows, Iterator start, Iterator stop) : data_(cols, rows)
    {
        if (cols == 0) throw std::logic_error("Matrix with 0 columns can not be created");
        if (rows == 0) throw std::logic_error("Matrix with 0 rows can not be created");

        for (size_t row = 0; row < data_.height; ++ row)
            for (size_t col = 0; col < data_.width && start != stop; ++ col, ++ start)
                data_.data[row][col] = *start;
    }

    std::unique_ptr<IMatrix<T>> Clone() const
    { return std::unique_ptr<IMatrix<T>>(new OrdinaryMatrix<T>(*this)); }

    size_t getWidth()  const noexcept { return data_.width; }
    size_t getHeight() const noexcept { return data_.height; }

    T& Access(size_t row, size_t col)
    {
        if (row >= data_.height) throw std::out_of_range("Row " + std::to_string(row) + " in matrix with height " + std::to_string(data_.height) + " can not be accessed");
        if (col >= data_.height) throw std::out_of_range("Column " + std::to_string(col) + " in matrix with width " + std::to_string(data_.width) + " can not be accessed");
        return data_.data[row][col];
    }
    const T& Access(size_t row, size_t col) const { return data_.data[row][col]; }
};

}
