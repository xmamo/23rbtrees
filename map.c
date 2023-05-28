#include "map.h"

#include <assert.h>
#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

typedef enum Color {
  BLACK = 0,
  RED = 1,
} Color;

typedef enum Direction {
  LEFT = 0,
  RIGHT = 1,
} Direction;

typedef struct Node {
  struct Node* children[2];
  struct Node* parent;
  unsigned char direction;
  unsigned char color;
  char data[];
} Node;

typedef struct Node_layout {
  size_t size;
  size_t key_offset;
  size_t key_size;
  size_t value_offset;
  size_t value_size;
} Node_layout;

static inline void* Node_key(const Node* node, const Node_layout* layout) {
  return (char*)node + layout->key_offset;
}

static inline void* Node_value(const Node* node, const Node_layout* layout) {
  return (char*)node + layout->value_offset;
}

static inline bool Node_is_black(const Node* node) {
  return node == NULL || node->color == BLACK;
}

static inline bool Node_is_red(const Node* node) {
  return node != NULL && node->color == RED;
}

static size_t Node_check(const Node* node) {
  if (node == NULL)
    return 1;

  if (node->parent != NULL) {
    assert(node->parent->children[node->direction] == node);
    assert(node->color == BLACK || node->parent->color == BLACK);
  }

  assert(Node_is_black(node->children[LEFT]) || Node_is_black(node->children[RIGHT]));

  size_t left_black_depth = Node_check(node->children[LEFT]);
  size_t right_black_depth = Node_check(node->children[RIGHT]);
  assert(left_black_depth == right_black_depth);

  return left_black_depth + (node->color == BLACK ? 1 : 0);
}

static Node* Node_xmost_node(const Node* node, Direction direction) {
  while (node->children[direction] != NULL) {
    node = node->children[direction];
  }

  return (Node*)node;
}

static Node* Node_xmost_leaf(const Node* node, Direction direction) {
  while (true) {
    if (node->children[direction] != NULL) {
      node = node->children[direction];
    } else if (node->children[1 - direction] != NULL) {
      node = node->children[1 - direction];
    } else {
      return (Node*)node;
    }
  }
}

static Node* Node_post_order_xcessor(const Node* node, Direction direction) {
  if (node->direction != direction && node->parent != NULL && node->parent->children[direction] != NULL) {
    return Node_xmost_leaf(node->parent->children[direction], 1 - direction);
  } else {
    return node->parent;
  }
}

static size_t Node_count(const Node* node) {
  size_t count = 0;

  if (node != NULL) {
    node = Node_xmost_leaf(node, LEFT);

    do {
      count += 1;
      node = Node_post_order_xcessor(node, RIGHT);
    } while (node != NULL);
  }

  return count;
}

static Node* Node_rotate(Node* B, Direction direction) {
  //       C                         A
  //     ┌╌┴╌┐         →B          ┌╌┴╌┐
  //    →B   d       ┌╌╌┴╌╌┐       a   B←
  //   ┌╌┴╌┐    ◁    A     C    ▷    ┌╌┴╌┐
  //   A   c       ┌╌┴╌┐ ┌╌┴╌┐       b   C
  // ┌╌┴╌┐         a   b c   d         ┌╌┴╌┐
  // a   b                             c   d

  Node* CA = B->children[1 - direction];
  Node* parent = B->parent;
  Direction B_direction = B->direction;
  Color B_color = B->color;

  Node* cb = CA->children[direction];
  Color cb_color = CA->color;

  if (cb != NULL) {
    cb->parent = B;
    cb->direction = 1 - direction;
  }

  B->children[1 - direction] = cb;
  B->parent = CA;
  B->direction = direction;
  B->color = cb_color;

  CA->children[direction] = B;
  CA->parent = parent;
  CA->direction = B_direction;
  CA->color = B_color;

  return CA;
}

struct Map {
  Node* root;
  size_t count;
  Comparator comparator;
  Node_layout node_layout;
};

Map* map_new(Layout key_layout, Layout value_layout, Comparator comparator) {
  Map* map = malloc(sizeof(Map));

  if (map != NULL) {
    map->root = NULL;
    map->count = 0;
    map->comparator = comparator;

    Layout layout = layout_init();
    layout_add(&layout, (Layout){sizeof(Node), alignof(Node)});
    size_t key_offset = layout_add(&layout, key_layout);
    size_t value_offset = layout_add(&layout, value_layout);
    map->node_layout.size = layout_build(&layout).size;
    map->node_layout.key_offset = key_offset;
    map->node_layout.key_size = key_layout.size;
    map->node_layout.value_offset = value_offset;
    map->node_layout.value_size = value_layout.size;
  }

  return map;
}

