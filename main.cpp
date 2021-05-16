#include "DBPool.h"
#include <iostream>
int main(int argc,char * argv[]){
    DBPool *pool = DBPool::getInstance();
    std::cout << pool->getPoolSize() << std::endl;
    return 0;
}
