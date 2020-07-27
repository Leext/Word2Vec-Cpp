//
// Created by Lee on 2020/7/24.
//

#ifndef WORD2VEC_CPP_WORD2VEC_H
#define WORD2VEC_CPP_WORD2VEC_H

#include "Module.h"
#include "WordDict.h"
#include "Reader.h"
#include "Embedding.h"
#include "NegativeSampling.h"
#include <string>

#define MAX_THREAD 64

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
    Embedding *embedding;
    NegativeSampling *negativeSampling;
};


#endif //WORD2VEC_CPP_WORD2VEC_H
