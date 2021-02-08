#include "CLHttpSpider.h"
#include "../_cl_string/CLString.h"


#define RANG_MAX 2000
#define ATTR_MAX 2000
#define LAB_MAX 100
LPCTSTR CLHttpSpider::_labelRecrsion(LPCTSTR ptcTag,LPCTSTR ptcRem,std::vector<CLString>& resuleSet,int nlayer,LPCTSTR lpHeader,size_t nAllSize)
{
	m_ceng = nlayer;
	TCHAR lab[LAB_MAX],rang[RANG_MAX],attr[ATTR_MAX];
	int i=0;lab[0]=0;
	LPCTSTR ptb = ptcRem;	
	for(;  *ptb != 0 && *ptb != _T('>') && i<LAB_MAX-1;lab[i]=*ptb++,lab[i+1]=0,i++);//extract lab inf
	if(*ptb != 0)ptb++;

// 	if(m_ceng == 5){
// 		Sleep(0);
// 	}

	std::vector<UINT> indlst;
	if(*(ptb) == _T('{')){  //extract {} inf
		for(i=0,rang[0]=0; *++ptb != _T('}') && i<RANG_MAX-1;rang[i]=*ptb,rang[i+1]=0,i++);
		ptb++;
		while(*ptb != _T('[') && *ptb != _T('<') && *ptb != 0)ptb++;
		m_tmp = rang;m_tmp.trim();
		if(m_tmp.strlen() == 0)indlst.push_back(1); //set 1 when strlen = 0
		else {
			long long pdot = m_tmp.find(_T(','));
			long long pSub = m_tmp.find(_T('-'));
			if(pdot > 0 && pSub > 0){ // 1-2,1
				m_tmp.split(_T(','));
				int step = _ttoi(m_tmp.getVT().at(1)); if(step < 1)step = 1;
				m_tmp = m_tmp.getVT().at(0);
				m_tmp.split(_T('-'));
				int starti = _ttoi(m_tmp.getVT().at(0));if(starti < 1)starti = 1;indlst.push_back(starti);
				int endi = _ttoi(m_tmp.getVT().at(1));if(endi < starti)endi = starti;
				for (int k=1,ci; (ci = starti+k*step)<= endi;k++)indlst.push_back(ci); //set tag lst
			}else if( pdot > 0 && pSub <=0 ){ //1,2,3
				m_tmp.split(_T(','));
				for(int i=0;(int)m_tmp.getVT().size() > i;i++){ indlst.push_back( _ttoi( m_tmp.getVT().at(i) ) ); }
			}
			else if( pdot <= 0 && pSub >0 ){ //1-3
				m_tmp.split(_T('-'));
				int starti = _ttoi(m_tmp.getVT().at(0));if(starti < 1)starti = 1;indlst.push_back(starti);
				int endi = _ttoi(m_tmp.getVT().at(1));if(endi < starti)endi = starti;
				for (int k=1,ci; (ci = starti+k*1)<= endi;k++)indlst.push_back(ci); //set tag lst
			}
			else indlst.push_back(  m_tmp.stoi() < 1 ? 1 : m_tmp.stoi() );
		}
	}
	else 
		indlst.push_back(1); //set 1 when no {} inf

	attr[0]=0;
	if(*(ptb) == _T('[')){ //get attribute inf
		for(i=0; *++ptb != _T(']') && i<ATTR_MAX-1;attr[i]=*ptb,attr[i+1]=0,i++);
		ptb++;
		while(*ptb != 0)ptb++;
	}

	//do main extract
	LPCTSTR pta = ptcTag;
	int k = 0,step = 0,top = 0;
	LPCTSTR p1,p2;i=0;
// 	if(nlayer == 3)
// 		Sleep(0);
n1:
	while( *pta && *pta!= _T('<') )
		pta++;
	if( *pta == 0){ 
		m_ceng--; 
		return pta;
	} //tatch end and quit
	else if( _isFlag(pta,lab,p1,p2,lpHeader,nAllSize) ){ //is flag
		step++;
		if( (i+1) <= (int)indlst.size() && step == indlst.at(i)){
			i++;
			p1 = pta = _labelRecrsion(p2,ptb,resuleSet,nlayer+1,lpHeader,nAllSize);
			while( *pta && *pta != _T('>'))
				pta++;
			if( *pta == 0){ 
				m_ceng--; 
				return pta;
			} //tatch end and quit
 			else if( _tcsicmp( attr,_T("_value_")) == 0){ //store tag value
				size_t num = (p1 > p2 ? p1 - p2 : 0)/sizeof(TCHAR);
				_tcsncpy_s( m_value.store(num),num+1,p2,num);
				resuleSet.push_back(m_value);
				//if(resuleSet.size() >= 250)
				//	Sleep(0);
			}
		}else{
			pta = _deleteFlagPair(pta,lpHeader,nAllSize);			
		}
		goto n1;
	}
	else if(*(pta+1) == _T('/')){ //is quit this layer		
		m_ceng--; 
		return pta;
	}
	else{ //is no flag
		pta = _deleteFlagPair(pta,lpHeader,nAllSize);
		goto n1;
	}
}

