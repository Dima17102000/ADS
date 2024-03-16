#ifndef ADS_SET_H
#define ADS_SET_H

#include <iostream>
#include <iomanip>
#include <algorithm>


template <typename Key, size_t N = 3>
class ADS_set
{
    public:
    class Iterator;
    Iterator begin()const;
    Iterator end()const;
    using value_type = Key;
    using key_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = Iterator;
    //using iterator = const_iterator;
    using key_equal = std::equal_to<key_type>;                       // Hashing
    using hasher = std::hash<key_type>;                              // Hashing


    class Bucket
    {
        private:
            key_type* values;
            size_type depth;
            size_type size;
            size_type max_size;

        public:

        Bucket(size_type depth): values{new key_type[N]},depth{depth}, size{0},max_size{N}
        {
        }
    
        ~Bucket() {delete[] values;}

        Bucket(const Bucket& other): values(new key_type[other.max_size]),depth(other.depth),size(other.size),max_size(other.max_size)
        {

            // Perform a deep copy of the values array
            for (size_type i = 0; i < other.size; ++i)
            {
                values[i] = other.values[i];
            }
        
        }
        
        Bucket& operator=(const Bucket& other){
            if (this != &other) // Check for self-assignment
            {
                depth = other.depth;
                size = other.size;

                for (size_type i = 0; i < other.size; ++i)
                {
                    values[i] = other.values[i]; // Perform a deep copy of values
                }
            }
            return *this;
        }
        
        
    
        void increase_depth() { // has to be private so that user can't sabotage the algorithm
            depth++;
        }

        size_type get_capacity() {
            return max_size;
        }

        size_type get_depth() const {
            return depth;
        }
        
        size_type get_size()const
        {
            return size;
        }
        
        key_type get_value(size_type index) const 
        {
          if(index < size)
          {
            return values[index];
          } 
        
          else 
          {
            return key_type();
          }
        
        }

        bool full() const 
        {
            return  size  ==  max_size;
        }
    
        void clear()
        {
            size  = 0;
        }

        void dump(std::ostream &o=std::cerr) const
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

        size_type count(const key_type &key) const
        {
            for(size_type i{0}; i < size; i++)
            {
                if(key_equal{}(values[i], key))
                {
                    return 1;
                }
            }
            return 0;
        }
        
        /*
        void insert (key_type key)
        {
            if(count(key)) return;
    
            if(size < max_size) {
                values[size++] = key;
            }
        }
       */
       
        bool insert(key_type key) 
        {
            if(count(key)) return false; // Key already exists

            if(size < max_size) 
            {
                values[size++] = key; // Insert the key
                return true; // Indicate success
            }
            return false; // Indicate failure (bucket is full)
        }
       
