#include <stdio.h>
#include "differential.h"

int main ()
{
    Tree tree = {};
    char* data_buf = EvalCtor (&tree);

    int tmp_count = 0;
    BaseOfData (&(tree.root), data_buf, &tmp_count);
    EvalDump (&tree, tree.root, ORIGINAL);

    FILE* latex_file = fopen ("eval.tex", "w");
    EvalLatex (tree.root, latex_file);
    fprintf (latex_file, "=");

    tree.root = d (tree.root);

    Simpler (tree.root, FIRST_PASS);
    EvalLatex (tree.root, latex_file);
    fclose (latex_file);
    EvalDump (&tree, tree.root, CALCULATED);

    PrintfNode (tree.root, &tree);

    //EvalDtor (&tree, tree.root);
}
