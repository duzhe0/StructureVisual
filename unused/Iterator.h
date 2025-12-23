#ifndef ITERATOR_H
#define ITERATOR_H
#include <stdexcept>
// ========== 迭代器类 ==========
template<typename T>
class Iterator{
public:
    virtual void First()=0;
    virtual void Next()=0;
    virtual bool isDone() const=0;
    virtual T CurrentItem() const=0;
protected:
    Iterator(){}
};

#endif // ITERATOR_H