size_t map_count(const Map* map) {
  return map->count;
}

void* map_lookup(const Map* map, const void* key) {
  const Node* node = map->root;

  while (node != NULL) {
    int ordering = comparator_compare(map->comparator, key, Node_key(node, &map->node_layout));

    if (ordering < 0) {
      node = node->children[LEFT];
    } else if (ordering > 0) {
      node = node->children[RIGHT];
    } else {
      return Node_value(node, &map->node_layout);
    }
  }

  return NULL;
}

bool map_insert(Map* map, const void* key, const void* value) {
  // Top-down pass:

  Node* node = map->root;
  Node* parent = NULL;
  Direction node_direction = LEFT;

  while (node != NULL) {
    int ordering = comparator_compare(map->comparator, key, Node_key(node, &map->node_layout));

    if (ordering < 0) {
      parent = node;
      node = node->children[node_direction = LEFT];
    } else if (ordering > 0) {
      parent = node;
      node = node->children[node_direction = RIGHT];
    } else {
      memmove(
        Node_value(node, &map->node_layout),
        value,
        map->node_layout.value_size
      );

      return true;
    }
  }

  if ((node = malloc(map->node_layout.size)) == NULL)
    return false;

  node->children[LEFT] = NULL;
  node->children[RIGHT] = NULL;
  node->parent = parent;
  node->direction = node_direction;
  node->color = RED;

  memmove(
    Node_key(node, &map->node_layout),
    key,
    map->node_layout.key_size
  );

  memmove(
    Node_value(node, &map->node_layout),
    value,
    map->node_layout.value_size
  );

  *(parent != NULL ? &parent->children[node_direction] : &map->root) = node;
  map->count += 1;

  // Bottom-up pass:

  while (node->parent != NULL) {
    assert(node->color == RED);

    if (node->parent->color == RED) {
      if (node->direction != node->parent->direction) {
        //              Rule from Figure 9a:
        //   A           A        ╎        C           C
        // ┌─┶━┓       ┌─┶━┓      ╎      ┏━┵─┐       ┏━┵─┐
        // a   C       a   B      ╎      B   d       A   d
        //   ┏━┵─┐  ▷    ┌─┶━┓    ╎    ┏━┵─┐    ◁  ┌─┶━┓
        //  →B   δ       b   C←   ╎   →A   c       a   B←
        // ┌─┴─┐           ┌─┴─┐  ╎  ┌─┴─┐           ┌─┴─┐
        // b   c           c   d  ╎  a   b           b   c
        node = node->parent;
        Node* B = Node_rotate(node, node->direction);
        B->parent->children[B->direction] = B;
      }

      //                  Rule from Figure 9b:
      //       C                    ╎                    A
      //     ┏━┵─┐          B       ╎       B          ┌─┶━┓
      //     B   d       ┏━━┷━━┓    ╎    ┏━━┷━━┓       a   B
      //   ┏━┵─┐    ▷   →A     C    ╎    A     C←   ◁    ┌─┶━┓
      //  →A   c       ┌─┴─┐ ┌─┴─┐  ╎  ┌─┴─┐ ┌─┴─┐       b   C←
      // ┌─┴─┐         a   b c   d  ╎  a   b c   d         ┌─┴─┐
      // a   b                      ╎                      c   d
      Node* B = Node_rotate(node->parent->parent, 1 - node->direction);
      *(B->parent != NULL ? &B->parent->children[B->direction] : &map->root) = B;
    }

    if (Node_is_red(node->parent->children[1 - node->direction])) {
      //            Rule from Figure 9c:
      //      ╷               ╻               ╷
      //      B              →B               B
      //   ┏━━┷━━┓    ▷    ┌──┴──┐    ◁    ┏━━┷━━┓
      //  →A     C         A     C         A    →C
      // ┌─┴─┐ ┌─┴─┐     ┌─┴─┐ ┌─┴─┐     ┌─┴─┐ ┌─┴─┐
      // a   b c   d     a   b c   d     a   b c   d
      node->color = BLACK;
      node->parent->children[1 - node->direction]->color = BLACK;
      node->parent->color = RED;
      node = node->parent;
    } else {  // if (Node_is_black(node->parent->children[1 - node->direction]))
      break;
    }
  }

  map->root->color = BLACK;
  return true;
}

