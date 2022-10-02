/**
    #define meta ...
    printf("%s\n", meta);
**/

/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Supercomputing_CollectiveOperations_balanced_binary_tree_h__
#define __Supercomputing_CollectiveOperations_balanced_binary_tree_h__

#include <stdlib.h>
#include <vector>
#include <tuple>

//**************************************************************************************************************//

namespace Supercomputing::CollectiveOperations
{
    //----------------------------------------------------
    // Forward declarations

    /// Adpoted from https://www.geeksforgeeks.org/sorted-array-to-balanced-bst/
    class balanced_binary_tree
    {
        // A Binary Tree node
        private: struct t_node
        {
            int data;
            struct t_node *parent;
            struct t_node *left;
            struct t_node *right;
        };

        // NodeID, NodeParent, NodeLeft, NodeRight
        // If an ID is null, we shows it as -1.
        public: typedef std::tuple<int, int, int, int> nodeType;
        /*---------------------fields-----------------*/
        private: std::vector<int> _array;
        private: std::vector<nodeType> _nodes;
        private: t_node *_root = NULL;
        private: int _root_value;
        /*---------------------methods----------------*/
        // balanced_binary_tree Class constructor.
        public: balanced_binary_tree(const std::vector<unsigned int> &v, unsigned int root_value);
        // balanced_binary_tree Class destructor.
        public: ~balanced_binary_tree();
        public: void dispose_tree();
        public: void dispose_tree_util(t_node *n);
        // A function that constructs Balanced Binary Search Tree from a sorted array.
        private: static struct t_node *sorted_array_to_bst(const std::vector<int> &arr, int start, int end, t_node *parent);
        // Helper function that allocates a new node with the given data and
        // NULL left and right pointers.
        private: static struct t_node *new_node(int data);
        // A utility function to print preorder traversal of BST (Balanced Binary Search Tree).
        private: void preorder(const struct t_node *node);
        private: static int compare(const void *a, const void *b);
        // Get s the nodes of the BST.
        public: std::vector<nodeType> &get_nodes();
    };
    //----------------------------------------------------
};
//**************************************************************************************************************//

#endif

