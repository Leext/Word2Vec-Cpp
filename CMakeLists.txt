cmake_minimum_required(VERSION 3.7)
project(word2vec_cpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(word2vec_cpp main.cpp include/WordDict.h include/Reader.h include/Word2vec.h include/Embedding.h include/NegativeSampling.h include/Vector.h include/OutputLayer.h include/HierarchicalSoftmax.h include/Common.h)