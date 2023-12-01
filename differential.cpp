#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "differential.h"

long FileSize (FILE* file)
{
    fseek (file, 0, SEEK_END);
    long size = ftell (file);
    fseek (file, 0, SEEK_SET);
    return size;
}

char* ReadText (long file_size, FILE* file)
{
    char* buf = (char*)calloc (file_size + 1, sizeof (char));
    fread (buf, sizeof (char), file_size, file);
    return buf;
}

char* EvalCtor (Tree* tree)
{
    tree->root = (TreeNode*)calloc (1, sizeof (TreeNode));
    tree->root->value = EMPTY;
    tree->root->left = NULL;
    tree->root->right = NULL;
    tree->root->type = EMPTY;

    tree->counter = 0;

    tree->file = fopen ("data_base.txt", "r");
    long file_size = FileSize (tree->file);
    char* tmp_buf = ReadText (file_size, tree->file);
    fclose (tree->file);
    tree->file = fopen ("data_base.txt", "w");
    return tmp_buf;
}

void EvalDtor (Tree* tree, TreeNode* node)
{
    if (tree->counter == 0)
    {
        fclose (tree->file);
        tree->counter++;
    }
    if (node != NULL)
    {
        node->value = EMPTY;
        node->type = EMPTY;
        EvalDtor (tree, node->left);
        EvalDtor (tree, node->right);
        free (node);
        tree->counter = 0;
    }
}

int Eval (TreeNode* node)
{
    if (node->type == NUM)
    {
        return node->value;
    }

    int left = Eval (node->left);
    int right = Eval (node->right);

    switch (node->value)
    {
        case OP_DIV:
            return left / right;
        case OP_SUB:
            return left - right;
        case OP_MUL:
            return left * right;
        case OP_ADD:
            return left + right;
        case OP_EXP:
            return pow (left, right);
    }
    return 0;
}

void BaseOfData (TreeNode** node, char* data_buf, int* tmp_count)
{
    int count = 0;
    char tmp_elem[100] = {};
    sscanf (data_buf + *tmp_count, "%s%n", tmp_elem, &count);
    *tmp_count += count;
    while (strcmp (")", tmp_elem) == 0)
    {
        sscanf (data_buf + *tmp_count, "%s%n", tmp_elem, &count);
        *tmp_count += count;
    }
    if (strcmp ("(", tmp_elem) == 0)
    {
        if (*tmp_count != count)
        {
            *node = (TreeNode*)calloc (1, sizeof (TreeNode));
        }
        int arg = 0;
        if (sscanf (data_buf + *tmp_count, "%d%n", &arg, &count) == 1)
        {
            (*node)->value = arg;
            (*node)->type = NUM;
            *tmp_count += count;
        }
        else
        {
            sscanf (data_buf + *tmp_count, "%s%n", tmp_elem, &count);
            if (strcmp ("div", tmp_elem) == 0)
            {
                (*node)->value = OP_DIV;
                (*node)->type = OP;
            }
            else if (strcmp ("sub", tmp_elem) == 0)
            {
                (*node)->value = OP_SUB;
                (*node)->type = OP;
            }
            else if (strcmp ("mul", tmp_elem) == 0)
            {
                (*node)->value = OP_MUL;
                (*node)->type = OP;
            }
            else if (strcmp ("add", tmp_elem) == 0)
            {
                (*node)->value = OP_ADD;
                (*node)->type = OP;
            }
            else if (strcmp ("exp", tmp_elem) == 0)
            {
                (*node)->value = OP_EXP;
                (*node)->type = OP;
            }
            else
            {
                (*node)->value = 0;
                (*node)->type = VAR;
            }
            *tmp_count += count;
        }
    }
    if (strcmp ("nil", tmp_elem) == 0)
    {
        *node = NULL;
        return;
    }
    if (strcmp ("\0", tmp_elem) == 0)
    {
        return;
    }
    BaseOfData (&((*node)->left), data_buf, tmp_count);
    BaseOfData (&((*node)->right), data_buf, tmp_count);
}

void PrintfNode (const TreeNode* node, const Tree* tree)
{
    if (node == NULL)
    {
        fprintf (tree->file, " nil ");
        return;
    }
    fprintf (tree->file, " ( ");
    if (node->type == NUM)
    {
        fprintf (tree->file, " %d ", node->value);
    }
    else if (node->type == OP)
    {
        if (node->value == OP_DIV)
        {
            fprintf (tree->file, " div ");
        }
        else if (node->value == OP_SUB)
        {
            fprintf (tree->file, " sub ");
        }
        else if (node->value == OP_MUL)
        {
            fprintf (tree->file, " mul ");
        }
        else if (node->value == OP_ADD)
        {
            fprintf (tree->file, " add ");
        }
        else if (node->value == OP_EXP)
        {
            fprintf (tree->file, " exp ");
        }
    }
    else if (node->type == VAR)
    {
        fprintf (tree->file, " x ");
    }
    PrintfNode (node->left, tree);
    PrintfNode (node->right, tree);
    fprintf (tree->file, " ) ");
}

