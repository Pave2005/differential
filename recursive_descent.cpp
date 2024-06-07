#include "recursive_descent.h"
#include "differential.h"

void SkipSpaces (Position* data)
{
    // isspace
    while (data->str[data->position] == ' ' || data->str[data->position] == '\n')
    {
        data->position++;
    }
}

TreeNode* GetP (Position* data)
{
    SkipSpaces (data);
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
    SkipSpaces (data);
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
    SkipSpaces (data);
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
    SkipSpaces (data);
    // atoi
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
    SkipSpaces (data);
    while (!isalpha (data->str[data->position]))
    {
        sprintf (arg + counter, "%c", data->str[data->position]);
        data->position++;
        counter++;
    }
    SkipSpaces (data);
    int func_code = GetFuncCode (arg);
    if (func_code != 0)
    {
        TreeNode* val = NULL;
        data->position++;
        val = GetE (data);
        data->position++;
        return NewNode (FUNC, func_code, NULL, val);
    }
    return NewNode (VAR, 0, NULL, NULL);
}

TreeNode* GetExp (Position* data)
{
    TreeNode* val = GetP (data);
    SkipSpaces (data);
    if (data->str[data->position] == '^')
    {
        data->position++;
        TreeNode* val2 = GetP (data);
        val = NewNode (OP, OP_EXP, val, val2);
    }
    return val;
}
