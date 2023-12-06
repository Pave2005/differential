#include "differential.h"

TreeNode* GetP (Position* data)
{
    if (data->str[data->position] == '(')
    {
        TreeNode* val = 0;
        data->position++;
        val = GetE (data);
        data->position++;
        return val;
    }
    if (isalpha (data->str[data->position]) != 0)
    {
        return GetId (data);
    }
    else
    {
        return GetN (data);
    }
}

TreeNode* GetT (Position* data)
{
    TreeNode* val = GetExp (data);
    while (data->str[data->position] == '*' || data->str[data->position] == '/') // if
    {
        int op = data->str[data->position];
        data->position++;
        TreeNode* val2 = GetExp (data);
        switch (op)
        {
            case '*':
            {
                val = CreateNode (OP, OP_MUL, val, val2);
                break;
            }
            case '/':
            {
                val = CreateNode (OP, OP_DIV, val, val2);
                break;
            }
        }
    }
    return val;
}

TreeNode* GetE (Position* data)
{
    TreeNode* val = GetT (data);
    while (data->str[data->position] == '+' || data->str[data->position] == '-')
    {
        int op = data->str[data->position];
        data->position++;
        TreeNode* val2 = GetT (data);
        switch (op)
        {
            case '+':
            {
                val = CreateNode (OP, OP_ADD, val, val2);
                break;
            }
            case '-':
            {
                val = CreateNode (OP, OP_SUB, val, val2);
                break;
            }
        }
    }
    return val;
}

TreeNode* GetG (const char* str, Position* data)
{
    data->str = str;
    data->position = 0;
    TreeNode* val = GetE (data);
    return val;
}

TreeNode* GetN (Position* data)
{
    int val = 0;
    int old_p = data->position;
    while ('0' <= data->str[data->position] && data->str[data->position] <= '9')
    {
        val = val * 10 + data->str[data->position] - '0';
        data->position++;
    }
    return CreateNode (NUM, val, NULL, NULL);
}

TreeNode* GetId (Position* data)
{
    char arg[20] = "";
    int counter = 0;
    while (isalpha (data->str[data->position]) != 0)
    {
        sprintf (arg + counter, "%c", data->str[data->position]);
        data->position++;
        counter++;
    }
    if (strcmp ("sin", arg) == 0 && data->str[data->position] == '(')
    {
        TreeNode* val = NULL;
        data->position++;
        val = GetE (data);
        data->position++;
        return CreateNode (FUNC, SIN, NULL, val);
    }
    else if (strcmp ("cos", arg) == 0 && data->str[data->position] == '(')
    {
        TreeNode* val = NULL;
        data->position++;
        val = GetE (data);
        data->position++;
        return CreateNode (FUNC, COS, NULL, val);
    }
    else if (strcmp ("tg", arg) == 0 && data->str[data->position] == '(')
    {
        TreeNode* val = NULL;
        data->position++;
        val = GetE (data);
        data->position++;
        return CreateNode (FUNC, TAN, NULL, val);
    }
    else if (strcmp ("ctg", arg) == 0 && data->str[data->position] == '(')
    {
        TreeNode* val = NULL;
        data->position++;
        val = GetE (data);
        data->position++;
        return CreateNode (FUNC, COT, NULL, val);
    }
    else if (strcmp ("ln", arg) == 0 && data->str[data->position] == '(')
    {
        TreeNode* val = NULL;
        data->position++;
        val = GetE (data);
        data->position++;
        return CreateNode (FUNC, LN, NULL, val);
    }
    return CreateNode (VAR, 0, NULL, NULL);
}

TreeNode* GetExp (Position* data)
{
    TreeNode* val = GetP (data);
    if (data->str[data->position] == '^')
    {
        data->position++;
        TreeNode* val2 = GetP (data);
        val = CreateNode (OP, OP_EXP, val, val2);
    }
    return val;
}




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
TreeNode* c (TreeNode* node)
{
    TreeNode* cpy_node = (TreeNode*)calloc (1, sizeof (TreeNode));

    cpy_node->type = node->type;
    cpy_node->value = node->value;

    if (node->left == NULL)
        cpy_node->left = NULL;
    else
        cpy_node->left = c (node->left);

    if (node->right == NULL)
        cpy_node->right = NULL;
    else
        cpy_node->right = c (node->right);

    return cpy_node;
}