void Insert (TreeNode** node, char* answer)
{
    if ((*node)->value == EMPTY)
    {
        if (strcmp ("div", answer) == 0)
        {
            (*node)->value = OP_DIV;
            (*node)->type = OP;
        }
        else if (strcmp ("sub", answer) == 0)
        {
            (*node)->value = OP_SUB;
            (*node)->type = OP;
        }
        else if (strcmp ("mul", answer) == 0)
        {
            (*node)->value = OP_MUL;
            (*node)->type = OP;
        }
        else if (strcmp ("add", answer) == 0)
        {
            (*node)->value = OP_ADD;
            (*node)->type = OP;
        }
        else if (strcmp ("exp", answer) == 0)
        {
            (*node)->value = OP_EXP;
            (*node)->type = OP;
        }
        else if (strcmp ("x", answer) == 0)
        {
            (*node)->value = 0;
            (*node)->type = VAR;
        }
        else
        {
            (*node)->value = atoi (answer);
            (*node)->type = NUM;
        }
    }

}

TreeNode* c (Tree* tree, TreeNode* node) // copy tree
{
    if (tree->counter == 0)
    {
        tree->cpy_root = (TreeNode*)calloc (1, sizeof (TreeNode)) ;
        tree->cpy_root->value = node->value;
        tree->cpy_root->type = node->type;
        tree->cpy_root->left = node->left;
        tree->cpy_root->right = node->right;
        tree->counter++;
    }
    else if (tree->counter != 0)
    {
        TreeNode* cpy_node = (TreeNode*)calloc (1, sizeof (TreeNode)) ;
        cpy_node->value = node->value;
        cpy_node->type = node->type;
        cpy_node->left = node->left;
        cpy_node->right = node->right;
    }
    if (node->left != 0)
        c (tree, node->left);
    if (node->right != 0)
        c (tree, node->right);
    tree->counter = 0;
    return tree->cpy_root;
}

TreeNode* CreateNode (enum TYPES TYPE, int VALUE, TreeNode* left, TreeNode* right)
{
    TreeNode* node = (TreeNode*)calloc (1, sizeof (TreeNode));
    node->type = TYPE;
    node->value = VALUE;
    node->left = left;
    node->right = right;
    return node;
}

TreeNode* d (Tree* tree, const TreeNode* node)
{
    if (node->type == NUM)
        return CreateNode (NUM, 0, NULL, NULL);
    if (node->type == VAR)
        return CreateNode (NUM, 1, NULL, NULL);

    switch (node->value)
    {
        case OP_ADD:
            return CreateNode (OP, OP_ADD, d (tree, node->left), d (tree, node->right));
        case OP_SUB:
            return CreateNode (OP, OP_SUB, d (tree, node->left), d (tree, node->right));
        case OP_MUL:
            return CreateNode (OP, OP_ADD, CreateNode (OP, OP_MUL, d (tree, node->left), c (tree, node->right)),
                                           CreateNode (OP, OP_MUL, c (tree, node->left), d (tree, node->right)));
        case OP_DIV:
            return CreateNode (OP, OP_DIV, CreateNode (OP, OP_SUB,
                                                       CreateNode (OP, OP_MUL, d (tree, node->left), c (tree, node->right)),
                                                       CreateNode (OP, OP_MUL, c (tree, node->left), d (tree, node->right))),
                                           CreateNode (OP, OP_MUL, c (tree, node->right), c (tree, node->right)));
        case OP_EXP:
            return CreateNode (OP, OP_MUL, c (tree, node->right),
                                           CreateNode (OP, OP_EXP, c (tree, node->left),
                                                                   CreateNode (OP, OP_SUB, c (tree, node->right),
                                                                                           CreateNode (NUM, 1, NULL, NULL))));

    };
    return 0;
}

void EvalDump (Tree* tree, TreeNode* node, enum EXPRESSION EXPRESSION)
{
    FILE* file = NULL;
    if (EXPRESSION == ORIGINAL)
    {
        file = fopen ("original.dot", "w");
    }
    else if (EXPRESSION == CALCULATED)
    {
        file = fopen ("calculated.dot", "w");
    }
    fprintf (file, "digraph G { \n"
                    "node [shape = record];\n"
                    " %o ", node);
    TreeBody (node, file);
    fprintf (file, "}\n");
    fclose (file);
    if (EXPRESSION == ORIGINAL)
    {
        system ("dot -T png original.dot -o original.png");
    }
    else if (EXPRESSION == CALCULATED)
    {
        system ("dot -T png calculated.dot -o calculated.png");
    }
}

void TreeBody (TreeNode* node, FILE* file)
{
    if (node->type == NUM)
    {
        fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                       " label = \" %d \"];\n",node, node->value);
    }
    else if (node->type == OP)
    {
        if (node->value == OP_MUL)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                           " label = \" * \"];\n",node);
        }
        else if (node->value == OP_DIV)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                           " label = \" / \"];\n",node);
        }
        else if (node->value == OP_SUB)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                           " label = \" - \"];\n",node);
        }
        else if (node->value == OP_ADD)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                           " label = \" + \"];\n",node);
        }
        else if (node->value == OP_EXP)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                           " label = \" ^ \"];\n",node);
        }
    }
    else if (node->type == VAR)
    {
        fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                       " label = \" x \"];\n",node);
    }

    if (node->left != NULL)
    {
        fprintf (file, "%o -> %o", node, node->left);
    }
    else
    {
        return;
    }

    if (node->right != NULL)
    {
        fprintf (file, "\n %o -> %o \n", node, node->right);
    }
    else
    {
        return;
    }

    TreeBody (node->left, file);
    TreeBody (node->right, file);
}
