#ifndef __PROGTEST__
#include <cassert>
#include <cstdarg>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <array>
#include <random>
#include <type_traits>

// We use std::set as a reference to check our implementation.
// It is not available in progtest :)
#include <set>

template < typename T >
struct Ref {
    size_t size() const { return _data.size(); }
    const T* find(const T& value) const {
        auto it = _data.find(value);
        if (it == _data.end()) return nullptr;
        return &*it;
    }
    bool insert(const T& value) { return _data.insert(value).second; }
    bool erase(const T& value) { return _data.erase(value); }

    auto begin() const { return _data.begin(); }
    auto end() const { return _data.end(); }

private:
    std::set<T> _data;
};

#endif


namespace config {
    // Enable to check that the tree is AVL balanced.
    inline constexpr bool CHECK_DEPTH = false;

    // Disable if your implementation does not have parent pointers
    inline constexpr bool PARENT_POINTERS = true;
}

template <typename T>
struct Node{

    T value;
    Node<T>* left = nullptr;
    Node<T>* right = nullptr;
    Node<T>* parent = nullptr;
    int m_depth = 0;

    Node() = default;

    Node<T>* BVSInsert(Node<T>*& node, const T& key) {
        if (node == nullptr) {
            node = new Node<T>();
            node->value = key;
            node->m_depth = 1;
            return node;
        }

        if (key < node->value) {
            node->left = BVSInsert(node->left, key);
            node->left->parent = node;
        } else if (key > node->value) {
            node->right = BVSInsert(node->right, key);
            node->right->parent = node;
        } else {
            // Key already exists, do not insert
            return node;
        }

        // Update depth
        int left_depth = node->left ? node->left->m_depth : 0;
        int right_depth = node->right ? node->right->m_depth : 0;
        node->m_depth = std::max(left_depth, right_depth) + 1;

        // Calculate balance factor
        int balance = right_depth - left_depth;

        // Balancing
        if (balance == 2) {
            int right_balance = (node->right->right ? node->right->right->m_depth : 0) - (node->right->left ? node->right->left->m_depth : 0);
            if (right_balance > 0) {
                RotateLeft(&node);
            } else {
                RotateRight(&(node->right));
                RotateLeft(&node);
            }
        } else if (balance ==-2 ) {
            int left_balance = (node->left->right ? node->left->right->m_depth : 0) - (node->left->left ? node->left->left->m_depth : 0);
            if (left_balance < 0) {
                RotateRight(&node);
            } else {
                RotateLeft(&(node->left));
                RotateRight(&node);
            }
        }

        return node;
    }


    void RotateLeft(Node<T>** node) {
        Node<T>* x = *node;
        Node<T>* y = x->right;

        x->right = y->left;
        if (x->right != nullptr) {
            x->right->parent = x;
        }

        y->left = x;
        y->parent = x->parent;

        if (x->parent != nullptr) {
            if (x->parent->left == x) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
        }

        x->parent = y;
        *node = y;

        // Update depths
        x->m_depth = std::max(
                x->left ? x->left->m_depth : 0,
                x->right ? x->right->m_depth : 0
        ) + 1;

        y->m_depth = std::max(
                y->left ? y->left->m_depth : 0,
                y->right ? y->right->m_depth : 0
        ) + 1;
    }



    void RotateRight(Node<T>** node) {
        Node<T>* x = *node;
        Node<T>* y = x->left;

        x->left = y->right;
        if (x->left != nullptr) {
            x->left->parent = x;
        }

        y->right = x;
        y->parent = x->parent;

        if (x->parent != nullptr) {
            if (x->parent->left == x) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
        }

        x->parent = y;
        *node = y;

        // Update depths
        x->m_depth = std::max(
                x->left ? x->left->m_depth : 0,
                x->right ? x->right->m_depth : 0
        ) + 1;

        y->m_depth = std::max(
                y->left ? y->left->m_depth : 0,
                x->m_depth
        ) + 1;
    }


