#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <functional>
#include <iostream>
#include "LinkedList.h"

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<const key_type, mapped_type>;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    class ConstIterator;
    class Iterator;
    using iterator = Iterator;
    using const_iterator = ConstIterator;
private:
    LinkedList<value_type> * buckets;
    size_type size;

    inline size_type amountOfBuckets() const
    {
        return 128000;
    }

    void initBuckets()
    {
        buckets = new LinkedList<value_type>[amountOfBuckets()];
    }

    void deallocBuckets()
    {
        if(buckets != nullptr)
            delete [] buckets;
    }

    size_type getHash(const key_type& key) const
    {
        return std::hash<key_type>{}(key) % amountOfBuckets();
    }

    value_type& getDataForKey(const key_type &key) const
    {
        auto hash = getHash(key);
        for(auto it = buckets[hash].begin(); it != buckets[hash].end(); ++it)
            if((*it).first == key)
                return *it;
        throw std::out_of_range("Attempt to get an element that is not in the map.");
    }

    bool isKeyInMap(const key_type & key) const
    {
        try
        {
            getDataForKey(key);
            return true;
        }
        catch(std::out_of_range)
        {
            return false;
        }
    }



public:
    HashMap()
    {
        size = 0;
        initBuckets();
    }

    ~HashMap()
    {
        deallocBuckets();
    }

    HashMap(std::initializer_list<value_type> list)
    : HashMap()
    {
        for(auto element: list)
            operator[](element.first) = element.second;
    }

    HashMap(const HashMap& other)
    : HashMap()
    {
        size = other.size;
        for(size_type i = 0; i < amountOfBuckets(); i++)
            buckets[i] = other.buckets[i];
    }

    HashMap(HashMap&& other)
    : buckets(other.buckets), size(other.size)
    {
        other.buckets = nullptr;
        other.size = 0;
    }

    HashMap& operator=(const HashMap& other)
    {
        if(other == *this)
            return *this;

        size = other.size;
        for(size_type i = 0; i < amountOfBuckets(); i++)
            buckets[i] = other.buckets[i];
        return *this;
    }

    HashMap& operator=(HashMap&& other)
    {
        if(other == *this)
            return *this;

        buckets = other.buckets;
        size = other.size;
        other.buckets = nullptr;
        other.size = 0;

        return *this;
    }

    bool isEmpty() const
    {
        return size == 0;
    }

    mapped_type& operator[](const key_type& key)
    {
        auto hash = getHash(key);
        auto position = find(key);
        if(position == end())
        {
            buckets[hash].prepend(std::make_pair(key, mapped_type{}));
            size++;
            return buckets[hash][0].second;
        }
        else
        {
            return buckets[position.whichBucket][position.whichElement].second;
        }
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        if(isEmpty())
            throw std::out_of_range("Attempt to get a value from an empty map.");

        return getDataForKey(key).second;
    }

    mapped_type& valueOf(const key_type& key)
    {
        if(isEmpty())
            throw std::out_of_range("Attempt to get a value from an empty map.");

        return getDataForKey(key).second;
    }

    const_iterator find(const key_type& key) const // can be optimised
    {
        for(size_type i = 0; i < amountOfBuckets(); i++)
        {
            for(size_type counter = 0; counter < buckets[i].getSize(); counter++)
            {
                if(buckets[i][counter].first == key)
                    return ConstIterator(this, i, counter);
            }
        }
        return cend();
    }

    iterator find(const key_type& key) // can be optimised
    {
        for(size_type i = 0; i < amountOfBuckets(); i++)
        {
            for(size_type counter = 0; counter < buckets[i].getSize(); counter++)
            {
                if(buckets[i][counter].first == key)
                    return Iterator(this, i, counter);
            }
        }
        return end();
    }

    void remove(const key_type& key)
    {
        auto position = find(key);
        if(position == end())
            throw std::out_of_range("Attempt to remove from an empty map.");

        size--;
        buckets[position.whichBucket].erase(position.whichElement);
    }

    void remove(const const_iterator& it)
    {
        if(it == cend())
            throw std::out_of_range("Attempt to remove from an empty map.");

        size--;
        buckets[it.whichBucket].erase(it.whichElement);
    }

    size_type getSize() const
    {
        return size;
    }

    bool operator==(const HashMap& other) const
    {
        if(size != other.size)
            return false;

        for(auto element : other)
        {
            auto foundEl = find(element.first);
            if(foundEl == cend() || foundEl->second != element.second)
                return false;
        }

        return true;
    }

    bool operator!=(const HashMap& other) const
    {
        return !(*this == other);
    }

    iterator begin()
    {
        return cbegin();
    }

    iterator end()
    {
        return cend();
    }

    const_iterator cbegin() const
    {
        for(size_type whichBucket = 0; whichBucket < amountOfBuckets(); whichBucket++)
        {
            if(buckets[whichBucket].getSize() > 0)
                return ConstIterator(this, whichBucket, 0);
        }
        return cend();

    }

    const_iterator cend() const
    {
        return ConstIterator(this, amountOfBuckets(), 0);
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
    friend HashMap<KeyType, ValueType>;
public:
    using reference = typename HashMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename HashMap::value_type;
    using pointer = const typename HashMap::value_type*;
    using size_type = HashMap::size_type;
protected:
    HashMap<KeyType, ValueType> * whichMap;
    size_type whichBucket;
    size_type whichElement;
    ConstIterator(const HashMap<KeyType, ValueType> * whichM, size_type whichB, size_type whichE)
    : whichBucket(whichB), whichElement(whichE)
    {
        whichMap = const_cast<HashMap<KeyType, ValueType> *>(whichM);
    }


public:
    explicit ConstIterator()
    {}

    ConstIterator(const ConstIterator& other)
    {
        whichMap = other.whichMap;
        whichBucket = other.whichBucket;
        whichElement = other.whichElement;
    }

    ConstIterator& operator++()
    {
        if(*this == whichMap->cend())
            throw std::out_of_range("Attempt to increment end() iterator.");

        if(whichElement + 1 < whichMap->buckets[whichBucket].getSize())
        {
            whichElement++;
            return *this;
        }

        for(size_type i = whichBucket + 1; i < whichMap->amountOfBuckets(); i++)
        {
            if(whichMap->buckets[i].getSize() > 0)
            {
                whichBucket = i;
                whichElement = 0;
                return *this;
            }
        }

        whichBucket = whichMap->amountOfBuckets(); // end iterator
        whichElement = 0;
        return *this;
    }

    ConstIterator operator++(int)
    {
        ConstIterator preObject(*this);
        operator++();
        return preObject;
    }

    ConstIterator& operator--()
    {
        if(*this == whichMap->cbegin())
            throw std::out_of_range("Attempt to decrement begin() iterator.");

        if(whichElement > 0)
        {
            whichElement--;
            return *this;
        }

        for(size_type i = whichBucket - 1; true; i--)
        {
            if(whichMap->buckets[i].getSize() > 0)
            {
                whichBucket = i;
                whichElement = whichMap->buckets[i].getSize() - 1;
                return *this;
            }
        }
    }

    ConstIterator operator--(int)
    {
        ConstIterator preObject(*this);
        operator--();
        return preObject;
    }

    reference operator*() const
    {
        if(whichMap->isEmpty())
            throw std::out_of_range("Attempt to dereference end() iterator in an empty map.");
        if(whichMap->begin() == whichMap->cend())
            throw std::out_of_range("Attempt to dereference end() iterator.");

        return whichMap->buckets[whichBucket][whichElement];
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    bool operator==(const ConstIterator& other) const
    {
        return whichMap == other.whichMap && whichBucket == other.whichBucket && whichElement == other.whichElement;
    }

    bool operator!=(const ConstIterator& other) const
    {
        return !(*this == other);
    }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
    friend HashMap<KeyType, ValueType>;
public:
    using reference = typename HashMap::reference;
    using pointer = typename HashMap::value_type*;
protected:
    Iterator(HashMap<KeyType, ValueType> * whichM, size_type whichB, size_type whichE)
    : ConstIterator(whichM, whichB, whichE)
    {

    }
public:
    explicit Iterator()
    {}

    Iterator(const ConstIterator& other)
        : ConstIterator(other)
    {}

    Iterator& operator++()
    {
        ConstIterator::operator++();
        return *this;
    }

    Iterator operator++(int)
    {
        auto result = *this;
        ConstIterator::operator++();
        return result;
    }

    Iterator& operator--()
    {
        ConstIterator::operator--();
        return *this;
    }

    Iterator operator--(int)
    {
        auto result = *this;
        ConstIterator::operator--();
        return result;
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    reference operator*() const
    {
        // ugly cast, yet reduces code duplication.
        return const_cast<reference>(ConstIterator::operator*());
    }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
