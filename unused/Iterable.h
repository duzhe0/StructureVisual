#ifndef ITERABLE_H
#define ITERABLE_H
#include "Iterator.h"
template<typename T>
class Iterable{
public:
    virtual Iterator<T>* createIterator()=0;
};
#endif // ITERABLE_H