bool map_remove(Map* map, const void* key) {
  // Top-down pass:

  Node* node = map->root;

  while (true) {
    if (node != NULL) {
      int ordering = comparator_compare(map->comparator, key, Node_key(node, &map->node_layout));

      if (ordering < 0) {
        node = node->children[LEFT];
      } else if (ordering > 0) {
        node = node->children[RIGHT];
      } else {
        break;
      }
    } else {
      return false;
    }
  }

  if (node->children[LEFT] != NULL && node->children[RIGHT] != NULL) {
    Node* in_order_predecessor = Node_xmost_node(node->children[LEFT], RIGHT);

    memmove(
      Node_key(node, &map->node_layout),
      Node_key(in_order_predecessor, &map->node_layout),
      map->node_layout.key_size
    );

    memmove(
      Node_value(node, &map->node_layout),
      Node_value(in_order_predecessor, &map->node_layout),
      map->node_layout.value_size
    );

    node = in_order_predecessor;
  }

  Node* parent = node->parent;
  Direction node_direction = node->direction;
  Color node_color = node->color;

  for (size_t i = 0; i < 2; ++i) {
    if (node->children[i] != NULL) {
      Node* child = node->children[i];
      child->parent = parent;
      child->direction = node_direction;
      child->color = node_color;
      free(node);
      *(parent != NULL ? &parent->children[node_direction] : &map->root) = child;
      map->count -= 1;
      return true;
    }
  }

  free(node);
  *(parent != NULL ? &parent->children[node_direction] : &map->root) = NULL;
  map->count -= 1;

  // Bottom-up pass:

  if (node_color == RED || parent == NULL)
    return true;

  do {
    Node* sibling = parent->children[1 - node_direction];

    if (sibling->color == RED) {
      //                              Rule from Figure 13c:
      //          D                 B           ╎           D                 B
      //      ┏━━━┵───┐         ┌───┶━━━┓       ╎       ┏━━━┵───┐         ┌───┶━━━┓
      //      B      →E         A       D       ╎       B       E        →A       D
      //   ┌──┴──┐  ┌─┴─┐  ▷  ┌─┴─┐  ┌──┴──┐    ╎    ┌──┴──┐  ┌─┴─┐  ◁  ┌─┴─┐  ┌──┴──┐
      //   A     C  e   f     a   b  C    →E    ╎   →A     C  e   f     a   b  C     E
      // ┌─┴─┐ ┌─┴─┐               ┌─┴─┐ ┌─┴─┐  ╎  ┌─┴─┐ ┌─┴─┐               ┌─┴─┐ ┌─┴─┐
      // a   b c   d               c   d e   f  ╎  a   b c   d               c   d e   f
      Node* DB = Node_rotate(parent, node_direction);
      *(DB->parent != NULL ? &DB->parent->children[DB->direction] : &map->root) = DB;
      sibling = parent->children[1 - node_direction];
    }

    //                    Rule from Figure 13b:
    //      B              →B       ╎       B←              B
    //   ┌──┴──┐         ┌──┶━━┓    ╎    ┏━━┵──┐         ┌──┴──┐
    //  →A     C    ▷    A     C    ╎    A     C    ◁    A     C←
    // ┌─┴─┐ ┌─┴─┐     ┌─┴─┐ ┌─┴─┐  ╎  ┌─┴─┐ ┌─┴─┐     ┌─┴─┐ ┌─┴─┐
    // a   b c   d     a   b c   d  ╎  a   b c   d     a   b c   d
    sibling->color = RED;

    if (Node_is_red(sibling->children[LEFT]) || Node_is_red(sibling->children[RIGHT])) {
      if (Node_is_black(sibling->children[sibling->direction])) {
        //                     Rule from Figure 15a:
        //                    A          ╎          D
        //    A             ┌─┶━┓        ╎        ┏━┵─┐             D
        //  ┌─┶━┓          →a   B        ╎        C   e←          ┏━┵─┐
        // →a   C             ┌─┶━┓      ╎      ┏━┵─┐             B   e←
        //   ┏━━┵──┐    ▷     b   C      ╎      B   d     ◁    ┌──┶━━┓
        //   B     D            ┌─┴─┐    ╎    ┌─┴─┐            A     C
        // ┌─┴─┐ ┌─┴─┐          c   D    ╎    A   c          ┌─┴─┐ ┌─┴─┐
        // b   c d   e            ┌─┴─┐  ╎  ┌─┴─┐            a   b c   d
        //                        d   e  ╎  a   b
        sibling = Node_rotate(sibling, sibling->direction);
        parent->children[sibling->direction] = sibling;
      }

      //                    Rule from Figure 15b:
      //       C                      ╎                      A
      //     ┏━┵─┐           B        ╎        B           ┌─┶━┓
      //     B   d←       ┏━━┷━━┓     ╎     ┏━━┷━━┓       →a   B
      //   ┏━┵─┐     ▷    A     C     ╎     A     C    ◁     ┌─┶━┓
      //   A   c        ┌─┴─┐ ┌─┴─┐   ╎   ┌─┴─┐ ┌─┴─┐        b   C
      // ┌─┴─┐          a   b c   d←  ╎  →a   b c   d          ┌─┴─┐
      // a   b                        ╎                        c   d
      Node* B = Node_rotate(parent, node_direction);
      *(B->parent != NULL ? &B->parent->children[B->direction] : &map->root) = B;

      //    Rule from Figure 15c:
      //      B               B
      //   ┏━━┷━━┓         ┌──┴──┐
      //   A     C    ▷    A     C
      // ┌─┴─┐ ┌─┴─┐     ┌─┴─┐ ┌─┴─┐
      // a   b c   d     a   b c   d
      B->children[LEFT]->color = BLACK;
      B->children[RIGHT]->color = BLACK;
      return true;
    }

    node = parent;
    parent = node->parent;
    node_direction = node->direction;
  } while (parent != NULL && node->color == BLACK);

  // Rule from Figure 13a:
  //      ╻         ╷
  //      A    ▷    A
  //    ┌─┴─┐     ┌─┴─┐
  //    a   b     a   b
  node->color = BLACK;
  return true;
}