    void BVSShow(const Node<T>* node, std::ostream& os) {
        if (node == nullptr) {
            return;
        }
        BVSShow(node->left, os);
        os << " " << node->value;
        BVSShow(node->right, os);
    }

    Node<T>* BVSMin(Node<T>* node) {
        if (node == nullptr) {
            return nullptr;
        }
        if (node->left == nullptr) {
            return node;
        }
        return BVSMin(node->left);
    }

    Node<T>* BVSMax(const Node<T>* node) {
        if (node == nullptr) {
            return nullptr;
        }
        if (node->right == nullptr) {
            return node;
        }
        return BVSMax(node->right);
    }

    Node<T>* BVSFind(Node<T>* node, const T& key) {
        if (node == nullptr) {
            return nullptr;
        }

        if (node->value == key) {
            return node;
        }

        if (key < node->value) {
            return BVSFind(node->left, key);
        } else if (key > node->value) {
            return BVSFind(node->right, key);
        }
        return nullptr;
    }

    Node<T>* BVSDelete(Node<T>*& node, const T& key) {
        if (node == nullptr) {
            return nullptr;
        }

        if (key > node->value) {
            node->right = BVSDelete(node->right, key);
            if (node->right != nullptr) {
                node->right->parent = node;
            }
        } else if (key < node->value) {
            node->left = BVSDelete(node->left, key);
            if (node->left != nullptr) {
                node->left->parent = node;
            }
        } else {
            // Node to be deleted found
            if (node->left == nullptr && node->right == nullptr) {
                // No child
                delete node;
                node = nullptr;
            } else if (node->left == nullptr) {
                // One child (right)
                Node<T>* temp = node->right;
                temp->parent = node->parent;
                delete node;
                node = temp;
            } else if (node->right == nullptr) {
                // One child (left)
                Node<T>* temp = node->left;
                temp->parent = node->parent;
                delete node;
                node = temp;
            } else {
                // Two children
                Node<T>* rightmin = BVSMin(node->right);
                node->value = rightmin->value;
                node->right = BVSDelete(node->right, rightmin->value);
                if (node->right != nullptr) {
                    node->right->parent = node;
                }
            }
        }

        if (node == nullptr) {
            return node;
        }

        // Update depth
        int left_depth = node->left ? node->left->m_depth : 0;
        int right_depth = node->right ? node->right->m_depth : 0;
        node->m_depth = std::max(left_depth, right_depth) + 1;

        // Calculate balance factor
        int balance = right_depth - left_depth;

        // Balancing
        if (balance > 1) {
            // Right heavy
            int right_balance = (node->right->right ? node->right->right->m_depth : 0) - (node->right->left ? node->right->left->m_depth : 0);
            if (right_balance >= 0) {
                // Right-Right case
                RotateLeft(&node);
            } else {
                // Right-Left case
                RotateRight(&(node->right));
                RotateLeft(&node);
            }
        } else if (balance < -1) {
            // Left heavy
            int left_balance = (node->left->right ? node->left->right->m_depth : 0) - (node->left->left ? node->left->left->m_depth : 0);
            if (left_balance <= 0) {
                // Left-Left case
                RotateRight(&node);
            } else {
                // Left-Right case
                RotateLeft(&(node->left));
                RotateRight(&node);
            }
        }

        return node;
    }


};

template < typename T >
struct Tree {
    size_t size() const{
        return this->m_size;
    }
    const T* find(const T& value) const;
    bool insert(T value){
        if(find(value) != nullptr){
            return false;
        }

        root->BVSInsert(root, value);
        m_size++;
        return true;
    }
    bool erase(const T& value){
        if(find(value) == nullptr){
            return false;
        }
        root->BVSDelete(root, value);
        m_size--;
        return true;
    }

    void print(){
        root->BVSShow(root, std::cout);
        std::cout<<std::endl;
    }

