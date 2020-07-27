//
// Created by Lee on 2020/7/25.
//

#ifndef WORD2VEC_CPP_EMBEDDING_H
#define WORD2VEC_CPP_EMBEDDING_H

#include <vector>
#include "Vector.h"

typedef float real;
using vector = Vector<real>;

class Embedding
{
public:
    Embedding(int vocabSize, int embeddingSize);

    ~Embedding();

    vector forward(std::vector<int> &wordIdx);

    void backward(std::vector<int> &wordIdx, vector &grad, real lr);

    vector get(int idx);

private:
    real *matrix;
    int vocabSize;
    int embeddingSize;
};


#endif //WORD2VEC_CPP_EMBEDDING_H
