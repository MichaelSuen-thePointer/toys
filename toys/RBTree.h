#pragma once

#include <cassert>

template<class T>
class RBTree
{
private:
    enum Color : char
    {
        Black, Red
    };

    struct TreeNode
    {
        T value;
        Color color;
        TreeNode* parent;
        TreeNode* left;
        TreeNode* right;
    };

    static TreeNode* nil = new TreeNode();

    TreeNode* root;

    static TreeNode*& parent(TreeNode*& node)
    {
        return node->parent;
    }
    static TreeNode*& left(TreeNode*& node)
    {
        return node->left;
    }
    static TreeNode*& right(TreeNode*& node)
    {
        return node->right;
    }

    void left_rotate(TreeNode* node)
    {
        TreeNode* rightSon = node->right;

        //node->right->left 和 node 之间指向
        node->right = rightSon->left;
        rightSon->right->parent = node; //nil 的 parent 随便指谁都没关系

        //node->right 和 node->parent 之间指向
        rightSon->parent = node->parent;
        if (node->parent == nil)
        {
            root = rightSon;
        }
        else if (node == node->parent->left)
        {
            node->parent->left = rightSon;
        }
        else
        {
            node->parent->right = rightSon;
        }

        //node->right 和 node 之间 指向
        rightSon->left = node;
        node->parent = rightSon;
    }

    void right_rotate(TreeNode* node)
    {
        TreeNode* leftNode = node->left;

        node->left = leftNode->right;
        leftNode->right->parent = node;

        leftNode->parent = node->parent;
        if (node->parent == nil)
        {
            root = leftNode;
        }
        else if (node->parent->left == node)
        {
            node->parent->left = leftNode;
        }
        else
        {
            node->parent->right = leftNode;
        }

        leftNode->right = node;
        node->parent = leftNode;
    }

    void insert(TreeNode* node)
    {
        TreeNode* parent = nil;
        TreeNode* current = root;
        while (current != nil)
        {
            parent = current;
            if (node->value < current->value)
            {
                current = current->left;
            }
            else
            {
                current = current->right;
            }
        }
        node->parent = parent;
        if (parent == nil)
        {
            root = node;
        }
        else if (node->value < parent->value)
        {
            parent->left = node;
        }
        else
        {
            parent->right = node;
        }

        node->left = nil;
        node->right = nil;
        node->color = Red;

        insert_fix_up(node);
    }

    void insert_fix_up(TreeNode* node)
    {
        while (node->parent->color == Red)
        {
            if (node->parent == node->parent->parent->left)
            {
                TreeNode* uncle = node->parent->parent->right;
                if (uncle->color = Red)
                {
                    node->parent->color = Black;
                    node->color = Black;
                    node->parent->parent->color = Red;
                    node = node->parent->parent;
                }
                else
                {
                    if (node->parent->right == node)
                    {
                        node = node->parent;
                        left_rotate(node);
                    }
                    node->parent->color = Black;
                    node->parent->parent->color = Red;
                    right_rotate(node->parent->parent);
                }
            }
            else
            {
                TreeNode* uncle = node->parent->parent->left;
                if (uncle->color = Red)
                {
                    node->parent->color = Black;
                    node->color = Black;
                    node->parent->parent->color = Red;
                    node = node->parent->parent;
                }
                else
                {
                    if (node->parent->left == node)
                    {
                        node = node->parent;
                        right_rotate(node);
                    }
                    node->parent->color = Black;
                    node->parent->parent->color = Red;
                    left_rotate(node->parent->parent);
                }
            }
        }
        root->color = Black;
    }

    static TreeNode* min(TreeNode* node)
    {
        while (node->left != nil)
        {
            node = node->left;
        }
        return node;
    }

    TreeNode* successor(TreeNode* node)
    {
        if (node->right != nil)
        {
            return min(node->right);
        }
        TreeNode* parent = node->parent;
        while (parent != nil && node == parent->right)
        {
            node = parent;
            parent = parent->parent;
        }
        return parent;
    }

    void transplant(TreeNode* u, TreeNode* v)
    {
        if (u->parent == nil)
        {
            root = v;
        }
        else if (u == u->parent->left)
        {
            u->parent->left = v;
        }
        else
        {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    TreeNode* remove(TreeNode* node)
    {
        TreeNode* succ = node;
        Color succOrigColor = node->color;

        TreeNode* son;
        if (node->left == nil)
        {
            son = node->right;
            transplant(node, node->right);
        }
        else if (node->right == nil)
        {
            son = node->left;
            transplant(node, node->left);
        }
        else
        {
            succ = min(node->right);
            succOrigColor = succ.Color;
            son = succ->right;
            if (succ->parent != node)
            {
                transplant(succ, succ->right);
                succ->right = node->right;
                succ->right->parent = succ;
            }
            transplant(node, succ);
            succ->left = node->left;
            succ->left->parent = succ;
            succ->color = node->color;
        }
        if (succOrigColor == Black)
        {
            remove_fix_up(son);
        }
        return node;
    }

    void remove_fix_up(TreeNode* node)
    {
        while (node != root && node->color == Black)
        {
            if (node == node->parent->left)
            {
                TreeNode* sibling = node->parent->right;
                if (sibling->color == Red)
                {
                    sibling->color = Black;
                    node->parent->color = Red;
                    left_rotate(node->parent);
                    sibling = node->parent->right;
                }
                if (sibling->left->color == Black && node->right->color == Black)
                {
                    sibling->color = Red;
                    node = node->parent;
                }
                else
                {
                    if (sibling->right->color == Black)
                    {
                        sibling->left->color = Black;
                        sibling->color = Red;
                        right_rotate(sibling);
                        sibling = node->parent->right;
                    }
                    sibling->color = node->parent->color;
                    node->parent->color = Black;
                    sibling->right->color = Black;
                    left_rotate(node->parent);
                    node = root;
                }
            }
            else
            {
                TreeNode* sibling = node->parent->left;
                if (sibling->color == Red)
                {
                    sibling->color = Black;
                    node->parent->color = Red;
                    right_rotate(node->parent);
                    sibling = node->parent->left;
                }
                if (sibling->right->color == Black && node->left->color == Black)
                {
                    sibling->color = Red;
                    node = node->parent;
                }
                else
                {
                    if (sibling->left->color == Black)
                    {
                        sibling->right->color = Black;
                        sibling->color = Red;
                        left_rotate(sibling);
                        sibling = node->parent->left;
                    }
                    sibling->color = node->parent->color;
                    node->parent->color = Black;
                    sibling->left->color = Black;
                    right_rotate(node->parent);
                    node = root;
                }
            }
        }
        node->color = Black;
    }
public:

};