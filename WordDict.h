//
// Created by Lee on 2020/7/23.
//

#ifndef WORD2VEC_CPP_WORDDICT_H
#define WORD2VEC_CPP_WORDDICT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct TrieNode
{
    std::unordered_map<char, int> children;
    int wordIdx;

    TrieNode();
};

struct Word
{
    std::shared_ptr<std::string> word;
    int count;

    Word();

    explicit Word(std::string &&w);

    explicit Word(std::string &w);

    ~Word();
};

class WordDict
{
public:
    WordDict();

    virtual void insert(std::string &word) = 0;

    virtual void insert(std::string &&word) = 0;

    virtual bool find(std::string &word) = 0;

    std::vector<Word> vocab;

    virtual ~WordDict();
};

class TrieDict : public WordDict
{
public:
    TrieDict();

    void insert(std::string &word) override;

    void insert(std::string &&word) override;

    bool find(std::string &word) override;

private:
    int rootIdx;
    std::vector<TrieNode> nodes;
};

class HashDict : public WordDict
{
public:
    HashDict();

    void insert(std::string &word) override;

    void insert(std::string &&word) override;

    bool find(std::string &word) override;


private:
    std::unordered_map<std::string, int> word2idx;

};


#endif //WORD2VEC_CPP_WORDDICT_H
