/**
	#define meta ...
	printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2018.
	Email: arp@poshtkohi.ir
	Website: http://www.poshtkohi.ir
*/

#include "balanced_binary_tree.h"

#include <stdexcept>
#include <string>
#include <algorithm>

//**************************************************************************************************************//
namespace Supercomputing::CollectiveOperations
{
    //----------------------------------------------------
    balanced_binary_tree::balanced_binary_tree(const std::vector<unsigned int> &v, unsigned int root_value)
    {
        if(v.size() == 0)
            throw std::runtime_error("v is empty");

        if(std::find(v.begin(), v.end(), root_value) == v.end())
            throw std::runtime_error("v has not the value " + std::to_string(root_value));

        for(unsigned int i = 0 ; i < v.size() ; i++)
            _array.push_back((int)v[i]);
        _root_value = root_value;

        qsort(&_array[0], _array.size(), sizeof(int), compare);

        int root_index = 0;
        //bool found = false;
        for(unsigned int i = 0 ; i < _array.size() ; i++)
        {
            if(_array[i] == _root_value)
                root_index = i;

            if(i != _array.size() - 1)
                if(_array[i] == _array[i + 1])
                    throw std::runtime_error("A duplicated item was found in v");
        }

        // Prepares an array which the root of the future balanced binary tree is root_value.
        int mid = (0 + _array.size() - 1) / 2;

        if(root_index != mid)
        {
            if(root_index > mid)
                for(int i = root_index, n = 0 ; n < root_index - mid ; i--, n++)
                    _array[i] = _array[i - 1];
            else
                for(int i = root_index, n = 0 ; n < mid - root_index ; i++, n++)
                    _array[i] = _array[i + 1];

            _array[mid] = _root_value;
        }

        //for(register int i = 0 ; i < _array.size() ; i++)
        //    std::cout << _array[i] << " ";
        //std::cout << "\nroot_index " << root_index << " mid " << mid <<  std::endl;
    }
    //----------------------------------------------------
    balanced_binary_tree::~balanced_binary_tree()
    {
    }
    //----------------------------------------------------
    void balanced_binary_tree::dispose_tree()
    {
        if(_root == NULL)
            return;

        dispose_tree_util(_root);

        _root = NULL;
    }
    //----------------------------------------------------
    void balanced_binary_tree::dispose_tree_util(t_node *n)
    {
        if(n == NULL)
            return;

        dispose_tree_util(n->left);
        dispose_tree_util(n->right);

        free(n);
    }
    //----------------------------------------------------
    std::vector<balanced_binary_tree::nodeType> &balanced_binary_tree::get_nodes()
    {
        if(_root == NULL)
        {
            _root = sorted_array_to_bst(_array, 0, _array.size() - 1, NULL);
            balanced_binary_tree::preorder(_root);
            dispose_tree();
        }

        return _nodes;
    }
    //----------------------------------------------------
    struct balanced_binary_tree::t_node *balanced_binary_tree::sorted_array_to_bst(const std::vector<int> &arr, int start, int end, t_node *parent)
    {
        // Base Case
        if(start > end)
          return NULL;

        // Get the middle element and make it root.
        int mid = (start + end) / 2;
        //printf("start %d end %d mid %d\n", start, end, mid);
        struct t_node *root = new_node(arr[mid]);

        // Recursively construct the left subtree and make it
        // left child of root.
        root->left =  sorted_array_to_bst(arr, start, mid - 1, root);

        // Recursively construct the right subtree and make it
        // right child of root.
        root->right = sorted_array_to_bst(arr, mid + 1, end, root);

        root->parent = parent;

        return root;
    }
    //----------------------------------------------------
    struct balanced_binary_tree::t_node *balanced_binary_tree::new_node(int data)
    {
        struct t_node *node = (struct t_node *)malloc(sizeof(struct t_node));
        node->data = data;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    //----------------------------------------------------
    void balanced_binary_tree::preorder(const struct t_node *node)
    {
        if(node == NULL)
            return;
            //throw std::runtime_error("node is NULL");

        int left = -1, right = -1, parent = -1;

        if(node->parent != NULL)
            parent = node->parent->data;

        if(node->left != NULL)
            left = node->left->data;

        if(node->right != NULL)
            right = node->right->data;

        //printf("node %d parent %d left %d right %d\n", node->data, parent, left, right);

        _nodes.push_back(std::make_tuple(node->data, parent, left, right));

        preorder(node->left);
        preorder(node->right);
    }
    //----------------------------------------------------
    int balanced_binary_tree::compare(const void *a, const void *b)
    {
        return ( *(int *)a - *(int *)b );
    }
    //----------------------------------------------------
};
//**************************************************************************************************************//