// TYPE, VALUE
TreeNode* CreateNode (enum TYPES TYPE, int VALUE, TreeNode* left, TreeNode* right)
{
    TreeNode* node = (TreeNode*)calloc (1, sizeof (TreeNode));
    node->type = TYPE;
    node->value = VALUE;
    node->left = left;
    node->right = right;
    return node;
}

TreeNode* d (const TreeNode* node)
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
                return CreateNode (OP, OP_MUL, d (node->right), CreateNode (FUNC, COS, NULL, c (node->right)));
            case COS:
                return CreateNode (OP, OP_MUL, CreateNode (OP, OP_MUL, CreateNode (NUM, -1, NULL, NULL), d (node->right)), CreateNode (FUNC, SIN, NULL, c (node->right)));
            case TAN:
                return CreateNode (OP, OP_MUL, d (node->right), CreateNode (OP, OP_DIV, CreateNode (NUM, 1, NULL, NULL), CreateNode (OP, OP_EXP, CreateNode (FUNC, COS, NULL, c (node->right)), CreateNode (NUM, 2, NULL, NULL))));
            case COT:
                return CreateNode (OP, OP_MUL, CreateNode (OP, OP_MUL, CreateNode (NUM, -1, NULL, NULL), d (node->right)), CreateNode (OP, OP_DIV, CreateNode (NUM, 1, NULL, NULL), CreateNode (OP, OP_EXP, CreateNode (FUNC, SIN, NULL, c (node->right)), CreateNode (NUM, 2, NULL, NULL))));
            case LN:
                return CreateNode (OP, OP_MUL, d (node->right), CreateNode (OP, OP_DIV, CreateNode (NUM, 1, NULL, NULL), c (node->right)));
        };
    }

    switch ((int)node->value)
    {
        // #define ADD(expr1, expr2) CreateNode(OP, OP_ADD, expr1, expr2)
        case OP_ADD:
            return CreateNode (OP, OP_ADD, d (node->left), d (node->right));
        case OP_SUB:
            return CreateNode (OP, OP_SUB, d (node->left), d (node->right));
        case OP_MUL:
            return CreateNode (OP, OP_ADD, CreateNode (OP, OP_MUL, d (node->left), c (node->right)),
                                           CreateNode (OP, OP_MUL, c (node->left), d (node->right)));
        case OP_DIV:
            // D(f(x) / g(x)) = D(f(x))*g(x) - ...
            return CreateNode (OP, OP_DIV, CreateNode (OP, OP_SUB,
                                                      CreateNode (OP, OP_MUL, d (node->left), c (node->right)),
                                                      CreateNode (OP, OP_MUL, c (node->left), d (node->right))),
                                           CreateNode (OP, OP_EXP, c (node->right), CreateNode (NUM, 2, NULL, NULL)));
        case OP_EXP:
        {
            return CreateNode (OP, OP_MUL,
                              c (node->right),
                              CreateNode (OP, OP_MUL,
                                         CreateNode (OP, OP_EXP,
                                                    c (node->left),
                                                    CreateNode (OP, OP_SUB,
                                                               c (node->right),
                                                               CreateNode (NUM, 1, NULL, NULL))),
                                         d (node->left)));
        }
    };
    return 0;
}

TreeNode* Taylor (const TreeNode* node, const int order)
{
    if (order == 0)
    {
        TreeNode* tmp_func = c ((TreeNode*)node);
        return CreateNode (NUM, EvalExpression (tmp_func, 0), NULL, NULL);
    }

    TreeNode* taylor_tree = CreateNode (OP, OP_ADD,
                                  CreateNode (NUM, CompWithZeroArg (node, 0), NULL, NULL),
                                  CompSum (node, order));
    return taylor_tree;
}

TreeNode* CompSum (const TreeNode* node, const int order)
{
    TreeNode* left_sum = NULL;

    for (int i = 0; i < order; i++)
    {
        if (left_sum == NULL)
            left_sum = Component (node, i + 1);
        else
        {
            left_sum = CreateNode (OP, OP_ADD, c (left_sum), Component (node, i + 1));
        }
    }

    return left_sum;
}

