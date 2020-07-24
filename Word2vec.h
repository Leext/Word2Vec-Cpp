//
// Created by Lee on 2020/7/24.
//

#ifndef WORD2VEC_CPP_WORD2VEC_H
#define WORD2VEC_CPP_WORD2VEC_H

#include "Module.h"
#include "Trie.h"
#include "Reader.h"
#include <string>

class Word2vec
{
public:
    Word2vec(int embeddingSize, int cbow, int hs);

    void buildVocab(const char *filename, int minFreq = 5);

    void train(std::string &filename, int thread = 1, double lr = 1e-3);

private:
    void runStep();
    int embeddingSize;
    int cbow;
    int hs;
    std::vector<Word> vocab;
    Module *embedding;
    Module *output;
};


#endif //WORD2VEC_CPP_WORD2VEC_H
