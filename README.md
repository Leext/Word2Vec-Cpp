# Word2Vec的C++实现

## 构建方法
构建需要CMake和支持C++11的g++。

在项目根目录下运行以下命令：

```
cmake CmakeList.txt
make
```
会在根目录生成`word2vec_cpp`(linux)或`word2vec_cpp.exe`(windows)可执行文件。

## 使用方法

程序接受以下命令行参数：
- `-size`   词向量维度大小
- `-lr`  学习率，默认0.05
- `-train`  训练使用的文件地址
- `-output` 词向量输出文件地址
- `-binary` 是否使用二进制输出文件，模型0
- `-window`  训练词向量的窗口大小
- `-cbow`    使用CBOW(1)还是skip-gram(0)，默认1
- `-hs`  是否使用hierarchical softmax，默认0
- `-negative`    负采样词向量的个数，0不开启，默认10
- `-threads` 并行训练线程数，默认1
- `-iter`   迭代次数，默认5
- `-min-count`  词汇表最小词频，默认5

例如
```shell script
./word2vec_cpp -train ./text8 \
                -output ./vector.bin \
                -lr 1e-3 \
                -size 200 \
                -binary 1 \
                -threads 8 \
                -window 8 \
                -iter 5 \
                -cbow 1
```