#include <iostream>
#include "Reader.h"
#include "Trie.h"
#include <algorithm>
#include <fstream>
#include "Word2vec.h"

int main()
{
//    Word2vec word2vec(100,1,1);
//    word2vec.buildVocab("../GOT.txt");
    Reader reader("../GOT.txt");
    Trie trie;
    std::cout << "build begin\n";
    while (!reader.end())
    {
        auto word = std::move(reader.getWord());
        if (word.empty())
            continue;
        trie.insert(std::move(word));
    }
    std::cout << "build end\n";
    auto vocab = std::move(trie.vocab);
    std::cout << "after move\n";
    std::sort(vocab.begin(), vocab.end(),
              [](const Word &a, const Word &b) -> bool
              { return a.count > b.count; }
    );
    std::cout << "after sort\n";
    auto cmpWord = Word();
    cmpWord.count = 5;
    auto it = std::lower_bound(vocab.begin(), vocab.end(), cmpWord,
                               [](const Word &a, const Word &b) -> bool
                               { return a.count > b.count; }
    );
//    vocab.erase(it, vocab.end());
    std::ofstream ofs("out.txt");
    for (auto &w: trie.vocab)
    {
        ofs << *w.word << " " << w.count << std::endl;
    }
    ofs.close();
    return 0;
}
