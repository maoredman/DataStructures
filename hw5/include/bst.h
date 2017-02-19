/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;
    BSTreeNode(BSTreeNode<T>* p){
        _parent = p;
    }
    BSTreeNode(const T& d, bool b, BSTreeNode<T>* p, bool c = false, bool e = false, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0){
      _data = d; right_parent = b; is_head = c; is_end = e; _parent = p; _left = l; _right = r;
      //cout << "initialize bstreenode" << endl;
    }

    T _data;
    bool right_parent; //requires specification
    bool is_head;
    bool is_end;
    BSTreeNode<T>* _parent; //requires specification
    //BSTreeNode<T>* _Node_endNode; // requires specification, passed in from BSTree
    BSTreeNode<T>* _left;
    BSTreeNode<T>* _right;

    BSTreeNode<T>* findmin(BSTreeNode<T>* _node) const {
        if(_node->_left != 0){ return findmin(_node->_left); }
        else { return _node; }
    }

    BSTreeNode<T>* findmax(BSTreeNode<T>* _node) const {
        if(_node->_right != 0){ return findmax(_node->_right); }
        else { return _node; }
    }

    BSTreeNode<T>* findnext(BSTreeNode<T>* _node, bool dont_right = false){
        //if has right, return right. else if right parent. else if left parent's right parent.
        if(_node->is_head && dont_right) { return _node->_parent; }

        if(_node->_right != 0 && !dont_right) { return findmin(_node->_right); }
        else if(_node->right_parent) { return _node->_parent; }
        else{ return findnext(_node->_parent, true); }
    }

    BSTreeNode<T>* findprev(BSTreeNode<T>* _node, bool dont_left = false){
        //if has left child, return that. else if
        //cout << "using this" << endl;
        if(_node->is_end) { return _node->_parent; }

        if(_node->_left != 0 && !dont_left) { return findmax(_node->_left); }
        else if(!_node->right_parent) { return _node->_parent; }
        else{ return findprev(_node->_parent, true); }
    }

};


template <class T>
class BSTree
{
   // TODO: design your own class!!

public:
    BSTree(){
        _size = 0; _head = 0; _endNode = new BSTreeNode<T>(T(), true, 0, false, true, 0, 0); //cout << "initialize bstree" << endl;
    }
    ~BSTree() { };

    class iterator {

        friend class BSTree;

    public:
        iterator(BSTreeNode<T>* n= 0): _node(n) {}
        iterator(const iterator& i) : _node(i._node) {}
        ~iterator() {} // Should NOT delete _node

        const T& operator * () const { return (_node->_data); }
        T& operator * () { return (_node->_data); }

