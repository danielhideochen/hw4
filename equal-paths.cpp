#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool equalPathsHelper(Node* root, int depth, int& leafDepth);


bool equalPaths(Node * root)
{
    int leafDepth = -1;          // -1 means "no leaf seen yet"
    return equalPathsHelper(root, 0, leafDepth);
}

bool equalPathsHelper(Node* root, int depth, int& leafDepth)
{
    // Empty subtree: doesn't introduce any leaves, so it's fine
    if (root == nullptr) {
        return true;
    }

    // Leaf node: check / set leafDepth
    if (root->left == nullptr && root->right == nullptr) {
        if (leafDepth == -1) {
            // First leaf: record its depth
            leafDepth = depth;
            return true;
        }
        else {
            // Later leaf: must match the first leaf's depth
            return depth == leafDepth;
        }
    }

    // Internal node: both subtrees (if they exist) must satisfy the condition
    bool leftOK  = equalPathsHelper(root->left,  depth + 1, leafDepth);
    bool rightOK = equalPathsHelper(root->right, depth + 1, leafDepth);

    return leftOK && rightOK;
}
