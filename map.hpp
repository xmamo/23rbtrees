#ifndef MAP_HPP
#define MAP_HPP

#include <cassert>
#include <compare>
#include <concepts>
#include <cstddef>
#include <memory>
#include <stdexcept>

/// @brief Ordered map data type, associating keys to values
/// @tparam K The type of keys
/// @tparam V The type of values
template <typename K, typename V>
requires std::three_way_comparable<K, std::weak_ordering> && std::copyable<K> && std::copyable<V>
class Map {
  // This implementation is based on the algorithms for 2-3 red-black trees described in the
  // following paper: https://arxiv.org/abs/2004.04344. It is written to be hopefully more readable
  // and understandable than the implementation provided by the authors at
  // https://github.com/k-ghiasi/RedBlackTrees/blob/main/ParitySeeking-23RedBlackBST.h.
  //
  // Compared to the original implementation, no sentinel node has been used; furthermore, each node
  // holds additional information to determine if it is the left or right child of its parent.

  /// @brief Red-black color enumeration
  enum Color : unsigned char {
    BLACK = 0,
    RED = 1,
  };

  /// @brief Left-right direction enumeration
  enum Direction : unsigned char {
    LEFT = 0,
    RIGHT = 1,
  };

  /// @brief Red-black tree node data type
  struct Node {
    /// @brief The key stored by the node
    K key;

    /// @brief The value stored by the node
    V value;

    /// @brief The children of the node
    Node* children[2];

    /// @brief The parent of the node
    Node* parent;

    /// @brief The direction of the node, determining if it is the left or right child of its parent
    Direction direction;

    /// @brief The color of the node
    Color color;

    /// @brief Determines if a node is black
    /// @note @c nullptr is considered black
    static constexpr bool is_black(const Node* node) noexcept {
      return node == nullptr || node->color == BLACK;
    }

    /// @brief Determines if a node is red
    /// @note @c nullptr is considered black
    static constexpr bool is_red(const Node* node) noexcept {
      return node != nullptr && node->color == RED;
    }

    /// @brief Checks that a tree respects the invariants of 2-3 red-black trees
    /// @return The black depth of the tree
    /// @exception std::logic_error If an invariant is violated
    static std::size_t check(const Node* node) {
      if (node == nullptr)
        return 1;

      if (node->parent != nullptr) {
        if (node->parent->children[node->direction] != node)
          throw std::logic_error("node->parent->children[node->direction] != node");

        if (node->color == RED && node->parent->color == RED)
          throw std::logic_error("node->color == RED && node->parent->color == RED");
      }

      if (Node::is_red(node->children[LEFT]) && Node::is_red(node->children[RIGHT]))
        throw std::logic_error("Node::is_red(node->children[LEFT]) && Node::is_red(node->children[RIGHT])");

      std::size_t left_black_depth = Node::check(node->children[LEFT]);
      std::size_t right_black_depth = Node::check(node->children[RIGHT]);

      if (left_black_depth != right_black_depth)
        throw std::logic_error("Node::check(node->children[LEFT]) != Node::check(node->children[RIGHT])");

      return left_black_depth + (node->color == BLACK ? 1 : 0);
    }

    /// @brief Retrieves the leftmost or rightmost descendant of this node
    /// @param direction @c LEFT for the leftmost node, @c RIGHT for the rightmost node
    const Node* xmost_node(Direction direction) const noexcept {
      const Node* node = this;

      while (node->children[direction] != nullptr) {
        node = node->children[direction];
      }

      return node;
    }

    /// @brief Retrieves the leftmost or rightmost descendant of this node
    /// @param direction @c LEFT for the leftmost node, @c RIGHT for the rightmost node
    Node* xmost_node(Direction direction) noexcept {
      return const_cast<Node*>(const_cast<const Node*>(this)->xmost_node(direction));
    }

    /// @brief Retrieves the leftmost or rightmost leaf descending from this node
    /// @param direction @c LEFT for the leftmost leaf, @c RIGHT for the rightmost leaf
    const Node* xmost_leaf(Direction direction) const noexcept {
      const Node* node = this;

      while (true) {
        if (node->children[direction] != nullptr) {
          node = node->children[direction];
        } else if (node->children[1 - direction] != nullptr) {
          node = node->children[1 - direction];
        } else {
          return node;
        }
      }
    }

    /// @brief Retrieves the leftmost or rightmost leaf descending from this node
    /// @param direction @c LEFT for the leftmost leaf, @c RIGHT for the rightmost leaf
    Node* xmost_leaf(Direction direction) noexcept {
      return const_cast<Node*>(const_cast<const Node*>(this)->xmost_leaf(direction));
    }

