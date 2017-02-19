/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (_node->_data); }
      T& operator * () { return (_node->_data); }
      iterator& operator ++ () {
         this->_node = this->_node->_next;
         return *(this);
      }
      iterator operator ++ (int) {
         iterator temp = *this;
         ++(*this);
         return temp;
      }
      iterator& operator -- () {
         this->_node = this->_node->_prev;
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
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); }
   bool empty() const { return _head->_next == _head; }
   size_t size() const {
      size_t result = 0;
      for(iterator i = begin(); i != end(); ++i){
         result++;
      }
      return result;
   }

   void push_back(const T& x) {
      if(empty()){
         _head->_next = new DListNode<T>(x, _head, _head);
         _head->_prev = _head->_next;
         _head = _head->_next;
      }
      else{
         DListNode<T>* newNode = new DListNode<T>(x, end()._node->_prev, end()._node);
         end()._node->_prev->_next = newNode;
         end()._node->_prev = newNode;
      }
   }
   void pop_front() {
      if(!empty()){
         DListNode<T>* _dummy = end()._node; //dummy node is found by _head->_prev
         _head = _head->_next;
         delete _head->_prev;
         _head->_prev = _dummy;
         _dummy->_next = _head;
      }
   }
   void pop_back() {
      if(!empty()){
         if(size() == 1) {
            _head = end()._node;
            delete _head->_next;
            _head->_next = _head->_prev = _head;
         }
         else{
            end()._node->_prev = end()._node->_prev->_prev;
            delete end()._node->_prev->_next;
            end()._node->_prev->_next = end()._node;
         }
      }
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if(empty()){ return false; }

      DListNode<T>* newPrev = pos._node->_prev;
      newPrev->_next = pos._node->_next;
      if(pos._node == _head){ _head = pos._node->_next; }
      delete pos._node;
      newPrev->_next->_prev = newPrev;
      return true;
   }

   bool erase(const T& x) {
      if(empty()) return false;
      for(iterator i = begin(); i != end(); ++i){
         if(i._node->_data == x){
            return erase(i);
         }
      }
      return false;
   }

   void clear() {  // delete all nodes except for the dummy node
      while(begin()._node != end()._node){
         pop_back();
      }
   }

   void sort(){ //ascending order
      /* BUBBLE SORT
      for(iterator curr_end = end(); curr_end != begin(); --curr_end){
         for(iterator i = begin(); i != curr_end; ++i){
            if(i._node->_data > i._node->_next->_data){
               T ori_next = i._node->_next->_data;
               i._node->_next->_data = i._node->_data;
               i._node->_data = ori_next;
            }
         }
      }
      */

      // MERGE SORT
      if(!empty()){ merge_sort(0, size()-1); }

   }
   //const method can only have const iterator (can we change _head->_data?)
   //can remove const
   //can sort just _data, or move ListNode

   void merge_sort(size_t left, size_t right){
      size_t mid;
      if(left < right){
         mid = (left + right) / 2;
         merge_sort(left, mid);
         merge_sort(mid+1, right); //mid is start of right list
         merge_merge(left, mid, right);
      }
      else{
         return;
      }
   }
   void merge_merge(size_t left, size_t mid, size_t right){
      //cout << left <<' ' << mid << ' ' << right << endl;
      if(right == left + 1){
         //cout << "test" << endl;

         iterator left_i = begin();
         iterator right_i = begin();
         for(int i = 0; i < left; ++i){
            ++left_i;
            ++right_i;
         }
         ++right_i;

         if(left_i._node->_data > right_i._node->_data){
            //cout << "in here" << endl;
            right_i._node->_prev = left_i._node->_prev;
            right_i._node->_prev->_next = right_i._node;
            left_i._node->_next = right_i._node->_next;
            left_i._node->_next->_prev = left_i._node;
            right_i._node->_next = left_i._node;
            left_i._node->_prev = right_i._node;
            if(left_i._node == _head){ _head = right_i._node;}
         }
      }

      else{
         size_t always_mid = mid;

         iterator left_i = begin();
         for(int i = 0; i < left; ++i){
            ++left_i;
         }

         iterator right_i = left_i;
         for(int i = 0; i < mid + 1 - left; ++i){
            ++right_i;
         }

         while(left <= always_mid && mid < right){
            if(left_i._node->_data > right_i._node->_data){
               iterator temp = right_i;
               ++right_i; ++mid;
               temp._node->_next = left_i._node;
               if(left_i._node == _head){ _head = temp._node;}
               temp._node->_prev = left_i._node->_prev;
               left_i._node->_prev = temp._node;
               temp._node->_prev->_next = temp._node;
            }
            else{
               if(left != always_mid){ ++left_i; }
               ++left;
            }
         }
         if(left > always_mid){ //left all gone
            left_i._node->_next = right_i._node;
            right_i._node->_prev = left_i._node;
         }
         else if(mid == right){ //right all gone
            while(left < always_mid) { ++left_i; ++left; }
            left_i._node->_next = right_i._node;
            right_i._node->_prev = left_i._node;
         }
      }
   }


private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
