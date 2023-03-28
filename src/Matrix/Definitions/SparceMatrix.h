#pragma once

#include <algorithm>
#include <type_traits>
#include <stack>
#include <vector>

#include "../Interface.h"

namespace Matrix {

template<typename T>
class SparceMatrix : public IMatrix<T>
{
    static constexpr T NullValue = T(0);
    struct Data
    {
        struct Row
        {
            size_t size = 0;
            struct Item
            {
                T val;
                size_t col;
            }* items = nullptr;
        }* rows = nullptr;

        using Item = Row::Item;

        size_t width = 0, height = 0;

        Data() : width(0), height(0), rows(0) {}
        Data(size_t w, size_t h) : width(w), height(h), rows(new Row[h]) {}

        Data(const Data& other) : width(other.width), height(other.height), rows(new Row[other.height])
        {
            for (size_t i = 0; i < height; ++ i)
            {
                Row& thisR = rows[i];
                Row& otherR = other.rows[i];

                thisR.size = otherR.size;
                thisR.items = new Row::Item[thisR.size];
                std::copy(otherR.items, otherR.items + otherR.size, thisR.items);
            }
        }
        Data(Data&& other) noexcept : width(other.width), height(other.height), rows(other.rows)
        {
            other.width = 0;
            other.height = 0;
            other.rows = nullptr;
        }

        Data& operator=(const Data& rhs)
        {
            if (&rhs = this)
                return &this;

            if (height < rhs.height)
            {
                Row* newRows = new Row[rhs.height];
                std::copy(rows, rows + height, newRows);

                delete[] rows;
                rows = newRows;
            }
            if (height > rhs.height)
            {
                Row* newRows = new Row[height];
                std::copy(rows, rows + rhs.height, newRows);

                for (size_t i = rhs.height; i < height; ++ i)
                    delete[] rows[i].items;
                delete[] rows;
                rows = newRows;
            }
            height = rhs.height;

            for (size_t i = 0; i < height; ++ i)
            {
                Row& thisR = rows[i];
                Row& otherR = rhs.rows[i];

                if (thisR.size != otherR.size)
                {
                    delete[] thisR.items;
                    thisR.items = new Row::Item[otherR.size];
                    thisR.size = otherR.size;
                }
                
                std::copy(otherR.items, otherR.items + otherR.size, thisR.items);
            }

            return *this;
        }
        Data& operator=(Data&& rhs)
        {
            std::swap(rows, rhs.rows);
            std::swap(width, rhs.width);
            std::swap(height, rhs.height);
            return *this;
        }

        ~Data()
        {
            for (size_t i = 0; i < height; ++ i)
                delete[] rows[i].items;
            delete[] rows;
        }

        void ResizeRow(size_t row, size_t newSize)
        {
            size_t& size = rows[row].size;
            Item*& curRow = rows[row].items;

            Item* newItems = new Item[newSize];

            std::move(curRow, curRow + std::min(size, newSize), newItems);
            size = newSize;

            delete[] curRow;
            curRow = newItems;
        }
        void Insert(Row::Item&& item, size_t row, size_t col)
        {
            auto& curRow = rows[row];

            Item* newItems = new Item[curRow.size + 1];

            size_t i = 0;
            for (; i < curRow.size && curRow.items[i].col < col; ++ i)
                newItems[i] = std::move(curRow.items[i]);
            newItems[i ++] = item;
            curRow.size ++;
            for (; i < curRow.size; ++ i)
                newItems[i] = std::move(curRow.items[i - 1]);

            delete[] curRow.items;
            curRow.items = newItems;
        }
        void Delete(size_t row, size_t col)
        {
            size_t& size = rows[row].size;
            if (size == 0) return;
            Item*& curRow = rows[row].items;

            Item* newItems = new Item[size - 1];

            for (size_t i = 0, j = 0; j < size; ++ i, ++ j)
            {
                if (curRow[j].col == col)
                    ++ j;

                newItems[i] = std::move(curRow[j]);
            }

            size --;
            delete[] curRow;
            curRow = newItems;
        }
    } data_;

    using DataItem = Data::Row::Item;

    struct Element
    {
        size_t row = 0;
        size_t col = 0;
        DataItem* itemp = nullptr;
    } accessElem_;

    DataItem bufItem_;

    void update()
    {
        if (accessElem_.itemp != &bufItem_ && accessElem_.itemp -> val == NullValue)
            data_.Delete(accessElem_.row, accessElem_.col);
        else if (accessElem_.itemp == &bufItem_ && accessElem_.itemp -> val != NullValue)
            data_.Insert(std::move(bufItem_), accessElem_.row, accessElem_.col);
        accessElem_.itemp = nullptr;
    }

