cmake_minimum_required(VERSION 3.16)
project(word2vec_cpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(word2vec_cpp main.cpp Module.cpp Module.h HierarchicalSoftmax.cpp HierarchicalSoftmax.h Trie.cpp Trie.h Reader.cpp Reader.h Word2vec.cpp Word2vec.h)