    /// @brief Retrieves the post-order predecessor or successor of this node, if any
    /// @param direction @c LEFT for the post-order predecessor, @c RIGHT for the post-order successor
    const Node* post_order_xcessor(Direction direction) const noexcept {
      if (this->direction != direction && this->parent != nullptr && this->parent->children[direction] != nullptr) {
        return this->parent->children[direction]->xmost_leaf(static_cast<Direction>(1 - direction));
      } else {
        return this->parent;
      }
    }

    /// @brief Retrieves the post-order predecessor or successor of this node, if any
    /// @param direction @c LEFT for the post-order predecessor, @c RIGHT for the post-order successor
    Node* post_order_xcessor(Direction direction) noexcept {
      return const_cast<Node*>(const_cast<const Node*>(this)->post_order_xcessor(direction));
    }

    /// @brief Counts the number of nodes in a tree
    static std::size_t count(const Node* node) noexcept {
      std::size_t count = 0;

      if (node != nullptr) {
        node = node->xmost_leaf(LEFT);

        do {
          count += 1;
          node = node->post_order_xcessor(RIGHT);
        } while (node != nullptr);
      }

      return count;
    }

    /// @brief Rotates this tree
    /// @return The root of the now rotated tree
    /// @note It is the callee’s responsibility to update the relevant child pointer of the parent
    /// @pre `this->children[1 - direction] != nullptr`
    Node* rotate(Direction direction) noexcept {
      //       C                         A
      //     ┌╌┴╌┐         →B          ┌╌┴╌┐
      //    →B   d       ┌╌╌┴╌╌┐       a   B←
      //   ┌╌┴╌┐    ◁    A     C    ▷    ┌╌┴╌┐
      //   A   c       ┌╌┴╌┐ ┌╌┴╌┐       b   C
      // ┌╌┴╌┐         a   b c   d         ┌╌┴╌┐
      // a   b                             c   d

      Node* B = this;
      Node* CA = B->children[1 - direction];
      Node* parent = B->parent;
      Direction B_direction = B->direction;
      Color B_color = B->color;

      Node* cb = CA->children[direction];
      Color cb_color = CA->color;

      if (cb != nullptr) {
        cb->parent = B;
        cb->direction = static_cast<Direction>(1 - direction);
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
  };

  /// @brief The root of the red-black tree internal to the map
  Node* _root;

  /// @brief The number of key-value pairs stored by the map
  std::size_t _count;

public:
  /// @brief Constructs an empty map
  Map() noexcept : _root(nullptr), _count(0) {}

  Map(const Map& map) {
    if (map._root != nullptr) {
      const Node* node0 = map._root;

      Node* node1 = this->_root = new Node{
        node0->key,
        node0->value,
        {nullptr, nullptr},
        nullptr,
        node0->direction,
        node0->color,
      };

      while (true) {
        Direction direction;

        if (node0->children[LEFT] != nullptr) {
          direction = LEFT;
        } else if (node0->children[RIGHT] != nullptr) {
          direction = RIGHT;
        } else {
          while (node0->children[RIGHT] == nullptr || node1->children[RIGHT] != nullptr) {
            if (node0->parent == nullptr) {
              this->_count = map._count;
              return;
            }

            node0 = node0->parent;
            node1 = node1->parent;
          }

          direction = RIGHT;
        }

        node1->children[direction] = new Node{
          node0->children[direction]->key,
          node0->children[direction]->value,
          {nullptr, nullptr},
          node1,
          direction,
          node0->children[direction]->color,
        };

        node0 = node0->children[direction];
        node1 = node1->children[direction];
      }
    } else {  // if (map._root == nullptr)
      this->_root = nullptr;
      this->_count = 0;
    }
  }

  Map(Map&& other) noexcept : _root(other._root), _count(other._count) {
    other._root = nullptr;
    other._count = 0;
  }

  ~Map() noexcept {
    this->clear();
  }

  /// @brief Verifies that this map is valid: that is, that no internal invariants are violated
  /// @exception std::logic_error If an invariant is violated
  void check() const {
    if (Node::is_red(this->_root))
      throw std::logic_error("Node::is_red(this->_root)");

    Node::check(this->_root);

    if (Node::count(this->_root) != this->_count)
      throw std::logic_error("Node::count(this->_root) != this->_count");
  }

  /// @brief Returns the number of key-value pairs currently being stored by this map
  std::size_t count() const noexcept {
    return this->_count;
  }

  /// @brief Finds the value associated to a given key, if any
  const V* lookup(const K& key) const noexcept {
    const Node* node = this->_root;

    while (node != nullptr) {
      std::weak_ordering ordering = key <=> node->key;

      if (std::is_lt(ordering)) {
        node = node->children[LEFT];
      } else if (std::is_gt(ordering)) {
        node = node->children[RIGHT];
      } else {
        return std::addressof(node->value);
      }
    }

    return nullptr;
  }

  /// @brief Finds the value associated to a key, if any
  V* lookup(const K& key) noexcept {
    return const_cast<V*>(const_cast<const Map*>(this)->lookup(key));
  }

  /// @brief Associates a key to a value
  void insert(const K& key, const V& value) {
    // Top-down pass:

    Node* node = this->_root;
    Node* parent = nullptr;
    Direction node_direction = LEFT;

    while (node != nullptr) {
      std::weak_ordering ordering = key <=> node->key;

      if (std::is_lt(ordering)) {
        parent = node;
        node = node->children[node_direction = LEFT];
      } else if (std::is_gt(ordering)) {
        parent = node;
        node = node->children[node_direction = RIGHT];
      } else {
        node->value = value;
        return;
      }
    }

    node = new Node{
      key,
      value,
      {nullptr, nullptr},
      parent,
      node_direction,
      RED,
    };

    (parent != nullptr ? parent->children[node_direction] : this->_root) = node;
    this->_count += 1;

    // Bottom-up pass:

    while (node->parent != nullptr) {
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
          Node* B = node->rotate(node->direction);
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
        Node* B = node->parent->parent->rotate(static_cast<Direction>(1 - node->direction));
        (B->parent != nullptr ? B->parent->children[B->direction] : this->_root) = B;
      }

      if (Node::is_red(node->parent->children[1 - node->direction])) {
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
      } else {  // if (Node::is_black(node->parent->children[1 - node->direction]))
        break;
      }
    }

    this->_root->color = BLACK;
  }

