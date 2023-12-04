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

    TreeNode* dif_root = d (tree.root);
    TreeDel (tree.root);

    Simpler (dif_root, FIRST_PASS);
    EvalLatex (dif_root, latex_file);
    fclose (latex_file);
    EvalDump (&tree, dif_root, CALCULATED);

    PrintfNode (dif_root, &tree);

    EvalDtor (&tree, dif_root);
}
