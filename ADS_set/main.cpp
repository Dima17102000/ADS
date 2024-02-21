#include "ADS_set.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <string>
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

// int test_constructors(){
//     // default constructor
//     ADS_set<int, 3> my_set_1;
//     my_set_1.dump(std::cout);
//     std::cout << std::endl;

//     // constructor with one parameter
//     ADS_set<int, 3> my_set_2(5);
//     my_set_2.dump(std::cout);
//     std::cout << std::endl;

//     // constructor with initializer list
//     ADS_set<int, 3> my_set_3{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 42, 1231, 5646, 756};
//     my_set_3.dump(std::cout);
//     std::cout << std::endl;
//     my_set_3.insert(526);
//     my_set_3.insert(800);
//     my_set_3.insert(900);


//     // copy constructor
//     ADS_set<int, 3> my_set_4 = my_set_3;
//     my_set_4.dump(std::cout);
//     std::cout << std::endl;
    
//     //iterator  constructor
//     std::vector<int> v{1,2,3,4,5,6};
//     ADS_set<int,3> my_set5(v.begin(),v.end());
//     my_set5.dump(std::cout);
//     std::cout << std::endl; 
//     return  0;
// }

// clang++ main.cpp  -o main

int teststresstest()
{
    size_t  const n = 60;
    using   my_type = int;
    std::vector<my_type>  vs(n);
    std::iota(vs.begin(),vs.end(),0);
    //std::random_shuffle(vs.begin(),vs.end());
    
    vs  =   {4,51,11,15,24,35,17,36,26,50,33,39,19,2,47,56,59,18,32,40,5,21,57,12,14,42,27,6,45,58,30,38,13};
    //ADS_set<my_type,5>  s;
    
    
    ADS_set<my_type,3> s;
    std::cout<< "s.dir_size = " <<    s.get_depth() <<  std::endl;
    s.insert(vs.begin(),vs.end());
    std::cout<<std::endl;
    std::cout   << "s.size ="   <<  s.size()    <<  std::endl;
    std::cout   << "s.dir_size = "   <<  s.get_depth() <<  std::endl;    
    std::cout<<std::endl;
    s.dump();
    //s.empty_2();
    s.test123();
    s.test124();
    s.test125();
    return  1;
}




/*
int dumptest()
{
  //size_t const n = 10;
  //using my_type = int;
  std::vector<std::string>vs={"A","B","C","D","E","F","G","M","K","L"};
  //std::iota(vs.begin(),vs.end(),0);
  //std::random_shuffle(vs.begin(),vs.end());
  ADS_set<std::string,2>s;
  s.insert(vs.begin(),vs.end());
  s.dump();
  return 1;
}
*/
int main() {
    // // Test for default constructor
    // ADS_set<int, 2> my_set_default;
    // std::cout << "Default Constructor Test: " << std::endl;
    // my_set_default.dump(std::cout);
    // std::cout << "Set size is " << my_set_default.size() << std::endl;
    // std::cout << std::endl;
    // std::cout<<std::endl;
    // std::cout << "Set size is " << my_set_init.size() << std::endl;


    // test_constructors();

    // int** array = new int[0];
    // for (int i = 0; i < 5; i++){
    //     array[i];
    // }
    teststresstest();
    //dumptest();
    return 0;
}
