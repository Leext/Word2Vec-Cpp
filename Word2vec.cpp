//
// Created by Lee on 2020/7/24.
//

#include "Word2vec.h"
#include <algorithm>
#include <iostream>

Word2vec::Word2vec(int embeddingSize, int cbow, int hs) : embeddingSize(embeddingSize), cbow(cbow), hs(hs)
{

}

void Word2vec::buildVocab(const char *filename, int minFreq)
{
    Reader reader(filename);
    Trie trie;
    while (!reader.end())
    {
        auto &&word = std::move(reader.getWord());
        if (word.empty())
            continue;
        trie.insert(std::move(word));
    }
    vocab = std::move(trie.vocab);
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
    std::ofstream ofs("out.txt");
    for (auto &w: vocab)
    {
        ofs << *w.word << " " << w.count << std::endl;
    }
    ofs.close();
}

void Word2vec::train(std::string &filename, int thread, double lr)
{

}

void Word2vec::runStep()
{

}
