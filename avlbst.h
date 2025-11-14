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

    // fix-up for insert and remove
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);
    void removeFix(AVLNode<Key,Value>* node, int8_t diff);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

    const Key& key = new_item.first;
    const Value& value = new_item.second;

    // Empty tree
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key,Value>(key, value, NULL);
        return;
    }

    // Find insertion point (or existing key)
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
            // key already in tree: overwrite value, no rotation
            curr->setValue(value);
            return;
        }
    }

    // Create new AVLNode under parent
    AVLNode<Key,Value>* node = new AVLNode<Key,Value>(key, value, parent);
    if(goLeft) parent->setLeft(node);
    else       parent->setRight(node);

    // Fix balances and rotate if needed
    insertFix(parent, node);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    Node<Key,Value>* n = this->internalFind(key);
    if(n == NULL) return;

    AVLNode<Key,Value>* node = static_cast<AVLNode<Key,Value>*>(n);

    // If two children, swap with predecessor, like in BST
    if(node->getLeft() != NULL && node->getRight() != NULL) {
        Node<Key,Value>* predBase =
            BinarySearchTree<Key,Value>::predecessor(node);
        AVLNode<Key,Value>* pred =
            static_cast<AVLNode<Key,Value>*>(predBase);
        nodeSwap(node, pred);
    }

    AVLNode<Key,Value>* parent = node->getParent();
    AVLNode<Key,Value>* child =
        (node->getLeft() != NULL)
            ? static_cast<AVLNode<Key,Value>*>(node->getLeft())
            : static_cast<AVLNode<Key,Value>*>(node->getRight());

    int8_t diff = 0;

    if(parent != NULL) {
        if(parent->getLeft() == node) {
            // left subtree got shorter: balance = L-R, so L-- ⇒ diff = -1
            diff = -1;
            parent->setLeft(child);
        } else {
            // right subtree got shorter: R-- ⇒ balance increases ⇒ diff = +1
            diff = +1;
            parent->setRight(child);
        }
    } else {
        // removing root
        this->root_ = child;
    }

    if(child != NULL) child->setParent(parent);

    delete node;

    if(parent != NULL) {
        removeFix(parent, diff);  
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

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* x)
{
    if(x == NULL) return;
    AVLNode<Key,Value>* y = x->getRight();
    if(y == NULL) return;

    AVLNode<Key,Value>* p = x->getParent();
    AVLNode<Key,Value>* beta = y->getLeft();

    // move beta to be x->right
    x->setRight(beta);
    if(beta != NULL) beta->setParent(x);

    // y becomes parent of x
    y->setLeft(x);
    x->setParent(y);

    // hook y into former parent p
    y->setParent(p);
    if(p == NULL) {
        this->root_ = y;
    } else if(p->getLeft() == x) {
        p->setLeft(y);
    } else {
        p->setRight(y);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* x)
{
    if(x == NULL) return;
    AVLNode<Key,Value>* y = x->getLeft();
    if(y == NULL) return;

    AVLNode<Key,Value>* p = x->getParent();
    AVLNode<Key,Value>* beta = y->getRight();

    // move beta to be x->left
    x->setLeft(beta);
    if(beta != NULL) beta->setParent(x);

    // y becomes parent of x
    y->setRight(x);
    x->setParent(y);

    // hook y into former parent p
    y->setParent(p);
    if(p == NULL) {
        this->root_ = y;
    } else if(p->getLeft() == x) {
        p->setLeft(y);
    } else {
        p->setRight(y);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent,
                                    AVLNode<Key,Value>* node)
{
    if(parent == NULL || node == NULL) return;

    // Go up until we either fix or find first unbalanced node
    while(parent != NULL)
    {
        // Update parent balance based on which side grew
        if(node == parent->getLeft()) {
            parent->updateBalance(1);
        } else {
            parent->updateBalance(-1);
        }

        int8_t b = parent->getBalance();

        if(b == 0) {
            // Height of subtree rooted at parent didn't change; stop
            break;
        }
        else if(b == 1 || b == -1) {
            // Height increased, but still balanced -> continue up
            node = parent;
            parent = static_cast<AVLNode<Key,Value>*>(parent->getParent());
            continue;
        }
        else if(b == 2) {
            // Left-heavy: need LL or LR rotation
            AVLNode<Key,Value>* left = parent->getLeft();

            if(node == left->getLeft()) {
                // LL case
                rotateRight(parent);
                parent->setBalance(0);
                left->setBalance(0);
            }
            else {
                // LR case: node == left->getRight()
                AVLNode<Key,Value>* child = node;
                rotateLeft(left);
                rotateRight(parent);

                int8_t cb = child->getBalance();
                if(cb == 1) {
                    parent->setBalance(-1);
                    left->setBalance(0);
                } else if(cb == 0) {
                    parent->setBalance(0);
                    left->setBalance(0);
                } else { // cb == -1
                    parent->setBalance(0);
                    left->setBalance(1);
                }
                child->setBalance(0);
            }
            break; // After first rotation, done for insert
        }
        else if(b == -2) {
            // Right-heavy: mirror of above
            AVLNode<Key,Value>* right = parent->getRight();

            if(node == right->getRight()) {
                // RR case
                rotateLeft(parent);
                parent->setBalance(0);
                right->setBalance(0);
            }
            else {
                // RL case: node == right->getLeft()
                AVLNode<Key,Value>* child = node;
                rotateRight(right);
                rotateLeft(parent);

                int8_t cb = child->getBalance();
                if(cb == -1) {
                    parent->setBalance(1);
                    right->setBalance(0);
                } else if(cb == 0) {
                    parent->setBalance(0);
                    right->setBalance(0);
                } else { // cb == 1
                    parent->setBalance(0);
                    right->setBalance(-1);
                }
                child->setBalance(0);
            }
            break; // fixed first unbalanced node
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int8_t diff)
{
    if(n == NULL) return;

    AVLNode<Key,Value>* p =
        static_cast<AVLNode<Key,Value>*>(n->getParent());

    // ndiff: +1 if n is left child of p, -1 otherwise
    int8_t ndiff = 0;
    if(p != NULL) {
        if(n == p->getLeft()) ndiff = 1;
        else                  ndiff = -1;
    }

    int8_t b = n->getBalance();

    // ----- Case 1: balance(n) + diff == -2 (left heavy) -----
    if(b + diff == -2) {
        AVLNode<Key,Value>* c = n->getLeft();  // taller child
        int8_t cb = c->getBalance();

        // zig-zig or straight case (c balance -1 or 0)
        if(cb == -1 || cb == 0) {
            // rotate right around n
            rotateRight(n);

            if(cb == -1) {
                // subtree height shrank, keep fixing above
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else { // cb == 0
                // height unchanged after rotation, we are done
                n->setBalance(-1);
                c->setBalance(1);
            }
        }
        // zig-zag case: cb == +1
        else { // cb == 1
            AVLNode<Key,Value>* g = c->getRight();
            int8_t gb = g->getBalance();

            rotateLeft(c);
            rotateRight(n);

            if(gb == 1) {
                n->setBalance(0);
                c->setBalance(-1);
                g->setBalance(0);
            }
            else if(gb == 0) {
                n->setBalance(0);
                c->setBalance(0);
                g->setBalance(0);
            }
            else { // gb == -1
                n->setBalance(1);
                c->setBalance(0);
                g->setBalance(0);
            }

            AVLNode<Key,Value>* pp = NULL;
            if(p != NULL)
                pp = static_cast<AVLNode<Key,Value>*>(p->getParent());
            removeFix(pp, ndiff);
        }
    }

    // ----- Case 2: balance(n) + diff == +2 (right heavy – mirror) -----
    else if(b + diff == 2) {
        AVLNode<Key,Value>* c = n->getRight();  // taller child
        int8_t cb = c->getBalance();

        // zig-zig / straight (cb == 1 or 0)
        if(cb == 1 || cb == 0) {
            rotateLeft(n);

            if(cb == 1) {
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else { // cb == 0
                n->setBalance(1);
                c->setBalance(-1);
            }
        }
        // zig-zag (cb == -1)
        else { // cb == -1
            AVLNode<Key,Value>* g = c->getLeft();
            int8_t gb = g->getBalance();

            rotateRight(c);
            rotateLeft(n);

            if(gb == -1) {
                n->setBalance(0);
                c->setBalance(1);
                g->setBalance(0);
            }
            else if(gb == 0) {
                n->setBalance(0);
                c->setBalance(0);
                g->setBalance(0);
            }
            else { // gb == +1
                n->setBalance(-1);
                c->setBalance(0);
                g->setBalance(0);
            }

            AVLNode<Key,Value>* pp = NULL;
            if(p != NULL)
                pp = static_cast<AVLNode<Key,Value>*>(p->getParent());
            removeFix(pp, ndiff);
        }
    }

    // ----- Case 3: no rotation needed -----
    else {
        int8_t nb = b + diff;

        if(nb == 0) {
            // height of this subtree decreased: keep going up
            n->setBalance(0);
            removeFix(p, ndiff);
        }
        else {
            // nb is -1 or +1: still balanced, but height unchanged → stop
            n->setBalance(nb);
        }
    }
}


#endif
