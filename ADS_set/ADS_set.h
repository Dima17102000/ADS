#ifndef ADS_SET_H
#define ADS_SET_H

#include <iostream>
#include <iomanip>


  using std::size_t;
  //using value_type = Key;
  //using key_type = Key;
  //using reference = value_type &;
  //using const_reference = const value_type &;
  //using size_type = size_t;
  //using difference_type = std::ptrdiff_t;
  //using const_iterator = /* iterator type */;
  //using iterator = const_iterator;
  //using key_compare = std::less<key_type>;                         // B+-Tree
  //using key_equal = std::equal_to<key_type>;                       // Hashing
  //using hasher = std::hash<key_type>;                              // Hashing

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
    
    size_t get_capacity()
    {
        return max_size;
    }


    size_t get_local_depth() const 
    {
        return local_depth;
    }

    void increase_depth()
    {
        local_depth++;
    }

    bool full()
    {
        return  size  ==  max_size;
    }
 
    Key get_value(size_t index) const 
    {
        if(index  < size)
        {
            return values[index];
        }

        else
        {
            return Key();
        }
    }
    /*
    friend std::ostream& operator<<(std::ostream& os,const Bucket& b)
    {
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
    */
    
    void dump(std::ostream &o) const 
    {
        for(size_t i{0}; i < max_size; i++) 
        {
            o << "------";
        }
        o << "-\n|";   
    
        for(size_t i{0}; i < max_size; i++) 
        {
            if(i >= size) 
            {
                o << std::setw(5) << "" << "|"; 
            } 
            
            else 
            {
                o << std::setw(5) << values[i] << "|";
            }
        }
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

    void clear()
    {
        size  = 0;
    }

    
    void erase(Key key)
    {
        for(size_t  i{0}; i < size; i++)
        { 
            if(values[i]  ==  key)
            {
                if(i  !=  size  - 1)
                {
                    values[i] = values[size-1];
                }        
                --size;
                return;
            }
        }
    }
};


template <typename Key, size_t N = 3>
class ADS_set
{ 
    private:
    Bucket<Key,N>** buckets;
    size_t directory_depth;
    size_t directory_size;
    size_t total_elements;
    
    public:

    ADS_set(size_t initial_directory_size): directory_depth(0),directory_size(initial_directory_size),total_elements(0)
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
            bool pointer_is_unique = true;
        for (size_t j = 0; j < i; j++)
        {
            if (buckets[i] == buckets[j]) pointer_is_unique = false;
        }
            if (pointer_is_unique)
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
        return std::hash<Key>{}(key);
    }
 
    size_t  size() const
    {
        return total_elements;
    }

    void insert(Key key)
    {
        size_t index = hashFunktion(key) % directory_size;
        if (buckets[index]->contains(key)) 
        {
            return;
        }

        if (buckets[index]->full()) 
        {
            splitBucket(index);
            return insert(key);
        }
        buckets[index]->insert(key);
        total_elements++;
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
  
    /*
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
    */
    void dump(std::ostream& o) const 
    {
        for(size_t i{0}; i < directory_size; i++) 
        {
            o << *(buckets[i]) << std::endl;   
        }

            if(directory_size >= 1) 
            {
        for(size_t i{0}; i < (buckets[0]->get_capacity()); i++) 
        {
            o << "------";
        }
        o << "-";
            }
    }
    
    bool contains(Key key)const
    {
        size_t index = hashFunktion(key) % directory_size;
        return buckets[index]->contains(key);
    }
  
  

    void splitBucket(size_t index) 
    {
        Bucket<Key, N>* bucket = buckets[index]; 
        if (bucket->get_capacity() < N) return;
        bucket->increase_depth();
        Bucket<Key, N>* newBucket = new Bucket<Key, N>(bucket->get_local_depth());
      
        if(bucket->get_local_depth() > directory_depth)
        {
            expandDirectory();
        }
        buckets[index] = newBucket;
        Key* temp = new Key[N]; 
        for (size_t i = 0; i < N; ++i) 
        {
            temp[i] = bucket->get_value(i); 
        }
            bucket->clear(); 
            total_elements -= N;

        for (size_t i = 0; i < N; ++i) 
        {   
            this->insert(temp[i]); 
        }
      delete[] temp; 
    }

    void  erase(Key key)    
    {
        size_t  index =  hashFunktion(key) % directory_size;
        if (!buckets[index]->contains(key))
        return;
        buckets[index]->erase(key);
        total_elements--;
    }
};

#endif












