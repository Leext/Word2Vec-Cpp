# Word2Vec的C++实现

## 概述

本项目使用C++实现了NLP经典模型Word2Vec

- 支持CBOW和Skip-gram两种训练架构
- 支持Hierarchical Softmax和Negative Sampling两种优化方法
- 支持多线程并行训练
- 使用面向对象封装，相比C语言代码更易于阅读与修改
- 训练速度仅稍慢于C语言实现

## 构建方法
构建需要CMake和支持C++11的g++。

在项目根目录下运行以下命令：

```
cmake CMakeList.txt
make
```
会在根目录生成`word2vec_cpp`(linux)或`word2vec_cpp.exe`(windows)可执行文件。

## 使用方法

程序接受以下命令行参数：
- `-size`   词向量维度大小
- `-lr`  学习率，默认0.05
- `-train`  训练使用的文件地址
- `-output` 词向量输出文件地址
- `-binary` 是否使用二进制输出文件，默认0输出文本
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

## 训练速度对比
运行环境：

- Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz
- CentOS Linux release 7.6
- GCC 4.8.5

词向量大小设置为200，CBOW模型，负采样大小为25，线程数8，窗口大小8，最小词频5。

使用[text8](http://mattmahoney.net/dc/text8.zip)（大约100M）作为训练集。

测试结果如下：

|      | 原版C语言实现 | C++实现 |
| :--: | :-----------: | :-----: |
| 用时 |    5 mins     | 8 mins  |

## 实现优化

### 乘法向量化

在代码中很多地方都用到了向量乘法，一般是使用for循环来实现。如果写成编译器能够识别的形式，在开启编译优化的时候，编译器会帮助使用SIMD实现向量化，大幅提升速度。如下循环就可以被编译器识别并优化。

```C++
for (int j = 0; j < embeddingSize; j++)
    u += matrix[base + j] * h[j];
```

在编译选项中加入`-ftree-vectorizer-verbose=1`就可以显示编译器对哪些代码进行了向量优化。

### 复用内存，增加cache命中率

在word2vec前向和后向计算的时候，会产生两个临时向量。如果每一轮迭代都重新生成新的局部变量，会产生内存到cache的移动，增加了访存时间。其实在同一个线程内，这两个临时向量都可以复用。因此在迭代循环体外定义两个临时向量，并在计算过程中复用，因为减少了内存的申请和释放，可以保持这两个向量始终在cache中，大大加快了训练速度。经过测试，优化后提速高达**10倍**。

## C语言实现与论文不一致的地方 or Tricks

在实现C++版word2vec的过程中，参考了[原版C语言](https://code.google.com/archive/p/word2vec/)的实现。有意思的是，在代码中发现了一些与理论不太一样的地方。

### 梯度更新

在skip-gram模型中，我们会选取一个中心词，和它的上下文，使用中心词预测上下文中的每一个词。在原论文中并没有给出skip-gram的优化目标函数，只是说要对每一个上下文中的词做预测。在[CS224N的讲义](http://web.stanford.edu/class/cs224n/readings/cs224n-2019-notes01-wordvecs1.pdf)中，给出了一个优化目标函数，即最大化上下文每一个词概率的联合分布函数。这样必然在更新中心词向量的梯度，来自于所有上下文词梯度的和。

但在原版C语言实现中，每次使用中心词预测上下文一个词时，就会立即更新所有梯度。假设上下文中有N个词，那么中心词就会被更新N次。

### 多线程不同步参数

理论上在多线程训练时，每一次更新梯度都应该对权重加锁，不然有可能出现线程a在前向推理使用了向量w，但还没有做反向传播时，向量w已经被线程b更新了的情况。

猜测这么做可能是考虑到了词表大小远远大于并行的线程数，两个线程同时更新同一个向量的概率不大；并且即时更新了同一个向量，但梯度的方向也大概率是正确的。如果为了同步权重加上锁，会大大影响训练的效率。