Map* map_copy(const Map* map) {
  Map* new_map = malloc(sizeof(Map));

  if (new_map != NULL) {
    new_map->comparator = map->comparator;
    new_map->node_layout = map->node_layout;

    if (map->root != NULL) {
      const Node* node0 = map->root;
      Node* node1 = new_map->root = malloc(map->node_layout.size);

      if (node1 != NULL) {
        node1->children[LEFT] = NULL;
        node1->children[RIGHT] = NULL;
        node1->parent = NULL;
        node1->direction = node0->direction;
        node1->color = node0->color;

        memmove(
          Node_key(node1, &map->node_layout),
          Node_key(node0, &map->node_layout),
          map->node_layout.key_size
        );

        memmove(
          Node_value(node1, &map->node_layout),
          Node_value(node0, &map->node_layout),
          map->node_layout.value_size
        );

        while (true) {
          Direction direction;

          if (node0->children[LEFT] != NULL) {
            direction = LEFT;
          } else if (node0->children[RIGHT] != NULL) {
            direction = RIGHT;
          } else {
            while (node0->children[RIGHT] == NULL || node1->children[RIGHT] != NULL) {
              if (node0->parent == NULL) {
                new_map->count = map->count;
                return new_map;
              }

              node0 = node0->parent;
              node1 = node1->parent;
            }

            direction = RIGHT;
          }

          Node* new_node1 = node1->children[direction] = malloc(map->node_layout.size);

          if (new_node1 != NULL) {
            new_node1->children[LEFT] = NULL;
            new_node1->children[RIGHT] = NULL;
            new_node1->parent = node1;
            new_node1->direction = direction;
            new_node1->color = node0->children[direction]->color;

            memmove(
              Node_key(new_node1, &map->node_layout),
              Node_key(node0->children[direction], &map->node_layout),
              map->node_layout.key_size
            );

            memmove(
              Node_value(new_node1, &map->node_layout),
              Node_value(node0->children[direction], &map->node_layout),
              map->node_layout.value_size
            );

            node0 = node0->children[direction];
            node1 = new_node1;
          } else {  // if (new_node1 == NULL)
            map_clear(new_map);
            free(new_map);
            return NULL;
          }
        }
      } else {  // if (node1 == NULL)
        free(new_map);
        return NULL;
      }
    } else {  // if (map->root == NULL)
      new_map->root = NULL;
      new_map->count = 0;
    }
  }

  return new_map;
}

void map_clear(Map* map) {
  if (map->root != NULL) {
    Node* node = Node_xmost_leaf(map->root, LEFT);

    do {
      Node* post_order_successor = Node_post_order_xcessor(node, RIGHT);
      free(node);
      node = post_order_successor;
    } while (node != NULL);
  }

  map->root = NULL;
  map->count = 0;
}

void map_check(const Map* map) {
  assert(Node_is_black(map->root));
  Node_check(map->root);
  assert(Node_count(map->root) == map->count);
}
