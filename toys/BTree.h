#pragma once

#include <type_traits>

template<class T, size_t MinDegree>
class BTree
{
private:
    const static size_t MaxKeyCount = 2 * MinDegree - 1;
    const static size_t MaxChildCount = 2 * MinDegree;
    struct TreeNode
    {
        size_t count;
        bool is_leaf;
        T keys[MaxKeyCount];
        TreeNode* children[MaxChildCount];
    };

    struct Iterator
    {
        TreeNode* node;
        size_t index;
    };

    static Iterator find(TreeNode* node, const T& key)
    {
        size_t i = 0;
        while (i < node->count && key > node->keys[i])
        {
            i++;
        }
        if (i < node->count && key == node->keys[i])
        {
            return Iterator{ node, i };
        }
        if (node->is_leaf)
        {
            return Iterator{ nullptr, 0 };
        }
        else
        {
            return find(node->children[i], key);
        }
    }

    static TreeNode* alloc_node(bool is_leaf = false)
    {
        TreeNode* node = new TreeNode;
        node->is_leaf = is_leaf;
        node->count = 0;
        return node;
    }

    void range_move(T* to, T* from, size_t count)
    {
        for (size_t i = 0; i != count; i++)
        {
            to[i] = std::move(from[i]);
        }
    }

    template<class U, size_t count>
    void range_insert(U(&head)[count], size_t place, U&& value)
    {
        for (size_t i = count - 1; i > place + 1; --i)
        {
            head[i] = head[i - 1]
        }
        head[place] = std::forward<U>(value);
    }

    static split_child(TreeNode* node, size_t index)
    {
        TreeNode* child = node->children[index];
        TreeNode* splited = alloc_node(child->is_leaf);

        splited->count = MinDegree - 1;

        range_move(splited->keys, child->keys + MinDegree, MinDegree - 1);

        if (!child->is_leaf)
        {
            range_move(splited->children, child->children + MinDegree, Mindegree);
        }

        children->count = MinDegree - 1;

        range_insert(node->children, index + 1, splited);

        range_insert(node->keys, index, children->keys[MaxDegree - 1]);

        node->count++;
    }

    Iterator insert(T&& value)
    {
        TreeNode* node = _root;
        if (node->count == MaxKeyCount)
        {
            TreeNode* splited = alloc_node(false);
            _root = splited;
            splited->children[0] = node;
            split_child(splited, 0);
            return insert_nonfull(splited, std::forward<T>(value));
        }
        else
        {
            return insert_nonfull(node, std::forward<T>(value));
        }
    }

    Iterator insert_nonfull(TreeNode* node, T&& value)
    {
        size_t i = node->count - 1;

        if (node->is_leaf)
        {
            while (i != -1 && value < node->keys[i])
            {
                i--;
            }
            node->keys[i + 1] = std::forward<T>(value);
            node->count++;
        }
        else
        {
            while (i != -1 && value < node->keys[i])
            {
                i--;
            }
            i++;
            if (node->children[i]->count == MaxKeyCount)
            {
                split_child(node, i);
                if (value < node->keys[i])
                {
                    i++;
                }
            }
            insert_nonfull(x->children[i], value);
        }
    }
public:
    BTree()
        : _root(alloc_node())
    {
    }

private:
    TreeNode* _root;
};