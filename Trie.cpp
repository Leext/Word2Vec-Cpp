//
// Created by Lee on 2020/7/23.
//

#include "Trie.h"
#include <iostream>

Trie::Trie()
{
    rootIdx = 0;
    nodes.emplace_back();
}

void Trie::insert(std::string &word)
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

void Trie::insert(std::string &&word)
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

bool Trie::find(std::string &word)
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
    word = new std::string(w);
}

Word::Word(std::string &w)
{
    count = 1;
    word = new std::string(w);
}

Word::~Word()
{
    delete word;
}
