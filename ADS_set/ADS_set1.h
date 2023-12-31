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
 
 void expandDirectory()
 {
    size_t new_directory_size = directory_size * 2;
    Bucket<Key, N>** new_buckets = new Bucket<Key, N>*[new_directory_size];
    for(size_t i{0}; i < directory_size; i++)
    {
        new_buckets[i] = buckets[i];
        new_buckets[i+directory_size] = buckets[i];
    }
    
    delete[]buckets;
    buckets = new_buckets;
    directory_size = new_directory_size;
    directory_depth++;
  }
  
 friend std::ostream& operator<<(std::ostream& os,const ADS_set& d1)
 {
  for(size_t i{0}; i < d1.directory_size; i++)
  {
   os <<  *(d1.buckets[i]) << std::endl;
   
  }
  
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





















#endif
