#include "rbtree.h"

#include <stdlib.h>
// 함수 프로토타입 추가
static void rb_insert_fixup(rbtree *t, node_t *z);
static void rb_erase_fixup(rbtree *t, node_t *z);
static int is_direction(node_t *target_node);
static void left_rotate(rbtree *t, node_t *x);
static void right_rotate(rbtree *t, node_t *y);
static void rb_trans_plant(rbtree *, node_t *, node_t *);
static node_t *find_successor(const rbtree *, node_t *);
static void free_subtree(rbtree *t, node_t *node);
static void inorder_traversal(const rbtree *t, node_t *node, key_t **arr);

rbtree *new_rbtree(void)
{
  // rb트리 구조체 동적 할당
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // nil노드 생성
  node_t *first_nil = (node_t *)calloc(1, sizeof(node_t));

  // nil 노드 초기값 선언 : color = black
  first_nil->color = RBTREE_BLACK;

  // RB트리 초기화
  p->nil = first_nil;
  p->root = first_nil;

  return p;
}

static node_t *create_node(rbtree *rb, const key_t key)
{
  // 노드 구조체 동적 할당
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->color = RBTREE_RED;
  new_node->key = key;
  new_node->left = rb->nil;
  new_node->right = rb->nil;
  new_node->parent = rb->nil;

  return new_node;
}


static void free_subtree(rbtree *t, node_t *node) {
  if (node == t->nil) {
    return; // nil 노드는 해제하지 않음
  }

  // 왼쪽과 오른쪽 서브트리를 재귀적으로 해제
  free_subtree(t, node->left);
  free_subtree(t, node->right);

  // 현재 노드 해제
  free(node);
}

void delete_rbtree(rbtree *t)
{
  if (t == NULL) {
    return; // 트리가 NULL인 경우 아무 작업도 하지 않음
  }
  // 트리의 모든 노드를 해제
  free_subtree(t, t->root);
  // nil 노드 해제
  free(t->nil);
  // 트리 구조체 해제
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // 노드 생성 후 키 할당 & nil 생성
  node_t *cur_root = t->root;
  node_t *node = create_node(t, key);
  node_t *parent = t->nil;

  // 부모노드 저장 & 넣을 자리 찾기
  while (cur_root != t->nil){
    parent = cur_root;
    cur_root = (key < cur_root->key) ? cur_root->left : cur_root->right;
  }

  if (parent == t->nil){
    // 트리가 비어있는 경우
    t->root = node;
    node->color = RBTREE_BLACK;
    return node;
  }

  else if (key < parent->key){
    parent->left = node;
    node->parent = parent;
  }

  else{
    parent->right = node;
    node->parent = parent;
  }

  // 조건을 만족하는지 검사.
  rb_insert_fixup(t, node);

  // 노드 리턴
  return node;
}

