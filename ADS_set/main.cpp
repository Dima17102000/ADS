#include "ADS_set.h"
#include <iostream>



int main()
{ 
  ADS_set<int,3> my_set(7);    
  //std::cout << my_set << " ";
  std::cout << std::endl;
  my_set.insert(33);
  my_set.insert(33);
  my_set.insert(5);
  my_set.insert(7);
  my_set.insert(9);
  std::cout << my_set << " ";
  std::cout << std::endl;
  //bool found = my_set.search(5);
  std::cout << (my_set.contains(5)? "Number found" : "Number not found") << std::endl;
  std::cout << (my_set.contains(8)? "Number found" : "Number not found") << std::endl;
  my_set.expandDirectory();
  std::cout << my_set << std::endl;
  return 0;
}

int main_1()
{
 int arr[10] = {1,2,3,4,5,6,7,8,9,10};
 int arr1[20];
 for(size_t i{0}; i < 10; i++)
 {
  arr1[i] = arr[i];
  arr1[i+10] = arr[i];
 }
 
 
 for(size_t i{0}; i < 20; i++)
 {
   std::cout << arr1[i] << " ";
 }
}


int main_2()
{
 ADS_set<std::string,3> my_set(7);    
  //std::cout << my_set << " ";
  std::cout << std::endl;
  my_set.insert("H");
  my_set.insert("J");
  my_set.insert("F");
  my_set.insert("J");
  my_set.insert("H");
  std::cout << my_set << " ";
  std::cout << std::endl;
  //bool found = my_set.search(5);
  std::cout << (my_set.contains("H")? "Number found" : "Number not found") << std::endl;
  std::cout << (my_set.contains("M")? "Number found" : "Number not found") << std::endl;
  
  return 0;
}
