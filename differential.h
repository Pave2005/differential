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

    // ??
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
    SIN = 6,
    COS = 7,
    LN = 8,
    TAN = 9,
    COT = 10,
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

void TextFuncName (TreeNode* node, FILE* file, char* func_name);
char* GetFuncName (int func);
char GetOpChar (int command);
void Substitute (TreeNode* node, const float value);
float EvalExpression (TreeNode* node, const float value);
char* EvalCtor (Tree* tree);
void EvalDtor (Tree* tree, TreeNode* node);
TreeNode* CopyTree (TreeNode* node);
float EvalNumExpression(TreeNode* node);
TreeNode* CreateNode (enum TYPES node_type, int node_value, TreeNode* left, TreeNode* right);
TreeNode* DiffTree (const TreeNode* node);
void EvalDump (TreeNode* node, enum EXPRESSION cmd_expression);
void DumpTreeNode (TreeNode* node, FILE* file);
void TreeDel (TreeNode* node);
void OptimiseExpressionTree (TreeNode* node, enum PASS pass);
void EvalLatex (TreeNode* node, FILE* latex_file);
void OptimiseNumTreeNode (TreeNode* node);
void OptimiseAndDelSubTree (TreeNode* node_parent, TreeNode* sub_node);
void OptimiseSpecialCases (TreeNode* node, float value);
void LatexOp (TreeNode* node, FILE* latex_file);
void PutBracketsBefore (TreeNode* node, FILE* latex_file, char bracket_type);
void PutBracketsAfter (TreeNode* node, FILE* latex_file, char bracket_type);
void PutOp (TreeNode* node, FILE* latex_file);

int Factorial (const int num);
TreeNode* Taylor (const TreeNode* node, const int order);
TreeNode* CompSum (const TreeNode* node, const int order);
TreeNode* Component (const TreeNode* node, const int order);
float CompWithZeroArg (const TreeNode* node, const int order);

#endif