static void rb_insert_fixup(rbtree *t, node_t *z){
  while ((z != t->root) && (z->parent->color == RBTREE_RED)){
    int dir = is_direction(z->parent); // 1이면 왼쪽, 0이면 오른쪽
    node_t *uncle = dir ? z->parent->parent->right : z->parent->parent->left;

    if (uncle->color == RBTREE_RED){ // Case 1: 삼촌 RED
      z->parent->color = RBTREE_BLACK;
      uncle->color = RBTREE_BLACK;
      z->parent->parent->color = RBTREE_RED;
      z = z->parent->parent;
    }

    else{
      if ((dir && (z == z->parent->right)) || (!dir && (z == z->parent->left))){ // Case 2
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

static int is_direction(node_t *target_node)
{
  return (target_node->parent->left == target_node) ? 1 : 0;
}

static void left_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right;

  if (y == t->nil)
    return;
  x->right = y->left;

  if (y->left != t->nil){
    y->left->parent = x;
  }

  y->parent = x->parent;

  if (x->parent == t->nil){
    t->root = y;
  }

  else if (x == x->parent->left){
    x->parent->left = y;
  }

  else{
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}

static void right_rotate(rbtree *t, node_t *y)
{
  node_t *x = y->left;

  if (x == t->nil)
    return;

  y->left = x->right;

  if (x->right != t->nil){
    x->right->parent = y;
  }

  x->parent = y->parent;

  if (y->parent == t->nil){
    t->root = x;
  }

  else if (y == y->parent->right){
    y->parent->right = x;
  }

  else{
    y->parent->left = x;
  }

  x->right = y;
  y->parent = x;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *cur = t->root;

  if(cur == t->nil) {
    return NULL;
  }

  while((cur != t->nil) && (cur->key != key)){
    cur = (key < cur->key) ? cur->left : cur->right;
  }

  return (cur->key == key) ? cur : NULL;
}
 
int rbtree_erase(rbtree *t, node_t *cur) {
  color_t original_color = cur->color;
  node_t *temp;

  if (cur->left == t->nil) {
    // 왼쪽 자식이 없는 경우, 오른쪽 자식을 대체
    temp = cur->right;
    rb_trans_plant(t, cur, cur->right);
  } else if (cur->right == t->nil) {
    // 오른쪽 자식이 없는 경우, 왼쪽 자식을 대체
    temp = cur->left;
    rb_trans_plant(t, cur, cur->left);
  } else {
    // 양쪽 자식이 있는 경우, 후임자(successor)를 찾음
    node_t *successor = find_successor(t, cur->right);
    original_color = successor->color;
    temp = successor->right;

    if(successor != cur->right){
      // trasplant함수에서는
      //타겟의 부모만 이어주고 가운데의 교환대상은 버려버림
      rb_trans_plant(t, successor, successor->right); 

      //오른쪽 자식을 이어줘야함...
      successor->right = cur->right;
      successor->right->parent = successor;
    }

    else{
      temp->parent = successor;
    }

    rb_trans_plant(t, cur, successor);
    successor->left = cur->left;
    successor->left->parent = successor;
    successor->color = cur->color;
  }
   // 삭제 후 균형을 맞추는 작업 필요
  if (original_color == RBTREE_BLACK) {
    rb_erase_fixup(t, temp);
  }

  free(cur);
  return 0;
}

static node_t *find_successor(const rbtree *t, node_t *z){
  node_t *cur = z;

  while(cur->left != t->nil){
    cur = cur->left;
  }

  return cur;
}

node_t *rbtree_min(const rbtree *t)
{
  node_t *cur = t->root;

  while(cur->left != t->nil)
    cur = cur->left;
  
  return cur;
}


static void rb_erase_fixup(rbtree *t, node_t *z){
  node_t *target_node = z;
  node_t *brother_node;

  while ((target_node == t->nil) && (target_node->color != RBTREE_BLACK)){
    // 1일떄 target_node은 왼쪽자식 0일때 오른쪽자식
    int dir = is_direction(target_node);

    if(dir){ // 타겟노드가 왼쪽 자식일때
      brother_node = target_node->parent->right;
      
      if(brother_node->color == RBTREE_RED){ // CASE 1
        brother_node->color = RBTREE_BLACK;
        target_node->color = RBTREE_RED;
        left_rotate(t, target_node->parent);
        brother_node = target_node->parent->right;
      }

      if((brother_node->left->color == RBTREE_BLACK) && (brother_node->right->color == RBTREE_BLACK)){ // CASE 2
        brother_node->color = RBTREE_RED;
        target_node = target_node->parent;
      }

      else{
        if(brother_node->right->color == RBTREE_BLACK){ // CASE 3
          brother_node->left->color = RBTREE_BLACK;
          brother_node->color = RBTREE_RED;
          right_rotate(t, brother_node);
          brother_node = target_node->parent->right;
        }

        brother_node->color = target_node->parent->color; // CASE 4 
        target_node->color = RBTREE_BLACK;
        brother_node->right->color = RBTREE_BLACK;
        left_rotate(t, target_node->parent);
        target_node = t->root;
      }
    }
  
    else{ // 타겟노드가 오른쪽 자식일 때 
      brother_node = target_node->parent->left;
      
      if(brother_node->color == RBTREE_RED){ // CASE 1
        brother_node->color = RBTREE_BLACK;
        target_node->color = RBTREE_RED;
        right_rotate(t, target_node->parent);
        brother_node = target_node->parent->left;
      }

      if((brother_node->right->color == RBTREE_BLACK) && (brother_node->left->color == RBTREE_BLACK)){ // CASE 2
        brother_node->color = RBTREE_RED;
        target_node = target_node->parent;
      }

      else{
        if(brother_node->left->color == RBTREE_BLACK){ // CASE 3
          brother_node->right->color = RBTREE_BLACK;
          brother_node->color = RBTREE_RED;
          left_rotate(t, brother_node);
          brother_node = target_node->parent->left;
        }

        brother_node->color = target_node->parent->color; // CASE 4 
        target_node->color = RBTREE_BLACK;
        brother_node->left->color = RBTREE_BLACK;
        right_rotate(t, target_node->parent);
        target_node = t->root;
      }
    }
  }

  target_node->color = RBTREE_BLACK;
}

node_t *rbtree_max(const rbtree *t)
{
  node_t *cur = t->root;

  while(cur->right != t->nil)
    cur = cur -> right;

  return cur;
}



static void rb_trans_plant(rbtree *t, node_t *u, node_t *v){
  if(u->parent == t->nil){
    t->root = v;
  }
  
  else if(u == u->parent->left){
    u->parent->left = v;
  }
  
  else{
    u->parent->right = v;
  }

  v->parent = u->parent;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (t->root == t->nil) {
    return 0; // 트리가 비어 있는 경우
  }

  key_t *arr_ptr = arr;
  inorder_traversal(t, t->root, &arr_ptr);

  return 0;
}

static void inorder_traversal(const rbtree *t, node_t *node, key_t **arr) {
  if (node == t->nil) {
    return; // nil 노드는 무시
  }

  // 왼쪽 서브트리 방문
  inorder_traversal(t, node->left, arr);

  // 현재 노드의 키를 배열에 저장
  **arr = node->key;
  (*arr)++;

  // 오른쪽 서브트리 방문
  inorder_traversal(t, node->right, arr);
}
