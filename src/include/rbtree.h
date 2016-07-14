/*
 * Copyright: (c) 2006-2007, 2008 Triductor Technology, Inc.
 * All Rights Reserved.
 *
 * File:        rbtree.h
 * Purpose:     Red black tree generic manipulation structures.
 * History:     07/15/2007, created by jetmotor
 */

#ifndef _RBTREE_H
#define _RBTREE_H

#include "types.h"
#include "list.h"


struct rb_node{
	struct rb_node *rb_parent;
	int32_t rb_color;
#define RB_RED          1
#define RB_BLACK        2
	struct rb_node *rb_right;
	struct rb_node *rb_left;
};
#define rb_entry(ptr, type, member) \
        ((type *)(uint32_t)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define rb_node_init(node) do{ \
	(node).rb_parent = (node).rb_right = (node).rb_left = NULL; \
	(node).rb_color = RB_RED; \
}while (0)

#define RB_NODE_INIT() {0, RB_RED, 0, 0}


struct rb_root{
	struct rb_node *rb_node;
};


struct rb_tree_s;

/**
 * rb_treenode_cmp_fp - Function pointer of the function that compare the two node.
 * @na:		the node in tree
 * @key:	the new-coming node's key
 * @return: 
 *		< 0, if na < nb 
 *		== 0, if na == nb
 *		> 0, if na > nb
 */
typedef int32_t (*rb_treenode_cmp_fp)(struct rb_node *na, void *key);

/**
 * rb_tree_find_fp - Function pointer of the function that find the right place in the 
 *	tree for the new-coming node.
 * @tree:	the tree
 * @n:		the new-coming node's key
 * @return:
 *		*(@return) points to the right place for inserting the new-coming node,	if cannot find one
 *		*(@return) points to the node in tree those key equals the new_coming node's one, if can find one
 */
typedef struct rb_node ** (*rb_tree_find_fp)(struct rb_tree_s *tree, void *key, struct rb_node **parent);

/**
 * rb_tree_insert_fp - Function pointer of the function that insert a new node into the 
 *	exact postion determined by the operation `find' of the tree.
 * @return:
 */
typedef void (*rb_tree_insert_fp)(struct rb_tree_s *tree, struct rb_node *n, struct rb_node *parent, struct rb_node **p);

/**
 * rb_tree_erase_fp - Function pointer of the function that delete a node from the tree.
 */
typedef void (*rb_tree_erase_fp)(struct rb_tree_s *tree, struct rb_node *n);

/**
 * rb_treenode_del_fp - Function pointer of the function that delete a tree node of any type.
 */
typedef void (*rb_treenode_del_fp)(struct rb_node *n);

/**
 * rb_tree_finalize_fp - Function pointer of the function that free all the nodes on the tree.
 */
typedef void (*rb_tree_finalize_fp)(struct rb_tree_s *tree);

typedef struct rb_tree_s{
	uint32_t nr;
	struct rb_root root;
	struct rb_node *first;
	
	rb_treenode_cmp_fp	cmp;
	rb_tree_find_fp		find;
	rb_tree_insert_fp	insert;
	rb_tree_erase_fp	erase;
	rb_treenode_del_fp	del;
	rb_tree_finalize_fp	finalize;
}rb_tree_t;

#define RB_TREE_INIT() {0, {0}, 0, 0, 0, 0, 0, 0, 0}

#endif	/* end of _RBTREE_H */
