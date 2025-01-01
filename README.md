This repository contains a template class Tree<T> that stores elements in an AVL tree. It provides the following methods:

size() – Returns the number of elements in the tree.
find(t) – Searches for the element t; returns a const pointer if found, or nullptr otherwise.
insert(t) – Attempts to insert element t; returns true if successful, or false if an element with the same value already exists.
erase(t) – Removes the element t from the tree; returns true if the element was found and removed, or false otherwise.
For testing purposes, the class also implements a TesterInterface with helper methods that expose the internal structure of the tree (e.g., root(), parent(), left(), right(), value()) for verifying AVL properties and binary search invariants.

Configuration
config::CHECK_DEPTH (default: false)
Enables full AVL shape checking when true. If false, only the binary search property is checked.
config::PARENT_POINTERS (default: true)
When true, tests parent pointers. If you do not store parent pointers and set this to false, you may remove or stub out the parent() method.
