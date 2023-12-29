#ifndef ADS_SET_H
#define ADS_SET_H

#include <iostream>
#include <iomanip>
#include <algorithm>


template <typename Key, size_t N = 3>
class ADS_set
{
    public:
    //class Iterator;
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
		size_type index = 0;
		size_type i = 0;

		while (true) 
		{
    			if (index >= directory_size) 
    			{
        			break;
    			}

    	
    			if (i < buckets[index]->get_size())
    			{
        		if (bucket_encounter_first_time(index)) 
        		{
            				value_type element = buckets[index]->get_value(i);
            				std::cout << element << " ";
        		}
        	 	i++;
    			}
    
    			else 
    			{
        			i = 0;
        			index++;
    			}
		}

	}
		 
		 
	bool bucket_encounter_first_time(size_type index)
	{
		 return get_bucket_first_index(index) == index;
	}
		 
		
};



/*
Iterator begin()
{
  for(size_type i{0}; i  < directory_size(); i++)
  if(bucket[i]->get_size() == 0)
  {
    return this->end();
  }
  
 return iterator(e,this);
}

Iterator end()
{
 return Iterator{e+size,this};
}


template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator 
{
   value_type** e;
   size_type elem_index;
   size_type dir_index;
   size_type directory_size;   
	public:
  	using value_type = Key;
  	using difference_type = std::ptrdiff_t;
  	using reference = const value_type &;
  	using pointer = const value_type *;
  	using iterator_category = std::forward_iterator_tag;
  	using size_type = size_t;

  	explicit Iterator(value_type *e, value_type elem_index, value_type dir_index, size_type directory_size): e{e},elem_index{elem_index},dir_index{dir_index},directory_size{directory_size}{}
  	reference operator*() const {return *e;}
  	pointer 	operator->() const {return e;}
  	Iterator &operator++()
  	{
     while (dir_index < directory_size) 
     {
        if (bucket_encounter_first_time(dir_index) && elem_index < buckets[dir_index]->get_size()) 
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
  	 }
  	Iterator operator++(int) {}
  	friend bool operator==(const Iterator &lhs, const Iterator &rhs) {}
  	friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {}
  	
}
  	
};
*/

#endif
