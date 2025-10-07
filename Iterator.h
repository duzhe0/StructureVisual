#ifndef ITERATOR_H
#define ITERATOR_H
#include <stdexcept>
//前向声明

template <typename T>
class DynamicArray;

// ========== 迭代器类 ==========
template<typename T>
class Iterator{
public:
    virtual void First()=0;
    virtual void Next()=0;
    virtual bool isDone() const=0;
    virtual T CurrentItem() const=0;
protected:
    Iterator(){};
};

//ArrayIterator begin

template<typename T>
class ArrayIterator:public Iterator<T>{
public:
    ArrayIterator(const DynamicArray<T>* array);
    virtual void First();
    virtual void Next();
    virtual bool isDone() const;
    virtual T CurrentItem() const;
private:
    const DynamicArray<T>* _array;
    long _current;
};

template<class T>
ArrayIterator<T>::ArrayIterator(const DynamicArray<T> *array):
    _array(array),_current(0){}

template<class T>
void ArrayIterator<T>::First(){
    _current=0;
}

template<class T>
void ArrayIterator<T>::Next(){
    _current++;
}

template<class T>
bool ArrayIterator<T>::isDone() const{
    return _current>=_array->size();
}

template<class T>
T ArrayIterator<T>::CurrentItem() const{
    if(isDone()){
        throw std::out_of_range("Array index");
    }
    return _array->at(_current);
}

//ArrayIterator end

#endif // ITERATOR_H