    // Needed to test the structure of the tree.
    // Replace Node with the real type of your nodes
    // and implementations with the ones matching
    // your attributes.
    struct TesterInterface {
        // using Node = ...
        static const Node<T> *root(const Tree *t) { return t->root; }
        // Parent of root must be nullptr, ignore if config::PARENT_POINTERS == false
        static const Node<T> *parent(const Node<T> *n) { return n->parent; }
        static const Node<T> *right(const Node<T> *n) { return n->right; }
        static const Node<T> *left(const Node<T> *n) { return n->left; }
        static const T& value(const Node<T> *n) { return n->value; }
    };

    Tree(){
        root = nullptr;
        m_size = 0;
    }





    Node<T> * root;
    size_t m_size;

    ~Tree() {
        destroySubtree(root);
        root = nullptr;
        m_size = 0;
    }

private:
    void destroySubtree(Node<T>* node) {
        if (node == nullptr) {
            return;
        }
        destroySubtree(node->left);
        destroySubtree(node->right);
        delete node;
    }

};

template <typename T>
const T* Tree<T>::find(const T& value) const{

        Node<T> * res = root->BVSFind(root, value);
         return &(res->value);
}



#ifndef __PROGTEST__

struct TestFailed : std::runtime_error {
    using std::runtime_error::runtime_error;
};

std::string fmt(const char *f, ...) {
    va_list args1;
    va_list args2;
    va_start(args1, f);
    va_copy(args2, args1);

    std::string buf(vsnprintf(nullptr, 0, f, args1), '\0');
    va_end(args1);

    vsnprintf(buf.data(), buf.size() + 1, f, args2);
    va_end(args2);

    return buf;
}

template < typename T >
struct Tester {
    Tester() = default;

    void size() const {
        size_t r = ref.size();
        size_t t = tested.size();
        if (r != t) throw TestFailed(fmt("Size: got %zu but expected %zu.", t, r));
    }

    void find(const T& x) const {
        auto r = ref.find(x);
        auto t = tested.find(x);
        bool found_r = r != nullptr;
        bool found_t = t != nullptr;

        if (found_r != found_t) _throw("Find mismatch", found_r);
        if (found_r && *t != x) throw TestFailed("Find: found different value");
    }

    void insert(const T& x, bool check_tree_ = false) {
        auto succ_r = ref.insert(x);
        auto succ_t = tested.insert(x);

        if (succ_r != succ_t) _throw("Insert mismatch", succ_r);
        size();
        if (check_tree_) check_tree();
    }

    void erase(const T& x, bool check_tree_ = false) {
        bool succ_r = ref.erase(x);
        auto succ_t = tested.erase(x);

        //tested.print();
        if (succ_r != succ_t) _throw("Erase mismatch", succ_r);
        size();
        if (check_tree_) check_tree();
    }

    struct NodeCheckResult {
        const T* min = nullptr;
        const T* max = nullptr;
        int depth = -1;
        size_t size = 0;
    };

    void check_tree() const {
        using TI = typename Tree<T>::TesterInterface;
        auto ref_it = ref.begin();
        bool check_value_failed = false;
        auto check_value = [&](const T& v) {
            if (check_value_failed) return;
            check_value_failed = (ref_it == ref.end() || *ref_it != v);
            if (!check_value_failed) ++ref_it;
        };

        auto r = check_node(TI::root(&tested), decltype(TI::root(&tested))(nullptr), check_value);
        size_t t_size = tested.size();

        if (t_size != r.size) throw TestFailed(
                    fmt("Check tree: size() reports %zu but expected %zu.", t_size, r.size));

        if (check_value_failed) throw TestFailed(
                    "Check tree: element mismatch");

        size();
    }

