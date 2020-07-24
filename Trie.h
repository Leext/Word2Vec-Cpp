//
// Created by Lee on 2020/7/23.
//

#ifndef WORD2VEC_CPP_TRIE_H
#define WORD2VEC_CPP_TRIE_H
#include <string>
#include <vector>
#include <unordered_map>

struct TrieNode
{
    std::unordered_map<char, int> children;
    int wordIdx;
    TrieNode();
};

struct Word
{
    std::string* word;
    int count;
    Word();
    explicit Word(std::string && w);
    explicit Word(std::string & w);
    ~Word();
};

class Trie {
public:
    Trie();
    void insert(std::string& word);
    void insert(std::string&& word);

    bool find(std::string& word);

    std::vector<Word> vocab;
private:
    std::vector<TrieNode> nodes;
    int rootIdx;
};


#endif //WORD2VEC_CPP_TRIE_H