BOOL CLHttpSpider::_checkSelfClose(LPCTSTR pc,LPCTSTR lpHeader,size_t nAllSize)
{
	static LPCTSTR flag[] = {_T("br"),_T("img"),_T("input"),_T("meta"),_T("link"),_T("!doctype"),0};//不一定需要 />结尾的
	//static LPCTSTR flag2[] = {_T("p"),0}; // p不是单标签 原代码有误
	BOOL re = FALSE; 
	for(int i=0;flag[i];i++){
		re = TRUE;
		int j=0;
		for (;flag[i][j];j++)
		{
			if( *(pc+j) != flag[i][j] && *(pc+j) != (flag[i][j]-32)){
				re = FALSE; break; //if just one not match continu;
			}
		}
		if( re && ( *(pc+j) == _T('>') || *(pc+j) == _T(' ')) )
			return TRUE; //if all match and end is space or > can sure true
	}
	return re;
}

BOOL CLHttpSpider::_checkNotes(LPCTSTR pc,LPCTSTR &p1,LPCTSTR lpHeader,size_t nAllSize)
{
	if( *pc != _T('!') ||  *(pc+1) != _T('-') || *(pc+2) != _T('-'))
		{ p1 = pc;return FALSE;}
	pc+=3;
	while( *pc != _T('-') || *(pc+1) != _T('-') || *(pc+2) != _T('>'))pc++;
	return p1 = pc+3 ,TRUE;
}

BOOL CLHttpSpider::_isFlag(LPCTSTR pc,LPCTSTR lpflag,LPCTSTR &p1,LPCTSTR &p2,LPCTSTR lpHeader,size_t nAllSize)
{
	//if(*pc!= lpflag[0]){p1 = p2 = pc;return FALSE;}
	LPCTSTR ptc = pc;
	for(int i =0;lpflag[i];i++){
		if( *ptc++ != lpflag[i]){
			p1 = p2 = pc;
			return FALSE;
		}
	}
	if( *ptc == _T(' ') || *ptc == _T('\t')){
		p1 = ptc;
		while(*ptc && *ptc != _T('>'))ptc++;
		if(*ptc == 0){p1 = p2 = pc;return FALSE;}
		p2 = ++ptc;
		return TRUE;
	}else if(*ptc == _T('>')){
		p1 = ptc;p2 = ptc+1;return TRUE;
	}else {
		p1 = p2 = pc;return FALSE;
	}
}

