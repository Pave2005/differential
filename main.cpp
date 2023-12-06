#include <stdio.h>
#include "differential.h"

int main ()
{
    Tree tree = {};
    Position data = {};

    char* data_buf = EvalCtor (&tree);
    tree.root = GetG (data_buf, &data);
    EvalDump (tree.root, ORIGINAL);

    FILE* latex_file = fopen ("eval.tex", "w");
    EvalLatex (tree.root, latex_file);
    fprintf (latex_file, "=");


    //TreeNode* dif_root = d (tree.root);
    TreeNode* dif_root = Taylor (tree.root, 3);
    TreeDel (tree.root);

    Optimizer (dif_root, FIRST_PASS);

    EvalLatex (dif_root, latex_file);
    fclose (latex_file);

    EvalDump (dif_root, CALCULATED);

    EvalDtor (&tree, dif_root);
}
