#include "ADS_set.h"
#include <iostream>
#include <vector>

// int test_insert_4()
// {
//   ADS_set<int, 2> my_set(2);
//   my_set.dump(std::cout);
//   // for (int i = 0; i < 8; i++){
//   //   my_set.insert(i);
//   // }
//   my_set.insert(0);
//   my_set.insert(8);
//   my_set.insert(16);
//   my_set.insert(32);
//   my_set.insert(32);

//   std::cout << std::endl;
//   my_set.dump(std::cout);
//   std::cout << std::endl;
//   std::cout << "set size is " << my_set.size() << std::endl;
//   return 0;
  
//   // map<string,std::set<int>> r;
//   // string  adveture1  = d1.get_empire();
//   // std::set<int> counties= d1.get_stats();
//   // r[adveture1].insert(counties.begin(),counties.end());
   
// }

// int test_insert_3() {
//   ADS_set<int, 2> my_set(2);
//   my_set.dump(std::cout);
//   std::cout << std::endl;
//   for (int i = 0; i < 8; i++){
//     my_set.insert(i);
//   }

//   std::cout << std::endl;
//    my_set.dump(std::cout);
//   std::cout << std::endl;
//   std::cout << std::endl;
//   return 0;
// }


// int test_insert_1()
// { 
//   ADS_set<int,3> my_set(7);

//   std::cout << std::endl;
//   my_set.insert(33);
//   my_set.insert(33);
//   my_set.insert(5);
//   my_set.insert(7);
//   my_set.insert(9);
//   my_set.dump(std::cout);
//   std::cout << std::endl;

//   std::cout << (my_set.contains(5)? "Number found" : "Number not found") << std::endl;
//   std::cout << (my_set.contains(8)? "Number found" : "Number not found") << std::endl;
//   return 0;
// }

// int main_1()
// {
//  int arr[10] = {1,2,3,4,5,6,7,8,9,10};
//  int arr1[20];
//  for(size_t i{0}; i < 10; i++)
//  {
//   arr1[i] = arr[i];
//   arr1[i+10] = arr[i];
//  }
 
 
//  for(size_t i{0}; i < 20; i++)
//  {
//    std::cout << arr1[i] << " ";
//  }
//  return 0;
// }


// int test_insert_2()
// {
//  ADS_set<std::string,3> my_set(7);    
//   //std::cout << my_set << " ";
//   std::cout << std::endl;
//   my_set.insert("H");
//   my_set.insert("J");
//   my_set.insert("F");
//   my_set.insert("J");
//   my_set.insert("H");
//   my_set.dump(std::cout);
//   std::cout << std::endl;
//   //bool found = my_set.search(5);
//   std::cout << (my_set.contains("H")? "Number found" : "Number not found") << std::endl;
//   std::cout << (my_set.contains("M")? "Number found" : "Number not found") << std::endl;
  
//   return 0;
// }

// int test_remove_1(){
//   ADS_set<int, 5> my_set(4);
//   for (int i = 0; i < 20; i++){
//     my_set.insert(i);
//   }
//   my_set.erase(8);
//   my_set.erase(9);
//   std::cout << std::endl;
//   my_set.dump(std::cout);
//   std::cout << std::endl;
//   std::cout << std::endl;
//   std::cout << "set size is " << my_set.size() << std::endl;
//   return 0;
// }



//   // я хочу создать переменную типа integer и присвоить ей значение 9. но таким образом
//   // чтобы класс integer был шаблоном




// int main(){
//   test_remove_1();
//   //template <typename T, T val>
//   //  int i = 9;
//   //ADS_set<int, 3> i{9};
//   return 0;
// }

int test_constructors(){
    // default constructor
    ADS_set<int, 3> my_set_1;
    my_set_1.dump(std::cout);
    std::cout << std::endl;

    // constructor with one parameter
    ADS_set<int, 3> my_set_2(5);
    my_set_2.dump(std::cout);
    std::cout << std::endl;

    // constructor with initializer list
    ADS_set<int, 3> my_set_3{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 42, 1231, 5646, 756};
    my_set_3.dump(std::cout);
    std::cout << std::endl;
    my_set_3.insert(526);
    my_set_3.insert(800);
    my_set_3.insert(900);


    // copy constructor
    ADS_set<int, 3> my_set_4 = my_set_3;
    my_set_4.dump(std::cout);
    std::cout << std::endl;
    
    //iterator  constructor
    std::vector<int> v{1,2,3,4,5,6};
    ADS_set<int,3> my_set5(v.begin(),v.end());
    my_set5.dump(std::cout);
    std::cout << std::endl; 
    return  0;
}


int main() {
    // // Test for default constructor
    // ADS_set<int, 2> my_set_default;
    // std::cout << "Default Constructor Test: " << std::endl;
    // my_set_default.dump(std::cout);
    // std::cout << "Set size is " << my_set_default.size() << std::endl;
    // std::cout << std::endl;
    // std::cout<<std::endl;
    // std::cout << "Set size is " << my_set_init.size() << std::endl;


    test_constructors();

    // int** array = new int[0];
    // for (int i = 0; i < 5; i++){
    //     array[i];
    // }

    return 0;
}
