#include <iostream>
#include <map>

int main()
{

    for(auto iter = dict.begin(); iter != dict.end(); iter++){
        std::cout << iter->first << " : " << iter->second << std::endl;
    }

    std::cout << dict["banana"] << std::endl;

    auto it = dict.find("lemon");
    std::cout << it->first << " : " << it->second << std::endl;
}
