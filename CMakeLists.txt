cmake_minimum_required(VERSION 3.7)
project(word2vec_cpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(word2vec_cpp main.cpp Module.cpp Module.h HierarchicalSoftmax.cpp HierarchicalSoftmax.h WordDict.cpp WordDict.h Reader.cpp Reader.h Word2vec.cpp Word2vec.h Embedding.cpp Embedding.h NegativeSampling.cpp NegativeSampling.h Vector.cpp Vector.h)