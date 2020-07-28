#include "include/Word2vec.h"

int main()
{
    auto corpus = "../text8";
    Word2vec word2vec(200, 1, 1, 8, 25);
    word2vec.buildVocab(corpus);
    printf("start training\n");
    word2vec.init();
    word2vec.train(corpus, 2, 8, 0.025);
    word2vec.dump("result.txt");
    return 0;
}
