#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

// 메모리: 트리 생성/삭제
rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);

//노드 생성
node_t *create_node(rbtree *, const key_t);

// 노드 삽입/삭제
node_t *rbtree_insert(rbtree *, const key_t);
int rbtree_erase(rbtree *, node_t *);

// 삽입/삭제 fixup
void rb_insert_fixup(rbtree *, node_t *);
void rb_erase_fixup(rbtree *, node_t *);

// 노드 검색
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
node_t *find_uncle(const rbtree *, const node_t *);

//위치확인
int is_direction(node_t *);

// 회전
void left_rotate(rbtree *, node_t *);
void right_rotate(rbtree *, node_t *);

// 트리를 배열로 변환 -> inorder traversing으로 구현!!! 
// traversing 순서: node, node->left, node->right
int rbtree_to_array(const rbtree *, key_t *, const size_t);

#endif  // _RBTREE_H_
