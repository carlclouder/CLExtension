
#include "CLMatrix.h"
#include "CLMatrixEx.h"

unsigned long long matrixCreateTimes = 0;//ͳ�Ƽ���
bool matrixUseSSE = true;                //ʹ��sseָ��
size_t matrixUseSSEMinRank = 8;         //ʹ��sseָ�����;�����
