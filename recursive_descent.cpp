#include "recursive_descent.h"
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
                val = NewNode (OP, OP_MUL, val, val2);
                break;
            }
            case '/':
            {
                val = NewNode (OP, OP_DIV, val, val2);
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
                val = NewNode (OP, OP_ADD, val, val2);
                break;
            }
            case '-':
            {
                val = NewNode (OP, OP_SUB, val, val2);
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
    return NewNode (NUM, val, NULL, NULL);
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
        return NewNode (FUNC, SIN, NULL, val);
    }
    else if (strcmp ("cos", arg) == 0 && data->str[data->position] == '(')
    {
        TreeNode* val = NULL;
        data->position++;
        val = GetE (data);
        data->position++;
        return NewNode (FUNC, COS, NULL, val);
    }
    else if (strcmp ("tg", arg) == 0 && data->str[data->position] == '(')
    {
        TreeNode* val = NULL;
        data->position++;
        val = GetE (data);
        data->position++;
        return NewNode (FUNC, TAN, NULL, val);
    }
    else if (strcmp ("ctg", arg) == 0 && data->str[data->position] == '(')
    {
        TreeNode* val = NULL;
        data->position++;
        val = GetE (data);
        data->position++;
        return NewNode (FUNC, COT, NULL, val);
    }
    else if (strcmp ("ln", arg) == 0 && data->str[data->position] == '(')
    {
        TreeNode* val = NULL;
        data->position++;
        val = GetE (data);
        data->position++;
        return NewNode (FUNC, LN, NULL, val);
    }
    return NewNode (VAR, 0, NULL, NULL);
}

TreeNode* GetExp (Position* data)
{
    TreeNode* val = GetP (data);
    if (data->str[data->position] == '^')
    {
        data->position++;
        TreeNode* val2 = GetP (data);
        val = NewNode (OP, OP_EXP, val, val2);
    }
    return val;
}
