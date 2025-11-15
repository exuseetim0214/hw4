#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    int getHeight(Node<Key,Value>* n) const;
    void updateBalances(Node<Key,Value>* n);
    void leftRotate(AVLNode<Key,Value>* n);
    void rightRotate(AVLNode<Key,Value>* n);
    void fixUpwards(AVLNode<Key,Value>* n);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == nullptr)
    {
        AVLNode<Key,Value>* r = new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
        r->setBalance(0);
        this->root_ = r;
        return;
    }

    Node<Key,Value>* walk = this->root_;
    Node<Key,Value>* parent = nullptr;

    while(walk != nullptr)
    {
        parent = walk;

        if(new_item.first < walk->getKey())
            walk = walk->getLeft();
        else if(walk->getKey() < new_item.first)
            walk = walk->getRight();
        else
        {
            walk->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key,Value>* avParent = static_cast<AVLNode<Key,Value>*>(parent);
    AVLNode<Key,Value>* newNode =
        new AVLNode<Key,Value>(new_item.first, new_item.second, avParent);

    if(new_item.first < parent->getKey())
        parent->setLeft(newNode);
    else
        parent->setRight(newNode);

    fixUpwards(avParent);
    updateBalances(this->root_);


}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    Node<Key,Value>* target = this->internalFind(key);
    if(target == nullptr)
        return;

    if(target->getLeft() != nullptr && target->getRight() != nullptr)
    {
        Node<Key,Value>* pred = BinarySearchTree<Key,Value>::predecessor(target);
        nodeSwap(
            static_cast<AVLNode<Key,Value>*>(target),
            static_cast<AVLNode<Key,Value>*>(pred)
        );
    }

    Node<Key,Value>* child =
        (target->getLeft() != nullptr ? target->getLeft() : target->getRight());

    AVLNode<Key,Value>* parent =
        static_cast<AVLNode<Key,Value>*>(target->getParent());

    if(child != nullptr)
        child->setParent(parent);

    if(parent == nullptr)
        this->root_ = child;
    else if(target == parent->getLeft())
        parent->setLeft(child);
    else
        parent->setRight(child);

    delete target;

    if(parent != nullptr)
        fixUpwards(parent);
    else if(this->root_ != nullptr)
        fixUpwards(static_cast<AVLNode<Key,Value>*>(this->root_));

    updateBalances(this->root_);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
int AVLTree<Key, Value>::getHeight(Node<Key,Value>* n) const
{
 if(n == nullptr) {
        return 0;
    }

    int leftSide  = getHeight(n->getLeft());
    int rightSide = getHeight(n->getRight());

    if(leftSide > rightSide)
        return leftSide + 1;
    else
        return rightSide + 1;
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalances(Node<Key,Value>* n)
{
    if(n == nullptr)
        return;

    updateBalances(n->getLeft());
    updateBalances(n->getRight());

    int hLeft  = getHeight(n->getLeft());
    int hRight = getHeight(n->getRight());
    int diff   = hRight - hLeft;

    AVLNode<Key,Value>* av = static_cast<AVLNode<Key,Value>*>(n);
    av->setBalance((int8_t)diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::leftRotate(AVLNode<Key,Value>* n)
{
    if(n == nullptr || n->getRight() == nullptr)
        return;

    AVLNode<Key,Value>* child = n->getRight();
    AVLNode<Key,Value>* parent = n->getParent();
    AVLNode<Key,Value>* middle = child->getLeft();

    child->setParent(parent);

    if(parent == nullptr)
        this->root_ = child;
    else if(n == parent->getLeft())
        parent->setLeft(child);
    else
        parent->setRight(child);

    child->setLeft(n);
    n->setParent(child);

    n->setRight(middle);
    if(middle != nullptr)
        middle->setParent(n);
}


template<class Key, class Value>
void AVLTree<Key, Value>::rightRotate(AVLNode<Key,Value>* n)
{
    if(n == nullptr || n->getLeft() == nullptr)
        return;

    AVLNode<Key,Value>* child = n->getLeft();
    AVLNode<Key,Value>* parent = n->getParent();
    AVLNode<Key,Value>* middle = child->getRight();

    child->setParent(parent);

    if(parent == nullptr)
        this->root_ = child;
    else if(n == parent->getLeft())
        parent->setLeft(child);
    else
        parent->setRight(child);

    child->setRight(n);
    n->setParent(child);

    n->setLeft(middle);
    if(middle != nullptr)
        middle->setParent(n);
}


template<class Key, class Value>
void AVLTree<Key,Value>::fixUpwards(AVLNode<Key,Value>* n)
{
    AVLNode<Key,Value>* cur = n;

    while(cur != nullptr)
    {
        int leftH  = getHeight(cur->getLeft());
        int rightH = getHeight(cur->getRight());
        int balance = rightH - leftH;

        
        if(balance < -1)
        {
            AVLNode<Key,Value>* L = cur->getLeft();
            int LL = getHeight(L->getLeft());
            int LR = getHeight(L->getRight());

            if(LL >= LR)
            {
                rightRotate(cur);
            }
            else
            {
                leftRotate(L);
                rightRotate(cur);
            }
        }
      
        else if(balance > 1)
        {
            AVLNode<Key,Value>* R = cur->getRight();
            int RR = getHeight(R->getRight());
            int RL = getHeight(R->getLeft());

            if(RR >= RL)
            {
                leftRotate(cur);
            }
            else
            {
                rightRotate(R);
                leftRotate(cur);
            }
        }

        cur = cur->getParent();
    }
}



#endif