    public:
    SparceMatrix() = delete;
    SparceMatrix(size_t cols, size_t rows)
    {
        if (cols == 0) throw Matrix::MatrixError("Matrix with 0 columns can not be created");
        if (rows == 0) throw Matrix::MatrixError("Matrix with 0 rows can not be created");

        data_ = Data(cols, rows);
    }
    template <typename Iterator>
    SparceMatrix (size_t cols, size_t rows, Iterator start)
    {
        if (cols == 0) throw Matrix::MatrixError("Matrix with 0 columns can not be created");
        if (rows == 0) throw Matrix::MatrixError("Matrix with 0 rows can not be created");

        data_ = Data(cols, rows);

        std::vector<DataItem> itemBuf(cols);
        for (size_t row = 0; row < rows; ++ row)
        {
            size_t rowSize = 0;
            for (size_t col = 0; col < cols; ++ col, ++ start)
                if (*start != NullValue)
                {
                    itemBuf[rowSize].col = col;
                    itemBuf[rowSize].val = *start;
                    rowSize ++;
                }

            if (rowSize > 0)
            {
                data_.ResizeRow(row, rowSize);
                std::move(itemBuf.begin(), itemBuf.begin() + rowSize, data_.rows[row].items);
            }
        }
    }

    std::unique_ptr<IMatrix<T>> Clone() const
    { return std::unique_ptr<IMatrix<T>>(new SparceMatrix<T>(*this)); }

    Type getType() const noexcept { return Type::SPARCE_MATRIX; }

    size_t getWidth()  const noexcept { return data_.width; }
    size_t getHeight() const noexcept { return data_.height; }

          T& Access(size_t row, size_t col)
    {
        if (accessElem_.itemp)
        { 
           // std::cout << accessElem_.itemp << ": {" << accessElem_.itemp -> val << ", (" << accessElem_.row << ", " << accessElem_.itemp -> col << ")}\n";
            update(); 
        }

        if (row >= data_.height) throw Matrix::MatrixError("Row " + std::to_string(row) + " in matrix with height " + std::to_string(data_.height) + " can not be accessed");
        if (col >= data_.height) throw Matrix::MatrixError("Column " + std::to_string(col) + " in matrix with width " + std::to_string(data_.width) + " can not be accessed");
        
        accessElem_.row = row;
        accessElem_.col = col;

        DataItem* ptr = nullptr;
        auto& curRow = data_.rows[row];
        if (curRow.items)
            ptr = std::lower_bound(curRow.items, curRow.items + curRow.size, col, [](const DataItem& item, size_t val) { return item.col < val; });

        if (ptr && ptr != curRow.items + curRow.size && ptr -> col == col)
            accessElem_.itemp = ptr;
        else
        {
            bufItem_.col = col;
            bufItem_.val = NullValue;
            accessElem_.itemp = &bufItem_;
        }
        return accessElem_.itemp -> val;
    }
    const T& Access(size_t row, size_t col) const
    {
        if (row >= data_.height) throw Matrix::MatrixError("Row " + std::to_string(row) + " in matrix with height " + std::to_string(data_.height) + " can not be accessed");
        if (col >= data_.height) throw Matrix::MatrixError("Column " + std::to_string(col) + " in matrix with width " + std::to_string(data_.width) + " can not be accessed");

        const DataItem* ptr = nullptr;
        auto& curRow = data_.rows[row];
        if (curRow.items)
            ptr = std::lower_bound(curRow.items, curRow.items + curRow.size, col, [](const DataItem& item, size_t elem) { return elem >= item.col; });

        if (ptr && ptr != curRow.items + curRow.size && ptr -> col == col)
            return ptr -> val;
        else
            return (NullValue);
    }