        void erase(key_type key)
        {
            for(size_type  i{0}; i < size; i++)
            {
                if (key_equal{}(values[i], key))
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
    private:
    Bucket** buckets;
    size_type depth;
    size_type directory_size;
    size_type total_elements;
   
    
    void insertToBucket(size_type index,key_type key) {
        buckets[index]->insert(key);
    }

    void expandDirectory()
    {
        size_type new_directory_size = directory_size * 2;
        Bucket** new_buckets = new Bucket*[new_directory_size];
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
        get_bucket_first_index(index);
        size_type first_meet_index = get_bucket_first_index(index);
        size_type distance = get_bucket_distance(index);
        
        Bucket* bucket = buckets[index];
        bucket->increase_depth();
        Bucket* newBucket = new Bucket(bucket->get_depth());
        if(bucket->get_depth() > depth) {
            expandDirectory();
        }
        
        //size_type bucket_count = pow(2,(depth - (buckets[index]->get_depth()-1)));
        /*
        size_type bucket_count = 1;
    		size_type bucket_depth = depth - (buckets[index]->get_depth() - 1);
    		while (bucket_depth > 0) 
    		{	
        		bucket_count *= 2;
        		--bucket_depth;
    		}
    		
        // finding of positions for newBucket and rewriting them
        size_type distance = directory_size / bucket_count; 
       */
        for(size_type i = first_meet_index; i < directory_size;i+=2*distance)
        {   // it gives us the first time meeting index in our directory.
        		buckets[i] = newBucket;
        }
        
        // delete elements from old bucket and reinsert them to set(recursively)
        value_type* temp = new key_type[N];
        for (size_type i = 0; i < N; ++i) {
            temp[i] = bucket->get_value(i);
        }

        bucket->clear();
        total_elements -= N;

        for (size_type i = 0; i < N; ++i) {
            this->insert1(temp[i]);
        }
        delete[] temp;
    }
   
    public:
    ADS_set() : ADS_set(4)
    {

    }
    
  
    ADS_set(size_type directory_size1): depth(0), directory_size(directory_size1), total_elements(0)
    {
        if(directory_size  < 1)
        {
            directory_size  =  1;
        }
        
        buckets = new Bucket*[directory_size];
        for (size_type i = 0; i < directory_size; ++i)
        {
            buckets[i] = new Bucket(depth);
        }
    }

    ADS_set(std::initializer_list<key_type> ilist):ADS_set(2*ilist.size()/N)
    {
        for (const key_type& key : ilist)
        {
            insert(key);
        }
    }
    
    
    
    ADS_set& operator=(const ADS_set& other) 
    {
        if (this == &other) return *this; // Self-assignment guard

        // First, clear current contents
        // Your cleanup code here

        // Then, copy from 'other'
        // Deep copy of members from 'other' to 'this'
        depth = other.depth;
        directory_size = other.directory_size;
        total_elements = other.total_elements;

        // Deep copy the buckets
        buckets = new Bucket*[directory_size];
        for (size_t i = 0; i < directory_size; ++i) 
        {
            buckets[i] = new Bucket(*other.buckets[i]); // Assuming Bucket has a copy constructor
        }

        return *this;
    }
    
    template<typename InputIt>
    ADS_set(InputIt first,InputIt last):ADS_set(4)//ADS_set(2 * std::distance(first, last) / N)
    {
        for (InputIt it = first; it != last; ++it) {
            insert(*it);
        }
    }

    ADS_set(const ADS_set& other) : ADS_set(other.directory_size) //doesn't work correctly
    {
        depth = other.depth;
        directory_size = other.directory_size;
        total_elements = other.total_elements;
        
        for (size_type i = 0; i < directory_size; ++i) {
            *buckets[i] = *other.buckets[i];
        }
    }
     
    ~ADS_set() 
    {
       for (size_type i = directory_size; i > 0; --i) 
       {
          if (bucket_encounter_first_time(i - 1)) 
          {
            delete buckets[i - 1];
          }
       }
      delete[] buckets;
    }
 
    size_type size() const
    {
        return total_elements;
    }

    void dump(std::ostream& o =std::cerr) const
    {
        o << "global_depth = " << depth << std::endl;
        for(size_type i{0}; i < directory_size; i++)
        {
            buckets[i]->dump(o);
            o << "d=" << buckets[i]->get_depth() << "  " << (buckets[i]);
            o << std::endl;
        }

        if(directory_size >= 1)
        {
            for(size_type i{0}; i < (buckets[0]->get_capacity()); i++)
            {
                o << "------";
            }
            o << "-";
        }
    }

    size_type count(const key_type &key) const
    {
        size_type index = hasher{}(key) % directory_size;
        return buckets[index]->count(key);
    }
    
    size_type get_depth()const
    {
     	  return depth;
    }
    

    void insert1(key_type key)
    {
        /*
        size_type index = hasher{}(key) % directory_size;
        if (buckets[index]->count(key))
        {
            return;
        }

        if (buckets[index]->full())
        {
            splitBucket(index);
            return insert1(key);
        }
        buckets[index]->insert(key);
        total_elements++;
        */
        
        size_type index = hasher{}(key) % directory_size;

        // No need to check for existence here, as Bucket::insert already does that.

        if (buckets[index]->full()) 
        {
          splitBucket(index);
          // After splitting, the key could potentially fit into the current or new bucket,
          // so we try inserting again. Note: splitBucket might change the structure, 
          // requiring a rehash or a different bucket might be selected.
          insert1(key); // Recursive call to try inserting again
        } 
        
        else 
        {
            // Attempt to insert the key into the bucket
            if (buckets[index]->insert(key)) 
            {
                total_elements++; // Increment only if insertion was successful
            }
        }
    }
    
    
    std::pair<iterator, bool> insert(const key_type& key) 
    {
        size_type index = hasher{}(key) % directory_size;
        bool inserted = false;

        if (!buckets[index]->count(key)) 
        {
            if (buckets[index]->full()) 
            {
                splitBucket(index);
                index = hasher{}(key) % directory_size; // Recalculate index as the directory might have changed
            }
            inserted = buckets[index]->insert(key);
            if (inserted) total_elements++;
        }

     // Iterate through the bucket to find the position of the key
     // This is necessary whether we just inserted the key or it already existed
        for (size_type pos = 0; pos < buckets[index]->get_size(); ++pos) 
        {
            if (key_equal{}(buckets[index]->get_value(pos), key)) 
            {
                // Found the key, return iterator and insertion result
                return {iterator(this, index, pos), inserted};
            }
        }

     // If for some reason the key is not found, return end iterator and false
     // This situation should not happen if the code is correct
        return {end(), false}; 
    
    }
    
    void insert(std::initializer_list<key_type> ilist)
    {
        for (const key_type& key : ilist)
        {
            insert1(key);
        }
    
    }
    
    size_type get_bucket_first_index(size_type index)const
    {
        size_type bucket_delay = depth - (buckets[index]->get_depth());
        size_type bucket_count = 1;
        while(bucket_delay > 0)
        {
            bucket_count *= 2;
            --bucket_delay;
        }
        size_type distance = directory_size/bucket_count;
        size_type first_meet_index = index % distance;
     
        return first_meet_index;
    }
    
    size_type get_bucket_distance(size_type index)
    {
        size_type bucket_delay = depth - (buckets[index]->get_depth());
        size_type bucket_count = 1;
        while(bucket_delay > 0)
        {
            bucket_count *= 2;
            --bucket_delay;
        }
        size_type distance = directory_size/bucket_count;
        return distance;
    }
    
    
    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        for (InputIt it = first; it != last; ++it)
        {
            insert1(*it);
        }
    }
    
    size_type erase(key_type key)
    {
        size_type  index =  hasher{}(key) % directory_size;
        if (!buckets[index]->count(key))
        {
            return 0;
        }
        buckets[index]->erase(key);
        total_elements--;
        
        return 1;
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
    
    void test123()
    {
       ADS_set<value_type,N> uniqueElements; 
       std::cout << "\n";
       for(size_t index{0}; index < directory_size; index++) 
       { 
           for(size_t i{0}; i < buckets[index]->get_size(); i++) 
           {
              value_type element = buckets[index]->get_value(i);
              if(uniqueElements.count(element) == 0) 
              {
                std::cout << element << " ";
                uniqueElements.insert1(element); 
              }
           }
       }
    }


    void test124()
    { 
       ADS_set<Bucket*,N> uniqueBuckets;
       std::cout << "\n";
       for (size_t index{0}; index < directory_size; index++) 
       {
              if (uniqueBuckets.count(buckets[index]) == 0) 
              {	
                uniqueBuckets.insert1(buckets[index]);
                for (size_t i{0}; i < buckets[index]->get_size(); i++) 
                {
                  value_type element = buckets[index]->get_value(i);
                  std::cout << element << " ";
                }
              }
       }
       std::cout << std::endl;
    }
		 
		 
		

    void test125()
    { 
       size_type index = 0;
       size_type i = 0;

        while(true) 
        { 
           if(index >= directory_size) 
           {
                break;
           }

    	
           if(bucket_encounter_first_time(index) && i < buckets[index]->get_size())
           {
                value_type element = buckets[index]->get_value(i);
                std::cout << element << " ";
           }
           
           i++;
            
           if(i >= buckets[index]->get_size()) 
           {
                i = 0;
                index++;
           }
        }
    }
		 
		 
    bool bucket_encounter_first_time(size_type index)const
    {
        return get_bucket_first_index(index) == index;
    }
    
    iterator find(const key_type& key)const 
    {
       size_type index = hasher{}(key) % directory_size; // Calculate bucket index
       if (buckets[index]->count(key) > 0) 
       {
          // Key exists in the bucket, find its position
          for (size_type pos = 0; pos < buckets[index]->get_size(); ++pos) 
          {
            if (key_equal{}(buckets[index]->get_value(pos), key)) 
            {
              // Found the key, return an iterator to it
              return iterator(const_cast<ADS_set<Key, N>*>(this), index, pos);
            }
          }
       }
        // Key not found, return end iterator
        return end();
    }
		
};


template<typename Key, size_t N>
bool operator==(const ADS_set<Key, N>& lhs, const ADS_set<Key, N>& rhs) 
{
   if (lhs.size() != rhs.size()) return false; // Sets of different sizes are not equal

   for (auto it = lhs.begin(); it != lhs.end(); ++it) 
   {
     // Use the count method of rhs to check if each element in lhs is present in rhs 
     if (rhs.count(*it) == 0) return false;
   }
   return true;
}


template<typename Key, size_t N>
bool operator!=(const ADS_set<Key, N>& lhs, const ADS_set<Key, N>& rhs) 
{
   return !(lhs == rhs); // Use the already defined == operator for comparison
}




template <typename Key, size_t N>
typename ADS_set<Key, N>::Iterator ADS_set<Key, N>::begin()const 
{
   for (size_t i = 0; i < directory_size; ++i) 
   {
     if(buckets[i]->get_size() > 0) 
     {
       return iterator(this, i, 0); 
     }
   } 
   return this->end(); // All buckets are empty
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::Iterator ADS_set<Key, N>::end()const
{
   return iterator(this, directory_size, 0);
}


template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator 
{
   const ADS_set* s;
   size_type bucket_index;
   size_type elem_index;   
   public:
   using value_type = Key;
   using difference_type = std::ptrdiff_t;
   using reference = const value_type &;
   using pointer = const value_type *;
   using iterator_category = std::forward_iterator_tag;
   //using size_type = size_t;
  	
  	
   Iterator(): s(nullptr), bucket_index(0), elem_index(0) {}
   explicit Iterator(const ADS_set* s, size_type bucket_index, size_type elem_index): s{s},bucket_index{bucket_index},elem_index{elem_index}{}
   reference operator*() const {return s->buckets[bucket_index]->get_value(elem_index);}  
   pointer operator->() const {return &(s->buckets[bucket_index]->get_value(elem_index));}
   /*
   Iterator &operator++()
   {
    while (dir_index < directory_size) 
    {
      if(bucket_encounter_first_time(dir_index) && elem_index < buckets[dir_index]->get_size()) 
      {
        value_type element = buckets[dir_index]->get_value(elem_index);
        std::cout << element << " ";
        elem_index++;
      }  
        
      else 
      {
        dir_index++;
        elem_index = 0;
      }
    }
    std::cout << std::endl;
   }
   return *e;
   */
  	 
   Iterator& operator++() 
   {
     // Move to the next element within the current bucket
     ++elem_index;

     // Check if we've reached the end of the current bucket's elements
    while (bucket_index < s->directory_size) 
    {
      if (elem_index >= s->buckets[bucket_index]->get_size()) 
      {  // Find the next bucket with elements, ensuring we don't revisit the same bucket
         do 
         {
           ++bucket_index;
         }while (bucket_index < s->directory_size && (s->buckets[bucket_index]->get_size() == 0 || !s->bucket_encounter_first_time(bucket_index)));
        // Reset element index for the new bucket
        elem_index = 0;
      }

       // If a valid bucket with elements is found, return
      if (bucket_index < s->directory_size && s->bucket_encounter_first_time(bucket_index)) 
      {
         return *this;
      } 

      break; // No more elements or buckets
    }

     // End of the container
     *this = s->end();
     return *this;
   }
    
   Iterator operator++(int) 
   {
     Iterator temp = *this;
     ++(*this);
     return temp;
   }
  	  
   friend bool operator==(const Iterator &lhs, const Iterator &rhs) 
   {
     return lhs.s == rhs.s && lhs.bucket_index == rhs.bucket_index && lhs.elem_index == rhs.elem_index;
   }
  	  
   friend bool operator!=(const Iterator &lhs, const Iterator &rhs)  
   {
     return !(lhs == rhs);
   }
  	

  	
};


#endif