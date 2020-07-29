#include "include/Word2vec.h"

//#include <gperftools/profiler.h>
int main()
{
//    ProfilerStart("test_capture.prof");
    auto corpus = "../text8";
    Word2vec word2vec(100, 1, 0, 8, 10);
    word2vec.buildVocab(corpus);
    printf("start training\n");
    word2vec.init();
    printf("init complete\n");
    word2vec.train(corpus, 1, 1, 0.025);
    word2vec.dump("result.txt");
//    ProfilerStop();
    return 0;
}
