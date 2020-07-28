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

};

class WordDict
{
public:
    virtual void insert(std::string &word) = 0;

    virtual void insert(std::string &&word) = 0;

    virtual bool find(std::string &word) = 0;

    virtual ~WordDict();

    std::vector<Word> vocab;
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
    void insert(std::string &word) override;

    void insert(std::string &&word) override;

    bool find(std::string &word) override;


private:
    std::unordered_map<std::string, int> word2idx;

};


TrieDict::TrieDict()
{
    rootIdx = 0;
    nodes.emplace_back();
}

void TrieDict::insert(std::string &word)
{
    int nodeIdx = rootIdx;
    for (auto &c: word)
    {
        if (nodes[nodeIdx].children.find(c) == nodes[nodeIdx].children.end())
        {
            nodes[nodeIdx].children[c] = nodes.size();
            nodes.emplace_back();
        }
        nodeIdx = nodes[nodeIdx].children[c];
    }
    if (nodeIdx != rootIdx && nodes[nodeIdx].wordIdx < 0)
    {
        nodes[nodeIdx].wordIdx = vocab.size();
        vocab.emplace_back(word);
    } else
    {
        vocab[nodes[nodeIdx].wordIdx].count++;
    }
}

void TrieDict::insert(std::string &&word)
{
    int nodeIdx = rootIdx;
    for (auto &c: word)
    {
        if (nodes[nodeIdx].children.find(c) == nodes[nodeIdx].children.end())
        {
            nodes[nodeIdx].children[c] = nodes.size();;
            nodes.emplace_back();
        }
        nodeIdx = nodes[nodeIdx].children[c];
    }
    if (nodeIdx != rootIdx && nodes[nodeIdx].wordIdx < 0)
    {
        nodes[nodeIdx].wordIdx = vocab.size();
        vocab.emplace_back(word);
    } else
    {
        vocab[nodes[nodeIdx].wordIdx].count++;
    }
}

bool TrieDict::find(std::string &word)
{
    int nodeIdx = rootIdx;
    for (auto &c: word)
    {
        if (nodes[nodeIdx].children.find(c) == nodes[nodeIdx].children.end())
            return false;
        nodeIdx = nodes[nodeIdx].children[c];
    }
    return nodes[nodeIdx].wordIdx >= 0;
}


TrieNode::TrieNode() : wordIdx(-1)
{
}

Word::Word() : word(nullptr), count(0)
{
}

Word::Word(std::string &&w)
{
    count = 1;
    word = std::make_shared<std::string>(w);
}

Word::Word(std::string &w)
{
    count = 1;
    word = std::make_shared<std::string>(w);
}


void HashDict::insert(std::string &word)
{
    if (word2idx.find(word) == word2idx.end())
    {
        word2idx[word] = vocab.size();
        vocab.emplace_back(word);
    } else
    {
        vocab[word2idx[word]].count++;
    }

}

void HashDict::insert(std::string &&word)
{
    if (word2idx.find(word) == word2idx.end())
    {
        word2idx[word] = vocab.size();
        vocab.emplace_back(word);
    } else
    {
        vocab[word2idx[word]].count++;
    }
}

bool HashDict::find(std::string &word)
{
    return word2idx.find(word) != word2idx.end();
}


WordDict::~WordDict()
= default;


#endif //WORD2VEC_CPP_WORDDICT_H
