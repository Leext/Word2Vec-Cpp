//
// Created by Lee on 2020/7/24.
//

#ifndef WORD2VEC_CPP_READER_H
#define WORD2VEC_CPP_READER_H

#include <fstream>
#include <vector>

class Reader
{
public:
    explicit Reader(const char *filename);

    Reader(const char *filename, int totalPart, int part);

    std::string getWord();

    std::vector<std::string> getSentence();

    bool end();

    void reset();

    ~Reader();

private:
    std::ifstream fs;
    unsigned int startPos, endPos;
};


#endif //WORD2VEC_CPP_READER_H
