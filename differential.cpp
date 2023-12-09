#include "file_reader.h"
#include "differential.h"

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
    // static is_file_closed = 0;
    if (tree->counter == 0)
    {
        fclose (tree->file);
        // is_file_closed = 1;
        tree->counter++;
    }
    if (node != NULL)
    {
        EvalDtor (tree, node->left);
        EvalDtor (tree, node->right);
        free (node);
        tree->counter = 0;
    }
}

// SubstitutePoint
void Substitute (TreeNode* node, const float value)
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
        Substitute (node->left, value);
        Substitute (node->right, value);
    }
}

float EvalExpression (TreeNode* node, const float value)
{
    Substitute (node, value);
    return EvalNumExpression (node);
}

float EvalNumExpression (TreeNode* node)
{
    if (node->type == NUM)
    {
        return (float)node->value;
    }
    float right = EvalNumExpression (node->right);
    // EvalUnaryOp
    // EvalBinaryOp
    if (node->type == FUNC)
    {
        switch ((int)node->value)
        {
            case SIN:
                return sin (right);
            case COS:
                return cos (right);
            case TAN:
                return tan (right);
            case COT:
                return 1 / tan (right);
            case LN:
                return log (right);
                // default
        };
    }
    float left = EvalNumExpression (node->left);

    switch ((int)node->value)
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
        {
            return pow (left, right);
        }
    }
    return 0;
}

// ReadDatabase
// PrintNode
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

// TreeNide* array = new TreeNode {left, right};

// NewNode
TreeNode* CreateNode (enum TYPES node_type, int node_value, TreeNode* left, TreeNode* right)
{
    TreeNode* node = (TreeNode*)calloc (1, sizeof (TreeNode));
    node->type = node_type;
    node->value = node_value;
    node->left = left;
    node->right = right;
    return node;
}

TreeNode* DiffTree (const TreeNode* node)
{
    if (node->type == NUM || (node->type == FUNC && node->right->type == NUM))
        return CreateNode (NUM, 0, NULL, NULL);
    if (node->type == VAR)
        return CreateNode (NUM, 1, NULL, NULL);
    if (node->type == FUNC)
    {
        switch ((int)node->value)
        {
            case SIN: // сделать унарный минус
                return CreateNode (OP, OP_MUL, DiffTree (node->right), CreateNode (FUNC, COS, NULL, CopyTree (node->right)));
            case COS:
                return CreateNode (OP, OP_MUL, CreateNode (OP, OP_MUL, CreateNode (NUM, -1, NULL, NULL), DiffTree (node->right)), CreateNode (FUNC, SIN, NULL, CopyTree (node->right)));
            case TAN:
                return CreateNode (OP, OP_MUL, DiffTree (node->right), CreateNode (OP, OP_DIV, CreateNode (NUM, 1, NULL, NULL), CreateNode (OP, OP_EXP, CreateNode (FUNC, COS, NULL, CopyTree (node->right)), CreateNode (NUM, 2, NULL, NULL))));
            case COT:
                return CreateNode (OP, OP_MUL, CreateNode (OP, OP_MUL, CreateNode (NUM, -1, NULL, NULL), DiffTree (node->right)), CreateNode (OP, OP_DIV, CreateNode (NUM, 1, NULL, NULL), CreateNode (OP, OP_EXP, CreateNode (FUNC, SIN, NULL, CopyTree (node->right)), CreateNode (NUM, 2, NULL, NULL))));
            case LN:
                return CreateNode (OP, OP_MUL, DiffTree (node->right), CreateNode (OP, OP_DIV, CreateNode (NUM, 1, NULL, NULL), CopyTree (node->right)));
        };
    }

    switch ((int)node->value)
    {
        // #define ADD(expr1, expr2) CreateNode(OP, OP_ADD, expr1, expr2)
        case OP_ADD:
            return CreateNode (OP, OP_ADD, DiffTree (node->left), DiffTree (node->right));
        case OP_SUB:
            return CreateNode (OP, OP_SUB, DiffTree (node->left), DiffTree (node->right));
        case OP_MUL:
            return CreateNode (OP, OP_ADD, CreateNode (OP, OP_MUL, DiffTree (node->left), CopyTree (node->right)),
                                           CreateNode (OP, OP_MUL, CopyTree (node->left), DiffTree (node->right)));
        case OP_DIV:
            // D(f(x) / g(x)) = D(f(x))*g(x) - ...
            return CreateNode (OP, OP_DIV, CreateNode (OP, OP_SUB,
                                                      CreateNode (OP, OP_MUL, DiffTree (node->left), CopyTree (node->right)),
                                                      CreateNode (OP, OP_MUL, CopyTree (node->left), DiffTree (node->right))),
                                           CreateNode (OP, OP_EXP, CopyTree (node->right), CreateNode (NUM, 2, NULL, NULL)));
        case OP_EXP:
        {
            return CreateNode (OP, OP_MUL,
                              CopyTree (node->right),
                              CreateNode (OP, OP_MUL,
                                         CreateNode (OP, OP_EXP,
                                                    CopyTree (node->left),
                                                    CreateNode (OP, OP_SUB,
                                                               CopyTree (node->right),
                                                               CreateNode (NUM, 1, NULL, NULL))),
                                         DiffTree (node->left)));
        }
    };
    return 0;
}

