#include "radix_tree.hpp"
#include "compressed_radix_tree.hpp"
#include <cstring>
#include <fstream>
#include <iostream>

// input file format:
// n
// op1 v1
// ...
// opn vn

// output file format:
// result1
// ...
// resultn
void testTree(const char *input, const char *output, TreeType treeType)
{
    Tree *tree;
    switch (treeType)
    {
    case TreeType::RadixT:
        tree = new class RadixTree();
        break;
    case TreeType::CompressedRadixT:
        tree = new class CompressedRadixTree();
        break;
    default:
        break;
    }
    std::ifstream ifs(input);
    std::ofstream ofs(output);
    int n;
    ifs >> n;
    for (int i = 0; i < n; i++)
    {
        char op[10];
        int value;
        ifs >> op;
        if (strcmp(op, "print") != 0)
        {
            ifs >> value;
        }
        if (strcmp(op, "insert") == 0)
        {
            tree->insert(value);
        }
        else if (strcmp(op, "remove") == 0)
        {
            tree->erase(value);
        }
        else if (strcmp(op, "find") == 0)
        {
            ofs << (tree->find(value) ? "True" : "False") << std::endl;
        }
        else if (strcmp(op, "print") == 0)
        {
            ofs << tree->size() << " " << tree->height() << std::endl;
        }
    }
}

// argv[1]: input file path
// argv[2]: output file path
// argv[3]: tree type
int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cout << "[usage]: ./main [input_file_path] [output_file_path] [tree_type]" << std::endl;
        return 0;
    }
    TreeType treeType;
    if (strcmp(argv[3], "RadixTree") == 0)
    {
        treeType = TreeType::RadixT;
    }
    else if (strcmp(argv[3], "CompressedRadixTree") == 0)
    {
        treeType = TreeType::CompressedRadixT;
    }
    testTree(argv[1], argv[2], treeType);
    return 0;
}