#include "file_reader.h"
#include "differential.h"

char* TreeCtor (Tree* tree, FILE* file)
{
    tree->root = (TreeNode*)calloc (1, sizeof (TreeNode));
    tree->root->value = EMPTY;
    tree->root->left = NULL;
    tree->root->right = NULL;
    tree->root->type = EMPTY;

    long file_size = FileSize (file);
    char* tmp_buf = ReadText (file_size, file);
    fclose (file);
    return tmp_buf;
}

void SubstitutePoint (TreeNode* node, const float value)
{
    if (node == NULL)
        return;

    if (node->type == VAR)
    {
        node->type = NUM;
        node->value = (float)value;
    }
    else
    {
        SubstitutePoint (node->left, value);
        SubstitutePoint (node->right, value);
    }
}

float EvalExpression (TreeNode* node, const float value)
{
    SubstitutePoint (node, value);
    return EvalNumExpression (node);
}

float EvalBinaryOp (TreeNode* node)
{
    float right_num = EvalNumExpression (node->right);
    float left_num = EvalNumExpression (node->left);

    switch ((int)node->value)
    {
        case OP_DIV:
            return left_num / right_num;
        case OP_SUB:
            return left_num - right_num;
        case OP_MUL:
            return left_num * right_num;
        case OP_ADD:
            return left_num + right_num;
        case OP_EXP:
            return pow (left_num, right_num);
    }
    return 0;
}

float EvalUnaryOp (TreeNode* node)
{
    float num = EvalNumExpression (node->right);
    switch ((int)node->value)
    {
        case SIN:
            return sin (num);
        case COS:
            return cos (num);
        case TAN:
            return tan (num);
        case COT:
            return 1 / tan (num);
        case LN:
            return log (num);
    };
    return 0;
}

float EvalNumExpression (TreeNode* node)
{
    switch (node->type)
    {
        case NUM:
            return (float)node->value;
        case FUNC:
            return EvalUnaryOp (node);
        case OP:
            return EvalBinaryOp (node);
    };
    return 0;
}

TreeNode* CopyTree (TreeNode* node)
{
    TreeNode* cpy_node = (TreeNode*)calloc (1, sizeof (TreeNode));

    cpy_node->type = node->type;
    cpy_node->value = node->value;

    if ( node->left != NULL )
    {
        cpy_node->left = CopyTree (node->left);
    }

    if ( node->right != NULL )
    {
        cpy_node->right = CopyTree (node->right);
    }

    return cpy_node;
}

TreeNode* NewNode (enum TYPES node_type, int node_value, TreeNode* left, TreeNode* right)
{
    TreeNode* node = (TreeNode*)calloc (1, sizeof (TreeNode));
    node->type = node_type;
    node->value = node_value;
    node->left = left;
    node->right = right;
    return node;
}

TreeNode* DiffUnaryOp (int value, TreeNode* expr)
{
    switch (value)
    {
        case SIN:
            return SIN(expr);
        case COS:
            return COS(expr);
        case TAN:
            return TAN(expr);
        case COT:
            return COT(expr);
        case LN:
            return LN(expr);
    };
    return 0;
}

TreeNode* DiffBinaryOp (int value, TreeNode* expr1, TreeNode* expr2)
{
    switch (value)
    {
        case OP_ADD:
            return ADD(expr1, expr2);
        case OP_SUB:
            return SUB(expr1, expr2);
        case OP_MUL:
            return MUL(expr1, expr2);
        case OP_DIV:
            return DIV(expr1, expr2);
        case OP_EXP:
            return EXP(expr1, expr2);
    };
    return 0;
}

TreeNode* DiffTree (const TreeNode* node)
{
    if (node->type == NUM || (node->type == FUNC && node->right->type == NUM))
        return NewNode (NUM, 0, NULL, NULL);
    if (node->type == VAR)
        return NewNode (NUM, 1, NULL, NULL);
    if (node->type == FUNC)
    {
        return DiffUnaryOp ((int)node->value, node->right);
    }
    return DiffBinaryOp ((int)node->value, node->left, node->right);
}

