#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

int height(Node* root)
{
    if(root == NULL){
        return 1;
    }

    //leaf
    if(root->left == NULL && root ->right == NULL){
        return 1;
    }

    int left_height = height(root -> left);
    int right_height = height(root -> right);

    if(left_height == -1 || right_height == -1){
        return -1;
    }

    if(root->left != NULL && root->right != NULL){
        if(left_height != right_height){
            return -1;
        }
        return left_height + 1;
    }
    if(root-> left != NULL){
        return left_height +1;
    }
    else{
        return right_height +1;
    }

}
bool equalPaths(Node * root)
{
    // Add your code below
    return height(root) != -1;
}

