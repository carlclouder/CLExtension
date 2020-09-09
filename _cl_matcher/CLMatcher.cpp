#include "CLMatcher.h"

CLMatcherR CLMatcher::setOrg(LPCTSTR text){
	assert( text != NULL );
	pOrgHead = text;
	return *this;
}
CLMatcherR CLMatcher::setKeyLst(LPCTSTR keyString){	
	m_splitLst = keyString;
	m_kwLst.reset();
	m_splitLst.split(_T(','));
	for (size_t i = 0,j=0;i<m_splitLst.getVT().size() && j < 256;i++)
	{
		m_splitLst = m_splitLst.getVT().at(i);
		m_splitLst.trim();
		if(m_splitLst.size() == 0)continue;
		_tcsncpy_s(m_kwLst.unit[j].kwd,256-1,m_splitLst.string(),m_splitLst.size());
		m_kwLst.unit[j].nCharCounts = _tcslen(m_kwLst.unit[j].kwd);
		m_kwLst.nUnitCounts = ++j;
	}
	return *this;
}
CLMatcherR CLMatcher::match(){
	size_t cp = 0;	
	while(m_currentChar = *(pOrgHead+cp)){		
		KwUnit* cpu = &(m_kwLst.unit[0]);
		for (byte i =0 ;i<m_kwLst.nUnitCounts;i++,cpu++)
		{            
			if(cpu->kwd[cpu->nCp] == m_currentChar){
				cpu->nCp ++;
				if(cpu->nCp == cpu->nCharCounts){
					//record
					if(cpu->lpFirst == NULL)
						cpu->lpFirst = pOrgHead + cp + 1 - cpu->nCp;
					cpu->nMatch++;
					cpu->nCp = 0;
				}
			}
			else{cpu->nCp = 0;}
		}
		cp ++;
	}
	return *this;
}

CLMatcher::CLMatcher()
{
	reset();
}

CLMatcherR CLMatcher::reset()
{
	m_kwLst.reset();  
	pOrgHead = NULL;
	m_currentChar = 0;
	m_splitLst.empty();
	m_splitLst.clearInnerStringVector();	
	return *this;
}

const KeyWordsLst& CLMatcher::getResult() const
{
	return m_kwLst;
}

UINT CLMatcher::getKeyWordCounts() const
{
	return m_kwLst.nUnitCounts;	
}

float CLMatcher::matchRat() const
{
	float nm = 0;
	for (size_t i=0;i<MAX_LEN;i++)
	{
		if(m_kwLst.unit[i].nMatch > 0)
			nm+=1;
		else if (m_kwLst.unit[i].kwd[0] == 0)
			break;
	}
	return getKeyWordCounts() == 0 ? -1 : nm/getKeyWordCounts();
}
