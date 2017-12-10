#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>
#include <list>
#include <functional>

namespace aisdi
{
using std::vector;
using std::list;

template <typename KeyType, typename ValueType>
class HashMap
{
public:
    using key_type = KeyType;
    using mapped_type = ValueType;

    using value_type = std::pair<key_type, mapped_type>; // we need a copy-assignable objects - `const` qualifier removed
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Bucket = list<value_type>;

    class ConstIterator;
    class Iterator;
    using iterator = Iterator;
    using const_iterator = ConstIterator;
protected:
    vector< Bucket > buckets;
    size_type count;

    size_type getHash(const key_type& key) const
    {
        return std::hash<key_type>()(key) % amountOfBuckets();
    }
    size_type amountOfBuckets() const
    {
        return 2048;
    }
    void initBuckets()
    {
        buckets.resize(amountOfBuckets());
    }
public:
    HashMap()
    {
        count = 0;
        initBuckets();
    }

    HashMap(std::initializer_list<value_type> initList)
    {
        count = initList.size();
        initBuckets();
        for(value_type element: initList)
        {
            buckets[getHash(element.first)].push_back(element);
        }
    }

    HashMap(const HashMap& other)
    : buckets(other.buckets.begin(), other.buckets.end()), count(other.count)
    {

    }

    HashMap(HashMap&& other)
    : buckets(std::move(other.buckets)), count(other.count)
    {

    }

    HashMap& operator=(const HashMap& other)
    {
        if(*this != other)
        {
            count = other.count;
            buckets = other.buckets;
        }
        return *this;
    }

    HashMap& operator=(HashMap&& other)
    {
        if(*this != other)
        {
            count = other.count;
            buckets = std::move(other.buckets);
        }
        return *this;
    }

    bool isEmpty() const
    {
        return count == 0;
    }

    mapped_type& operator[](const key_type& key)
    {
        auto position = find(key);
        if(position == end())
        {
            size_type hash = getHash(key);
            buckets[hash].push_back(std::make_pair(key, mapped_type{}));
            count++;
            return (--buckets[hash].end())->second;
        }
        else
            return position->second;
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        auto position = find(key);
        if(position == end())
            throw std::out_of_range("Item with such key not found");
        return position->second;
    }

    mapped_type& valueOf(const key_type& key)
    {
        auto position = find(key);
        if(position == end())
            throw std::out_of_range("Item with such key not found");
        return position->second;
    }

    const_iterator find(const key_type& key) const
    {
        auto bucketIterator = buckets.begin() + getHash(key);
        for(auto inBucketIterator = bucketIterator->begin(); inBucketIterator != bucketIterator->end(); ++inBucketIterator)
            if(inBucketIterator->first == key)
                return ConstIterator(bucketIterator, inBucketIterator);
        return cend();
    }

    iterator find(const key_type& key)
    {

        auto bucketIterator = buckets.begin() + getHash(key);
        for(auto inBucketIterator = bucketIterator->begin(); inBucketIterator != bucketIterator->end(); ++inBucketIterator)
            if(inBucketIterator->first == key)
                return Iterator(bucketIterator, inBucketIterator);
        return end();
    }

    void remove(const key_type& key)
    {
        (void)key;
        throw std::runtime_error("TODO");
        count--;
    }

    void remove(const const_iterator& it)
    {
        (void)it;
        throw std::runtime_error("TODO");
        count--;
    }

    size_type getSize() const
    {
        return count;
    }

    bool operator==(const HashMap& other) const
    {

        return *this == other ? true : count == other.count && buckets == other.buckets;
    }

    bool operator!=(const HashMap& other) const
    {
        return !(*this == other);
    }

    iterator begin()
    {
        auto whichBucket = buckets.begin();
        return Iterator(whichBucket, whichBucket->begin());
    }

    iterator end()
    {
        auto whichBucket = buckets.end();
        return Iterator(whichBucket, whichBucket->end());
    }

    const_iterator cbegin() const
    {
        auto whichBucket = buckets.begin();
        return ConstIterator(whichBucket, whichBucket->begin());
    }

    const_iterator cend() const
    {
        auto whichBucket = buckets.end();
        return ConstIterator(whichBucket, whichBucket->end());
    }

    const_iterator begin() const
    {
        auto whichBucket = buckets.begin();
        return ConstIterator(whichBucket, whichBucket->begin());
    }

    const_iterator end() const
    {
        auto whichBucket = buckets.end();
        return ConstIterator(whichBucket, whichBucket->end());
    }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
    friend HashMap<KeyType, ValueType>;
    using reference = typename HashMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename HashMap::value_type;
    using pointer = const typename HashMap::value_type*;

    using VectorOfBuckets = typename std::vector< std::list<value_type> >;
    using VectorOfBucketsIterator = typename std::vector< std::list<value_type> >::const_iterator;
    using Bucket = typename HashMap::Bucket;
    using BucketIterator = typename HashMap::Bucket::const_iterator;
protected:
    VectorOfBucketsIterator whichBucket;
    BucketIterator whichElement;


    ConstIterator(VectorOfBucketsIterator whichB, BucketIterator whichE)
    : whichBucket(whichB), whichElement(whichE)
    {

    }
public:
    explicit ConstIterator()
    {}

    ConstIterator(const ConstIterator& other)
    {
        (void)other;
        throw std::runtime_error("TODO");
    }

    ConstIterator& operator++()
    {
        throw std::runtime_error("TODO");
    }

    ConstIterator operator++(int)
    {
        throw std::runtime_error("TODO");
    }

    ConstIterator& operator--()
    {
        throw std::runtime_error("TODO");
    }

    ConstIterator operator--(int)
    {
        throw std::runtime_error("TODO");
    }

    reference operator*() const
    {
        throw std::runtime_error("TODO");
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    bool operator==(const ConstIterator& other) const
    {
        (void)other;
        throw std::runtime_error("TODO");
    }

    bool operator!=(const ConstIterator& other) const
    {
        return !(*this == other);
    }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
    friend HashMap<KeyType, ValueType>;
    using reference = typename HashMap::reference;
    using pointer = typename HashMap::value_type*;

    using VectorOfBuckets = typename std::vector< std::list<value_type> >;
    using VectorOfBucketsIterator = typename std::vector< std::list<value_type> >::iterator;
    using Bucket = typename HashMap::Bucket;
    using BucketIterator = typename HashMap::Bucket::iterator;
protected:
    VectorOfBucketsIterator whichBucket;
    BucketIterator whichElement;

    Iterator(VectorOfBucketsIterator whichB, BucketIterator whichE)
    : whichBucket(whichB), whichElement(whichE)
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