    T det()
    {
        if (accessElem_.itemp) update();

        if (data_.width != data_.height) throw Matrix::MatrixError("Can not find the discriminant of non-square matrix");

        size_t dim = data_.width;

        T det = NullValue;

        for (size_t row = 0; row < dim; ++ row)
            if (data_.rows[row].size == 0)
                return NullValue;

        std::vector<size_t> usedCols(dim);
        std::fill(usedCols.begin(), usedCols.end(), dim);
        auto usedColsDat = usedCols.data();

        struct StackVal
        {
            T curProd;
            size_t elemNum;
        };
        std::vector<StackVal> stack(dim);
        auto stackDat = stack.data();

        stackDat[0].curProd = T(1);
        usedCols[0] = data_.rows[0].items[0].col;

        for (size_t row = 0; ;)
        {
            auto& curRow = data_.rows[row];
            auto& top = stackDat[row];
            std::cout << "Row " << row << ", top: {" << top.curProd << ", " << top.elemNum << "}\n";
            if (top.elemNum >= curRow.size)
            {
                if (row > 0) -- row;
                else break;
                ++ stackDat[row].elemNum;
            }
            else if (std::find(usedColsDat, usedColsDat + row,
                     curRow.items[top.elemNum].col) != usedColsDat + row)
                ++ top.elemNum;
            else
            {
                T prod = top.curProd * 
                         ((row + curRow.items[top.elemNum].col) % 2 ? -1 : 1) *
                         curRow.items[top.elemNum].val;
                if (row < dim - 1)
                {
                    usedColsDat[row] = curRow.items[top.elemNum].col;
                    ++ row;
                    stackDat[row] = StackVal{std::move(prod), 0};
                }
                else
                {
                    det += prod;
                    ++ top.elemNum;
                }
            }
        }

        return det;

        /*
        struct ColCntr
        {
            size_t col;
            size_t i = 0;

        };

        int sign = 1;
        std::vector<ColCntr> colBuf(data_.height);
        for (size_t i = 0; i < dim; ++ i)
        {
            if (copy.rows[i].items)
                colBuf[i].col = copy.rows[i].items[0].col;
            else
                return NullValue;
        }

        T ak_1k_1 = T(1);
        for (size_t k = 0; k < dim - 1; ++ k)
        {
            std::cout << "copy.rows: " << copy.rows << ": {\n";
            for (size_t i = 0; i < copy.height; ++ i)
            {
                std::cout << "{" << copy.rows[i].size << ", " << copy.rows[i].items;
                if (copy.rows[i].items)
                {
                    std::cout << " : {\n";
                    for (size_t j = 0; j < copy.rows[i].size; ++ j)
                        std::cout << "{" << copy.rows[i].items[j].col << ", " << copy.rows[i].items[j].val << "}, ";
                }
                std::cout << "} {" << colBuf[i].i << ", " << colBuf[i].col << "}\n";
            }
            std::cout << "}\n\n";

            auto& colBufk = colBuf[k];
            auto& rowk = copy.rows[k];

            if (k < colBufk.col || rowk.items[colBufk.i].val == NullValue)
            {
                for (size_t i = k + 1; i < dim; ++ i)
                    if (k == colBuf[i].col && copy.rows[i].items[colBuf[i].i].val != NullValue)
                    {
                        std::swap(rowk, copy.rows[i]);
                        std::swap(colBufk, colBuf[i]);
                        sign *= -1;
                        break;
                    }
                if (k < colBufk.col || rowk.items[colBufk.i].val == NullValue)
                    return NullValue;
            }

            const T& akk = rowk.items[colBufk.i].val;

            if (colBufk.i + 1 < rowk.size)
                colBufk.col = rowk.items[++ colBuf[k].i].col;
            else
                colBufk.col = dim;

            for (size_t i = k + 1; i < dim; ++ i)
            {
                auto& rowi = copy.rows[i];
                auto& colBufi = colBuf[i];

                const T& aik = rowi.items[colBufi.i].val;

                if (colBufi.col <= k)
                {
                    if (colBufi.i + 1 < rowi.size)
                        colBufi.col = rowi.items[++ colBufi.i].col;
                    else
                        colBufi.col = dim;
                }

                if (colBufi.col == dim)
                    continue;

                for (ColCntr colk = colBufk, coli = colBufi; coli.i < rowi.size; ++ coli.i)
                {
                    coli.col = rowi.items[coli.i].col;

                    while (colk.col < coli.col)
                    {
                        if (++ colk.i < rowk.size)
                            colk.col = rowk.items[++ colk.i].col;
                        else
                            colk.col = dim;
                    }

                    if      (colk.col > coli.col)
                        rowi.items[coli.i].val = rowi.items[coli.i].val * akk / ak_1k_1;
                    else
                        rowi.items[coli.i].val = (rowi.items[coli.i].val * akk - rowk.items[colk.i].val * aik) / ak_1k_1;
                }
            }
            ak_1k_1 = akk;
        }

        return sign * copy.rows[dim - 1].items[colBuf[dim - 1].i].val;
        */
    }
};

} // namespace Matrix
