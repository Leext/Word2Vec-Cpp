//
// Created by Lee on 2020/7/27.
//

#ifndef WORD2VEC_CPP_HIERARCHICALSOFTMAX_H
#define WORD2VEC_CPP_HIERARCHICALSOFTMAX_H

#include <queue>
#include "OutputLayer.h"
#include "Common.h"

struct HuffmanNode
{
    HuffmanNode *left, *right;
    int count;
    // when node is leaf, wordIdx is real word in vocabulary
    // when node is inner node, wordIdx is virtual word in matrix
    int wordIdx;

    HuffmanNode(int count, int wordIdx, HuffmanNode *left = nullptr, HuffmanNode *right = nullptr);

    ~HuffmanNode();
};

HuffmanNode::HuffmanNode(int count, int wordIdx, HuffmanNode *left, HuffmanNode *right) : left(left), right(right),
                                                                                          count(count), wordIdx(wordIdx)
{
}

HuffmanNode::~HuffmanNode()
{
    delete left;
    delete right;
}

struct PathInfo
{
    int idx; // index in matrix
    int direction; // 1 for left, 0 for right
};

template<typename T>
class HierarchicalSoftmax : public OutputLayer<T>
{
public:
    HierarchicalSoftmax(int embeddingSize, int vocabSize, const std::vector<Word> &vocab);

    ~HierarchicalSoftmax();

    virtual Vector<T> forwardAndBackward(Vector<T> &h, int wordIdx, double lr);

private:
    T *matrix;
    int embeddingSize, vocabSize;
    std::vector<std::vector<PathInfo>> wordidx2path;

    HuffmanNode *buildHuffmanTree(const std::vector<Word> &vocab);

    void dfsGeneratePath(const std::vector<Word> &vocab, std::vector<PathInfo> &path, HuffmanNode *node);
};

template<typename T>
Vector<T> HierarchicalSoftmax<T>::forwardAndBackward(Vector<T> &h, int wordIdx, double lr)
{
    auto gradH = Vector<T>(embeddingSize, .0);
    auto &path = wordidx2path[wordIdx];
    for (auto &node : path)
    {
        // forward
        auto w = Vector<T>(embeddingSize, &matrix[embeddingSize * node.idx]);
        auto u = sigmoid(w.mul(h)) - node.direction;

        // backward
        for (int i = 0; i < embeddingSize; i++)
        {
            gradH[i] += u * w[i];
            w[i] -= lr * u * h[i];
        }
    }
    return std::move(gradH);
}

template<typename T>
HierarchicalSoftmax<T>::HierarchicalSoftmax(int embeddingSize, int vocabSize, const std::vector<Word> &vocab)
        :embeddingSize(embeddingSize), vocabSize(vocabSize)
{
    int n = embeddingSize * (vocabSize - 1);
    matrix = new T[n];
    // init
    std::default_random_engine generator;
    std::normal_distribution<T> distribution;
    for (int i = 0; i < n; i++)
        matrix[i] = distribution(generator) / embeddingSize;
    // build huffman tree
    auto huffmanRoot = buildHuffmanTree(vocab);
    // build path for each word
    wordidx2path = std::vector<std::vector<PathInfo>>(vocabSize);
    auto path = std::vector<PathInfo>();
    dfsGeneratePath(vocab, path, huffmanRoot);
    delete huffmanRoot;
}

template<typename T>
HierarchicalSoftmax<T>::~HierarchicalSoftmax()
{
    delete matrix;
}

template<typename T>
HuffmanNode *HierarchicalSoftmax<T>::buildHuffmanTree(const std::vector<Word> &vocab)
{
    auto cmp = [](const HuffmanNode *a, const HuffmanNode *b) -> bool
    { return a->count > b->count; };
    auto q = std::priority_queue<HuffmanNode *, std::vector<HuffmanNode *>, decltype(cmp)>(cmp);
    for (int i = 0; i < vocab.size(); i++)
        q.push(new HuffmanNode(vocab[i].count, i));
    int huffmanCount = 0;
    while (q.size() > 1)
    {
        auto left = q.top();
        q.pop();
        auto right = q.top();
        q.pop();
        q.push(new HuffmanNode(left->count + right->count, huffmanCount++, left, right));
    }
    return q.top();
}

template<typename T>
void
HierarchicalSoftmax<T>::dfsGeneratePath(const std::vector<Word> &vocab, std::vector<PathInfo> &path, HuffmanNode *node)
{
    if (node == nullptr) // impossible
        return;
    if (node->left == nullptr && node->right == nullptr) // leaf node
    {
        wordidx2path[node->wordIdx] = path;
        return;
    }
    // inner node
    path.push_back({node->wordIdx, 1});
    dfsGeneratePath(vocab, path, node->left);
    path.back().direction = 0;
    dfsGeneratePath(vocab, path, node->right);
    path.pop_back();
}


#endif //WORD2VEC_CPP_HIERARCHICALSOFTMAX_H