TreeNode* Taylor (const TreeNode* node, const int order)
{
    TreeNode* tmp_func = CopyTree ((TreeNode*)node);

    if (order == 0)
    {
        TreeNode* taylor_tree = NewNode (NUM, EvalExpression (tmp_func, 0), NULL, NULL);
        TreeDel (tmp_func);
        return taylor_tree;
    }

    TreeNode* taylor_tree =  NewNode (OP, OP_ADD,
                                     NewNode (NUM, EvalExpression (tmp_func, 0), NULL, NULL),
                                     GetPartialSum (node, order));
    TreeDel (tmp_func);
    return taylor_tree;
}

TreeNode* GetPartialSum (const TreeNode* node, const int order)
{
    TreeNode* left_sum = NULL;

    for (int i = 0; i < order; i++)
    {
        if (left_sum == NULL)
            left_sum = GetSeriesMember (node, i + 1);
        else
        {
            left_sum = NewNode (OP, OP_ADD, CopyTree (left_sum), GetSeriesMember (node, i + 1));
        }
    }

    return left_sum;
}

TreeNode* GetSeriesMember (const TreeNode* node, const int order)
{
    TreeNode* tmp_diff = CopyTree ((TreeNode*)node);
    TreeNode* tmp_tree = NULL;
    for (int i = 0; i < order; i++)
    {
        tmp_tree = tmp_diff;
        tmp_diff = DiffTree (tmp_diff);
        if (tmp_tree != node)
            TreeDel (tmp_tree);
    }

    return NewNode (OP, OP_MUL,
                       NewNode (OP, OP_DIV,
                                   NewNode (NUM, EvalExpression (tmp_diff, 0), NULL, NULL),
                                   NewNode (NUM, Factorial (order), NULL, NULL)),
                       NewNode (OP, OP_EXP,
                                   NewNode (VAR, 0, NULL, NULL),
                                   NewNode (NUM, order, NULL, NULL)));
}

int Factorial (const int num)
{
    if (num == 0)
        return 1;

    int value = 1;

    for (int i = 1; i <= num; i++)
        value *= i;

    return value;
}

void ExpressionTreeDump (TreeNode* node, enum EXPRESSION cmd_expression)
{
    FILE* file = NULL;
    if (cmd_expression == ORIGINAL)
    {
        file = fopen ("original.dot", "w");
    }
    else if (cmd_expression == CALCULATED)
    {
        file = fopen ("calculated.dot", "w");
    }
    fprintf (file, "digraph G { \n"
                    "node [shape = record];\n"
                    " %o ", node);
    DumpTreeNode (node, file);
    fprintf (file, "}\n");
    fclose (file);

    if (cmd_expression == ORIGINAL)
    {
        system ("dot -T png original.dot -o original.png");
    }
    else if (cmd_expression == CALCULATED)
    {
        system ("dot -T png calculated.dot -o calculated.png");
    }
}

char GetOpChar (int command)
{
    switch (command)
    {
        case OP_MUL:
            return '*';
        case OP_DIV:
            return '/';
        case OP_ADD:
            return '+';
        case OP_SUB:
            return '-';
        case OP_EXP:
            return '^';
    };

    return 0;
}

char* GetFuncName (int func)
{
    switch (func)
    {
        case SIN:
            return "sin";
        case COS:
            return "cos";
        case TAN:
            return "tg";
        case COT:
            return "ctg";
        case LN:
            return "ln";
    };

    return NULL;
}

int GetFuncCode (char* func_name)
{
    if (strcmp ("sin", func_name) == 0)
        return SIN;
    else if (strcmp ("cos", func_name) == 0)
        return COS;
    else if (strcmp ("tg", func_name) == 0)
        return TAN;
    else if (strcmp ("ctg", func_name) == 0)
        return COT;
    else if (strcmp ("ln", func_name) == 0)
        return LN;

    return 0;
}

