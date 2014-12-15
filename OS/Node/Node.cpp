#include <cstdlib>

#pragma once

template <typename Type>
class Node
{
public:
	Node* next;		// pointer to the next node
	Node* prev;		// pointer to the previous node
	Type* data;		// pointer to the data object

	// create a node for data passed as literal
	Node(Type data)
	{	this->data = new Type(data);
		next = NULL;
		prev = NULL;
	}

	// create a node for data passed as reference
	Node(Type* data)
	{	this->data = data;
		next = NULL;
		prev = NULL;
	}
};