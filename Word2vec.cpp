//
// Created by Lee on 2020/7/24.
//

#include "Word2vec.h"
#include <algorithm>
#include <iostream>
#include <ctime>
#include <thread>

#define PRINT_PER_LINE 1000

Word2vec::Word2vec(int embeddingSize, int cbow, int hs, int window) : embeddingSize(embeddingSize), cbow(cbow), hs(hs),
                                                                      window(window)
{
    embedding = nullptr;
    negativeSampling = nullptr;
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
                auto h = std::move(embedding->forward(context));
                auto hGrad = std::move(negativeSampling->forwardAndBackward(h, central, lr));
                embedding->backward(context, hGrad, lr);
            }
            lineCount++;
        }
    }
}


void Word2vec::init()
{
    embedding = new Embedding(vocab.size(), embeddingSize);
    negativeSampling = new NegativeSampling(embeddingSize, vocab.size(), 10, vocab);
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

