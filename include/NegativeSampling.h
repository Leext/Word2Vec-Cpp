//
// Created by Lee on 2020/7/25.
//

#ifndef WORD2VEC_CPP_NEGATIVESAMPLING_H
#define WORD2VEC_CPP_NEGATIVESAMPLING_H

#include <vector>
#include <random>
#include <cmath>
#include <cstring>
#include "WordDict.h"
#include "Vector.h"
#include "OutputLayer.h"
#include "Common.h"

const int tableSize = 1e8;

template<typename T>
class NegativeSampling : public OutputLayer<T>
{
public:
    NegativeSampling(int embeddingSize, int vocabSize, int sampleSize, const std::vector<Word> &vocab);

    ~NegativeSampling();

    Vector<T> forwardAndBackward(Vector<T> &h, int wordIdx, double lr);

private:
    T *matrix;
    std::vector<int> table;
    int embeddingSize, vocabSize, sampleSize;

    std::uniform_int_distribution<int> intDistribution = std::uniform_int_distribution<int>(0, tableSize - 1);
    std::default_random_engine generator;

    void buildUnigramTable(const std::vector<Word> &vocab, double power = 0.75);

    int negSample();
};


template<typename T>
NegativeSampling<T>::NegativeSampling(int embeddingSize, int vocabSize, int sampleSize, const std::vector<Word> &vocab)
        : embeddingSize(embeddingSize),
          vocabSize(vocabSize),
          sampleSize(sampleSize)
{
    int n = embeddingSize * vocabSize;
    matrix = new T[n];
    // init
    memset(matrix, 0, n * sizeof(T));
    buildUnigramTable(vocab);
}

template<typename T>
Vector<T> NegativeSampling<T>::forwardAndBackward(Vector<T> &h, int wordIdx, double lr)
{
    Vector<T> gradH(embeddingSize, .0);
    auto w = Vector<T>(embeddingSize, &matrix[wordIdx * embeddingSize]);
    // forward
    auto u = sigmoid(w.mul(h)) - 1;
    // backward
    for (int i = 0; i < embeddingSize; i++)
    {
        gradH[i] += u * w[i];
        w[i] -= lr * u * h[i];
    }
    // neg word
    int negIdx;
    for (int i = 0; i < sampleSize; i++)
    {
        while ((negIdx = negSample()) == wordIdx);
        // forward
        w = Vector<T>(embeddingSize, &matrix[negIdx * embeddingSize]);
        u = sigmoid(w.mul(h));
        // backward
        for (int i = 0; i < embeddingSize; i++)
        {
            gradH[i] += u * w[i];
            w[i] -= lr * u * h[i];
        }
    }
    return std::move(gradH);
}

template<typename T>
void NegativeSampling<T>::buildUnigramTable(const std::vector<Word> &vocab, double power)
{
    table = std::vector<int>(tableSize);
    double power_sum = 0;
    for (auto &w: vocab)
        power_sum += pow(w.count, power);
    double cumProb = pow(vocab.front().count, power) / power_sum;
    int i = 0;
    for (int a = 0; a < tableSize; a++)
    {
        table[a] = i;
        if ((double) a / tableSize > cumProb)
        {
            i++;
            cumProb += pow(vocab[i].count, power) / power_sum;
        }
        if (i >= vocab.size())
            i = vocab.size() - 1;
    }

}

template<typename T>
NegativeSampling<T>::~NegativeSampling()
{
    delete[] matrix;
}

template<typename T>
int NegativeSampling<T>::negSample()
{
    return table[intDistribution(generator)];
}


#endif //WORD2VEC_CPP_NEGATIVESAMPLING_H
