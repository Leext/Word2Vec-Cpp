#include "include/Word2vec.h"
#include <cstring>

//#include <gperftools/profiler.h>
char *parseArg(const char *argName, int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
        if (strcmp(argName, argv[i]) == 0)
            return (i + 1 < argc) ? argv[i + 1] : nullptr;
    return nullptr;
}


int main(int argc, char **argv)
{
//    ProfilerStart("test_capture.prof");
    char trainFile[200];
    trainFile[0] = 0;
    char output[200];
    output[0] = 0;
    int embeddingSize = 128;
    float lr = 0.05;
    int window = 5;
    int cbow = 1;
    int hs = 0;
    int negative = 10;
    int threads = 1;
    int iter = 5;
    int minCount = 5;
    int binary = 0;
    char *tempArg;
    if ((tempArg = parseArg("-size", argc, argv)) != nullptr)
        embeddingSize = atoi(tempArg);
    if ((tempArg = parseArg("-lr", argc, argv)) != nullptr)
        lr = atof(tempArg);
    if ((tempArg = parseArg("-train", argc, argv)) != nullptr)
        strcpy(trainFile, tempArg);
    if ((tempArg = parseArg("-window", argc, argv)) != nullptr)
        window = atoi(tempArg);
    if ((tempArg = parseArg("-cbow", argc, argv)) != nullptr)
        cbow = atoi(tempArg);
    if ((tempArg = parseArg("-hs", argc, argv)) != nullptr)
        hs = atoi(tempArg);
    if ((tempArg = parseArg("-negative", argc, argv)) != nullptr)
        negative = atoi(tempArg);
    if ((tempArg = parseArg("-threads", argc, argv)) != nullptr)
        threads = atoi(tempArg);
    if ((tempArg = parseArg("-iter", argc, argv)) != nullptr)
        iter = atoi(tempArg);
    if ((tempArg = parseArg("-output", argc, argv)) != nullptr)
        strcpy(output, tempArg);
    if ((tempArg = parseArg("-min-count", argc, argv)) != nullptr)
        minCount = atoi(tempArg);
    if ((tempArg = parseArg("-binary", argc, argv)) != nullptr)
        binary = atoi(tempArg);
    if (strlen(trainFile) <= 0)
    {
        printf("no input file\n");
        return 0;
    }
    if (strlen(output) <= 0)
    {
        printf("no output file\n");
        return 0;
    }
    printf("input file: %s\n", trainFile);
    printf("output file: %s\n", output);
    Word2vec word2vec(embeddingSize, cbow, hs, window, negative);
    word2vec.buildVocab(trainFile, minCount);
    word2vec.init();
    word2vec.train(trainFile, iter, threads, lr);
    word2vec.dump(output, binary);
//    ProfilerStop();
}
