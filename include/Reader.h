//
// Created by Lee on 2020/7/24.
//

#ifndef WORD2VEC_CPP_READER_H
#define WORD2VEC_CPP_READER_H

#include <fstream>
#include <vector>
#include <cstdio>
#include <iostream>
#include <string>

#define MAX_WORD_LENGTH 100

class Reader
{
public:
    explicit Reader(const char *filename);

    Reader(const char *filename, int totalPart, int part);

    int getChar();

    std::string getWord();

    std::vector<std::string> getSentence();

    bool end();

    void reset();

    ~Reader();

private:
    char *buffer;
    char *wordBuffer;
    int bufferSize;
    int bufferLen;
    int point;
    FILE *fin;
    long int startPos, endPos;
};


using string = std::string;

inline bool isSpace(int c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

int Reader::getChar()
{

    if (point >= bufferLen)
    {
        bufferLen = fread(buffer, 1, bufferSize, fin);
        point = 0;
    }
    if (point < bufferLen)
    {
//        printf("%c ", buffer[point]);
        return buffer[point++];
    } else
        return 0;
}


std::string Reader::getWord()
{
    int len = 0;
    int c;
//    while (!end() && isSpace(c = getChar()));
//    if (end())
//        return std::string("");
//    wordBuffer[len++] = c;
//
//    while (!end() && len < MAX_WORD_LENGTH)
//    {
//        c = getChar();
//        if (isSpace(c))
//        {
//            if (c == '\n')
//                point--;
//            break;
//        }
//        if (!end())
//            wordBuffer[len++] = c;
//    }
    while (!end() && len < MAX_WORD_LENGTH)
    {
        c = getChar();
        if (isSpace(c))
        {
            if (len > 0)
            {
                if (c == '\n')
                    point--;
                break;
            }
            continue;
        }
        wordBuffer[len++] = c;
    }
    return std::string(wordBuffer, len);
}

Reader::Reader(const char *filename)
{
    fin = fopen(filename, "r");
    if (fin == NULL)
    {
        std::cout << "cannot open file " << filename << std::endl;
        return;
    }
    startPos = 0;
    fseek(fin, 0, SEEK_END);
    endPos = ftell(fin);
    rewind(fin);
    bufferSize = 1 << 26;
    buffer = new char[bufferSize];
    wordBuffer = new char[MAX_WORD_LENGTH];
    point = bufferSize;
    bufferLen = 0;
}

Reader::Reader(const char *filename, int totalPart, int part)
{
    fin = fopen(filename, "r");
    if (fin == NULL)
    {
        std::cout << "cannot open file " << filename << std::endl;
        return;
    }
    fseek(fin, 0, SEEK_END);
    unsigned int fileSize = ftell(fin);
    startPos = part * (fileSize / totalPart);
    fseek(fin, startPos, SEEK_SET);
    if (part + 1 == totalPart)
        endPos = fileSize;
    else
        endPos = (part + 1) * (fileSize / totalPart);
    bufferSize = 1 << 20;
    buffer = new char[bufferSize];
    wordBuffer = new char[MAX_WORD_LENGTH];
    point = bufferSize;
    bufferLen = 0;
}

bool Reader::end()
{
    return (point >= bufferLen && feof(fin)) || ftell(fin) >= endPos;
}

Reader::~Reader()
{
    fclose(fin);
    delete buffer;
    delete wordBuffer;
}

std::vector<string> Reader::getSentence()
{
    auto sentence = std::vector<string>();
    while (!end() && getChar() == '\n');
    if (end())
        return sentence;
    point--;
    while (!end() && getChar() != '\n')
    {
        point--;
        sentence.push_back(std::move(getWord()));
    }
    return sentence;
}

void Reader::reset()
{
    if (fin != NULL)
        fseek(fin, startPos, SEEK_SET);
}


#endif //WORD2VEC_CPP_READER_H
