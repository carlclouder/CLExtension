
#include "CLJsonTask.h"


JTASK_TRDINF::JTASK_TRDINF()
{
	reset();
}

JTASK_TRDINF::~JTASK_TRDINF()
{
	reset();
}

void JTASK_TRDINF::reset()
{
	index = 0;
	nId = ownerTrdId = 0;
	handle = 0;
	trdParm_json.clear();
	strTmp.empty();
	fatherRootKey.empty();
	fatherRootKeyIndex = 0;
	quitMsgId = WM_NULL;
	hWnd = 0;
	quitEvent = 0;
	iRet = -1;
}
