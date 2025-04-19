#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  //rb트리 구조체 동적 할당
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  //nil노드 생성
  node_t *first_nil = (node_t *)calloc(1, sizeof(node_t));

  //nil 노드 초기값 선언 : color = black
  first_nil->color = RBTREE_BLACK;
  
  // RB트리 초기화
  p->nil = first_nil;
  p->root = first_nil;

  return p;
}

node_t *create_node(rbtree *rb, const key_t key) {
  //노드 구조체 동적 할당
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->color = RBTREE_RED;
  new_node->key = key;
  new_node->left = rb->nil;
  new_node->right = rb->nil;
  new_node->parent = rb->nil;
  
  return new_node;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

void rb_erase_fixup(rbtree *t, node_t *x){

}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 노드 생성 후 키 할당 & nil 생성
  node_t *cur_root = t->root;
  node_t *node = create_node(t, key);
  node_t *parent = t->nil;

  // 부모노드 저장 & 넣을 자리 찾기
  while(cur_root != t->nil){
    parent = cur_root;
    cur_root = (key < cur_root->key) ? cur_root->left : cur_root->right;
  }

  if (parent == t->nil) {
    // 트리가 비어있는 경우
    t->root = node;
    node->color = RBTREE_BLACK;
    return node;
  } else if (key < parent->key) {
    parent->left = node;
    node->parent = parent;
  } else {
    parent->right = node;
    node->parent = parent;
  }

  //조건을 만족하는지 검사.  
  rb_insert_fixup(t, node);

  // 노드 리턴
  return node;
  }

  void rb_insert_fixup(rbtree *t, node_t *z){
    while((z != t->root) && (z->parent->color == RBTREE_RED)){
      int dir = is_direction(z->parent);  // 1이면 왼쪽, 0이면 오른쪽
      node_t *uncle = dir ? z->parent->parent->right : z->parent->parent->left;
  
      if (uncle->color == RBTREE_RED) {  // Case 1: 삼촌 RED
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } 
      
      else {
        if ((dir && (z == z->parent->right)) || (!dir && (z == z->parent->left))) {  // Case 2
          z = z->parent;
          dir ? left_rotate(t, z) : right_rotate(t, z);
        }
  
        // Case 3
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        dir ? right_rotate(t, z->parent->parent) : left_rotate(t, z->parent->parent);
      }
    }
  
    t->root->color = RBTREE_BLACK;
  }

  int is_direction(node_t *target){
    return (target->parent->left == target) ? 1 : 0;
  }


node_t *find_uncle(const rbtree *t, const node_t *z){
  if ((z->parent == t->nil) || (z->parent->parent == t->nil))
    return t->nil;

  node_t *grand = z->parent->parent;
  return (z->parent == grand->left) ? grand->right : grand->left;
}

void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  if (y == t->nil) return;

  x->right = y->left;
  if (y->left != t->nil) {
    y->left->parent = x;
  }

  y->parent = x->parent;
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}



void right_rotate(rbtree *t, node_t *y) {
  node_t *x = y->left;
  if (x == t->nil) return;

  y->left = x->right;
  if (x->right != t->nil) {
    x->right->parent = y;
  }

  x->parent = y->parent;
  if (y->parent == t->nil) {
    t->root = x;
  } else if (y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }

  x->right = y;
  y->parent = x;
}


node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
