// map::begin/end
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>

int main ()
{
  std::unordered_set<int> mymap;

  mymap.insert(100);
  mymap.insert(111);
  mymap.insert(112);
  
    if(mymap.end() != mymap.find(100))
    {
        
        std::cout<<"Here I am"<<std::endl;
    }

  return 0;
}