  /// @brief Removes the value associated to a key, if any
  /// @return @c true if an association to the key existed prior to removal, @c false otherwise
  bool remove(const K& key) {
    // Top-down pass:

    Node* node = this->_root;

    while (true) {
      if (node != nullptr) {
        std::weak_ordering ordering = key <=> node->key;

        if (std::is_lt(ordering)) {
          node = node->children[LEFT];
        } else if (std::is_gt(ordering)) {
          node = node->children[RIGHT];
        } else {
          break;
        }
      } else {
        return false;
      }
    }

    if (node->children[LEFT] != nullptr && node->children[RIGHT] != nullptr) {
      Node* in_order_predecessor = node->children[LEFT]->xmost_node(RIGHT);
      node->key = in_order_predecessor->key;
      node->value = in_order_predecessor->value;
      node = in_order_predecessor;
    }

    Node* parent = node->parent;
    Direction node_direction = node->direction;
    Color node_color = node->color;

    for (std::size_t i = 0; i < 2; ++i) {
      if (node->children[i] != nullptr) {
        Node* child = node->children[i];
        child->parent = parent;
        child->direction = node_direction;
        child->color = node_color;
        delete node;
        (parent != nullptr ? parent->children[node_direction] : this->_root) = child;
        this->_count -= 1;
        return true;
      }
    }

    delete node;
    (parent != nullptr ? parent->children[node_direction] : this->_root) = nullptr;
    this->_count -= 1;

    // Bottom-up pass:

    if (node_color == RED || parent == nullptr)
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
        Node* DB = parent->rotate(node_direction);
        (DB->parent != nullptr ? DB->parent->children[DB->direction] : this->_root) = DB;
        sibling = parent->children[1 - node_direction];
      }

      //                    Rule from Figure 13b:
      //      B              →B       ╎       B←              B
      //   ┌──┴──┐         ┌──┶━━┓    ╎    ┏━━┵──┐         ┌──┴──┐
      //  →A     C    ▷    A     C    ╎    A     C    ◁    A     C←
      // ┌─┴─┐ ┌─┴─┐     ┌─┴─┐ ┌─┴─┐  ╎  ┌─┴─┐ ┌─┴─┐     ┌─┴─┐ ┌─┴─┐
      // a   b c   d     a   b c   d  ╎  a   b c   d     a   b c   d
      sibling->color = RED;

      if (Node::is_red(sibling->children[LEFT]) || Node::is_red(sibling->children[RIGHT])) {
        if (Node::is_black(sibling->children[sibling->direction])) {
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
          sibling = sibling->rotate(sibling->direction);
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
        Node* B = parent->rotate(node_direction);
        (B->parent != nullptr ? B->parent->children[B->direction] : this->_root) = B;

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
    } while (parent != nullptr && node->color == BLACK);

    // Rule from Figure 13a:
    //      ╻         ╷
    //      A    ▷    A
    //    ┌─┴─┐     ┌─┴─┐
    //    a   b     a   b
    node->color = BLACK;
    return true;
  }

  /// @brief Empties this map, clearing all associations from keys to values
  void clear() noexcept {
    if (this->_root != nullptr) {
      Node* node = this->_root->xmost_leaf(LEFT);

      do {
        Node* post_order_successor = node->post_order_xcessor(RIGHT);
        delete node;
        node = post_order_successor;
      } while (node != nullptr);
    }

    this->_root = nullptr;
    this->_count = 0;
  }
};

#endif
