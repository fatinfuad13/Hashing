#pragma once
#include <string>
#include <iostream>
using namespace std;

class Node {
public:
    string key;
    int stock;
    bool color; // 1 = red, 0 = black
    Node* left;
    Node* right;
    Node* parent;

    Node(const string& key, int stock = 0)
        : key(key), stock(stock), color(1), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RedBlackTree {
private:
    Node* root;
    Node* TNULL;
    int nodeCount;

    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != TNULL)
            y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == TNULL)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right != TNULL)
            x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == TNULL)
            root = x;
        else if (y == y->parent->right)
            y->parent->right = x;
        else
            y->parent->left = x;
        x->right = y;
        y->parent = x;
    }

    void fixInsert(Node* k) {
        Node* u;
        while (k->parent->color == 1) {
            if (k->parent == k->parent->parent->right) {
                u = k->parent->parent->left;
                if (u->color == 1) {
                    u->color = 0;
                    k->parent->color = 0;
                    k->parent->parent->color = 1;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->left) {
                        k = k->parent;
                        rightRotate(k);
                    }
                    k->parent->color = 0;
                    k->parent->parent->color = 1;
                    leftRotate(k->parent->parent);
                }
            } else {
                u = k->parent->parent->right;

                if (u->color == 1) {
                    u->color = 0;
                    k->parent->color = 0;
                    k->parent->parent->color = 1;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->right) {
                        k = k->parent;
                        leftRotate(k);
                    }
                    k->parent->color = 0;
                    k->parent->parent->color = 1;
                    rightRotate(k->parent->parent);
                }
            }
            if (k == root)
                break;
        }
        root->color = 0;
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == TNULL)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        v->parent = u->parent;
    }

    Node* minimum(Node* node) {
        while (node->left != TNULL)
            node = node->left;
        return node;
    }

    void fixDelete(Node* x) {
        Node* s;
        while (x != root && x->color == 0) {
            if (x == x->parent->left) {
                s = x->parent->right;
                if (s->color == 1) {
                    s->color = 0;
                    x->parent->color = 1;
                    leftRotate(x->parent);
                    s = x->parent->right;
                }
                if (s->left->color == 0 && s->right->color == 0) {
                    s->color = 1;
                    x = x->parent;
                } else {
                    if (s->right->color == 0) {
                        s->left->color = 0;
                        s->color = 1;
                        rightRotate(s);
                        s = x->parent->right;
                    }
                    s->color = x->parent->color;
                    x->parent->color = 0;
                    s->right->color = 0;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                s = x->parent->left;
                if (s->color == 1) {
                    s->color = 0;
                    x->parent->color = 1;
                    rightRotate(x->parent);
                    s = x->parent->left;
                }
                if (s->left->color == 0 && s->right->color == 0) {
                    s->color = 1;
                    x = x->parent;
                } else {
                    if (s->left->color == 0) {
                        s->right->color = 0;
                        s->color = 1;
                        leftRotate(s);
                        s = x->parent->left;
                    }
                    s->color = x->parent->color;
                    x->parent->color = 0;
                    s->left->color = 0;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = 0;
    }

    void clear(Node* node) {
        if (node == TNULL) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    RedBlackTree() {
        TNULL = new Node("");
        TNULL->color = 0;
        TNULL->left = TNULL->right = TNULL->parent = TNULL;
        root = TNULL;
        nodeCount = 0;
    }

    ~RedBlackTree() {
        clear(root);
        delete TNULL;
    }

    void insertNode(const string& key, const string& value, int stock = 0) {
        Node* node = new Node(key, stock);
        node->parent = node->left = node->right = TNULL;
        node->color = 1;

        Node* y = TNULL;
        Node* x = root;

        while (x != TNULL) {
            y = x;
            if (node->key < x->key)
                x = x->left;
            else if (node->key > x->key)
                x = x->right;
            else {
                delete node;
                return; // no duplicate
            }
        }

        node->parent = y;
        if (y == TNULL)
            root = node;
        else if (node->key < y->key)
            y->left = node;
        else
            y->right = node;

        nodeCount++;
        fixInsert(node);
    }

    void deleteNode(const string& key) {
        Node* z = root;
        while (z != TNULL) {
            if (key < z->key)
                z = z->left;
            else if (key > z->key)
                z = z->right;
            else
                break;
        }

        if (z == TNULL) return;

        Node* y = z;
        Node* x;
        int yOriginalColor = y->color;

        if (z->left == TNULL) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == TNULL) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == z)
                x->parent = y;
            else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        delete z;
        nodeCount--;

        if (yOriginalColor == 0)
            fixDelete(x);
    }

    Node* searchTree(const string& key) {
        Node* node = root;
        while (node != TNULL) {
            if (key < node->key)
                node = node->left;
            else if (key > node->key)
                node = node->right;
            else
                return node;
        }
        return TNULL;
    }

    Node* getSentinelNode() const {
        return TNULL;
    }

    int getNodeCount() const {
        return nodeCount;
    }
};
