#include "include/Word2vec.h"

int main()
{
    Word2vec word2vec(128, 1, 1, 5);
    word2vec.buildVocab("../GOT.txt");
    word2vec.init();
    word2vec.train("../GOT.txt", 5, 8, 1e-4);
    word2vec.dump("result.txt");
    return 0;
}
