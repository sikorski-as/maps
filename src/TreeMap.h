#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
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
protected:
    struct Node
    {
        Node * left;
        Node * right;
        Node * parent;
        value_type data;
        Node() {}
        Node(const key_type& key)
        : left(nullptr), right(nullptr), data( std::make_pair(key, mapped_type {}) )
        {

        }
    }
    * head; // sentinel
    size_type size; // number of elements in the tree

    void initTree()
    {
        head = new Node();
        head->left = head;          // required to detect empty list
        head->right = head;         // used for detecting illegal --begin() with empty collection
        head->parent = nullptr;     // because sentinel has no parent
        size = 0;
    }

    const_iterator search(Node *startNode, const key_type& key) const // searches if key is found in the given tree
    {
        while(startNode != nullptr)
        {
            if(key == startNode->data.first)
                return const_iterator(startNode);
            if(key < startNode->data.first)
                startNode = startNode->left;
            else
                startNode = startNode->right;
        }
        return cend(); // if not, end() iterator is returned
    }

    Node* getMinimalSubtreeNode(Node *node) // search for the smallest element in the left subtree
    {
        while(node->left != nullptr)
            node = node->left;
        return node;
    }

    void moveTree(Node *node1, Node *node2)
    {
        if(node1->parent == nullptr) // case of moving the root of the tree
            head->left = node2;
        else if (node1 == node1->parent->left)  // node1 is the root of the left subtree
            node1->parent->left = node2;
        else                                    // node2 is the root of the right subtree
            node1->parent->right = node2;

        if(node2 != nullptr) //
            node2->parent = node1->parent;
    }

    void deallocTree(Node *node) // deallocs subtree, does not set parent to null!
    {
        if(node->left != nullptr)
            deallocTree(node->left);
        if(node->right != nullptr)
            deallocTree(node->right);
        delete (node);
    }

    void deallocTree() // remove whole tree, sentinel gets removed, too
    {
        if(!isEmpty())
            deallocTree(head->left);
        delete head;
    }

    void swap(TreeMap& first, TreeMap& second) // to be removed
    {
        std::swap(first.head, second.head);
        std::swap(first.size, second.size);
    }

