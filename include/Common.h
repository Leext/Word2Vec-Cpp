//
// Created by Lee on 2020/7/27.
//

#ifndef WORD2VEC_CPP_COMMON_H
#define WORD2VEC_CPP_COMMON_H

#include <cmath>

#define EXP_MAX 6

template<typename T>
inline T exp_fn(T x)
{
    if (x > EXP_MAX)
        return 1.0;
    else if (x < -EXP_MAX)
        return 0.0;
    else
        return exp(x);
}

template<typename T>
inline T sigmoid(T x)
{
    return 1 / (1 + exp_fn<T>(-x));
}

#endif //WORD2VEC_CPP_COMMON_H
