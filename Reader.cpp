//
// Created by Lee on 2020/7/24.
//

#include "Reader.h"
#include <iostream>
#include <string>

using string = std::string;

inline bool isSpace(int c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

std::string Reader::getWord()
{
    string word;
    int c;
    while (!end() && isSpace(c = fs.get()));
    if (end())
        return word;
    word.push_back(c);
    while (!end())
    {
        c = fs.get();
        if (isSpace(c))
        {
            if (c == '\n')
                fs.unget();
            break;
        }
        if(!end())
            word.push_back(c);
    }
    return word;
}

Reader::Reader(const char *filename)
{
    fs.open(filename);
    if (!fs.good())
        std::cout << "cannot open file " << filename << std::endl;
}

bool Reader::end()
{
    return fs.eof();
}

Reader::~Reader()
{
    fs.close();
}

std::vector<string> Reader::getSentence()
{
    auto sentence = std::vector<string>();
    while (!end() && fs.get() == '\n');
    if (end())
        return sentence;
    fs.unget();
    while (!end() && fs.get() != '\n')
    {
        fs.unget();
        sentence.push_back(std::move(getWord()));
    }
    return sentence;
}
