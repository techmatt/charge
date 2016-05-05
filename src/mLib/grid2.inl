
#ifndef CORE_BASE_GRID2D_H_INL_
#define CORE_BASE_GRID2D_H_INL_

template <class T> Grid2<T>::Grid2()
{
    _dimX = 0;
    _dimY = 0;
    _data = nullptr;
}

template <class T> Grid2<T>::Grid2(size_t dimX, size_t dimY)
{
    _dimX = dimX;
    _dimY = dimY;
    _data = new T[dimX * dimY];
}

template <class T> Grid2<T>::Grid2(size_t dimX, size_t dimY, const T &value)
{
    _dimX = dimX;
    _dimY = dimY;
    _data = new T[dimX * dimY];
    setValues(value);
}

template <class T> Grid2<T>::Grid2(const Grid2<T> &G)
{
    _dimX = G._dimX;
    _dimY = G._dimY;

    const size_t totalEntries = _dimX * _dimY;
    _data = new T[totalEntries];
    for (size_t i = 0; i < totalEntries; i++)
        _data[i] = G._data[i];
}

template <class T> Grid2<T>::Grid2(Grid2<T> &&G)
{
    _dimX = G._dimX;
    _dimY = G._dimY;
    _data = G._data;

    G._dimX = 0;
    G._dimY = 0;
    G._data = nullptr;
}

template <class T> Grid2<T>::Grid2(size_t dimX, size_t dimY, const std::function< T(size_t, size_t) > &fillFunction)
{
    _dimX = dimX;
    _dimY = dimY;
    _data = new T[dimX * dimY];
    fill(fillFunction);
}

template <class T> Grid2<T>::~Grid2()
{
    free();
}

template <class T> void Grid2<T>::free()
{
    _dimX = 0;
    _dimY = 0;
    SAFE_DELETE(_data);
}

template <class T> Grid2<T>& Grid2<T>::operator = (const Grid2<T> &G)
{
    if (_data) delete[] _data;
    _dimX = G._dimX;
    _dimY = G._dimY;

    const size_t totalEntries = _dimX * _dimY;
    _data = new T[totalEntries];
    for (size_t i = 0; i < totalEntries; i++)
        _data[i] = G._data[i];

    return *this;
}

template <class T> Grid2<T>& Grid2<T>::operator = (Grid2<T> &&G)
{
    std::swap(_dimX, G._dimX);
    std::swap(_dimY, G._dimY);
    std::swap(_data, G._data);
    return *this;
}

template <class T> void Grid2<T>::allocate(size_t dimX, size_t dimY)
{
    if (dimX == 0 || dimY == 0) {
        free();
    }
    else {
        _dimX = dimX;
        _dimY = dimY;
        if (_data) delete[] _data;
        _data = new T[dimX * dimY];
    }
}

template <class T> void Grid2<T>::allocate(size_t dimX, size_t dimY, const T &value)
{
    if (dimX == 0 || dimY == 0) {
        free();
    }
    else {
        allocate(dimX, dimY);
        setValues(value);
    }
}

template <class T> void Grid2<T>::setValues(const T &value)
{
    const size_t totalEntries = _dimX * _dimY;
    for (size_t i = 0; i < totalEntries; i++)
        _data[i] = value;
}

template <class T> void Grid2<T>::fill(const std::function< T(size_t, size_t) > &fillFunction)
{
    for (size_t y = 0; y < _dimY; y++)
        for (size_t x = 0; x < _dimX; x++)
        {
            _data[y * _dimX + x] = fillFunction(x, y);
        }
}

#endif  // CORE_BASE_GRID2D_H_INL_
