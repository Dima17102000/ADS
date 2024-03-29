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
    Iterator z()const;
    using value_type = Key;
    using key_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = Iterator;
    using const_iterator = iterator;
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
    

        Bucket(const Bucket& other): values(new key_type[other.max_size]),depth(other.depth),size(other.size),max_size(other.max_size)
        {

            // Perform a deep copy of the values array
            for (size_type i = 0; i < other.size; ++i)
            {
                values[i] = other.values[i];
            }
        
        }
        
        ~Bucket() {delete[] values;}
        
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

        size_type get_capacity()   {  return max_size; }

        size_type get_depth()const {  return depth;    }
        
        size_type get_size()const  {  return size;     }
        
        key_type& get_value(size_type index) const 
        {
          if(index < size)
          {
            return values[index];
          } 
        
          else 
          {
            throw  std::out_of_range("index out of range.");
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
    ADS_set() : ADS_set(4){}
    
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
    
    template<typename InputIt>
    ADS_set(InputIt first,InputIt last):ADS_set(4)//ADS_set(2 * std::distance(first, last) / N)
    {
        for (InputIt it = first; it != last; ++it) {
            insert(*it);
        }
    }

    ADS_set(const ADS_set& other) : ADS_set(other.directory_size) //doesn't work correctly
    {
        
        for(auto it = other.begin(); it != other.end(); ++it)
        {
          insert(*it);
        }
        
    }
    
    ADS_set& operator=(const ADS_set& other) 
    {
        
        if(this == &other) return *this;
        
         this->clear();
         for(auto it = other.begin(); it != other.end(); ++it)
        {
          insert(*it);
        }
        return *this;
        
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
    
    size_type size() const{   
        return total_elements;
    }

    size_type count(const key_type &key) const{
        size_type index = hasher{}(key) % directory_size;
        return buckets[index]->count(key);
    }
    
    size_type get_depth()const{
     	  return depth;
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

    void insert1(key_type key)
    {
        
        size_type index = hasher{}(key) % directory_size;

        if (buckets[index]->full()) 
        {
          splitBucket(index);
          insert1(key); 
        } 
        
        else 
        {
            if (buckets[index]->insert(key)) 
            {
                total_elements++;
            }
        }
    }
    
    
    std::pair<iterator, bool> insert(const key_type& key) 
    {
        size_type index = hasher{}(key) % directory_size;
        index = get_bucket_first_index(index);
        bool inserted = false;

        if (!buckets[index]->count(key)) 
        {
            if (buckets[index]->full()) 
            {
                splitBucket(index);
                return insert(key); // Recalculate index as the directory might have changed
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
    
   
    bool bucket_encounter_first_time(size_type index)const
    {
        return get_bucket_first_index(index) == index;
    }
    
    iterator find(const key_type& key)const 
    {
       size_type index = hasher{}(key) % directory_size; // Calculate bucket index
       index = get_bucket_first_index(index);
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
typename ADS_set<Key, N>::Iterator ADS_set<Key, N>::z()const 
{
   for (size_t i = 0; i < directory_size; ++i) 
   {
     if(buckets[i]->get_size() > 0) 
     {
       return iterator(this, i, 0,buckets[i]->get_value(0)); 
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
   value_type first_element;   
   public:
   using value_type = Key;
   using difference_type = std::ptrdiff_t;
   using reference = const value_type &;
   using pointer = const value_type *;
   using iterator_category = std::forward_iterator_tag;
   //using size_type = size_t;
  	
  	
   Iterator(): s(nullptr), bucket_index(0), elem_index(0) {}
   explicit Iterator(const ADS_set* s, size_type bucket_index, size_type elem_index): s{s},bucket_index{bucket_index},elem_index{elem_index}{}
   explicit Iterator(const ADS_set* s, size_type bucket_index, size_type elem_index,value_type first_element): s{s},bucket_index{bucket_index},elem_index{elem_index},first_element{first_element}{}
   reference operator*() const {return s->buckets[bucket_index]->get_value(elem_index);}  
   pointer operator->() const {return &(s->buckets[bucket_index]->get_value(elem_index));}
   
   Iterator& operator++()
   {
      increment_helper();
      if(*this == this->s->end())
         return *this;
      value_type value1 = first_element;
      value_type value2 = **this;
      if(value2 < value1)
      {
        return ++(*this); 
      }
     //std::cout << *(this->s->begin()) << " " ;
     //std::cout << first_element << " ";
     return *this;
   }
   	 
   Iterator& increment_helper() 
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

int main()
{
  /*
  ADS_set <int,3> set = {10,12,15,16,17};
  set.dump();
  for(auto it = set.z(); it != set.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << std::endl;
  */
  
  ADS_set <int,1> set1 = {1,2,4,3,5,6,7};
  set1.dump();
  for(auto it = set1.begin(); it != set1.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nUsual set1! " << std::endl; 
  std::cout << std::endl << std::endl;
  
  set1.dump();
  
  for(auto it = set1.z(); it != set1.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nTest 1 work perfect for Michael! " << std::endl; 
  std::cout << std::endl << std::endl;
  
  
  ADS_set <int,8> set2 = {4,2,3,1,5,6};
  set2.dump();
  for(auto it = set2.begin(); it != set2.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nUsual set2! " << std::endl; 
  std::cout << std::endl << std::endl;
  
  set2.dump();
  for(auto it = set2.z(); it != set2.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nTest 2 work perfect for Michael! " << std::endl;
  std::cout << std::endl << std::endl; 
  
  
  ADS_set <int,8> set3 = {4,2,6,5,1};
  set3.dump();
  for(auto it = set3.begin(); it != set3.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nUsual set3! " << std::endl; 
  std::cout << std::endl << std::endl;
  set3.dump();
  for(auto it = set3.z(); it != set3.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nTest 3 work perfect for Michael! " << std::endl; 
  std::cout << std::endl << std::endl;
  
  
  ADS_set <int,8> set4 = {5,3,4,1,2};
  set4.dump();
  for(auto it = set4.begin(); it != set4.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nUsual set4! " << std::endl; 
  std::cout << std::endl << std::endl;
  set4.dump();
  for(auto it = set4.z(); it != set4.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nTest 4 work perfect for Michael! " << std::endl; 
  std::cout << std::endl << std::endl;
  
  
  ADS_set <int,8> set5 = {7,8};
  set5.dump();
  for(auto it = set5.begin(); it != set5.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nUsual set5! " << std::endl; 
  std::cout << std::endl << std::endl;
  set5.dump();
  for(auto it = set5.z(); it != set5.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nTest 5 work perfect for Michael! " << std::endl; 
  std::cout << std::endl << std::endl;
  
  
  ADS_set <int,7> set6 = {7,6};
  set6.dump();
  for(auto it = set6.begin(); it != set6.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nUsual set6! " << std::endl; 
  std::cout << std::endl << std::endl;
  set6.dump();
  for(auto it = set6.z(); it != set6.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nTest 6 work perfect for Michael! " << std::endl; 
  std::cout << std::endl << std::endl;
  
  
  ADS_set <int,8> set7 = {7};
  set7.dump();
  for(auto it = set7.begin(); it != set7.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nUsual set7! " << std::endl; 
  std::cout << std::endl << std::endl;
  set7.dump();
  for(auto it = set7.z(); it != set7.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nTest 7 work perfect for Michael! " << std::endl; 
  std::cout << std::endl << std::endl;
  
  
  ADS_set <int,8> set8 = {};
  set8.dump();
  for(auto it = set8.begin(); it != set8.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nUsual set8! " << std::endl; 
  std::cout << std::endl << std::endl;
  set8.dump();
  for(auto it = set8.z(); it != set8.end(); ++it)
  {
   std::cout << *it << " "; 
  }
  std::cout << "\nTest 8 work perfect for Michael! " << std::endl; 
  std::cout << std::endl << std::endl;
  return 0;
}


#endif