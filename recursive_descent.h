#ifndef DESENT
#define DESENT

#include "differential.h"
#include "recursive_descent.h"

TreeNode* GetExp (Position* data);
TreeNode* GetG (const char* str, Position* data);
TreeNode* GetN (Position* data);
TreeNode* GetT (Position* data);
TreeNode* GetE (Position* data);
TreeNode* GetP (Position* data);
TreeNode* GetId (Position* data);

#endif
