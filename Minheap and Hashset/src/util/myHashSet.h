/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(vector<Data>* h_b = 0, int b_n = 0, int b_i = 0, int d_i = 0){
         hash_buckets = h_b; bucket_number = b_n; bucket_index = b_i; data_index = d_i;
      }
      iterator(const iterator& i){
         hash_buckets = i.hash_buckets; bucket_number = i.bucket_number; bucket_index = i.bucket_index; data_index = i.data_index;
      }
      ~iterator() { } //shouldn't be able to delete anything

      const Data& operator * () const { cout << "here" << endl; return hash_buckets[bucket_index][data_index]; } // a = *i
      Data& operator * () { return hash_buckets[bucket_index][data_index]; } // *i = a

      iterator& operator ++ () {
         while(data_index == hash_buckets[bucket_index].size() - 1 || hash_buckets[bucket_index].empty()){
            this->bucket_index++; this->data_index = 0;
            if(this->bucket_index == bucket_number || !hash_buckets[bucket_index].empty()) { return *(this); } //reached end(), bucket_index = bucket_number, data_index = 0
         }

         this->data_index++;
         return *(this);
     }
     iterator operator ++ (int) {
         iterator temp = *this;
         ++(*this);
         return temp;
     }

     iterator& operator -- () {
         if(data_index > 0){ this->data_index--; return *(this); }

         while(data_index == 0 || hash_buckets[bucket_index].size() == 0){
            this->bucket_index--; // what if -1
            if(bucket_index == -1) { break; } //reaches "before start()", in which case data_index may be -1
            this->data_index = hash_buckets[bucket_index].size() - 1; //what if size() == 0
         }
         return *(this);
     }
     iterator operator -- (int) {
         iterator temp = *this;
         --(*this);
         return temp;
     }

     iterator& operator = (const iterator& i) {
         this->hash_buckets = i.hash_buckets;
         this->bucket_number = i.bucket_number;
         this->bucket_index = i.bucket_index;
         this->data_index = i.data_index;
         return *(this);
     }

      bool operator != (const iterator& i) const { return this->hash_buckets != i.hash_buckets || this->bucket_number != i.bucket_number || this->bucket_index != i.bucket_index || this->data_index != i.data_index; }
      bool operator == (const iterator& i) const { return this->hash_buckets != i.hash_buckets && this->bucket_number != i.bucket_number && this->bucket_index != i.bucket_index && this->data_index != i.data_index; }

   private:
      vector<Data>* hash_buckets;
      int bucket_number;
      int bucket_index;
      int data_index;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; } //array of vectors
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear(); //clear is a vector function
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      iterator i(_buckets, _numBuckets, 0, 0);
      if(_buckets[0].empty()) { ++i; }
      return i;
   }
   // Pass the end
   iterator end() const { return iterator(_buckets, _numBuckets, _numBuckets, 0); }
   // return true if no valid data
   bool empty() const { return size() == 0; }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for(int i = 0; i < _numBuckets; i++){
         if(!_buckets[i].empty()){
            s += _buckets[i].size();
         }
      }
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t bucketnum = bucketNum(d);
      for(int i = 0; i < _buckets[bucketnum].size(); i++) {
         // the bucket [bucketnum] contains d
         if( _buckets[bucketnum][i].getName() == d.getName() ) { return true; }
      }
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t bucketnum = bucketNum(d);
      for(int i = 0; i < _buckets[bucketnum].size(); i++) {
         // the bucket [bucketnum] contains d
         if( _buckets[bucketnum][i].getName() == d.getName() ) {
            d += _buckets[bucketnum][i].getLoad() - d.getLoad();
            return true;
         }
      }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      size_t bucketnum = bucketNum(d);
      for(int i = 0; i < _buckets[bucketnum].size(); i++) {
         // the bucket [bucketnum] contains d
         if( _buckets[bucketnum][i].getName() == d.getName() ) {
            _buckets[bucketnum][i] += d.getLoad() - _buckets[bucketnum][i].getLoad();
            return true;
         }
      }
      _buckets[bucketnum].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      if(check(d)) { return false; }
      else{
         size_t bucketnum = bucketNum(d);
         _buckets[bucketnum].push_back(d);
         return true;
      }
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      size_t bucketnum = bucketNum(d);
      for(int i = 0; i < _buckets[bucketnum].size(); i++) {
         // the bucket [bucketnum] contains d
         if( _buckets[bucketnum][i].getName() == d.getName() ) {
            _buckets[bucketnum].erase(_buckets[bucketnum].begin()+i);
            return true;
         }
      }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
