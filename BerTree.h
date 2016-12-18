#ifndef BERTREE_H
#define BERTREE_H

#include <vector>
#include <string>
#include <map>
#include <list>
#include "defines.h"

class BerTree {
public:
    BerTree();
    ~BerTree();

    void convert(std::string type);
    void print_tree(int n);

    char type;
    std::list<char> length;
    std::list<char> content;

    std::vector<BerTree> sub;


};




#endif /* BERTREE_H */