TreeNode* Component (const TreeNode* node, const int order)
{
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
    TreeNode* tmp_diff = c ((TreeNode*)node);
    TreeNode* tmp_tree = NULL;
    for (int i = 0; i < order; i++)
    {
        tmp_tree = tmp_diff; // for free
        tmp_diff = d (tmp_diff);
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

void EvalDump (TreeNode* node, enum EXPRESSION EXPRESSION)
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

// DumpTreeNode
void TreeBody (TreeNode* node, FILE* file)
{
    if (node->type == NUM)
    {
        fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = cornflowerblue "
                       " label = \" %f \"];\n",node, (float)node->value);
    }
    else if (node->type == FUNC)
    {
        if (node->value == SIN)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                           " label = \" sin \"];\n",node);
        }
        else if (node->value == COS)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                           " label = \" cos \"];\n",node);
        }
        else if (node->value == TAN)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                           " label = \" tg \"];\n",node);
        }
        else if (node->value == COT)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                           " label = \" ctg \"];\n",node);
        }
        else if (node->value == LN)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                           " label = \" ln \"];\n",node);
        }
    }
    else if (node->type == OP)
    {
        // getOpChar(OP_MUL) == '*'

        // fprintf( "... label = \" %c \"", getOpChar(node->value));
        if (node->value == OP_MUL)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                           " label = \" * \"];\n",node);
        }
        else if (node->value == OP_DIV)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                           " label = \" / \"];\n",node);
        }
        else if (node->value == OP_SUB)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                           " label = \" - \"];\n",node);
        }
        else if (node->value == OP_ADD)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                           " label = \" + \"];\n",node);
        }
        else if (node->value == OP_EXP)
        {
            fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = aqua "
                           " label = \" ^ \"];\n",node);
        }
    }
    else if (node->type == VAR)
    {
        fprintf (file, " %o [shape = doubleoctagon, style = filled, fillcolor = lightblue2 "
                       " label = \" x \"];\n",node);
    }

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
        TreeBody (node->left, file);
    }
    TreeBody (node->right, file);
}

