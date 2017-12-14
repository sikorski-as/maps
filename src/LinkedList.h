#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

template <typename Type>
class LinkedList
{
public:
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    using value_type = Type;
    using pointer = Type*;
    using reference = Type&;
    using const_pointer = const Type*;
    using const_reference = const Type&;

    class ConstIterator;
    class Iterator;
    using iterator = Iterator;
    using const_iterator = ConstIterator;

private:
    class Node
    {
    public:
        Node() : item(value_type{}), next(nullptr), prev(nullptr)
        {

        }
        Node(const value_type &item) :item(item), next(nullptr), prev(nullptr)
        {

        }
        value_type item;
        Node * next;
        Node * prev;

    } * first, * last; // for head and tail (sentinel)
    size_type count;

public:

    LinkedList()
    {
        count = 0;
        Node * newNode = new Node();
        first = newNode;
        last = newNode;
    }

    LinkedList(std::initializer_list<Type> l)
    {
        count = 0;
        Node * newNode = new Node(); // sentinel
        first = newNode;
        last = newNode;
        if(l.size() != 0)
            for(auto p = l.begin(); p != l.end(); ++p)
                append(*p);
    }

    LinkedList(const LinkedList& other)
    {
        count=0;
        Node * newNode = new Node();
        first = newNode;
        last = newNode;
        for(auto i = other.cbegin(); i != other.cend(); ++i)
            append(*i);
    }

    LinkedList(LinkedList&& other)
    {
        first = other.first;
        last = other.last;
        count = other.count;
        other.count = 0;
        other.last = nullptr;
        other.first = nullptr;
    }

    ~LinkedList()
    {
        Node * i = last;
        while(i != first)
        {
            i = i->prev;
            delete i->next;
        }
        delete i;
    }

    LinkedList& operator=(const LinkedList& other)
    {
        if(first == other.first)
            return *this;

        erase(begin(), end());
        for(auto it = other.begin(); it != other.end(); ++it)
            append(*it);

        return *this;
    }

    LinkedList& operator=(LinkedList&& other)
    {
        if(first == other.first)
            return *this;

        erase(begin(),end());
        delete last;
        first = other.first;
        last = other.last;
        count = other.count;
        other.first = nullptr;
        other.last = nullptr;
        other.count = 0;

        return *this;
    }

    bool isEmpty() const
    {
        return count == 0;
    }

    size_type getSize() const
    {
        return count;
    }

    void append(const Type& item)
    {
        Node * ptr = new Node(item);
        if(count == 0)
        {

            first = ptr;
            last->prev = ptr;
            ptr->next = last;

        }
        else
        {
            ptr->prev = last->prev;
            last->prev->next = ptr;
            last->prev = ptr;
            ptr->next = last;
        }
        ++count;
    }

    Type& operator[](size_type index)
    {
        return *(begin() + index);
    }

    void prepend(const Type& item)
    {
        Node * ptr = new Node(item); // node to be added
        if(count == 0)
        {

            first = ptr;
            last->prev = ptr;
            ptr->next = last;

        }
        else
        {
            ptr -> prev = nullptr;
            first -> prev = ptr;
            ptr -> next = first;
            first = ptr;
        }
        ++count;
    }

    void insert(const const_iterator& insertPosition, const Type& item)
    {

        if(isEmpty()) // not sure if it's needed
            append(item);
        else if(insertPosition == end())
            append(item);
        else if(insertPosition == begin())
            prepend(item);
        else
        {
            auto inserted = new Node(item); // node to be added
            insertPosition.getNode() -> prev -> next = inserted;
            inserted -> prev = insertPosition.getNode() -> prev;
            insertPosition.getNode() -> prev = inserted;
            inserted -> next = insertPosition.getNode();
            ++count;
        }
    }

    Type popFirst()
    {
        if(isEmpty())
            throw std::logic_error("Attempt to pop from an empty container");

        Node * ptr = first;
        value_type returned = ptr->item;
        erase(begin());

        return returned;
    }

    Type popLast()
    {
        if(isEmpty())
            throw std::logic_error("Attempt to pop from an empty container");

        Node * ptr = last -> prev;
        value_type returned = ptr -> item;
        erase(--end());

        return returned;
    }


