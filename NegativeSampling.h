//
// Created by Lee on 2020/7/25.
//

#ifndef WORD2VEC_CPP_NEGATIVESAMPLING_H
#define WORD2VEC_CPP_NEGATIVESAMPLING_H

#include <vector>
#include <random>
#include "WordDict.h"
#include "Vector.h"
#include <cmath>

typedef float real;
using vector = Vector<real>;
const int tableSize = 1e8;

class NegativeSampling
{
public:
    NegativeSampling(int embeddingSize, int vocabSize, int sampleSize, const std::vector<Word> &vocab);

    ~NegativeSampling();

    vector forwardAndBackward(vector &h, std::vector<int> &wordIdx, double lr);

private:
    real *matrix;
    std::vector<int> table;
    int embeddingSize, vocabSize, sampleSize;

    std::uniform_int_distribution<int> intDistribution = std::uniform_int_distribution<int>(0, tableSize - 1);
    std::default_random_engine generator;

    void buildUnigramTable(const std::vector<Word> &vocab, double power = 0.75);

    int negSample();
};


#endif //WORD2VEC_CPP_NEGATIVESAMPLING_H
