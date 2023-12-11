#include <stdio.h>
#include "differential.h"
#include "recursive_descent.h"
#include "file_reader.h"

int main ()
{
    Tree tree = {};
    FILE* file = fopen ("data_base.txt", "r");
    Position data = {};

    char* data_buf = TreeCtor (&tree, file);
    tree.root = GetG (data_buf, &data);
    ExpressionTreeDump (tree.root, ORIGINAL);

    FILE* latex_file = fopen ("eval.tex", "w");
    PrintLatex (tree.root, latex_file);
    fprintf (latex_file, "=");


    //TreeNode* dif_root = DiffTree (tree.root);
    TreeNode* dif_root = Taylor (tree.root, 3);
    TreeDel (tree.root);

    OptimiseExpressionTree (dif_root, FIRST_PASS);

    PrintLatex (dif_root, latex_file);
    fclose (latex_file);

    ExpressionTreeDump (dif_root, CALCULATED);

    TreeDel (dif_root);
}
