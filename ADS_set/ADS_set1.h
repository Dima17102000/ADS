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
    using value_type = Key;
    using key_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    //using const_iterator = Iterator;
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

        void insert (key_type key)
        {
            if(count(key)) return;
    
            if(size < max_size) {
                values[size++] = key;
            }
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
            this->insert(temp[i]);
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
          if (bucket_encounter_first_time(i-1)) 
          {
            delete buckets[i-1];
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
    

    void insert(key_type key)
    {
        size_type index = hasher{}(key) % directory_size;
        if (buckets[index]->count(key))
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

    void insert(std::initializer_list<key_type> ilist)
    {
        for (const key_type& key : ilist)
        {
            insert(key);
        }
    
    }
    
    size_type get_bucket_first_index(size_type index)
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
            insert(*it);
        }
    }
    
    void erase(key_type key)
    {
        size_type  index =  hasher{}(key) % directory_size;
        if (!buckets[index]->count(key))
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
                uniqueElements.insert(element); 
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
                uniqueBuckets.insert(buckets[index]);
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
       
       size_type dir_index = 0;
       size_type elem_index = 0;

        while (true) 
       { 
           if (dir_index >= directory_size) 
           {
              break;
           }

    	
           if (bucket_encounter_first_time(dir_index) && elem_index < buckets[dir_index]->get_size())
           {
              
                value_type element = buckets[dir_index]->get_value(elem_index);
                std::cout << element << " ";
           }
           
           elem_index++;
    
           if(elem_index >= buckets[dir_index]->get_size())
           {
               elem_index = 0;
               dir_index++;
           }
       }
       
     }
		 
		 
     bool bucket_encounter_first_time(size_type index)
     {
        return get_bucket_first_index(index) == index;
     }
	
	

    /*	 
	Iterator find(const key_type &key) const {
    size_type index = hasher{}(key) % directory_size;
    size_type pos = buckets[index]->count(key);
    if (pos > 0) {
        return Iterator(buckets, index, pos - 1, directory_size);
    } else {
        return end();
    }
}

    std::pair<Iterator,bool> insert(const key_type &key) {
    size_type index = hasher{}(key) % directory_size;
    size_type count = buckets[index]->count(key);
    if (count > 0) {
        return std::make_pair(find(key), false);
    }

    if (buckets[index]->full()) {
        splitBucket(index);
        return insert(key);
    }

    buckets[index]->insert(key);
    total_elements++;
    return std::make_pair(find(key), true);
}

   void swap(ADS_set &other) {
    std::swap(buckets, other.buckets);
    std::swap(depth, other.depth);
    std::swap(directory_size, other.directory_size);
    std::swap(total_elements, other.total_elements);
}
*/
	
};


/*
template <typename Key, size_t N>
bool operator==(const ADS_set<Key,N> &lhs, const ADS_set<Key,N> &rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (const auto &key : lhs) {
        if (rhs.count(key) == 0) {
            return false;
        }
    }
    return true;
}

template <typename Key, size_t N>
bool operator!=(const ADS_set<Key,N> &lhs, const ADS_set<Key,N> &rhs) {
    return !(lhs == rhs);
}

template <typename Key, size_t N>
    void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) 
    {
      lhs.swap(rhs);
    }
*/

/*
template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator 
{
    using value_type = Key;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using size_type = size_t;

    Bucket** buckets; // Assuming you have access to the buckets array
    size_type dir_index;
    size_type elem_index;
    size_type directory_size;

public:
    explicit Iterator(Bucket** buckets_ptr, size_type index, size_type elem_idx, size_type dir_size)
        : buckets(buckets_ptr), dir_index(index), elem_index(elem_idx), directory_size(dir_size) {}

    reference operator*() const 
    {
        return buckets[dir_index]->get_value(elem_index);
    }
    
    pointer operator->() const 
    {
     return &(buckets[dir_index]->get_value(elem_index));
    }
    Iterator& operator++() 
    {
    	++elem_index; // Увеличиваем индекс элемента

    	while (dir_index < directory_size) 
    	{
          // Проверяем, не превысили ли мы количество элементов в текущей корзине
          
          if (elem_index < buckets[dir_index]->get_size()) 
          {
             break; // Если нет, то мы закончили
          }
           
          else 
          {
            ++dir_index; // Переходим к следующей корзине
            elem_index = 0; // Сбрасываем индекс элемента до 0 для новой корзины
          }
    	}

    // Возвращаем ссылку на итератор
    	return *this;
   	}
   	
   	Iterator operator++(int) 
   	{
        Iterator temp = *this;
        ++(*this);
        return temp;
    }
    
    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
        return (lhs.dir_index == rhs.dir_index) && (lhs.elem_index == rhs.elem_index);
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
        return !(lhs == rhs);
    }
    
    
    
    Iterator begin()const {
    size_type index = 0;
    // Найдем первую непустую корзину
    while (index < directory_size && buckets[index]->get_size() == 0) {
        ++index;
    }
    // Если найдена непустая корзина, возвращаем итератор на ее начало
    if (index < directory_size) {
        return Iterator(buckets, index, 0, directory_size);
    } else {
        // Если непустых корзин нет, возвращаем итератор на конец
        return this->end();
    }
}

		Iterator end()const 
		{
    	// Возвращаем итератор, указывающий за последний элемент последней корзины
    		return Iterator(buckets, directory_size - 1, buckets[directory_size - 1]->get_size(), directory_size);
		}
};
*/


#endif
