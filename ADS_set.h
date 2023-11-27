#ifndef ADS_SET_H
#define ADS_SET_H

#include <iostream>
#include <iomanip>

//clang++ main.cpp -o main -std=c++17 -ferror-limit=100

template <typename Key, size_t N = 3>
class ADS_set
{
    public:
    using value_type = Key;
    using key_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    //using const_iterator = /* iterator type */;
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
        // Bucket& operator=(const Bucket& other)
        // {
        //     if (this != &other) // Check for self-assignment
        //     {
        //         depth = other.depth;
        //         size = other.size;
        //         max_size = other.max_size;

        //         delete[] values; // Free existing memory
        //         values = new key_type[max_size];
        //         for (size_type i = 0; i < other.size; ++i)
        //         {
        //             values[i] = other.values[i]; // Perform a deep copy of values
        //         }
        //     }
        //     return *this;
        // }
    
        void increase_depth() { // has to be private so that user can't sabotage the algorithm
            depth++;
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

        bool count(key_type key)
        {
            for(size_type i{0}; i < size; i++)
            {
                if(key_equal{}(values[i], key))
                {
                    return true;
                }
            }
            return false;
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
        Bucket* bucket = buckets[index];
        if (bucket->get_capacity() < N)
            return;
        bucket->increase_depth();
        Bucket* newBucket = new Bucket(bucket->get_depth());
      
        if(bucket->get_depth() > depth) {
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
    ADS_set() : ADS_set(4)
    {

    }
    
    // argument with default value
    ADS_set(size_type directory_size1): depth(0), directory_size(directory_size1), total_elements(0)
    {
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
    
    ADS_set(Input first,Input first):ADS_set(2 * std::distance(first, last) / N)
    {
        for (InputIt it = first; it != last; ++it) {
            insert(*it);
        }
    }
    ADS_set(const ADS_set& other) : ADS_set(other.directory_size)
    {
        depth = other.depth;
        directory_size = other.directory_size;
        total_elements = other.total_elements;
        buckets = new Bucket*[directory_size];
        for (size_type i = 0; i < directory_size; ++i) {
            buckets[i] = new Bucket(*(other.buckets[i])); // Assuming Bucket has a copy constructor
        }
    }
    
    
    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        for (InputIt it = first; it != last; ++it)
        {
            insert(*it);
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
            buckets[i]->dump(o);
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

    bool count(key_type key) const
    {
        size_type index = hasher{}(key) % directory_size;
        return buckets[index]->count(key);
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
};

#endif

















