#include <iostream>
#include "Reader.h"
#include "Trie.h"
#include <algorithm>
#include <fstream>
#include "Word2vec.h"

int main()
{
    Word2vec word2vec(100,1,1);
    word2vec.buildVocab("../GOT.txt");
    return 0;
}
