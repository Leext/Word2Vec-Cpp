//
// Created by Lee on 2020/7/25.
//

#include "Embedding.h"
#include <random>

Embedding::Embedding(int vocabSize, int embeddingSize) : vocabSize(vocabSize), embeddingSize(embeddingSize)
{
    int n = embeddingSize * vocabSize;
    matrix = new real[n];
    // init
    std::default_random_engine generator;
    std::normal_distribution<real> distribution;
    for (int i = 0; i < n; i++)
        matrix[i] = distribution(generator);
}

Embedding::~Embedding()
{
    delete[] matrix;
}

vector Embedding::forward(std::vector<int> &wordIdx)
{
    auto res = vector(embeddingSize, .0);
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

void Embedding::backward(std::vector<int> &wordIdx, vector &grad, real lr)
{
    int C = wordIdx.size();
    for (auto &idx: wordIdx)
    {
        int base = embeddingSize * idx;
        for (int i = 0; i < embeddingSize; i++)
            matrix[base + i] -= lr * grad[i] / C;
    }
}

vector Embedding::get(int idx)
{
    if (idx >= 0 && idx < vocabSize)
        return vector(embeddingSize, &matrix[idx * embeddingSize]);
    else
    {
        printf("invalid index %d\n", idx);
        return vector(0);
    }
}
