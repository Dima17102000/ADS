#ifndef ADS_SET_H
#define ADS_SET_H

#include <iostream>
#include <iomanip>

using std::size_t;
 
template <typename Key, size_t N = 3>

class Bucket
{
 private:
 Key* values;
 size_t size;
 size_t local_depth;
 size_t max_size;
 public:
 Bucket(size_t initial_local_depth): values(new Key [N]),max_size(N),local_depth(initial_local_depth),size(0){}
 
 ~Bucket()
 {
  delete[] values;
 }
 
 
 
 void insert (Key key)
 {
   if(contains(key))
   {
    return;
   }
   
   if(size < max_size)
   {
    values[size++] = key;
   }
   
 }
 
 size_t get_capacity()//size_t bucket_size)
 // this method must return a max_size of bucket, like arguments it size_t bucket_size and returns it's size
 {
  return max_size;
 }
 
 friend std::ostream& operator<<(std::ostream& os,const Bucket& b)
 {
  //os << "Bucket :";
  for(size_t i{0}; i < b.max_size; i++)
  {
   os << "------";
  }
  os << "-\n|"; 
  
  for(size_t i{0}; i < b.max_size; i++)
  {
   
   if(i >= b.size)
   {
    os << std::setw(5) << "" << "|"; 
   }
   else
   {
    os << std::setw(5) << b.values[i] << "|";
   }
  }
  return os;
 }
 
 bool contains(Key key)
 {
  for(size_t i{0}; i < size; i++)
  {
   if(values[i] == key)
   {
    return true;
   }
  }
  return false;
 }
 
};

template <typename Key, size_t N = 3>
class ADS_set
{ 
  private:
  Bucket<Key,N>** buckets;
  size_t directory_depth;
  size_t directory_size;
  public:
  ADS_set(size_t initial_directory_size): directory_depth(0),directory_size(initial_directory_size)
  {
   buckets = new Bucket<Key,N>*[directory_size];
   for(size_t i = 0; i  < directory_size; i++)
   {
    
    buckets[i] = new Bucket<Key,N>(directory_depth);
   }
  }
  ~ADS_set()
  {
   for(size_t i = 0; i  < directory_size; i++)
   {
    delete buckets[i];
   }
   delete[] buckets;
  }
  
  void insertToBucket(size_t index,Key key)
  {
   buckets[index]->insert(key);
  }
  
  size_t hashFunktion(Key key)const
 {
   //using hasher = std::hash<key_type>;  
   return std::hash<Key>{}(key);
 }
 
  void insert(Key key)
 {
  size_t index = hashFunktion(key) % directory_size;
  insertToBucket(index,key);
 }
 
 
 friend std::ostream& operator<<(std::ostream& os,const ADS_set& d1)
 {
  for(size_t i{0}; i < d1.directory_size; i++)
  {
   os <<  *(d1.buckets[i]) << std::endl;
   
  }
  
  //for(size_t i{0}; i < (d1.buckets[i]->get_capacity()); i++)
  // i takes values from 0 to bucket_size.
  // in array d1.buckets we have directory_size elements.
  // in this case index i I take from bucket_size an we try to put this index in array of buckets.
  // we assume that directory_size = 5.
  // we assume that bucket_size = 100.
  // d1.buckets[99] will call segfault
  // the problem can be the following we take grenzen of the first array and put on the second. 
  //{
   //os << "------";
  //}
  //os << "-";
  if(d1.directory_size >= 1)
  {
    for(size_t i{0}; i < (d1.buckets[0]->get_capacity()); i++)
    {
    os << "------";
    }
    os << "-";
  }
   return os;
  }
  
  bool contains(Key key)const
  {
   size_t index = hashFunktion(key) % directory_size;
   return buckets[index]->contains(key);
  }
  
};
// Why two classes are better than 3:
// because it is better to read and to make known what happens in two classes instead of three
// Method insert calls a method from another class and it takes some time, so when we write wrapper round a class so a wrapper will work slower than the original class. 
/* 
class ADS_set
{
 private:
 Directory directory;
 public:
 ADS_set(size_t initial_directory_size,size_t bucket_size):directory(initial_directory_size,bucket_size)
 {
 }
 
 void insert(int key)
 {
  directory.insert(key);
 }
 
 friend std:: ostream& operator<<(std::ostream& os,const ADS_set& s1)
 {
   os << s1.directory;
   
   return os;
 }
 
 bool contains(int key)const
 {
  return directory.contains(key);
 }
};
*/




















#endif