    void erase(const const_iterator& position)
    {
        if(isEmpty() || position == cend())
            throw std::out_of_range("Attempt to erase an item out of scope or the container is empty");

        if(position == cbegin())
        {
            if(getSize() == 1)
            {
                delete first;
                first = last;
            }
            else if(getSize() > 1)
            {
                auto toBeErased = first;
                first = first->next;
                first->prev = nullptr;
                delete toBeErased;
            }
        }
        else if(position == (cend()-1) )

        {
            auto ptr = last->prev;
            last->prev = last->prev->prev;
            last->prev->next = last;
            delete ptr;

        }
        else
        {
            Node * erased = position.getNode();
            erased->prev->next = erased->next;
            erased->next->prev = erased->prev;
            delete erased;
        }
        --count;
    }

    void erase(difference_type whichIndex)
    {
        erase(begin() + whichIndex);
    }


    void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
    {
        auto ptr = firstIncluded;
        while(ptr != lastExcluded)
        {
            auto erased = ptr; // temporary for deletion
            ++ptr;
            erase(erased);
        }
    }


    iterator begin()
    {
        return iterator(first);
    }

    iterator end()
    {
        return iterator(last);

    }

    const_iterator cbegin() const
    {
        return const_iterator(first);
    }

    const_iterator cend() const
    {
        return const_iterator(last);
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

template <typename Type>
class LinkedList<Type>::ConstIterator
{
    friend LinkedList<Type>;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename LinkedList::value_type;
    using difference_type = typename LinkedList::difference_type;
    using pointer = typename LinkedList::const_pointer;
    using reference = typename LinkedList::const_reference;

protected:
    Node* current;
    Node* getNode() const // should it be public or friend declatarion is needed?
    {
        return current;
    }
    ConstIterator(Node* node):current(node)
    {

    }
public:
    explicit ConstIterator()
    {}

    reference operator*() const
    {
        if(current->next == nullptr) // sentinel detected
            throw std::out_of_range("Attempt to dereference the end() iterator");
        return current->item;
    }

    ConstIterator& operator++()
    {
        if(current->next == nullptr) // sentinel detected
            throw std::out_of_range("Attempt to increment the end() itertator");
        current = current->next;
        return *this;
    }

    ConstIterator operator++(int)
    {
        auto result = *this;
        ++(*this);
        return result;
    }

    ConstIterator& operator--()
    {
        if(current->prev == nullptr) // head detected
            throw std::out_of_range("Attempt to decrement the begin() iterator");
        current = current->prev;
        return *this;
    }

    ConstIterator operator--(int)
    {
        auto result = *this;
        --(*this);
        return result;
    }

    ConstIterator operator+(difference_type d) const
    {
        auto it = *this;
        for(difference_type i = 0; i < d; ++i)
        {
            if(it.current->next == nullptr) // sentinel detected
                throw std::range_error("Attempt to move the iterator beyond end()");
            it.current = it.current->next;
        }
        return it;
    }

    ConstIterator operator-(difference_type d) const
    {
        auto it = *this;
        for(difference_type i = 0; i < d; ++i)
        {
            if(it.current->prev == nullptr)
                break; // or exception should be thrown?
            it.current = it.current->prev;
        }

        return it;
    }

    bool operator==(const ConstIterator& other) const
    {
        return current == other.current;
    }

    bool operator!=(const ConstIterator& other) const
    {
        return current!=other.current;
    }
};

template <typename Type>
class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator
{
    friend LinkedList<Type>;
public:
    using pointer = typename LinkedList::pointer;
    using reference = typename LinkedList::reference;

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

    Iterator operator+(difference_type d) const
    {
        return ConstIterator::operator+(d);
    }

    Iterator operator-(difference_type d) const
    {
        return ConstIterator::operator-(d);
    }

    reference operator*() const
    {
        // ugly cast, yet reduces code duplication.
        return const_cast<reference>(ConstIterator::operator*());
    }

};

}

#endif // AISDI_LINEAR_LINKEDLIST_H
