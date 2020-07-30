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

    Vector<T> *forwardAndBackward(Vector<T> &h, int wordIdx, T lr, Vector<T> *out = nullptr);

private:
    T *matrix;
    std::vector<int> table;
    const int embeddingSize;
    int vocabSize, sampleSize;

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
Vector<T> *NegativeSampling<T>::forwardAndBackward(Vector<T> &h, int wordIdx, T lr, Vector<T> *out)
{
    Vector<T> *gradH;
    if (out == nullptr)
        gradH = new Vector<T>(embeddingSize, 0.0);
    else
    {
        gradH = out;
        for (int i = 0; i < embeddingSize; i++)
            (*gradH)[i] = 0;
    }

    int base = wordIdx * embeddingSize;
    // forward
    T u = 0;
    for (int i = 0; i < embeddingSize; i++)
        u += matrix[base + i] * h[i];
    u = sigmoid(u) - 1;
    // backward
    for (int i = 0; i < embeddingSize; i++)
    {
        (*gradH)[i] += u * matrix[base + i];
        matrix[base + i] -= lr * u * h[i];
    }
    // neg word
    int negIdx;
    for (int i = 0; i < sampleSize; i++)
    {
        while ((negIdx = negSample()) == wordIdx);
        // forward
        base = negIdx * embeddingSize;
        u = 0;
        for (int j = 0; j < embeddingSize; j++)
            u += matrix[base + j] * h[j];
        u = sigmoid(u);
        // backward
        for (int j = 0; j < embeddingSize; j++)
        {
            (*gradH)[j] += u * matrix[base + j];
            matrix[base + j] -= lr * u * h[j];
        }
    }
    return gradH;
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
