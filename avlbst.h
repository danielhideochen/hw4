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

    // rotations
    void rotateLeft(AVLNode<Key,Value>* x);
    void rotateRight(AVLNode<Key,Value>* x);
    int  height(Node<Key,Value>* node) const;
    int  getBalanceFactor(AVLNode<Key,Value>* node) const;
    void rebalance(AVLNode<Key,Value>* node);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

    const Key& key   = new_item.first;
    const Value& val = new_item.second;

    // Empty tree
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key,Value>(key, val, NULL);
        return;
    }

    // Standard BST insert, but allocate AVLNode
    Node<Key,Value>* curr = this->root_;
    AVLNode<Key,Value>* parent = NULL;
    bool goLeft = false;

    while(curr != NULL) {
        parent = static_cast<AVLNode<Key,Value>*>(curr);
        if(key < curr->getKey()) {
            curr = curr->getLeft();
            goLeft = true;
        }
        else if(key > curr->getKey()) {
            curr = curr->getRight();
            goLeft = false;
        }
        else {
            // key already exists: just update value
            curr->setValue(val);
            return;
        }
    }

    AVLNode<Key,Value>* node = new AVLNode<Key,Value>(key, val, parent);
    if(goLeft) parent->setLeft(node);
    else       parent->setRight(node);

    // Rebalance all ancestors
    AVLNode<Key,Value>* cur = parent;
    while(cur != NULL) {
        rebalance(cur);
        cur = static_cast<AVLNode<Key,Value>*>(cur->getParent());
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    // Find node
    Node<Key,Value>* n = this->internalFind(key);
    if(n == NULL) return;

    AVLNode<Key,Value>* node = static_cast<AVLNode<Key,Value>*>(n);

    // If two children, swap with predecessor first (like BST)
    if(node->getLeft() != NULL && node->getRight() != NULL) {
        Node<Key,Value>* predBase =
            BinarySearchTree<Key,Value>::predecessor(node);
        AVLNode<Key,Value>* pred =
            static_cast<AVLNode<Key,Value>*>(predBase);
        nodeSwap(node, pred);
    }

    // Now node has at most one child
    AVLNode<Key,Value>* parent =
        static_cast<AVLNode<Key,Value>*>(node->getParent());
    AVLNode<Key,Value>* child =
        (node->getLeft() != NULL)
            ? node->getLeft()
            : node->getRight();

    if(child != NULL) {
        child->setParent(parent);
    }

    if(parent == NULL) {
        // removing root
        this->root_ = child;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    delete node;

    // Rebalance while going up to root
    AVLNode<Key,Value>* cur = parent;
    while(cur != NULL) {
        rebalance(cur);
        cur = static_cast<AVLNode<Key,Value>*>(cur->getParent());
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

// ----- Helper: compute height of a subtree -----
template<class Key, class Value>
int AVLTree<Key, Value>::height(Node<Key,Value>* node) const
{
    if(node == NULL) return 0;
    int hl = height(node->getLeft());
    int hr = height(node->getRight());
    return 1 + (hl > hr ? hl : hr);
}

// ----- Helper: compute balance factor = height(left) - height(right) -----
template<class Key, class Value>
int AVLTree<Key, Value>::getBalanceFactor(AVLNode<Key,Value>* node) const
{
    if(node == NULL) return 0;
    return height(node->getLeft()) - height(node->getRight());
}

// ----- Left rotation around x -----
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* x)
{
    if(x == NULL) return;
    AVLNode<Key,Value>* y = x->getRight();
    if(y == NULL) return;

    AVLNode<Key,Value>* p =
        static_cast<AVLNode<Key,Value>*>(x->getParent());
    AVLNode<Key,Value>* beta = y->getLeft();

    // x's right becomes beta
    x->setRight(beta);
    if(beta != NULL) beta->setParent(x);

    // y becomes parent of x
    y->setLeft(x);
    x->setParent(y);

    // hook y into old parent p
    y->setParent(p);
    if(p == NULL) {
        this->root_ = y;
    } else if(p->getLeft() == x) {
        p->setLeft(y);
    } else {
        p->setRight(y);
    }

    // update balances using full subtree heights
    x->setBalance(getBalanceFactor(x));
    y->setBalance(getBalanceFactor(y));
}

// ----- Right rotation around x -----
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* x)
{
    if(x == NULL) return;
    AVLNode<Key,Value>* y = x->getLeft();
    if(y == NULL) return;

    AVLNode<Key,Value>* p =
        static_cast<AVLNode<Key,Value>*>(x->getParent());
    AVLNode<Key,Value>* beta = y->getRight();

    // x's left becomes beta
    x->setLeft(beta);
    if(beta != NULL) beta->setParent(x);

    // y becomes parent of x
    y->setRight(x);
    x->setParent(y);

    // hook y into old parent p
    y->setParent(p);
    if(p == NULL) {
        this->root_ = y;
    } else if(p->getLeft() == x) {
        p->setLeft(y);
    } else {
        p->setRight(y);
    }

    // update balances
    x->setBalance(getBalanceFactor(x));
    y->setBalance(getBalanceFactor(y));
}

// ----- Rebalance a node: recompute balance and rotate if needed -----
template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key,Value>* node)
{
    if(node == NULL) return;

    int bf = getBalanceFactor(node);
    node->setBalance(bf);

    // Left heavy
    if(bf > 1) {
        AVLNode<Key,Value>* L = node->getLeft();
        if(getBalanceFactor(L) < 0) {
            // LR case
            rotateLeft(L);
        }
        // LL case
        rotateRight(node);
    }
    // Right heavy
    else if(bf < -1) {
        AVLNode<Key,Value>* R = node->getRight();
        if(getBalanceFactor(R) > 0) {
            // RL case
            rotateRight(R);
        }
        // RR case
        rotateLeft(node);
    }
    // else |bf| <= 1 : already balanced, nothing more to do
}




#endif
