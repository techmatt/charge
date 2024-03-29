
#ifndef CORE_BASE_GRID2D_H_
#define CORE_BASE_GRID2D_H_

template <class T> class Grid2
{
public:
    //
    // Grid2 iterators
    //

    struct iteratorEntry
    {
        iteratorEntry(size_t _x, size_t _y, T &_value)
            : x(_x), y(_y), value(_value)
        {

        }
        size_t x;
        size_t y;
        T &value;
    };

    struct constIteratorEntry
    {
        constIteratorEntry(size_t _x, size_t _y, const T &_value)
            : x(_x), y(_y), value(_value)
        {

        }
        size_t x;
        size_t y;
        const T &value;
    };


    struct iterator
    {
        iterator(Grid2<T> *_grid)
        {
            x = 0;
            y = 0;
            grid = _grid;
        }
        iterator(const iterator &i)
        {
            x = i.x;
            y = i.x;
            grid = i.grid;
        }
        ~iterator() {}
        iterator& operator=(const iterator &i)
        {
            x = i.x;
            y = i.y;
            grid = i.grid;
            return *this;
        }
        iterator& operator++()
        {
            x++;
            if (x == grid->dimX())
            {
                x = 0;
                y++;
                if (y == grid->dimY())
                {
                    grid = NULL;
                }
            }
            return *this;
        }
        iteratorEntry operator* () const
        {
            return iteratorEntry(x, y, (*grid)(x, y));
        }

        bool operator != (const iterator &i) const
        {
            return i.grid != grid;
        }

        friend void swap(iterator &a, iterator &b);

        size_t x, y;

    private:
        Grid2<T> *grid;
    };

    struct constIterator
    {
        constIterator(const Grid2<T> *_grid)
        {
            x = 0;
            y = 0;
            grid = _grid;
        }
        constIterator(const constIterator &i)
        {
            x = i.x;
            y = i.x;
            grid = i.grid;
        }
        ~constIterator() {}
        constIterator& operator=(const constIterator &i)
        {
            x = i.x;
            y = i.y;
            grid = i.grid;
            return *this;
        }
        constIterator& operator++()
        {
            x++;
            if (x == grid->dimX())
            {
                x = 0;
                y++;
                if (y == grid->dimY())
                {
                    grid = NULL;
                }
            }
            return *this;
        }
        constIteratorEntry operator* () const
        {
            return constIteratorEntry(x, y, (*grid)(x, y));
        }

        bool operator != (const constIterator &i) const
        {
            return i.grid != grid;
        }

        friend void swap(const constIterator &a, const constIterator &b);

        size_t x, y;

    private:
        const Grid2<T> *grid;
    };

    //
    // Grid2 implementation
    //

    Grid2();
    Grid2(size_t dimX, size_t dimY);
    Grid2(size_t dimX, size_t dimY, const T &value);

    Grid2(const Grid2<T> &G);
    Grid2(Grid2<T> &&G);
    Grid2(size_t dimX, size_t dimY, const std::function< T(size_t x, size_t y) > &fillFunction);

    ~Grid2();

    //
    // Memory
    //
    void free();
    Grid2<T>& operator = (const Grid2<T> &G);
    Grid2<T>& operator = (Grid2<T> &&G);

    void allocate(size_t dimX, size_t dimY);
    void allocate(size_t dimX, size_t dimY, const T &value);

    inline Grid2<T>& operator += (const Grid2<T> &value)
    {
        MLIB_ASSERT_STR(_dimX == value._dimX && _dimY == value._dimY, "grid dimensions must be equal");
        for (size_t y = 0; y < _dimY; y++)
            for (size_t x = 0; x < _dimX; x++)
                _data[y * _dimX + x] += value(x, y);
        return *this;
    }

    inline Grid2<T>& operator *= (T value)
    {
        for (size_t y = 0; y < _dimY; y++)
            for (size_t x = 0; x < _dimX; x++)
                _data[y * _dimX + x] *= value;
        return *this;
    }

    inline Grid2<T> operator * (T value)
    {
        Grid2<T> result(_dimX, _dimY);
        for (size_t y = 0; y < _dimY; y++)
            for (size_t x = 0; x < _dimX; x++)
                result(x, y) = _data[y * _dimX + x] * value;
        return result;
    }

    //
    // Accessors
    //
    inline T& operator() (size_t x, size_t y)
    {
#if defined(MLIB_BOUNDS_CHECK) || defined(_DEBUG)
        MLIB_ASSERT_STR((x < _dimX) && (y < _dimY), "Out-of-bounds grid access");
#endif
        return _data[y * _dimX + x];
    }
    inline const T& operator() (size_t x, size_t y) const
    {
#if defined(MLIB_BOUNDS_CHECK) || defined(_DEBUG)
        MLIB_ASSERT_STR((x < _dimX) && (y < _dimY), "Out-of-bounds grid access");
#endif
        return _data[y * _dimX + x];
    }
    inline T& operator() (const vec2i &v)
    {
        return (*this)(v.x, v.y);
    }
    inline const T& operator() (const vec2i &v) const
    {
        return (*this)(v.x, v.y);
    }

    inline size_t dimX() const
    {
        return _dimX;
    }

    inline size_t dimY() const
    {
        return _dimY;
    }

    inline vec2i dims() const
    {
        return vec2i(_dimX, _dimY);
    }

    inline size_t width() const
    {
        return _dimX;
    }

    inline size_t height() const
    {
        return _dimY;
    }

    inline size_t getNumElements() const
    {
        return _dimX * _dimY;
    }

    inline bool isSquare() const
    {
        return (_dimX == _dimY);
    }

    inline T* data()
    {
        return _data;
    }

    inline const T* data() const
    {
        return _data;
    }

    //
    // Query
    //
    inline bool coordValid(int x, int y) const
    {
        return (x >= 0 && x < int(_dimX) && y >= 0 && y < int(_dimY));
    }

    inline bool coordValid(vec2i coord) const
    {
        return coordValid(coord.x, coord.y);
    }

    void setRow(size_t row, const std::vector<T> &values)
    {
        for (size_t col = 0; col < _dimY; col++) _data[row * _dimX + col] = values[col];
    }

    void setCol(size_t col, const std::vector<T> &values)
    {
        for (size_t row = 0; row < _dimX; row++) _data[row * _dimX + col] = values[row];
    }

    std::vector<T> getRow(size_t y) const
    {
        std::vector<T> result(_dimX);
        const T *CPtr = _data;
        for (size_t x = 0; x < _dimX; x++)
        {
            result[x] = CPtr[y * _dimX + x];
        }
        return result;
    }

    std::vector<T> getCol(size_t x) const
    {
        std::vector<T> result(_dimY);
        const T *CPtr = _data;
        for (size_t y = 0; y < _dimY; y++)
        {
            result[y] = CPtr[y * _dimX + x];
        }
        return result;
    }

    //
    // Modifiers
    //
    void setValues(const T &value);

    void fill(const std::function< T(size_t x, size_t y) > &fillFunction);

    std::vector<T> toStdVector() const
    {
        std::vector<T> result;
        for (size_t i = 0; i < _dimX * _dimY; i++)
            result.push_back(i);
        return result;
    }

    iterator begin()
    {
        return iterator(this);
    }

    iterator end()
    {
        return iterator(NULL);
    }

    constIterator begin() const
    {
        return constIterator(this);
    }

    constIterator end() const
    {
        return constIterator(NULL);
    }

protected:
    T *_data;
    size_t _dimX, _dimY;
};

template <class T> inline bool operator == (const Grid2<T> &a, const Grid2<T> &b)
{
    if (a.getDimX() != b.getDimX() || a.getDimY() != b.getDimY()) return false;
    for (size_t y = 0; y < a.getDimY(); y++)
        for (size_t x = 0; x < a.getDimX(); x++)
            if (a(x, y) != b(x, y))
                return false;
    return true;
}

template <class T> inline bool operator != (const Grid2<T> &a, const Grid2<T> &b)
{
    return !(a == b);
}

typedef Grid2<float> Grid2f;
typedef Grid2<double> Grid2d;

typedef Grid2<RGBColor> Bitmap;

#include "grid2.inl"

#endif  // CORE_BASE_GRID2D_H_
