//
// Created by Lee on 2020/7/25.
//

#ifndef WORD2VEC_CPP_EMBEDDING_H
#define WORD2VEC_CPP_EMBEDDING_H

#include <vector>
#include <random>
#include "Vector.h"

template<typename T>
class Embedding
{
public:
    Embedding(int vocabSize, int embeddingSize);

    ~Embedding();

    Vector<T> forward(std::vector<int> &wordIdx);

    void backward(std::vector<int> &wordIdx, Vector<T> &grad, double lr);

    Vector<T> get(int idx);

private:
    T *matrix;
    int vocabSize;
    int embeddingSize;
};

template<typename T>
Embedding<T>::Embedding(int vocabSize, int embeddingSize) : vocabSize(vocabSize), embeddingSize(embeddingSize)
{
    int n = embeddingSize * vocabSize;
    matrix = new T[n];
    // init
    std::default_random_engine generator;
    std::normal_distribution<T> distribution;
    for (int i = 0; i < n; i++)
        matrix[i] = distribution(generator);
}

template<typename T>
Embedding<T>::~Embedding()
{
    delete[] matrix;
}

template<typename T>
Vector<T> Embedding<T>::forward(std::vector<int> &wordIdx)
{
    auto res = Vector<T>(embeddingSize, .0);
    if (wordIdx.empty())
    {
        printf("warning : empty input wordIdx!\n");
        return res;
    }
    int C = wordIdx.size();
    for (auto &idx: wordIdx)
    {
        int base = embeddingSize * idx;
        for (int i = 0; i < embeddingSize; i++)
            res[i] += matrix[base + i];
    }
    for (int i = 0; i < embeddingSize; i++)
        res[i] /= C;
    return std::move(res);
}

template<typename T>
void Embedding<T>::backward(std::vector<int> &wordIdx, Vector<T> &grad, double lr)
{
    int C = wordIdx.size();
    for (auto &idx: wordIdx)
    {
        int base = embeddingSize * idx;
        for (int i = 0; i < embeddingSize; i++)
            matrix[base + i] -= lr * grad[i] / C;
    }
}

template<typename T>
Vector<T> Embedding<T>::get(int idx)
{
    if (idx >= 0 && idx < vocabSize)
        return Vector<T>(embeddingSize, &matrix[idx * embeddingSize]);
    else
    {
        printf("invalid index %d\n", idx);
        return Vector<T>(0);
    }
}

#endif //WORD2VEC_CPP_EMBEDDING_H