void DumpTreeNode (TreeNode* node, FILE* file)
{
    if (node->type == NUM)
    {
        fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                       " label = \" %f \"];\n",
                       node,
                       (float)node->value);
    }
    else if (node->type == FUNC)
    {
        fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                       " label = \" %s \"];\n",
                       node,
                       GetFuncName ((int)node->value));
    }
    else if (node->type == OP)
    {
        fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                       " label = \" %c \"];\n",
                       node,
                       GetOpChar ((int)node->value));
    }
    else if (node->type == VAR)
    {
        fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = lightblue2 "
                       " label = \" x \"];\n",
                       node);
    }

    if (node->type != FUNC)
    {
        if (node->left != NULL)
            fprintf (file, "%o -> %o", node, node->left);
        else
            return;
    }

    if (node->right != NULL)
        fprintf (file, "\n %o -> %o \n", node, node->right);
    else
        return;

    if (node->type != FUNC)
        DumpTreeNode (node->left, file);
    DumpTreeNode (node->right, file);
}

void OptimiseNumTreeNode (TreeNode* node)
{
    node->value = EvalNumExpression (node);
    node->type = NUM;
    TreeDel (node->left);
    TreeDel (node->right);
    node->left = NULL;
    node->right = NULL;
}

void OptimiseAndDelSubTree (TreeNode* node_parent, TreeNode* sub_node)
{
    if (sub_node == node_parent->right)
        TreeDel (node_parent->left);
    else
        TreeDel (node_parent->right);

    node_parent->type = sub_node->type;
    node_parent->value = sub_node->value;

    TreeNode* tmp_sub_node_right = sub_node->right;
    TreeNode* tmp_sub_node_left = sub_node->left;
    free (sub_node);
    node_parent->left = tmp_sub_node_left;
    node_parent->right = tmp_sub_node_right;
}

void OptimiseSpecialCases (TreeNode* node, float value)
{
    TreeDel (node->left);
    TreeDel (node->right);
    node->type = NUM;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
}

void OptimiseExpressionTree (TreeNode* node, enum PASS pass)
{
    if (node->type == OP)
    {
        if (node->value == OP_ADD || node->value == OP_SUB)
        {
            if (node->left->type == NUM && node->right->type == NUM)
            {
                OptimiseNumTreeNode (node);
            }
            else if (CHECK_NUM(node->left, 0))
            {
                OptimiseAndDelSubTree (node, node->right);
            }
            else if (CHECK_NUM(node->right, 0))
            {
                OptimiseAndDelSubTree (node, node->left);
            }
        }
        else if (node->value == OP_MUL || node->value == OP_DIV)
        {
            if (node->left->type == NUM && node->right->type == NUM)
            {
                OptimiseNumTreeNode (node);
            }

            else if (CHECK_NUM(node->left, 0) ||CHECK_NUM(node->right, 0))
            {
                OptimiseSpecialCases (node, 0);
            }
            else if (node->left->type != NUM && node->right->type == NUM)
            {
                if (node->right->value == 1)
                {
                    OptimiseAndDelSubTree (node, node->left);
                }
            }
            else if (node->left->type == NUM && node->right->type != NUM && node->value == OP_MUL)
            {
                if (node->left->value == 1)
                {
                    OptimiseAndDelSubTree (node, node->right);
                }
            }
        }
        else if (node->value == OP_EXP)
        {
            if (CHECK_NUM(node->right, 0))
            {
                OptimiseSpecialCases (node, 1);
            }
            else if (CHECK_NUM(node->right, 1))
            {
                OptimiseAndDelSubTree (node, node->left);
            }
            else if (node->left->type == NUM && node->right->type == NUM)
            {
                OptimiseNumTreeNode (node);
            }
            else if (node->left->type == OP && node->left->value == OP_EXP)
            {
                if (node->right->type == NUM && node->left->right->type == NUM)
                {
                    node->left->right->value = node->left->right->value * node->right->value;
                    free (node->right);
                    node->type = node->left->type;
                    node->value = node->left->value;
                    node->right = node->left->right;

                    TreeNode* tmp_left = node->left->left;
                    free (node->left);
                    node->left = tmp_left;
                }
            }
        }
    }
    if (pass == FIRST_PASS)
    {
        if (node->left != NULL)
            OptimiseExpressionTree (node->left, FIRST_PASS);
        if (node->right != NULL)
            OptimiseExpressionTree (node->right, FIRST_PASS);
        OptimiseExpressionTree (node, SECOND_PASS);
    }
}

void TreeDel (TreeNode* node)
{
    if (node != NULL)
    {
        TreeDel (node->left);
        TreeDel (node->right);
        free (node);
    }
}

