#ifndef DIF
#define DIF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

const int EMPTY = -888;

struct TreeNode
{
    TreeNode* left;
    TreeNode* right;
    float value;
    int type;
};

struct Position
{
    const char* str;
    int position;
};

struct Tree
{
    TreeNode* root;

    FILE* file;
    int counter;
};

enum OPERATORS
{
    OP_DIV = 1,
    OP_SUB = 2,
    OP_MUL = 3,
    OP_ADD = 4,
    OP_EXP = 5,
};

enum FUNCTIONS
{
    SIN = 1,
    COS = 2,
    LN = 3,
    TAN = 4,
    COT = 5,
};

enum TYPES
{
    NUM = 1,
    OP  = 2,
    VAR = 3,
    FUNC = 4,
};

enum EXPRESSION
{
    ORIGINAL = 1,
    CALCULATED = 2,
};

enum PASS
{
    FIRST_PASS = 1,
    SECOND_PASS = 2,
};

TreeNode* GetExp (Position* data);
TreeNode* GetG (const char* str, Position* data);
TreeNode* GetN (Position* data);
TreeNode* GetT (Position* data);
TreeNode* GetE (Position* data);
TreeNode* GetP (Position* data);
TreeNode* GetId (Position* data);

void Substitute (TreeNode* node, const float value);
float EvalExpression (TreeNode* node, const float value);
char* EvalCtor (Tree* tree);
void EvalDtor (Tree* tree, TreeNode* node);
TreeNode* c (const TreeNode* node);
long FileSize (FILE* file);
char* ReadText (long file_size, FILE* file);
float EvalNumExpression(TreeNode* node);
TreeNode* CreateNode (enum TYPES TYPE, int VALUE, TreeNode* left, TreeNode* right);
TreeNode* d (const TreeNode* node);
void EvalDump (TreeNode* node, enum EXPRESSION EXPRESSION);
void TreeBody (TreeNode* node, FILE* file);
void TreeDel (TreeNode* node);
// pass
void Optimizer (TreeNode* node, enum PASS PASS);
void EvalLatex (TreeNode* node, FILE* latex_file);

int Factorial (const int num);
TreeNode* Taylor (const TreeNode* node, const int order);
TreeNode* CompSum (const TreeNode* node, const int order);
TreeNode* Component (const TreeNode* node, const int order);
float CompWithZeroArg (const TreeNode* node, const int order);

#endif