BOOL CLHttpSpider::_tatchEnd(LPCTSTR pc,LPCTSTR lpflag,LPCTSTR &p1,LPCTSTR &p2,LPCTSTR lpHeader,size_t nAllSize)
{
	LPCTSTR ptc = pc;
	int i;
n1:
	while(*ptc != _T('<'))ptc++;
	p1 = ptc++ ;
	if(*ptc!=_T('/'))
		goto n1;
	ptc++;
	for(i =0;lpflag[i];i++){
		if( *ptc++ != lpflag[i]){
			goto n1;
		}
	}
	while(*ptc == _T(' ')||*ptc==_T('\t'))ptc++;
	if(*ptc == _T('>')){
		p2 = ptc+1;return TRUE;
	}else {
		goto n1;
	}
}

LPCTSTR CLHttpSpider::_deleteFlagPair(LPCTSTR pc,LPCTSTR lpHeader,size_t nAllSize)
{
	int top = 0;
	LPCTSTR pta1 = pc;
	LPCTSTR p1=0;
	LPCTSTR p2=0;
	size_t t=0;
	auto deadEnd = lpHeader + nAllSize * sizeof(TCHAR);

//#ifdef _DEBUG
//#define USE_CEHCK
#ifdef USE_CEHCK
#define CHECK_MEM { if( (deadEnd < pta1) /*|| ::IsBadStringPtr(pta1,1)*/){ \
CLString alt; \
	alt.format(_T("_deleteFlagPair func param: char_size = %lld,is too long!"), nAllSize).printf()\
		.messageBoxTimeRef(_T("CLHttpSpider error"), MB_ICONERROR, 5 * 1000).throw_overflow_error(); throw _T("string is bad end!");	\
goto badTatch;}}
#else
#define CHECK_MEM
#endif
//#else 
//#define CHECK_MEM
//#endif
n2:
	while(*pta1++ != _T('<'))
		CHECK_MEM;
	if(*pta1 == _T('/')){
		top--;
		while(*pta1++ != _T('>'))
			CHECK_MEM;
		if(top > 0) 
			goto n2;
		else if(top <= 0)
			return pta1;
	}
	else{ 
// 		if(m_ceng == 3)
// 			if(top >= 1)
// 				Sleep(0);

		if(!_checkSelfClose(pta1,lpHeader,nAllSize)){ //is self close flag
			if(_checkNotes(pta1,p1,lpHeader,nAllSize)){
				pta1 = p1;
				if(top <= 0)
					return pta1;
				else
					goto n2;	
			}
			if(_isFlag(pta1,_T("script"),p1,p2,lpHeader,nAllSize)){
				pta1 = p2;
				if(_tatchEnd(pta1,_T("script"),p1,p2,lpHeader,nAllSize))
					pta1 = p2;
				if(top <= 0)
					return pta1;
				else
					goto n2;	 
			}
			top++;		
		}
		while(*pta1++ != _T('>'))
			CHECK_MEM;
		if(top <= 0)
			return pta1;
		else 
			goto n2;
	}
badTatch:
	throw _T("Never arrive this end!");	
	return 0;
}

CLHttpSpider::CLHttpSpider(){}

CLHttpSpiderR CLHttpSpider::setRem(LPCTSTR lpRem)
{
	assert( lpRem != NULL);
	m_rem = lpRem;
	return *this;
}

CLHttpSpiderR CLHttpSpider::setOrg(LPCTSTR lpOrg)
{
	assert( lpOrg != NULL);
	m_org = lpOrg;
	return *this;
}

CLHttpSpiderR CLHttpSpider::set(LPCTSTR lpOrg,LPCTSTR lpRem)
{
	assert( lpRem != NULL && lpOrg != NULL);
	m_rem = lpRem;
	m_org = lpOrg;
	return *this;
}

std::vector<CLString>& CLHttpSpider::extract(std::vector<CLString>& resuleSet,BOOL bClearSetsBeforeExtract /*= TRUE*/)
{
	if(bClearSetsBeforeExtract)
		resuleSet.clear();
	_labelRecrsion(m_org.string(),m_rem.string(),resuleSet,1,m_org.string(),m_org.size());
	return resuleSet;
}

