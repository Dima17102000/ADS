#ifndef ADS_SET_H
#define ADS_SET_H

#include <iostream>
#include <iomanip>



template <typename Key, size_t N = 3>

class ADS_set
{   
    using value_type = Key;
    using key_type = Key;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = /* iterator type */;
    using iterator = const_iterator;
    using key_compare = std::less<key_type>;                         // B+-Tree
    using key_equal = std::equal_to<key_type>;                       // Hashing
    using hasher = std::hash<key_type>;                              // Hashing

    private:
    Bucket<key_type,N>** buckets;
    size_type depth;
    size_type directory_size;
    size_type total_elements;
    
    void insertToBucket(size_type index,key_type key) {
        buckets[index]->insert(key);
    }

    void expandDirectory()
    {
        size_type new_directory_size = directory_size * 2;
        Bucket<key_type, N>** new_buckets = new Bucket<key_type, N>*[new_directory_size];
        for(size_type i{0}; i < directory_size; i++)
        { 
            new_buckets[i] = buckets[i];
            new_buckets[i + directory_size] = buckets[i];
        }
        delete[] buckets;
        buckets = new_buckets;
        directory_size = new_directory_size;
        
        depth++;
    }

    void splitBucket(size_type index) 
    {
        Bucket<key_type, N>* bucket = buckets[index]; 
        if (bucket->get_capacity() < N)
            return;
        bucket->increase_depth();
        Bucket<key_type, N>* newBucket = new Bucket<key_type, N>(bucket->get_local_depth());
      
        if(bucket->get_local_depth() > directory_depth) {
            expandDirectory();
        }
        buckets[index] = newBucket;
        value_type* temp = new key_type[N]; 
        for (size_type i = 0; i < N; ++i) {
            temp[i] = bucket->get_value(i); 
        }

        bucket->clear(); 
        total_elements -= N;

        for (size_type i = 0; i < N; ++i) {   
            this->insert(temp[i]); 
        }
        delete[] temp; 
    }
   
    public:
    ADS_set() : depth(0), directory_size(0), total_elements(0) 
    {
        buckets = new Bucket<key_type, N>*[directory_size];
        for(size_type i = 0; i < directory_size; i++) {
            buckets[i] = new Bucket<key_type, N>(depth);
        }
    }
    
    ADS_set(std::initializer_list<key_type> ilist)
    {
        for (const key_type& key : init_list) 
        {
            insert(key);
        }
    }
    
    ADS_set(size_type directory_size): depth(0), directory_size(directory_size), total_elements(0)
    {
        buckets = new Bucket<key_type, N>*[directory_size];
        for(size_type i = 0; i  < directory_size; i++)
        {
            buckets[i] = new Bucket<key_type,N>(depth);
        }
    }

    ~ADS_set()
    {   
        for(size_type i = 0; i  < directory_size; i++)
        {
            bool pointer_is_unique = true;
            for (size_type j = 0; j < i; j++)
            {
                if (buckets[i] == buckets[j]) pointer_is_unique = false;
            }
            if (pointer_is_unique)
                delete buckets[i];
        }
        delete[] buckets;
    }
  
 
    size_type size() const
    {
        return total_elements;
    }

    void dump(std::ostream& o) const 
    {
        for(size_type i{0}; i < directory_size; i++) 
        {
            o << *(buckets[i]) << std::endl;
        }

            if(directory_size >= 1) 
            {
        for(size_type i{0}; i < (buckets[0]->get_capacity()); i++) 
        {
            o << "------";
        }
        o << "-";
            }

        //Bucket::dump
        //ADS_set::dump
    }

    bool contains(key_type key) const
    {
        size_type index = hasher(key) % directory_size;
        return buckets[index]->contains(key);
    }

    void insert(key_type key)
    {
        size_type index = hasher(key) % directory_size;
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

    void erase(key_type key)    
    {
        size_type  index =  hasher(key) % directory_size;
        if (!buckets[index]->contains(key))
            return;
        buckets[index]->erase(key);
        total_elements--;
    }
    
    void clear()
    {
        for (size_type i = 0; i < directory_size; ++i) {
            buckets[i]->clear();
        }
        total_elements = 0;
    }
    
    bool empty() const
    {
        return total_elements == 0;
    }
    
    class Bucket
{
    private:
        key_type* values;
        size_type depth;
        size_type size;
        size_type max_size;

        void increase_depth() { // has to be private so that user can't sabotage the algorithm
            depth++;
        }
    
    public:

    Bucket(size_type depth): values(new key_type[N]), max_size(N), depth(depth), size(0){}
 
    ~Bucket()
    {
        delete[] values;
    }
    
    size_type get_capacity() {
        return max_size;
    }

    size_type get_depth() const {
        return depth;
    }

    key_type get_value(size_type index) const 
    {
        if(index  < size){
            return values[index];
        } else {
            return key_type();
        }
    }

    bool full() const {
        return  size  ==  max_size;
    }
    
    void clear()
    {
        size  = 0;
    }

    void dump(std::ostream &o) const 
    {
        for(size_type i{0}; i < max_size; i++) 
        {
            o << "------";
        }
        o << "-\n|";   
    
        for(size_type i{0}; i < max_size; i++) 
        {
            if(i >= size) {
                o << std::setw(5) << "" << "|"; 
            } else {
                o << std::setw(5) << values[i] << "|";
            }
        }
    }

    bool contains(key_type key)
    {
        for(size_type i{0}; i < size; i++)
        {
            if(values[i] == key)
            {
                return true;
            }
        }
        return false;
    }

    void insert (key_type key)
    {
        if(contains(key)) return; 
   
        if(size < max_size) {
            values[size++] = key;
        }
    }
    
    void erase(key_type key)
    {
        for(size_type  i{0}; i < size; i++)
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

};

#endif
