        iterator& operator ++ () {
            this->_node = this->_node->findnext(this->_node);
            return *(this);
        }
        iterator operator ++ (int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator& operator -- () {
            this->_node = this->_node->findprev(this->_node);
            return *(this);
        }
        iterator operator -- (int) {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        iterator& operator = (const iterator& i) {
         this->_node = i._node;
         return *(this);
        }

        bool operator != (const iterator& i) const { return this->_node != i._node; }
        bool operator == (const iterator& i) const { return this->_node == i._node; }


    private:
        BSTreeNode<T>* _node;
    };

    iterator begin() const {
        if(empty()) { return end(); }
        else{
            return iterator(_head->findmin(_head));
        }
    }

    iterator end() const {
        //cout << "using end()" << endl;
        return iterator(_endNode);
    }

    bool empty() const { return (_size == 0); }
    size_t size() const { return _size; }

    void insert(const T& x){//remember to set is_head for first node
        if(empty()){
            _head = new BSTreeNode<T>(x, true, _endNode, true);
            _endNode->_parent = _head;
            ++_size;
        }
        else{
            BSTreeNode<T>* temp = _head;
            bool right_parent;
            bool not_max = false;
            while(true){
                if(x <= temp->_data){
                    not_max = true;
                    right_parent = true;
                    if(temp->_left == 0){
                        temp->_left = new BSTreeNode<T>(x, right_parent, temp);
                        break;
                    }
                    temp = temp->_left;
                }
                else{
                    right_parent = false;
                    if(temp->_right == 0){
                        temp->_right = new BSTreeNode<T>(x, right_parent, temp);
                        if(not_max == false){ //inserting new max!
                            _endNode->_parent = temp->_right;
                            //cout << "new max is " << x << endl;
                        }
                        break;
                    }
                    temp = temp->_right;
                }
            }
            ++_size;
        }
        //cout << "max is now"
    }

    bool erase(iterator pos){
        if(empty()) { return false; }

        if(!( (pos._node->_left == 0 && pos._node->_right == 0) || (pos._node->_left != 0 && pos._node->_right != 0) )){ //one child case
            if(pos._node->right_parent){
                if(pos._node->_left == 0){
                    if(pos._node == _head){
                        _head = _head->_right;
                        _head->_parent = _endNode;
                        _head->right_parent = true;
                        _head->is_head = true;
                    }
                    else{
                        pos._node->_right->right_parent = true;

                        pos._node->_parent->_left = pos._node->_right;
                        pos._node->_right->_parent = pos._node->_parent;

                        assert(_endNode->_parent != pos._node);
                    }
                }
                else {
                    if(pos._node == _head){
                        _head = _head->_left;
                        _head->_parent = _endNode;
                        _head->right_parent = true;
                        _head->is_head = true;
                        if(_endNode->_parent == pos._node){
                            _endNode->_parent = _head->findmax(_head);
                        }
                    }
                    else{
                        pos._node->_parent->_left = pos._node->_left;
                        pos._node->_left->_parent = pos._node->_parent;
                    }
                }
            }
            else{ //cannot be _head, because _head->right_parent == true
                if(pos._node->_left == 0){
                    pos._node->_parent->_right = pos._node->_right;
                    pos._node->_right->_parent = pos._node->_parent;

                    pos._node->_right->right_parent = pos._node->right_parent;
                }
                else { //POP BACK SCENARIO HERE!!!
                    pos._node->_parent->_right = pos._node->_left;
                    pos._node->_left->_parent = pos._node->_parent;

                    if(_endNode->_parent == pos._node){ //in leaf case, if pos._node is max, right_parent == false
                        _endNode->_parent = pos._node->findmax(pos._node->_left);
                    }

                    pos._node->_left->right_parent = pos._node->right_parent;
                }
            }
            delete pos._node;
        }
        else if(pos._node->_left == 0 && pos._node->_right == 0){ //leaf case

            if(pos._node == _head) {
                _head = 0;
                _endNode->_parent = 0;
            }
            else{
                if(pos._node->right_parent) { pos._node->_parent->_left = 0; }
                else { pos._node->_parent->_right = 0; } //"seal off parent's left/right"

                if(_endNode->_parent == pos._node){ //in leaf case, if pos._node is max, right_parent == false
                    _endNode->_parent = pos._node->_parent;
                }
                //cout << "ok" << endl;
            }
            delete pos._node;
        }
        else{ //two child case
            BSTreeNode<T>* succ = pos._node->findmin(pos._node->_right);
            if(succ->right_parent == true) {
                succ->_parent->_left = 0; //cut off succ

                succ->right_parent = pos._node->right_parent;

                succ->_parent = pos._node->_parent;
                if(succ->right_parent) { succ->_parent->_left = succ; }
                else { succ->_parent->_right = succ; } //connected to parent

                succ->is_head = pos._node->is_head;

                assert(succ->is_end == false);
                assert(succ->_left == 0);

                succ->_left = pos._node->_left;
                succ->_left->_parent = succ; //connected to left child

                pos._node->_right->_parent = succ->findmax(succ);
                succ->findmax(succ)->_right = pos._node->_right; //connected to right child

                if(_endNode->_parent == pos._node) { _endNode->_parent = succ; }
                delete pos._node;
            }
            else{
                succ->right_parent = pos._node->right_parent;

                succ->_parent = pos._node->_parent;
                if(succ->right_parent) { succ->_parent->_left = succ; }
                else { succ->_parent->_right = succ; } //connected to parent

                succ->_left = pos._node->_left;
                succ->_left->_parent = succ; //connected to left child

                if(_endNode->_parent == pos._node) { _endNode->_parent = succ; }
                delete pos._node;
            }
        }

        --_size;
        if(_size == 1) { _endNode->_parent = _head; }
        else if(_size == 0) { _endNode->_parent = 0; }
        return true;
    }
    bool erase(const T& x){
        if(empty()) { return false; }

        for(iterator i = begin(); i != end(); ++i){
            if(i._node->_data == x) { return erase(i); }
        }
        return false;
    }

    void pop_front() {
        if(!empty()){
            erase(begin());
        }
    }
    void pop_back() {
        if(!empty()){
            //cout << "max is " << end()._node->_parent->_data << endl;
            erase( iterator(end()._node->_parent) );
        }
    }

    void clear() {
        for(iterator i = begin(); i != end(); ++i) { erase(i); }
    }
    void print(){ };
    void sort() { }; //already sorted XDD

private:
    size_t _size;
    BSTreeNode<T>* _head;
    BSTreeNode<T>* _endNode;



};

#endif // BST_H
