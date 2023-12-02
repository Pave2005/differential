#ifndef DIF
#define DIF

const int EMPTY = -888;

struct TreeNode
{
    TreeNode* left;
    TreeNode* right;
    int value;
    int type;
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

enum TYPES
{
    NUM = 1,
    OP = 2,
    VAR = 3,
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

char* EvalCtor (Tree* tree);
void EvalDtor (Tree* tree, TreeNode* node);
TreeNode* c (const TreeNode* node);
long FileSize (FILE* file);
char* ReadText (long file_size, FILE* file);
void BaseOfData (TreeNode** node, char* data_buf, int* tmp_count);
void PrintfNode (const TreeNode* node, const Tree* tree);
int Eval (TreeNode* node);
TreeNode* CreateNode (enum TYPES TYPE, int VALUE, TreeNode* left, TreeNode* right);
TreeNode* d (const TreeNode* node);
void EvalDump (Tree* tree, TreeNode* node, enum EXPRESSION EXPRESSION);
void TreeBody (TreeNode* node, FILE* file);
void TreeDel (TreeNode* node);
void Simpler (TreeNode* node, enum PASS PASS);
void EvalLatex (TreeNode* node, FILE* latex_file);

#endif
