#include <stdio.h>
#include "differential.h"

int main ()
{
    Tree tree = {};
    char* data_buf = EvalCtor (&tree);

    int tmp_count = 0;
    BaseOfData (&(tree.root), data_buf, &tmp_count);
    EvalDump (&tree, tree.root, ORIGINAL);

    tree.root = d (&tree, tree.root);
    EvalDump (&tree, tree.root, CALCULATED);

    PrintfNode (tree.root, &tree);

    EvalDtor (&tree, tree.root);
}