void Optimizer (TreeNode* node, enum PASS PASS)
{
    if (node->type == OP)
    {
        if (node->value == OP_ADD || node->value == OP_SUB)
        {
            if (node->left->type  == NUM &&
                node->right->type == NUM)
            {
                node->value = EvalNumExpression (node);
                node->type = NUM;
                TreeDel (node->left);
                TreeDel (node->right);
                node->left = NULL;
                node->right = NULL;
            }
            else if (node->left->type  == NUM &&
                     node->left->value == 0)
            {
                TreeDel (node->left);
                node->type = node->right->type;
                node->value = node->right->value;
                node->left = node->right->left;

                TreeNode* tmp_right = node->right->right;
                free (node->right);
                node->right = tmp_right;

            }
            else if (node->right->type == NUM && node->right->value == 0)
            {
                TreeDel (node->right);
                node->type = node->left->type;
                node->value = node->left->value;
                node->right = node->left->right;

                TreeNode* tmp_left = node->left->left;
                free (node->left);
                node->left = tmp_left;

            }
        }
        else if (node->value == OP_MUL || node->value == OP_DIV) // сделать проверку на то сто знаменатель не ноль
        {
            if (node->left->type == NUM && node->right->type == NUM)
            {
                node->value = EvalNumExpression (node);
                node->type = NUM;
                TreeDel (node->left);
                TreeDel (node->right);
                node->left = NULL;
                node->right = NULL;
            }
            else if ((node->left->type == NUM && node->left->value == 0) ||
                     (node->right->type == NUM && node->right->value == 0))
            {
                TreeDel (node->left);
                TreeDel (node->right);
                node->type = NUM;
                node->value = 0;
                node->left = NULL;
                node->right = NULL;
            }
            // function
            else if (node->left->type != NUM && node->right->type == NUM)
            {
                if (node->right->value == 1)
                {
                    TreeDel (node->right);
                    node->right = NULL;
                    node->type = node->left->type;
                    node->value = node->left->value;
                    node->right = node->left->right;

                    TreeNode* tmp_left = node->left->left;
                    free (node->left);
                    node->left = tmp_left;
                }
            }
            else if (node->left->type == NUM && node->right->type != NUM && node->value == OP_MUL)
            {
                if (node->left->value == 1)
                {
                    TreeDel (node->left);
                    node->left = NULL;
                    node->type = node->right->type;
                    node->value = node->right->value;
                    node->left = node->right->left;

                    TreeNode* tmp_right = node->right->right;
                    free (node->right);
                    node->right = tmp_right;
                }
            }
        }
        else if (node->value == OP_EXP)
        {
            if (node->right->type == NUM && node->right->value == 0)
            {
                TreeDel (node->right);
                TreeDel (node->left);
                node->type = NUM;
                node->value = 1;
                node->left = NULL;
                node->right = NULL;
            }
            else if (node->right->type == NUM && node->right->value == 1)
            {
                TreeDel (node->right);
                node->type = node->left->type;
                node->value = node->left->value;
                node->right = node->left->right;

                TreeNode* tmp_left = node->left->left;
                free (node->left);
                node->left = tmp_left;
            }
            else if (node->left->type == NUM && node->right->type == NUM)
            {
                node->value = EvalNumExpression (node);
                node->type = NUM;
                TreeDel (node->left);
                TreeDel (node->right);
                node->left = NULL;
                node->right = NULL;
            }
            // comment
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
    if (PASS == FIRST_PASS)
    {
        if (node->left != NULL)
            Optimizer (node->left, FIRST_PASS);
        if (node->right != NULL)
            Optimizer (node->right, FIRST_PASS);
        Optimizer (node, SECOND_PASS);
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

void EvalLatex (TreeNode* node, FILE* latex_file)
{
    if (node->type == OP)
    {
        if (node->value == OP_ADD)
        {
            if (node->left->type == NUM)
            {
                fprintf (latex_file, "%f", (float)node->left->value);
            }
            else if (node->left->type == VAR)
            {
                fprintf (latex_file, "x");
            }
            else
            {
                if (node->left->value != OP_MUL && node->left->value != OP_DIV)
                    fprintf (latex_file, "(");
                EvalLatex (node->left, latex_file);
                if (node->left->value != OP_MUL && node->left->value != OP_DIV)
                    fprintf (latex_file, ")");
            }
            fprintf (latex_file, "+");
            if (node->right->type == NUM)
            {
                fprintf (latex_file, "%f", (float)node->right->value);
            }
            else if (node->right->type == VAR)
            {
                fprintf (latex_file, "x");
            }
            else
            {
                if (node->right->value != OP_MUL && node->right->value != OP_DIV)
                    fprintf (latex_file, "(");
                EvalLatex (node->right, latex_file);
                if (node->right->value != OP_MUL && node->right->value != OP_DIV)
                    fprintf (latex_file, ")");
            }
        }
        else if (node->value == OP_SUB)
        {
            if (node->left->type == NUM)
            {
                fprintf (latex_file, "%f", (float)node->left->value);
            }
            else if (node->left->type == VAR)
            {
                fprintf (latex_file, "x");
            }
            else
            {
                if (node->left->value != OP_MUL && node->left->value != OP_DIV)
                    fprintf (latex_file, "(");
                EvalLatex (node->left, latex_file);
                if (node->left->value != OP_MUL && node->left->value != OP_DIV)
                    fprintf (latex_file, ")");
            }
            fprintf (latex_file, "-");
            if (node->right->type == NUM)
            {
                fprintf (latex_file, "%f", (float)node->right->value);
            }
            else if (node->right->type == VAR)
            {
                fprintf (latex_file, "x");
            }
            else
            {
                if (node->right->value != OP_MUL && node->right->value != OP_DIV)
                    fprintf (latex_file, "(");
                EvalLatex (node->right, latex_file);
                if (node->right->value != OP_MUL && node->right->value != OP_DIV)
                    fprintf (latex_file, ")");
            }
        }
        else if (node->value == OP_MUL)
        {
            if (node->left->type == NUM)
            {
                if (node->left->value == -1)
                {
                    fprintf (latex_file, "-");
                }
                else
                {
                    fprintf (latex_file, "%f", (float)node->left->value);
                }
            }
            else if (node->left->type == VAR)
            {
                fprintf (latex_file, "x");
            }
            else
            {
                if (node->left->value != OP_MUL && node->left->value != OP_EXP && (node->left->value != OP_DIV || node->right->value != OP_DIV))
                    fprintf (latex_file, "(");
                EvalLatex (node->left, latex_file);
                if (node->left->value != OP_MUL && node->left->value != OP_EXP && (node->left->value != OP_DIV || node->right->value != OP_DIV))
                    fprintf (latex_file, ")");
            }
            if ((node->left->type != NUM || (node->right->type != VAR &&
               ((node->right->type != OP || node->right->value != OP_EXP) || node->right->left->type != VAR))) && (node->left->type != NUM || node->left->value != -1))
                fprintf (latex_file, " \\cdot ");
            if (node->right->type == NUM)
            {
                fprintf (latex_file, "%f", (float)node->right->value);
            }
            else if (node->right->type == VAR)
            {
                fprintf (latex_file, "x");
            }
            else
            {
                if ((node->right->value != OP_MUL && node->right->value != OP_EXP) && (node->left->type != NUM || (node->right->type != OP || node->right->value != OP_DIV)) && (node->left->value != OP_DIV || node->right->value != OP_DIV))
                    fprintf (latex_file, "(");
                EvalLatex (node->right, latex_file);
                if ((node->right->value != OP_MUL && node->right->value != OP_EXP) && (node->left->type != NUM || (node->right->type != OP || node->right->value != OP_DIV)) && (node->left->value != OP_DIV || node->right->value != OP_DIV))
                    fprintf (latex_file, ")");
            }
        }
        else if (node->value == OP_DIV)
        {
            fprintf (latex_file, " \\frac{");
            if (node->left->type == NUM)
            {
                fprintf (latex_file, "%f", (float)node->left->value);
            }
            else if (node->left->type == VAR)
            {
                fprintf (latex_file, "x");
            }
            else
            {
                EvalLatex (node->left, latex_file);
            }
            fprintf (latex_file, "}{");
            if (node->right->type == NUM)
            {
                fprintf (latex_file, "%f", (float)node->right->value);
            }
            else if (node->right->type == VAR)
            {
                fprintf (latex_file, "x");
            }
            else
            {
                EvalLatex (node->right, latex_file);
            }
            fprintf (latex_file, "}");
        }
        else if (node->value == OP_EXP)
        {
            if (node->left->type == NUM)
            {
                fprintf (latex_file, "%f", (float)node->left->value);
            }
            else if (node->left->type == VAR)
            {
                fprintf (latex_file, "x");
            }
            else
            {
                fprintf (latex_file, "(");
                EvalLatex (node->left, latex_file);
                fprintf (latex_file, ")");
            }
            fprintf (latex_file, "^");
            fprintf (latex_file, "{");
            if (node->right->type == NUM)
            {
                fprintf (latex_file, "%f", (float)node->right->value);
            }
            else if (node->left->type == VAR)
            {
                fprintf (latex_file, "x");
            }
            else
            {
                fprintf (latex_file, "(");
                EvalLatex (node->right, latex_file);
                fprintf (latex_file, ")");
            }
            fprintf (latex_file, "}");
        }
    }
    else if (node->type == FUNC)
    {
        switch ((int)node->value) // ячейка int не преобразуется в float и в итоге дерево не может хранить float
        {
            case SIN:
            {
                fprintf (latex_file, "sin ");
                if (node->right->type != OP || node->right->value != OP_DIV)
                    fprintf (latex_file, "(");
                EvalLatex (node->right, latex_file);
                if (node->right->type != OP || node->right->value != OP_DIV)
                    fprintf (latex_file, ")");
                break;
            }
            case COS:
            {
                fprintf (latex_file, "cos ");
                if (node->right->type != OP || node->right->value != OP_DIV)
                    fprintf (latex_file, "(");
                EvalLatex (node->right, latex_file);
                if (node->right->type != OP || node->right->value != OP_DIV)
                    fprintf (latex_file, ")");
                break;
            }
            case TAN:
            {
                fprintf (latex_file, "tg ");
                if (node->right->type != OP || node->right->value != OP_DIV)
                    fprintf (latex_file, "(");
                EvalLatex (node->right, latex_file);
                if (node->right->type != OP || node->right->value != OP_DIV)
                    fprintf (latex_file, ")");
                break;
            }
            case COT:
            {
                fprintf (latex_file, "ctg ");
                if (node->right->type != OP || node->right->value != OP_DIV)
                    fprintf (latex_file, "(");
                EvalLatex (node->right, latex_file);
                if (node->right->type != OP || node->right->value != OP_DIV)
                    fprintf (latex_file, ")");
                break;
            }
            case LN:
            {
                fprintf (latex_file, "ln ");
                if (node->right->type != OP || node->right->value != OP_DIV)
                    fprintf (latex_file, "(");
                EvalLatex (node->right, latex_file);
                if (node->right->type != OP || node->right->value != OP_DIV)
                    fprintf (latex_file, ")");
                break;
            }
        };
    }
    else if (node->type == NUM)
    {
        fprintf (latex_file, "%f", (float)node->value);
    }
    else if (node->type == VAR)
    {
        fprintf (latex_file, "x");
    }
}
