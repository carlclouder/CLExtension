
#include "CLMatrix.h"

unsigned long long matrixCreateTimes = 0;//统计计数
bool matrixUseSSE = true;                //使用sse指令
size_t matrixUseSSEMinRank = 10;         //使用sse指令的最低矩阵宽度
