#ifndef MULTIMAP_H_
#define MULTIMAP_H_

#include <stdexcept>
#include <vector>
#include <iostream>

template <typename Key, typename Value>
class Multimap {
 public:
  Multimap() : root_(nullptr) {}
  ~Multimap() { DeleteTree(root_); }

  bool Contains(const Key& key) const {
    return GetNode(key) != nullptr;
  }

  const Value& Get(const Key& key) const {
    Node* n = GetNode(key);
    if (!n || n->values.empty()) {
      throw std::runtime_error("Key not found");
    }
    return n->values[0];
  }

  void Insert(const Key& key, const Value& value) {
    root_ = Insert(root_, key, value);
    root_->color = BLACK;
  }

  void Remove(const Key& key) {
    if (!Contains(key)) return;
    if (!IsRed(root_->left) && !IsRed(root_->right)) root_->color = RED;
    root_ = Remove(root_, key);
    if (root_) root_->color = BLACK;
  }

  Key Min() const {
    Node* n = MinNode(root_);
    if (!n) throw std::runtime_error("Empty");
    return n->key;
  }

  Key Max() const {
    Node* n = MaxNode(root_);
    if (!n) throw std::runtime_error("Empty");
    return n->key;
  }

  void Print() const { Print(root_, 0); }

 private:
  enum Color { RED, BLACK };

  struct Node {
    Key key;
    std::vector<Value> values;
    Node* left;
    Node* right;
    Color color;
    Node(const Key& k, const Value& v)
        : key(k), values({v}), left(nullptr),
          right(nullptr), color(RED) {}
  };

  Node* root_;

  // Utility
  static bool IsRed(Node* n) { return n && n->color == RED; }

  static Node* RotateLeft(Node* h) {
    Node* x = h->right;
    h->right = x->left;
    x->left = h;
    x->color = h->color;
    h->color = RED;
    return x;
  }

  static Node* RotateRight(Node* h) {
    Node* x = h->left;
    h->left = x->right;
    x->right = h;
    x->color = h->color;
    h->color = RED;
    return x;
  }

  static void FlipColors(Node* h) {
    h->color = (h->color == RED ? BLACK : RED);
    if (h->left) h->left->color = (h->left->color == RED ? BLACK : RED);
    if (h->right) h->right->color = (h->right->color == RED ? BLACK : RED);
  }

  Node* Insert(Node* h, const Key& key, const Value& value) {
    if (!h) return new Node(key, value);

    if (key < h->key) h->left = Insert(h->left, key, value);
    else if (key > h->key) h->right = Insert(h->right, key, value);
    else {
      // MULTIMAP CHANGE — append new value
      h->values.push_back(value);
      return h;
    }

    if (IsRed(h->right) && !IsRed(h->left)) h = RotateLeft(h);
    if (IsRed(h->left) && IsRed(h->left->left)) h = RotateRight(h);
    if (IsRed(h->left) && IsRed(h->right)) FlipColors(h);

    return h;
  }

  Node* MoveRedLeft(Node* h) {
    FlipColors(h);
    if (IsRed(h->right->left)) {
      h->right = RotateRight(h->right);
      h = RotateLeft(h);
      FlipColors(h);
    }
    return h;
  }

  Node* MoveRedRight(Node* h) {
    FlipColors(h);
    if (IsRed(h->left->left)) {
      h = RotateRight(h);
      FlipColors(h);
    }
    return h;
  }

  Node* RemoveMin(Node* h) {
    if (!h->left) {
      delete h;
      return nullptr;
    }

    if (!IsRed(h->left) && !IsRed(h->left->left)) {
      h = MoveRedLeft(h);
    }

    h->left = RemoveMin(h->left);
    return FixUp(h);
  }

  Node* Remove(Node* h, const Key& key) {
    if (key < h->key) {
      if (h->left) {
        if (!IsRed(h->left) && !IsRed(h->left->left))
          h = MoveRedLeft(h);
        h->left = Remove(h->left, key);
      }
    } else {
      if (IsRed(h->left)) h = RotateRight(h);

      if (key == h->key && !h->right) {
        // MULTIMAP REMOVE LOGIC
        if (h->values.size() > 1) {
          h->values.erase(h->values.begin());
          return h;
        }
        delete h;
        return nullptr;
      }

      if (!IsRed(h->right) && !IsRed(h->right->left))
        h = MoveRedRight(h);

      if (key == h->key) {
        // MULTIMAP: remove only first value
        if (h->values.size() > 1) {
          h->values.erase(h->values.begin());
          return h;
        }
        Node* m = MinNode(h->right);
        h->key = m->key;
        h->values = m->values;
        h->right = RemoveMin(h->right);
      } else {
        h->right = Remove(h->right, key);
      }
    }
    return FixUp(h);
  }

  Node* FixUp(Node* h) {
    if (IsRed(h->right)) h = RotateLeft(h);
    if (IsRed(h->left) && IsRed(h->left->left)) h = RotateRight(h);
    if (IsRed(h->left) && IsRed(h->right)) FlipColors(h);
    return h;
  }

  Node* GetNode(const Key& key) const {
    Node* x = root_;
    while (x) {
      if (key < x->key) x = x->left;
      else if (key > x->key) x = x->right;
      else return x;
    }
    return nullptr;
  }

  Node* MinNode(Node* x) const {
    while (x && x->left) x = x->left;
    return x;
  }

  Node* MaxNode(Node* x) const {
    while (x && x->right) x = x->right;
    return x;
  }

  void Print(Node* h, int depth) const {
    if (!h) return;
    Print(h->right, depth + 1);
    for (int i = 0; i < depth; ++i) std::cout << "   ";
    std::cout << h->key << " (";
    for (size_t i = 0; i < h->values.size(); ++i) {
      std::cout << h->values[i];
      if (i + 1 < h->values.size()) std::cout << ",";
    }
    std::cout << ")" << std::endl;
    Print(h->left, depth + 1);
  }

  void DeleteTree(Node* h) {
    if (!h) return;
    DeleteTree(h->left);
    DeleteTree(h->right);
    delete h;
  }
};

#endif
