//
// Created by Lee on 2020/7/27.
//

#ifndef WORD2VEC_CPP_OUTPUTLAYER_H
#define WORD2VEC_CPP_OUTPUTLAYER_H

#include <vector>
#include "Vector.h"

template<typename T>
class OutputLayer
{
public:
    virtual Vector<T> forwardAndBackward(Vector<T> &h, int wordIdx, double lr) = 0;
};


#endif //WORD2VEC_CPP_OUTPUTLAYER_H