public:
    TreeMap()
    {
        initTree();
    }

    TreeMap(std::initializer_list<value_type> list)
    {
        initTree();
        for(auto element : list)
            operator[](element.first) = element.second;
    }

    TreeMap(const TreeMap& other)
    {
        initTree();
        for(auto element : other)
        {
            operator[](element.first) = element.second;
        }
    }

    TreeMap(TreeMap&& other) : head(other.head), size(other.size)
    {
        other.head = nullptr; // make useless
        other.size = 0;
    }

    TreeMap& operator=(const TreeMap & other)
    {
        if(&other == this) // there is no sense of copying this object
            return *this;

        deallocTree(); // remove current nodes
        initTree();

        for(auto element : other) // copy
        {
            operator[](element.first) = element.second;
        }
        return *this;
    }

    TreeMap& operator=(TreeMap&& other)
    {
        deallocTree(); // remove current nodes

        head = other.head; // copy
        size = other.size;

        other.head = nullptr; // make useless
        other.size = 0;

        return *this;
    }

    ~TreeMap()
    {
        deallocTree();
    }

    bool isEmpty() const
    {
        return size == 0;
    }

    mapped_type& operator[](const key_type& key)
    {
        if(isEmpty())
        {
            Node *newNode = new Node(key);
            newNode->parent = head;
            head->left = newNode; // list is no longer empty
            head->right = nullptr; // important for check against decrementing begin() in empty map
            size++;
            return newNode->data.second; // return reference to the created element
        }

        Node * next = head->left;
        Node * current = nullptr;
        while(next != nullptr)
        {
            current = next;
            if(key == current->data.first)   //node with this key already exists
            {
                return current->data.second;
            }

            if(key < current->data.first)   // if value less than the given key, go to the left subtree
                next = current->left;
            else                            // else: go to the right subtree
                next = current->right;
        }

        Node * newNode = new Node(key);
        newNode->parent = current;          // current node is going to be the parent of the newly created node
        if(key < current->data.first)
            current->left = newNode;
        else
            current->right = newNode;
        size++;
        return newNode->data.second;
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        if(isEmpty())
            throw std::out_of_range("Attempt to access an element in an empty map.");

        const_iterator position = find(key);
        if(position == cend())
            throw std::out_of_range("Attempt to access an element that is not in the map.");

        return position->second;
    }

    mapped_type& valueOf(const key_type& key)
    {
        if(isEmpty())
            throw std::out_of_range("Attempt to access an element in an empty map.");

        iterator position = find(key);
        if(position == end())
            throw std::out_of_range("Attempt to access an element that is not in the map.");

        return position->second;
    }

    const_iterator find(const key_type& key) const
    {
        if(isEmpty())
            return cend();
        return search(head->left, key);
    }

    iterator find(const key_type& key)
    {
        if(isEmpty())
            return end();
        return search(head->left, key);
    }

    void remove(const key_type& key)
    {
        if(isEmpty())
            throw std::out_of_range("Attempt to remove an element from an empty map.");

        auto nodeBeingRemoved = find(key).currentNode; // which node iterator points to
        if(nodeBeingRemoved == head)
            throw std::out_of_range("Attempt to remove an element that is not in the map.");

        if(nodeBeingRemoved->left == nullptr)                       // only one child - right
            moveTree(nodeBeingRemoved, nodeBeingRemoved->right);
        else if(nodeBeingRemoved->right == nullptr)                 // only one child - left
            moveTree(nodeBeingRemoved, nodeBeingRemoved->left);
        else
        {
            Node *tmp = getMinimalSubtreeNode(nodeBeingRemoved->right); // find successor - the smallest element in the right subtree
            if(tmp->parent != nodeBeingRemoved)
            {
                moveTree(tmp, tmp->right);
                tmp->right = nodeBeingRemoved->right;
                tmp->right->parent = tmp;
            }
            moveTree(nodeBeingRemoved, tmp);
            tmp->left = nodeBeingRemoved->left;
            tmp->left->parent = tmp;
        }

        delete nodeBeingRemoved;
        size--;

        if(isEmpty())   // setup the sentinel
        {
            head->right = head;
            head->left = head;
        }
    }

    void remove(const const_iterator& it)
    {
        if(it == end())
            throw std::out_of_range("Attempt to remove an element with end() iterator.");

        remove(it->first); // remove node by key
    }

    size_type getSize() const
    {
        return size;
    }

    bool operator==(const TreeMap& other) const
    {
        if(size != other.size)
            return false;

        auto otherIt = other.cbegin();
        auto ownIt = cbegin();
        for(size_type i = 0; i < size; i++)
        {
            if(!(otherIt->first == ownIt->first && otherIt->second == ownIt->second))
                return false;
            otherIt++;
            ownIt++;
        }

        return true;
    }

    bool operator!=(const TreeMap& other) const
    {
        return !operator==(other);
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
        if(head == head->left)
            return ConstIterator(head); // the map is empty

        Node * leftmostNode = head;
        while(leftmostNode->left != nullptr) // lowest in the left subtree is the first element
            leftmostNode = leftmostNode->left;

        return ConstIterator(leftmostNode);
    }

    const_iterator cend() const
    {
        return ConstIterator(head);
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
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename TreeMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename TreeMap::value_type;
    using pointer = const typename TreeMap::value_type*;

    Node *currentNode;

    explicit ConstIterator()
    {

    }

    explicit ConstIterator(Node *startNode) : currentNode(startNode)
    {

    }

    ConstIterator(const ConstIterator& other) : currentNode(other.currentNode)
    {

    }

    ConstIterator& operator++()
    {
        if(currentNode->parent == nullptr)
            throw std::out_of_range("Attempt to increment end() iterator.");

        if(currentNode->right != nullptr) // if there is a right subtree - find the leftmost element in there
        {
            currentNode = currentNode->right;
            while(currentNode->left != nullptr)
                currentNode = currentNode->left;
            return *this;
        }

        Node *tmp = currentNode->parent;
        while(tmp != nullptr && currentNode == tmp->right)      // there's no right subtree - find in the parent tree
        {                                                       // if we come from the left subtree, tmp is our successor
            currentNode = tmp;
            tmp = tmp->parent;
        }

        currentNode = tmp;
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
        if(currentNode->right == currentNode) // the sentinel is used here
            throw std::out_of_range("Attempt to decrement begin() iterator in an empty map.");

        if(currentNode->left != nullptr) // left subtree is not empty - go there
        {
            currentNode = currentNode->left;
            while(currentNode->right != nullptr) // find the biggest element in this subtree
                currentNode = currentNode->right;
            return *this;
        }

        Node *tmp = currentNode->parent;
        while(tmp != nullptr && currentNode == tmp->left)   // left subtree is empty, go to the parent tree
        {                                                   // if we come from the right subtree, our parent is the predecessor
            if(tmp->parent == nullptr)
                throw std::out_of_range("Attempt to decrement begin() iterator.");

            currentNode = tmp;
            tmp = tmp->parent;
        }

        currentNode = tmp;
        return *this;
    }

    ConstIterator operator--(int)
    {
        ConstIterator preObject(*this);
        operator--();
        return preObject;
    }

    reference operator*() const
    {
        if(currentNode->right == currentNode) // tree is empty, sentinel is used here
            throw std::out_of_range("Attempt to dereference in an empty tree.");
        return currentNode->data;
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    bool operator==(const ConstIterator& other) const
    {
        return this->currentNode == other.currentNode;
    }

    bool operator!=(const ConstIterator& other) const
    {
        return !(*this == other);
    }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename TreeMap::reference;
    using pointer = typename TreeMap::value_type*;

    explicit Iterator()
    {

    }

    Iterator(const ConstIterator& other)
        : ConstIterator(other)
    {

    }

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

#endif /* AISDI_MAPS_MAP_H */
