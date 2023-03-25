#pragma once

#include "Interface.h"
#include "Definitions/OrdinaryMatrix.h"
#include <string>
#include <ostream>

namespace Matrix {

struct Format
{
    std::string ItemSep = " ";
    std::string RowBeg  = "[";
    std::string RowEnd  = "]";
    std::string RowSep  = "\n";
    std::string MatrixBeg = "";
    std::string MatrixEnd = "\n";
} OutputFormat;

template<typename T>
class Matrix
{
    struct MatrixPtr
    {
        IMatrix<T>* m_ptr = nullptr;

        explicit MatrixPtr(IMatrix<T>* mtrx) : m_ptr(mtrx) {}

        MatrixPtr(const MatrixPtr& other) : m_ptr(ptr.m_ptr -> Clone().release()) {}
        MatrixPtr(MatrixPtr&& other) noexcept : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }

        MatrixPtr& operator=(const MatrixPtr& rhs)
        {
            if (&rhs == this)
                return *this;
            
            m_ptr = rhs.m_ptr -> Clone().release();
            return *this;
        }
        MatrixPtr& operator=(MatrixPtr&& rhs) noexcept
        {
            std::swap(m_ptr, rhs.m_ptr);
            return *this;
        }

        ~MatrixPtr() { delete m_ptr; }
    } ptr;

    Matrix() = default;
     
    public:

    Matrix(IMatrix<T>* mtrx) : ptr(mtrx) {}

    class Row
    {
        IMatrix<T>* m_ptr_ = nullptr;
        size_t row_ = 0;

        public:
        Row(IMatrix<T>* m_ptr, size_t row) : m_ptr_(m_ptr), row_(row) {}

        inline       T& operator[](size_t col)       { return m_ptr_ -> Access(row_, col); }
        inline const T& operator[](size_t col) const { return m_ptr_ -> Access(row_, col); }
    };


    inline size_t getWidth()  const { return ptr.m_ptr -> getWidth(); }
    inline size_t getHeight() const { return ptr.m_ptr -> getHeight(); }

    inline       Row operator[](size_t row)       { return Row(ptr.m_ptr, row); }
    inline const Row operator[](size_t row) const { return Row(ptr.m_ptr, row); }

    inline       T& Access(size_t row, size_t col)       { return ptr.m_ptr -> Access(row, col); }
    inline const T& Access(size_t row, size_t col) const { return ptr.m_ptr -> Access(row, col); }

    void Print (std::ostream& stream = std::cout) const
    {
        size_t width = getWidth();
        size_t height = getHeight();
        stream << OutputFormat.MatrixBeg;

        stream << OutputFormat.RowBeg;
        stream << Access(0, 0);
        for (size_t col = 1; col < width; ++ col)
        {
            stream << OutputFormat.ItemSep;
            stream << Access(0, col);
        }
        stream << OutputFormat.RowEnd;

        for (size_t row = 1; row < height; ++ row)
        {
            stream << OutputFormat.RowSep;
            stream << OutputFormat.RowBeg;
            stream << Access(row, 0);
            for (size_t col = 1; col < width; ++ col)
            {
                stream << OutputFormat.ItemSep;
                stream << Access(row, col);
            }
            stream << OutputFormat.RowEnd;
        }
        stream << OutputFormat.MatrixEnd;
    }


    static Matrix Zeros(size_t width, size_t height = 0)
    {
        if (height == 0) height = width;
        return Matrix<T>(new OrdinaryMatrix<T>(width, height));
    }
    static Matrix Ones(size_t width, size_t height = 0)
    {
        if (height == 0) height = width;
        return Matrix<T>(new OrdinaryMatrix<T>(width, height, T(1)));
    }
    static Matrix Eye(size_t width, size_t height = 0)
    {
        if (height == 0) height = width;
        Matrix<T> res(new OrdinaryMatrix<T>(width, height));
        for (size_t i = 0; i < width && i < height; ++ i)
            res[i][i] = T(1);
        return res;
    }
};

} // namespace Matrix
