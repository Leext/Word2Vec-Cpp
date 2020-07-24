//
// Created by Lee on 2020/7/23.
//

#ifndef WORD2VEC_CPP_MODULE_H
#define WORD2VEC_CPP_MODULE_H
class Module
{
public:
    virtual void forward() = 0;
    virtual void backward() = 0;
    virtual void update() = 0;
};
#endif //WORD2VEC_CPP_MODULE_H