TreeNode* Taylor (const TreeNode* node, const int order)
{
    if (order == 0)
    {
        TreeNode* tmp_func = CopyTree ((TreeNode*)node);
        return CreateNode (NUM, EvalExpression (tmp_func, 0), NULL, NULL);
    }

    TreeNode* taylor_tree = CreateNode (OP, OP_ADD,
                                  CreateNode (NUM, CompWithZeroArg (node, 0), NULL, NULL),
                                  CompSum (node, order));
    return taylor_tree;
}

// from 1 to n series member
// GetPartialSum()
TreeNode* CompSum (const TreeNode* node, const int order)
{
    TreeNode* left_sum = NULL;

    for (int i = 0; i < order; i++)
    {
        if (left_sum == NULL)
            left_sum = Component (node, i + 1);
        else
        {
            left_sum = CreateNode (OP, OP_ADD, CopyTree (left_sum), Component (node, i + 1));
        }
    }

    return left_sum;
}

// GetSeriesMember
TreeNode* Component (const TreeNode* node, const int order)
{
    // F'(n) (0)

    return CreateNode (OP, OP_MUL,
                       CreateNode (OP, OP_DIV,
                                   CreateNode (NUM, CompWithZeroArg (node, order), NULL, NULL),
                                   CreateNode (NUM, Factorial (order), NULL, NULL)),
                       CreateNode (OP, OP_EXP,
                                   CreateNode (VAR, 0, NULL, NULL),
                                   CreateNode (NUM, order, NULL, NULL)));
}

float CompWithZeroArg (const TreeNode* node, const int order)
{
    TreeNode* tmp_diff = CopyTree ((TreeNode*)node);
    TreeNode* tmp_tree = NULL;
    for (int i = 0; i < order; i++)
    {
        tmp_tree = tmp_diff; // for free
        tmp_diff = DiffTree (tmp_diff);
        if (tmp_tree != node)
            TreeDel (tmp_tree);
    }
    return EvalExpression (tmp_diff, 0);
}

int Factorial (const int num)
{
    if (num == 0)
        return 1;

    return num * Factorial (num - 1);
}

void EvalDump (TreeNode* node, enum EXPRESSION cmd_expression)
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
    }
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

    // Print unary operation
    if (node->type != FUNC)
    {
        if (node->left != NULL)
        {
            fprintf (file, "%o -> %o", node, node->left);
        }
        else
        {
            return;
        }
    }

    if (node->right != NULL)
    {
        fprintf (file, "\n %o -> %o \n", node, node->right);
    }
    else
    {
        return;
    }

    if (node->type != FUNC)
    {
        DumpTreeNode (node->left, file);
    }
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

//
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
            else if (node->left->type == NUM && node->left->value == 0)
            {
                OptimiseAndDelSubTree (node, node->right);
            }
            else if (node->right->type == NUM && node->right->value == 0)
            {
                OptimiseAndDelSubTree (node, node->left);
            }
        }
        else if (node->value == OP_MUL || node->value == OP_DIV) // сделать проверку на то сто знаменатель не ноль
        {
            if (node->left->type == NUM && node->right->type == NUM)
            {
                OptimiseNumTreeNode (node);
            }
            // #define CHECK_NUM(node, val) (node->type == NUM && node->value == val)

            // else if ( CHECK_NUM(node->left, 0) ||
            //           CHECK_NUM(node->right, 0)
            else if ((node->left->type == NUM && node->left->value == 0) ||
                     (node->right->type == NUM && node->right->value == 0))
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
            if (node->right->type == NUM && node->right->value == 0)
            {
                OptimiseSpecialCases (node, 1);
            }
            else if (node->right->type == NUM && node->right->value == 1)
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
            EvalLatex (node->left, latex_file);
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
            EvalLatex (node->right, latex_file);
            PutBracketsAfter (node, latex_file, ')');
            break;
        }
    };

    if (node->value == OP_EXP || node->value == OP_DIV)
        fprintf (latex_file, "}");
}


void EvalLatex (TreeNode* node, FILE* latex_file)
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
    EvalLatex (node, file);
    if (node->type != OP || node->value != OP_DIV)
        fprintf (file, ")");
}
