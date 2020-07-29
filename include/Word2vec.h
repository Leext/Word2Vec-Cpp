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
#include "Common.h"
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
    Word2vec(int embeddingSize, int cbow, int hs, int window, int negSample = 10);

    void buildVocab(const char *filename, int minFreq = 5);

    void train(const char *filename, int numIter = 5, int numThread = 1, real lr = 1e-3);

    void init();

    void dump(const char *filename);

private:
    void trainThread(const char *filename, int numIter, int threadId, int numThread, real lr);

    int embeddingSize;
    int cbow;
    int hs;
    int window;
    int negSample;
    std::vector<Word> vocab;
    std::unordered_map<std::string, int> word2idx;
    Embedding<real> *embedding;
    OutputLayer<real> *outputLayer;
};


Word2vec::Word2vec(int embeddingSize, int cbow, int hs, int window, int negSample) : embeddingSize(embeddingSize),
                                                                                     cbow(cbow), hs(hs),
                                                                                     window(window),
                                                                                     negSample(negSample)
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
        auto word = std::move(reader.getWord());
        if (word.empty())
            continue;
        dict->insert(std::move(word));
    }
    printf("build complete\n");
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
    printf("vocab size %d\n", vocab.size());
    for (int i = 0; i < vocab.size(); i++)
        word2idx[*vocab[i].word] = i;
}

void Word2vec::train(const char *filename, int numIter, int numThread, real lr)
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

void Word2vec::trainThread(const char *filename, int numIter, int threadId, int numThread, real lr)
{
    Reader reader(filename, numThread, threadId);
    int lineCount = 0;
    int totalLine = 0;
    int bpCount = 0;
//    printf("start training\n");
    time_t begin = time(0);
    auto sentence = std::vector<std::string>();
    Vector<real> *h, *hGrad;
    h = new Vector<real>(embeddingSize);
    hGrad = new Vector<real>(embeddingSize);
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
            int n = wordIdx.size();
            for (int i = 0; i < n; i++)
            {
                auto sentView = SentenceView(wordIdx,
                                             std::max(0, i - window),
                                             std::min(n, i + window + 1),
                                             i);
                if (sentView.right - sentView.left - 1 <= 0)
                    continue;
                bpCount++;
                if (cbow) // CBOW
                {
                    embedding->forward(sentView, cbow, h);
                    outputLayer->forwardAndBackward(*h, sentView.sent[sentView.central], lr, hGrad);
                    embedding->backward(sentView, *hGrad, lr, cbow);
                } else // skip-gram
                {
                    for (int j = sentView.left; j < sentView.right; j++)
                    {
                        if (j == sentView.central)
                            continue;
                        embedding->forward(sentView, cbow, h);
                        outputLayer->forwardAndBackward(*h, sentView.sent[j], lr, hGrad);
                        embedding->backward(sentView, *hGrad, lr, cbow);
                    }
                }
            }
            lineCount++;
        }
    }
    printf("bp count %d\n", bpCount);
}


void Word2vec::init()
{
    embedding = new Embedding<real>(vocab.size(), embeddingSize);
    if (hs)
        outputLayer = new HierarchicalSoftmax<real>(embeddingSize, vocab.size(), vocab);
    else
        outputLayer = new NegativeSampling<real>(embeddingSize, vocab.size(), negSample, vocab);
}

void Word2vec::dump(const char *filename)
{
    FILE *fout = fopen(filename, "w");
    if (fout == NULL)
    {
        printf("cannot open %s\n", filename);
        return;
    }
    fprintf(fout, "%d %d\n", vocab.size(), embeddingSize);
    for (int i = 0; i < vocab.size(); i++)
    {
        fprintf(fout, "%s ", vocab[i].word->c_str());
        auto w = embedding->get(i);
        for (int j = 0; j < embeddingSize; j++)
            fprintf(fout, "%f ", w[j]);
        fprintf(fout, "\n");
    }
    fclose(fout);
}


#endif //WORD2VEC_CPP_WORD2VEC_H
