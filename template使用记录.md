template<typename K, typename V>
class HashMapIterator : public Iterator<HashMapNode<K,V>>

HashMapNode<K,V> CurrentItem() const override

这样的声明是允许的
