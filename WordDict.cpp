//
// Created by Lee on 2020/7/23.
//

#include "WordDict.h"
#include <iostream>

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

Word::~Word()
= default;

HashDict::HashDict()
= default;

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

WordDict::WordDict()
= default;

WordDict::~WordDict()
= default;
