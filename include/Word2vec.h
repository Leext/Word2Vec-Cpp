//
// Created by Lee on 2020/7/24.
//

#ifndef WORD2VEC_CPP_WORD2VEC_H
#define WORD2VEC_CPP_WORD2VEC_H

#include "WordDict.h"
#include "Reader.h"
#include "Embedding.h"
#include "NegativeSampling.h"
#include "HierarchicalSoftmax.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <thread>

#define PRINT_PER_LINE 1000
#define MAX_THREAD 64

typedef float real;

class Word2vec
{
public:
    Word2vec(int embeddingSize, int cbow, int hs, int window);

    void buildVocab(const char *filename, int minFreq = 5);

    void train(const char *filename, int numIter = 5, int numThread = 1, double lr = 1e-3);

    void init();

    void dump(const char *filename);

private:
    void trainThread(const char *filename, int numIter, int threadId, int numThread, double lr);

    int embeddingSize;
    int cbow;
    int hs;
    int window;
    std::vector<Word> vocab;
    std::unordered_map<std::string, int> word2idx;
    Embedding<real> *embedding;
    OutputLayer<real> *outputLayer;
};


Word2vec::Word2vec(int embeddingSize, int cbow, int hs, int window) : embeddingSize(embeddingSize), cbow(cbow), hs(hs),
                                                                      window(window)
{
    embedding = nullptr;
    outputLayer = nullptr;
}

void Word2vec::buildVocab(const char *filename, int minFreq)
{
    Reader reader(filename);
    WordDict *dict = new HashDict();
    while (!reader.end())
    {
        auto &&word = std::move(reader.getWord());
        if (word.empty())
            continue;
        dict->insert(std::move(word));
    }
    vocab = std::move(dict->vocab);
    delete dict;
    std::sort(vocab.begin(), vocab.end(),
              [](const Word &a, const Word &b) -> bool
              { return a.count > b.count; }
    );
    auto cmpWord = Word();
    cmpWord.count = minFreq;
    auto it = std::lower_bound(vocab.begin(), vocab.end(), cmpWord,
                               [](const Word &a, const Word &b) -> bool
                               { return a.count >= b.count; }
    );
    vocab.erase(it, vocab.end());
    for (int i = 0; i < vocab.size(); i++)
        word2idx[*vocab[i].word] = i;
}

void Word2vec::train(const char *filename, int numIter, int numThread, double lr)
{
    std::thread threads[MAX_THREAD];
    numThread = std::min(numThread, MAX_THREAD);
    if (numThread > 1)
    {
        for (int i = 0; i < numThread; i++)
            threads[i] = std::thread(&Word2vec::trainThread, this, filename, numIter, i, numThread, lr);
        for (int i = 0; i < numThread; i++)
            threads[i].join();
    } else
        trainThread(filename, numIter, 0, 1, lr);
}

void Word2vec::trainThread(const char *filename, int numIter, int threadId, int numThread, double lr)
{
    Reader reader(filename, numThread, threadId);
    int lineCount = 0;
    int totalLine = 0;
//    printf("start training\n");
    time_t begin = time(0);
    auto sentence = std::vector<std::string>();
    for (int i = 0; i < numIter; i++)
    {
        reader.reset();
        while (true)
        {
            sentence = std::move(reader.getSentence());
//            printf("good %d\n", threadId);
            if (sentence.empty())
                break;
            if (lineCount && lineCount % PRINT_PER_LINE == 0)
            {
                totalLine += lineCount;
                lineCount = 0;
//                printf("thread %d: %d lines, time consumed %ds\n", threadId, totalLine, time(0) - begin);
            }
            // get words index
            auto wordIdx = std::vector<int>();
            for (auto &w: sentence)
                if (word2idx.find(w) != word2idx.end())
                    wordIdx.push_back(word2idx[w]);
            if (wordIdx.empty())
                continue;
            std::vector<int> central, context;
            for (int i = 0; i < wordIdx.size(); i++)
            {
                central = {wordIdx[i]};
                context.clear();
                for (int j = i - 1; j >= 0 && j >= i - window; j--)
                    context.push_back(wordIdx[j]);
                for (int j = i + 1; j < wordIdx.size() && j <= i + window; j++)
                    context.push_back(wordIdx[j]);
                if (context.empty())
                    continue;
                if (cbow) // CBOW
                {
                    auto h = std::move(embedding->forward(context));
                    auto hGrad = std::move(outputLayer->forwardAndBackward(h, central.front(), lr));
                    embedding->backward(context, hGrad, lr);
                } else // skip-gram
                {
                    for (const int &idx: context)
                    {
                        auto h = std::move(embedding->forward(central));
                        auto hGrad = std::move(outputLayer->forwardAndBackward(h, idx, lr));
                        embedding->backward(central, hGrad, lr);
                    }
                }
            }
            lineCount++;
        }
    }
}


void Word2vec::init()
{
    embedding = new Embedding<real>(vocab.size(), embeddingSize);
    if (hs)
        outputLayer = new HierarchicalSoftmax<real>(embeddingSize, vocab.size(), vocab);
    else
        outputLayer = new NegativeSampling<real>(embeddingSize, vocab.size(), 10, vocab);
}

void Word2vec::dump(const char *filename)
{
    auto ofs = std::ofstream(filename);
    ofs << vocab.size() << " " << embeddingSize << std::endl;
    for (int i = 0; i < vocab.size(); i++)
    {
        ofs << *vocab[i].word << " ";
        auto w = embedding->get(i);
        for (int j = 0; j < embeddingSize; j++)
            ofs << w[j] << " ";
        ofs << std::endl;
    }
    ofs.close();
}


#endif //WORD2VEC_CPP_WORD2VEC_H
