//
// Created by Lee on 2020/7/25.
//

#ifndef WORD2VEC_CPP_EMBEDDING_H
#define WORD2VEC_CPP_EMBEDDING_H

#include <vector>
#include <random>
#include "Vector.h"
#include "Common.h"

template<typename T>
class Embedding
{
public:
    Embedding(int vocabSize, int embeddingSize);

    ~Embedding();

    Vector<T> *forward(SentenceView &sentView, int cbow, Vector<T> *out);

    void backward(SentenceView &sentView, Vector<T> &grad, T lr, int cbow);

    Vector<T> get(int idx);

    T *matrix;
private:
    int vocabSize;
    int embeddingSize;
};

template<typename T>
Embedding<T>::Embedding(int vocabSize, int embeddingSize) : vocabSize(vocabSize), embeddingSize(embeddingSize)
{
    int n = embeddingSize * vocabSize;
    if (n == 0)
    {
        printf("empty vocabulary!\n");
        n = 1;
    }
    matrix = new T[n];
    // init
    std::default_random_engine generator;
    std::normal_distribution<T> distribution;
    for (int i = 0; i < n; i++)
        matrix[i] = distribution(generator) / embeddingSize;
}

template<typename T>
Embedding<T>::~Embedding()
{
    delete[] matrix;
}

template<typename T>
Vector<T> *Embedding<T>::forward(SentenceView &sentView, int cbow, Vector<T> *out)
{
    Vector<T> *res;
    if (out == nullptr)
        res = new Vector<T>(embeddingSize, 0.0);
    else
    {
        res = out;
        for (int i = 0; i < embeddingSize; i++)
            res->base[i] = 0;
    }
    int idx, base;
    if (cbow)
    {
        if (sentView.right - sentView.left - 1 <= 0)
        {
            printf("warning : empty input wordIdx!\n");
            return res;
        }
        int C = sentView.right - sentView.left - 1;
        for (int j = sentView.left; j < sentView.right; j++)
        {
            if (j == sentView.central)
                continue;
            idx = sentView.sent[j];
            base = embeddingSize * idx;
            for (int i = 0; i < embeddingSize; i++)
                res->base[i] += matrix[base + i];
        }
        for (int i = 0; i < embeddingSize; i++)
            res->base[i] /= C;
    } else
    {
        idx = sentView.sent[sentView.central];
        base = embeddingSize * idx;
        for (int i = 0; i < embeddingSize; i++)
            res->base[i] += matrix[base + i];
    }
    return res;
}

template<typename T>
void Embedding<T>::backward(SentenceView &sentView, Vector<T> &grad, T lr, int cbow)
{
    int idx, base;
    if (cbow)
    {
        int C = sentView.right - sentView.left - 1;
        for (int j = sentView.left; j < sentView.right; j++)
        {
            if (j == sentView.central)
                continue;
            idx = sentView.sent[j];
            base = embeddingSize * idx;
            for (int i = 0; i < embeddingSize; i++)
                matrix[base + i] -= lr * grad.base[i] / C;
        }
    } else
    {
        idx = sentView.sent[sentView.central];
        base = embeddingSize * idx;
        for (int i = 0; i < embeddingSize; i++)
            matrix[base + i] -= lr * grad.base[i];
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
