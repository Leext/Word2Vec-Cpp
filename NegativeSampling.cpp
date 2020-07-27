//
// Created by Lee on 2020/7/25.
//

#include "NegativeSampling.h"
#include <cstring>

#define EXP_MAX 6


inline real exp_fn(real x)
{
    if (x > EXP_MAX)
        return 1.0;
    else if (x < -EXP_MAX)
        return 0.0;
    else
        return exp(x);
}

inline real sigmoid(real x)
{
    return 1 / (1 + exp_fn(-x));
}

NegativeSampling::NegativeSampling(int embeddingSize, int vocabSize, int sampleSize, const std::vector<Word> &vocab)
        : embeddingSize(embeddingSize),
          vocabSize(vocabSize),
          sampleSize(sampleSize)
{
    int n = embeddingSize * vocabSize;
    matrix = new real[n];
    // init
    memset(matrix, 0, n * sizeof(real));
    buildUnigramTable(vocab);
}

vector NegativeSampling::forwardAndBackward(vector &h, std::vector<int> &wordIdx, double lr)
{
    vector gradH(embeddingSize, .0);
//    vector u(embeddingSize);
    // pos word
    int posIdx = wordIdx.front();
    auto w = vector(embeddingSize, &matrix[posIdx * embeddingSize]);
    // forward
    auto u = sigmoid(w.mul(h)) - 1;
    // backward
    for (int i = 0; i < embeddingSize; i++)
    {
        gradH[i] += u * w[i];
        w[i] -= lr * u * h[i];
    }
    // neg word
    for (int i = 0; i < sampleSize; i++)
    {
        int negIdx = negSample();
        // forward
        w = vector(embeddingSize, &matrix[negIdx * embeddingSize]);
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

void NegativeSampling::buildUnigramTable(const std::vector<Word> &vocab, double power)
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


NegativeSampling::~NegativeSampling()
{
    delete[] matrix;
}

int NegativeSampling::negSample()
{
    return table[intDistribution(generator)];
}


