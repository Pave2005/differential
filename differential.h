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

struct Tree
{
    TreeNode* root;
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
void SubstitutePoint (TreeNode* node, const float value);
float EvalExpression (TreeNode* node, const float value);
char* TreeCtor (Tree* tree, FILE* file);
void TreeDtor (Tree* tree, FILE* file, TreeNode* node);
TreeNode* CopyTree (TreeNode* node);
float EvalNumExpression(TreeNode* node);
TreeNode* NewNode (enum TYPES node_type, int node_value, TreeNode* left, TreeNode* right);
TreeNode* DiffTree (const TreeNode* node);
void ExpressionTreeDump (TreeNode* node, enum EXPRESSION cmd_expression);
void DumpTreeNode (TreeNode* node, FILE* file);
void TreeDel (TreeNode* node);
void OptimiseExpressionTree (TreeNode* node, enum PASS pass);
void PrintLatex (TreeNode* node, FILE* latex_file);
void OptimiseNumTreeNode (TreeNode* node);
void OptimiseAndDelSubTree (TreeNode* node_parent, TreeNode* sub_node);
void OptimiseSpecialCases (TreeNode* node, float value);
void LatexOp (TreeNode* node, FILE* latex_file);
void PutBracketsBefore (TreeNode* node, FILE* latex_file, char bracket_type);
void PutBracketsAfter (TreeNode* node, FILE* latex_file, char bracket_type);
void PutOp (TreeNode* node, FILE* latex_file);
float EvalBinaryOp (TreeNode* node);
float EvalUnaryOp (TreeNode* node);
TreeNode* DiffUnaryOp (int value, TreeNode* expr);
TreeNode* DiffBinaryOp (int value, TreeNode* expr1, TreeNode* expr2);

int Factorial (const int num);
TreeNode* Taylor (const TreeNode* node, const int order);
TreeNode* GetPartialSum (const TreeNode* node, const int order);
TreeNode* GetSeriesMember (const TreeNode* node, const int order);


#define SIN(expr) NewNode (OP, OP_MUL, DiffTree (expr), NewNode (FUNC, COS, NULL, CopyTree (expr)))
#define COS(expr) NewNode (OP, OP_MUL, NewNode (OP, OP_MUL,                                                 \
                                               NewNode (NUM, -1, NULL, NULL),                               \
                                               DiffTree (expr)),                                            \
                                       NewNode (FUNC, SIN, NULL, CopyTree (expr)))
#define TAN(expr) NewNode (OP, OP_MUL, DiffTree (expr),                                                     \
                                       NewNode (OP, OP_DIV,                                                 \
                                               NewNode (NUM, 1, NULL, NULL),                                \
                                               NewNode (OP, OP_EXP,                                         \
                                                       NewNode (FUNC, COS, NULL, CopyTree (expr)),          \
                                                       NewNode (NUM, 2, NULL, NULL))))
#define COT(expr) NewNode (OP, OP_MUL, NewNode (OP, OP_MUL,                                                 \
                                               NewNode (NUM, -1, NULL, NULL),                               \
                                               DiffTree (expr)),                                            \
                                       NewNode (OP, OP_DIV,                                                 \
                                               NewNode (NUM, 1, NULL, NULL),                                \
                                               NewNode (OP, OP_EXP,                                         \
                                                       NewNode (FUNC, SIN, NULL, CopyTree (expr)),          \
                                                       NewNode (NUM, 2, NULL, NULL))))
#define LN(expr) NewNode (OP, OP_MUL, DiffTree (expr), NewNode (OP, OP_DIV,                                 \
                                                                      NewNode (NUM, 1, NULL, NULL),         \
                                                                      CopyTree (expr)));
#define ADD(expr1, expr2) NewNode (OP, OP_ADD, DiffTree (expr1), DiffTree (expr2))
#define SUB(expr1, expr2) NewNode (OP, OP_SUB, DiffTree (expr1), DiffTree (expr2))
#define MUL(expr1, expr2) NewNode (OP, OP_ADD, NewNode (OP, OP_MUL, DiffTree (expr1), CopyTree (expr2)),    \
                                  NewNode (OP, OP_MUL, CopyTree (expr1), DiffTree (expr2)))
#define DIV(expr1, expr2) NewNode (OP, OP_DIV, NewNode (OP, OP_SUB,                                         \
                                              NewNode (OP, OP_MUL, DiffTree (expr1), CopyTree (expr2)),     \
                                              NewNode (OP, OP_MUL, CopyTree (expr1), DiffTree (expr2))),    \
                                   NewNode (OP, OP_EXP, CopyTree (expr2), NewNode (NUM, 2, NULL, NULL)))
#define EXP(expr1, expr2) NewNode (OP, OP_MUL,                                                              \
                                  CopyTree (expr2),                                                         \
                                  NewNode (OP, OP_MUL,                                                      \
                                          NewNode (OP, OP_EXP,                                              \
                                                  CopyTree (expr1),                                         \
                                                  NewNode (OP, OP_SUB,                                      \
                                                          CopyTree (expr2),                                 \
                                                          NewNode (NUM, 1, NULL, NULL))),                   \
                                          DiffTree (expr1)))
#define CHECK_NUM(node, val) (node->type == NUM && node->value == val)

#endif
