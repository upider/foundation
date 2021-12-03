#ifndef __SELECTED_HPP__
#define __SELECTED_HPP__

#include "select/selectable.hpp"

class Selector;

class Selected
{
private:
    Selector *_selector;
    Selectable *_selectable;
    Selectable::OperationCollection _op;

private:
    friend class Selector;
    Selected(Selector *selector, Selectable *selectable, Selectable::OperationCollection op);

public:
    ~Selected();
    Selector& selector();
    Selectable& selectable();
    /**
     * @brief 返回可执行的Selectable操作集合
     * 
     * @return Selectable::OperationCollection 可执行操作集合
     */
    Selectable::OperationCollection operation();
    /**
     * @brief 释放持有的Selectable对象并回收相应资源
     */
    void release();
    /**
     * @brief 重新将持有的Selectable对象放回select循环
     * 
     * @param ops 监听的selectable操作集合
     */
    void select(Selectable::OperationCollection ops);
};

#endif /* __SELECTED_HPP__ */