    template < typename Node, typename F >
    NodeCheckResult check_node(const Node* n, const Node* p, F& check_value) const {
        if (!n) return {};

        using TI = typename Tree<T>::TesterInterface;
        if constexpr(config::PARENT_POINTERS) {
            if (TI::parent(n) != p) throw TestFailed("Parent mismatch.");
        }

        auto l = check_node(TI::left(n), n, check_value);
        check_value(TI::value(n));
        auto r = check_node(TI::right(n), n, check_value);

        if (l.max && !(*l.max < TI::value(n)))
            throw TestFailed("Max of left subtree is too big.");
        if (r.min && !(TI::value(n) < *r.min))
            throw TestFailed("Min of right subtree is too small.");

        if (config::CHECK_DEPTH && abs(l.depth - r.depth) > 1) throw TestFailed(fmt(
                    "Tree is not avl balanced: left depth %i and right depth %i.",
                    l.depth, r.depth
            ));

        return {
                l.min ? l.min : &TI::value(n),
                r.max ? r.max : &TI::value(n),
                std::max(l.depth, r.depth) + 1, 1 + l.size + r.size
        };
    }

    static void _throw(const char *msg, bool s) {
        throw TestFailed(fmt("%s: ref %s.", msg, s ? "succeeded" : "failed"));
    }

    Tree<T> tested;
    Ref<T> ref;
};


void test_insert() {
    Tester<int> t;


    for (int i = 0; i < 10; i++){
        t.insert(i, true);

    }

    for (int i = -10; i < 20; i++) t.find(i);

    for (int i = 0; i < 10; i++) t.insert((1 + i * 7) % 17, true);
    for (int i = -10; i < 20; i++) t.find(i);
}

void test_erase() {
    Tester<int> t;

    for (int i = 0; i < 10; i++) t.insert((1 + i * 7) % 17, true);
    for (int i = -10; i < 20; i++) t.find(i);

    for (int i = 3; i < 22; i += 2) t.erase(i, true);
    for (int i = -10; i < 20; i++) t.find(i);

    for (int i = 0; i < 10; i++) t.insert((1 + i * 13) % 17 - 8, true);
    for (int i = -10; i < 20; i++) t.find(i);

    for (int i = -4; i < 10; i++) t.erase(i, true);
    for (int i = -10; i < 20; i++) t.find(i);
}

enum RandomTestFlags : unsigned {
    SEQ = 1, NO_ERASE = 2, CHECK_TREE = 4
};

void test_random(size_t size, unsigned flags = 0) {
    Tester<size_t> t;
    std::mt19937 my_rand(24707 + size);

    bool seq = flags & SEQ;
    bool erase = !(flags & NO_ERASE);
    bool check_tree = flags & CHECK_TREE;

    for (size_t i = 0; i < size; i++)
        t.insert(seq ? 2*i : my_rand() % (3*size), check_tree);

    t.check_tree();

    for (size_t i = 0; i < 3*size + 1; i++) t.find(i);

    for (size_t i = 0; i < 30*size; i++) switch (my_rand() % 5) {
            case 1: t.insert(my_rand() % (3*size), check_tree);
                break;
            case 2: if (erase) t.erase(my_rand() % (3*size), check_tree);
                break;
            default:
                t.find(my_rand() % (3*size));
        }

    t.check_tree();
}

int main() {
    try {
        std::cout << "Insert test..." << std::endl;
        test_insert();

        std::cout << "Erase test..." << std::endl;
        test_erase();

        std::cout << "Tiny random test..." << std::endl;
        test_random(20, CHECK_TREE);

        std::cout << "Small random test..." << std::endl;
        test_random(200, CHECK_TREE);

        std::cout << "Big random test..." << std::endl;
        test_random(50'000);

        std::cout << "Big sequential test..." << std::endl;
        test_random(50'000, SEQ);

        std::cout << "All tests passed." << std::endl;
    } catch (const TestFailed& e) {
        std::cout << "Test failed: " << e.what() << std::endl;
    }
}

#endif


