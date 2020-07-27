//
// Created by Lee on 2020/7/26.
//

#ifndef WORD2VEC_CPP_VECTOR_H
#define WORD2VEC_CPP_VECTOR_H

#include <cstdio>

template<typename T>
class Vector
{
public:
    Vector(int size);

    Vector(int size, T val);

    Vector(int size, T *base);

    Vector(Vector<T> &other) noexcept;

    Vector(Vector<T> &&other) noexcept;

    Vector<T> &operator=(const Vector<T> &other);

    Vector<T> &operator=(Vector<T> &&other);

    ~Vector();

    T &operator[](int idx);

    T mul(const Vector<T> &other);

    int size() const;

private:
    T *base;
    bool shared;
    int _size;

};

template<typename T>
Vector<T>::Vector(int size):_size(size), shared(false)
{
    base = new T[_size];
}

template<typename T>
Vector<T>::Vector(int size, T val):_size(size), shared(false)
{
    base = new T[_size];
    for (int i = 0; i < _size; i++)
        base[i] = val;
}

template<typename T>
Vector<T>::Vector(int size, T *base):_size(size), shared(true), base(base)
{
}

template<typename T>
int Vector<T>::size() const
{
    return _size;
}

template<typename T>
Vector<T>::~Vector()
{
    if (!shared)
        delete[] base;
}

template<typename T>
T Vector<T>::mul(const Vector<T> &other)
{
    if (_size != other.size())
    {
        printf("vector sizes are not matched, %d and %d\n", _size, other.size());
        return 0;
    }
    T ans = 0;
    for (int i = 0; i < _size; i++)
        ans += base[i] * other.base[i];
    return ans;
}

template<typename T>
Vector<T>::Vector(Vector<T> &&other) noexcept
{
    base = other.base;
    _size = other._size;
    shared = other.shared;
    other.base = nullptr;
}

template<typename T>
T &Vector<T>::operator[](int idx)
{
    return base[idx];
}

template<typename T>
Vector<T>::Vector(Vector<T> &other) noexcept
{
    base = other.base;
    _size = other._size;
    shared = true;
}

template<typename T>
Vector<T> &Vector<T>::operator=(const Vector<T> &other)
{
    if (this == &other)
        return *this;
    base = other.base;
    _size = other._size;
    shared = true;
    return *this;
}

template<typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &&other)
{
    if (this == &other)
        return *this;
    base = other.base;
    _size = other._size;
    shared = other.shared;
    other.base = nullptr;
    return *this;
}


#endif //WORD2VEC_CPP_VECTOR_H
