cmake_minimum_required(VERSION 3.7)
project(word2vec_cpp)

set(CMAKE_CXX_STANDARD 14)
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 ")
set(DCMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-funroll-loops ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-O3 ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-lprofiler ${CMAKE_CXX_FLAGS}")

add_executable(word2vec_cpp main.cpp include/WordDict.h include/Reader.h include/Word2vec.h include/Embedding.h include/NegativeSampling.h include/Vector.h include/OutputLayer.h include/HierarchicalSoftmax.h include/Common.h)