#pragma once

#ifndef TREE_H
#define TREE_H

#include <cassert>

template<typename Ty>
struct TreeNode
{
	Ty Value;
	TreeNode* Left;
	TreeNode* Right;
	TreeNode* Parent;

	TreeNode(Ty value, TreeNode* left, TreeNode* right, TreeNode* parent)
		: Value(value), Left(left), Right(right), Parent(parent)
	{
	}
	TreeNode(Ty value)
		: TreeNode(value, nullptr, nullptr, nullptr)
	{
	}
	TreeNode(const TreeNode& node)
		: TreeNode(node.Value, node.Left, node.Right, nullptr)
	{
	}
	TreeNode(TreeNode&& node)
		: TreeNode(node.Value, node.Left, node.Right, nullptr)
	{
		node.Value = 0;
		node.Left = nullptr;
		node.Right = nullptr;
		node.Parent = nullptr;
	}
	TreeNode(TreeNode* node)
		: TreeNode(node ? node->Value : nullptr,
				   node ? node->Left : nullptr,
				   node ? node->Right : nullptr,
				   nullptr)
	{
	}
};

template<typename Ty>
class TreeIterator
{
	using Node = TreeNode<Ty>;
private:
	Node* _node;
public:
	TreeIterator(Node* node = nullptr)
		: _node(node)
	{
	}
	TreeIterator(const TreeIterator& iter)
		: _node(iter)
	{
	}
	Ty& operator*() const
	{
		return _node->Value;
	}
	TreeIterator& ToLeft()
	{
		_node = _node->Left;
		return *this;
	}
	TreeIterator& ToRight()
	{
		_node = _node->Right;
		return *this;
	}
	TreeIterator& ToParent()
	{
		_node = _node->Parent;
		return *this;
	}
	const TreeIterator Parent() const
	{
		return TreeIterator(_node->Parent);
	}
	const TreeIterator Left() const
	{
		return TreeIterator(_node->Left);
	}
	const TreeIterator Right() const
	{
		return TreeIterator(_node->Right);
	}
	bool IsLeftSon() const
	{
		return Parent()? Parent()->Left == _node : false
	}
	bool IsRightSon() const
	{
		return Parent() ? Parent()->Right == _node : false;
	}
	bool IsParent() const
	{
		return Left() || Right();
	}
	TreeIterator& operator=(const TreeIterator& iterator)
	{
		_node = iterator._node;
		return *this;
	}
	TreeIterator& operator=(Node* node)
	{
		_node = node;
		return *this;
	}
	operator bool()
	{
		return _node != nullptr;
	}
	operator Node*()
	{
		return _node;
	}
};

template<typename Ty>
class Tree
{
private:
	using Node = TreeNode<Ty>;
	using Iterator = TreeIterator<Ty>;
	Node* _root;
private:
	static Node* InternalCopy(const Node* node, const Node* parent)
	{
		if (node)
		{
			Node* newNode = new Node;
			newNode->Value = node->Value;
			newNode->Left = InternalCopy(node->Left, newNode);
			newNode->Right = InternalCopy(node->Right, newNode);
			newNode->Parent = parent;
			return newNode;
		}
		return nullptr;
	}
	static void InternalFree(Node* node)
	{
		if (node)
		{
			InternalFree(node->Left);
			InternalFree(node->Right);
			delete node;
		}
	}
public:
	Tree()
		: _root(nullptr)
	{
	}
	Tree(Ty value)
		: _root(new Node(value))
	{
	}
	Tree(Ty value, Node* left, Node* right)
		: _root(new Node(value, left, right))
	{
	}
	Tree(const Tree& tree)
		: _root(InternalCopy(tree._root, nullptr))
	{
	}
	Tree(Tree&& tree)
		: _root(tree._root)
	{
		tree._root = nullptr;
	}
	Tree(const Node* node)
		: _root(InternalCopy(node, nullptr))
	{
	}
	~Tree()
	{
		InternalFree(_root);
	}
public:
	Iterator Release()
	{
		Node* root = _root;
		_root = nullptr;
		return Iterator(root);
	}
	bool IsEmpty()
	{
		return _root == nullptr;
	}
	void Swap(Tree& tree)
	{
		Node* temp = _root;
		_root = tree._root;
		tree._root = temp;
	}
	static Tree Combine(Ty value, Tree& left, Tree& right)
	{
		return Tree(new Node(value, left.Release(), right.Release(), nullptr));
	}
	static void SetLeft(Iterator where, Tree& tree)
	{
		InternalFree(where.Left());
		where.Left() = tree.Release();
		where.Left()->Parent = where.Raw();
	}
	static void SetRight(Iterator where, Tree& tree)
	{
		InternalFree(where.Right());
		where.Right() = tree.Release();
		where.Right()->Parent = where.Raw();
	}
};

#endif // !TREE_H
