/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () {
         this->_node = this->_node + 1;
         return (*this);
      }  //++v   //do we have to worry about going over begin() and end()?
      iterator operator ++ (int) {
         iterator temp = *this;
         this->_node = this->_node + 1;
         return (temp);
      } //v++
      iterator& operator -- () {
         this->_node = this->_node - 1;
         return (*this);
      }
      iterator operator -- (int) {
         iterator temp = *this;
         this->_node = this->_node - 1;
         return (temp);
      }

      iterator operator + (int i) const {
         iterator temp = *this;
         temp._node += i;
         return (temp);
      }
      iterator& operator += (int i) {
         *this->_node += i;
         return *this;
      }

      iterator& operator = (const iterator& i) {
         this->_node = i.node;
         return (*this);
      }

      bool operator != (const iterator& i) const {
         return (this->_node != i._node);
      }
      bool operator == (const iterator& i) const {
         return (this->_node == i.node);
      }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data + _size); }
   bool empty() const { return _size == 0; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return *(_data + i); }
   const T& operator [] (size_t i) const { return *(_data + i); }

   void push_back(const T& x) {
      if(_capacity == 0){
         _capacity = 1;
         _data = new T(x);
         _size++;
      }
      else{
         if(_size >= _capacity){
            T* new_data = new T[2 * _capacity];
            for(size_t i = 0; i < _size; ++i){
               *(new_data + i) = *(_data + i);
            }
            if(_size == 1) delete _data;
            else delete[]_data;
            _data = new_data;
            _capacity *= 2;
         }
         *(_data + _size) = x;
         _size++;
      }
   }
   void pop_front() {
      if(!empty()){
         if(size() == 1){
            //cout << "size == 1" << endl;
            delete _data;
            _data = 0;
            _size--;
         }
         else{
            *_data = *(_data + _size - 1);
            _size--;//no need to delete original ending T, as no need to free memory
         }
      }
   }
   void pop_back() {
      if(!empty()){
         _size--; //no need to delete, as no need to free memory
      }
   }

   bool erase(iterator pos) {
      if(empty()) return false;
      *(pos._node) = *((end() + (-1))._node);
      pop_back();
      return true;
   }

   bool erase(const T& x) {
      if(empty()) { return false; }
      for(iterator i = begin(); i != end(); ++i){
         if(*(i._node) == x){
            return erase(i);
         }
      }
      return false;
   }

   void clear() { //don't release memory!!!
      _size = 0;
   }

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