void PutBracketsBefore (TreeNode* node, FILE* latex_file, char bracket_type)
{
    if (node->value == OP_MUL)
    {
        if (node->left->value != OP_MUL && node->left->value != OP_EXP &&
           (node->left->value != OP_DIV || node->right->value != OP_DIV))
            fprintf (latex_file, "%c", bracket_type);
    }
    else if (node->value != OP_DIV)
    {
        if (node->left->value != OP_MUL && node->left->value != OP_DIV &&
           (node->left->value != OP_ADD && node->left->value != OP_SUB) &&
           node->value != OP_EXP)
            fprintf (latex_file, "%c", bracket_type);
    }
}

void PutBracketsAfter (TreeNode* node, FILE* latex_file, char bracket_type)
{
    if (node->value == OP_MUL)
    {
        if ((node->right->value != OP_MUL && node->right->value != OP_EXP) &&
           (node->left->type != NUM || (node->right->type != OP || node->right->value != OP_DIV)) &&
           (node->left->value != OP_DIV || node->right->value != OP_DIV))
            fprintf (latex_file, "%c", bracket_type);
    }
    else if (node->value != OP_DIV)
    {
        if (node->right->value != OP_MUL && node->right->value != OP_DIV &&
           (node->right->value != OP_ADD && node->right->value != OP_SUB) &&
           node->value != OP_EXP)
            fprintf (latex_file, "%c", bracket_type);
    }
}

void PutOp (TreeNode* node, FILE* latex_file)
{
    if (node->value == OP_DIV)
        fprintf (latex_file, "}{");
    else if (node->value == OP_MUL)
    {
        if ((node->left->type != NUM || (node->right->type != VAR &&
           ((node->right->type != OP || node->right->value != OP_EXP) || node->right->left->type != VAR)))
           && (node->left->type != NUM || node->left->value != -1))
            fprintf (latex_file, " \\cdot ");
    }
    else
    {
        fprintf (latex_file, "%c", GetOpChar ((int)node->value));
        if (node->value == OP_EXP)
            fprintf (latex_file, "{");
    }
}

void LatexOp (TreeNode* node, FILE* latex_file)
{
    if (node->value == OP_DIV)
        fprintf (latex_file, " \\frac{");

    switch (node->left->type)
    {
        case NUM:
        {
            if (node->value == OP_MUL)
            {
                if (node->left->value == -1)
                    fprintf (latex_file, "-");
                else
                    fprintf (latex_file, "%f", (float)node->left->value);
            }
            else
                fprintf (latex_file, "%f", (float)node->left->value);
            break;
        }
        case VAR:
            fprintf (latex_file, "x");
            break;
        default:
        {
            PutBracketsBefore (node, latex_file, '(');
            PrintLatex (node->left, latex_file);
            PutBracketsBefore (node, latex_file, ')');
            break;
        }
    };

    PutOp (node, latex_file);

    switch (node->right->type)
    {
        case NUM:
            fprintf (latex_file, "%f", (float)node->right->value);
            break;
        case VAR:
            fprintf (latex_file, "x");
            break;
        default:
        {
            PutBracketsAfter (node, latex_file, '(');
            PrintLatex (node->right, latex_file);
            PutBracketsAfter (node, latex_file, ')');
            break;
        }
    };

    if (node->value == OP_EXP || node->value == OP_DIV)
        fprintf (latex_file, "}");
}


void PrintLatex (TreeNode* node, FILE* latex_file)
{
    switch (node->type)
    {
        case OP:
            LatexOp (node, latex_file);
            break;
        case FUNC:
            TextFuncName (node->right, latex_file, GetFuncName ((int)node->value));
            break;
        case NUM:
            fprintf (latex_file, "%f", (float)node->value);
            break;
        case VAR:
            fprintf (latex_file, "x");
            break;
    };
}

void TextFuncName (TreeNode* node, FILE* file, char* func_name)
{
    fprintf (file, "%s ", func_name);
    if (node->type != OP || node->value != OP_DIV)
        fprintf (file, "(");
    PrintLatex (node, file);
    if (node->type != OP || node->value != OP_DIV)
        fprintf (file, ")");
}
