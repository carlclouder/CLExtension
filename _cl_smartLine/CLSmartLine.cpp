#include "CLSmartLine.h"
#include <math.h>

BOOL usDebug = 1;
void CLSmartLine::_init() {
	/*lst = 0;
	avr = 0;
	dif = 0;
	var = 0;
	stdd = 0;*/
	lst.clear();
	avr.clear();
	dif.clear();
	var.clear();
	stdd.clear();
	flag = 0;
	nMax = 0;
	ci = 0;
	divN = 0;
	total = 0;
	defV = 0;
}
void CLSmartLine::_release(){
	/*if(lst){delete[] lst;lst =0;}
	if(avr){delete[] avr;avr =0;}
	if(dif){delete[] dif;dif =0;}
	if(var){delete[] var;var =0;}
	if(stdd){delete[] stdd;stdd =0;}*/
	lst.clear();
	avr.clear();
	dif.clear();
	var.clear();
	stdd.clear();
	nMax = 0;
};
void CLSmartLine::_construct(sizeType nmax, sizeType _divn, flagType _flag, valueType _defV) {
	divN = _divn < 1 ? 1 : _divn;
	defV = _defV;
	flag = _flag;
	if ((nmax = (divN > nmax ? divN : nmax)) != nMax)
		_release();
	nMax = nmax;
	/*if (!lst)lst = new valueType[nMax];

	if (!avr) { if (flag & SL_TYPE_AVER)avr = new valueType[nMax]; }
	else { if (!(flag & SL_TYPE_AVER)) { delete[] avr; avr = 0; } }

	if (!dif) { if (flag & SL_TYPE_DIFF)dif = new valueType[nMax]; }
	else { if (!(flag & SL_TYPE_DIFF)) { delete[] dif; dif = 0; } }
	int a = SL_TYPE_VARI, b = flag & SL_TYPE_VARI;
	if (!var) { if ((flag & SL_TYPE_VARI) == SL_TYPE_VARI) var = new valueType[nMax]; }
	else { if (!((flag & SL_TYPE_VARI) == SL_TYPE_VARI)) { delete[] var; var = 0; } }
	if (!stdd) { if ((flag & SL_TYPE_STDD) == SL_TYPE_STDD) stdd = new valueType[nMax]; }
	else { if (!((flag & SL_TYPE_STDD) == SL_TYPE_STDD)) { delete[] stdd; stdd = 0; } }*/
	lst.resize( nMax, defV);

	if (flag & SL_TYPE_AVER)avr.resize(nMax); 
	else { if (!(flag & SL_TYPE_AVER)) { releaseStdVector(avr); } }

	if (flag & SL_TYPE_DIFF)dif.resize(nMax);
	else { if (!(flag & SL_TYPE_DIFF)) { releaseStdVector(dif); } }
	int a = SL_TYPE_VARI, b = flag & SL_TYPE_VARI;
	if ((flag & SL_TYPE_VARI) == SL_TYPE_VARI) var.resize(nMax);
	else { if (!((flag & SL_TYPE_VARI) == SL_TYPE_VARI)) { releaseStdVector(var); } }
	if ((flag & SL_TYPE_STDD) == SL_TYPE_STDD) stdd.resize(nMax);
	else { if (!((flag & SL_TYPE_STDD) == SL_TYPE_STDD)) { releaseStdVector(stdd); } }
};
CLSmartLine::CLSmartLine(){_init();};
CLSmartLine::CLSmartLine(sizeType _nmax,sizeType _divn ,flagType _flag ,valueType _defV){_init();construct(_nmax,_divn,_flag,_defV);};
//CLSmartLine::CLSmartLine(const CLSmartLine& tag){
//	memcpy_s(this,sizeof(CLSmartLine),&tag,sizeof(CLSmartLine));
//	var = stdd = dif = avr = lst = 0;
//	if(tag.maxCounts() <= 0) return;
//	_construct(tag.maxCounts(),tag.moveStep(),tag.typeFlag(),tag.defaultValue());
//	memcpy_s(lst,sizeof(valueType)*maxCounts(),tag.vlst(),sizeof(valueType)*tag.maxCounts());
//	if(flag & SL_TYPE_AVER)memcpy_s(avr,sizeof(valueType)*maxCounts(),tag.avrLst(),sizeof(valueType)*tag.maxCounts());
//	if(flag & SL_TYPE_DIFF)memcpy_s(dif,sizeof(valueType)*maxCounts(),tag.difLst(),sizeof(valueType)*tag.maxCounts());
//	if((flag & SL_TYPE_VARI) == SL_TYPE_VARI)memcpy_s(var,sizeof(valueType)*maxCounts(),tag.varLst(),sizeof(valueType)*tag.maxCounts());
//	if((flag & SL_TYPE_STDD) == SL_TYPE_STDD)memcpy_s(stdd,sizeof(valueType)*maxCounts(),tag.stddLst(),sizeof(valueType)*tag.maxCounts());
//}

CLSmartLine::CLSmartLine(const valueType* _pv,sizeType _nmax,sizeType _divn /*= 0*/,flagType _flag /*= SL_TYPE_NO*/ )
{
	_init();
	construct(_pv,_nmax,_divn,_flag);
}
CLSmartLine::CLSmartLine(const std::vector<valueType>& _pv ,sizeType _divn ,flagType _flag)
{
	_init();
	construct(_pv,_divn,_flag);
}
void CLSmartLine::construct(sizeType _nmax,sizeType _divn ,flagType _flag ,valueType _defV){
	_construct(_nmax,_divn,_flag,_defV);
	reset();
};

void CLSmartLine::construct(const valueType* _pv,sizeType _nmax,sizeType _divn /*= 0*/,flagType _flag /*= SL_TYPE_NO*/ )
{
	assert( _pv != NULL );
	_construct(_nmax,_divn,_flag,0);
	reset();
	for (sizeType i=0;i<_nmax;i++)
		push(_pv[i]);
}

void CLSmartLine::construct( const std::vector<valueType>& _pv,sizeType _divn /*= 0*/,flagType _flag /*= SL_TYPE_NO*/ )
{
	sizeType si = _pv.size();
	if(si == 0)return;
	_construct(si,_divn,_flag,0);
	reset();
	for (sizeType i=0;i<si;i++)
		push(_pv.at(i));
}

CLSmartLine::~CLSmartLine(){_release();};		
void CLSmartLine::reset(){
#ifdef _DEBUG
	//lstCheck.clear();
#endif
	total=0;
	if(lst.size()){
		for(int i=0;i<nMax;i++)lst[i] = defaultValue();
		total = lst[0];
		if(avr.size())avr[0] = ((divN == 0) ? 0 : (sum()/(valueType)divN));
		if(dif.size())dif[0] = 0;
		if(var.size())var[0] = 0;
		if(stdd.size())stdd[0] = 0;
		ci= -1;
		do{
			push(defaultValue());
		}while( index(0) < (nMax-1));
	}
	ci= -1;
}
void CLSmartLine::release(){_release();reset();};
CLSmartLine::valueType& CLSmartLine::operator [](sizeType i){
	if(i < 0 || i >= nMax )goto err;
	return (valueType&)(lst[i]);
err:
	::MessageBox(NULL,_T("SmartLine “[]操作”越界 ！"),_T("SmartLine Alert"),MB_ICONERROR);
	throw std::logic_error("SmartLine “[]操作”越界 ！");
	return (valueType&)(lst[0]);
};
CLSmartLine::sizeType  CLSmartLine::push(valueType v){
	if(!lst.data())goto err;
	//CLCheckNanMsg(v, CLSmartLine_push_NanException );
	CLCheckNan(v);
	CLCheckInf(v);
	BOOL isCys;
	if( (isCys = ((flag & SL_TYPE_NOCYCLE) && (ci == (nMax -1)))) )
		for(sizeType i =1;i<nMax;lst[i-1]=lst[i],i++);
#ifdef _DEBUG
	//if(usDebug == 1 && ci == -1 || ci == 0) lstCheck.clear();
#endif
	ci = index(1);		
	if((flag & SL_TYPE_AVER)/*&&(avr)*/){
		if(isCys)for(sizeType i =1;i<nMax;avr[i-1]=avr[i],i++);
		total -= lst[index((-1)*divN)];
		total += (v);
		avr[ci] = ((divN == 0) ? 0 : (sum()/(valueType)divN));
	}
	if((flag & SL_TYPE_DIFF)/*&& dif*/){
		if(isCys)for(sizeType i =1;i<nMax;dif[i-1]=dif[i],i++);
		dif[ci] = (v)-lst[index((-1)*divN)];
	}
	if(((flag & SL_TYPE_VARI) == SL_TYPE_VARI)/*&& var*/){
		if(isCys)for(sizeType i =1;i<nMax;var[i-1]=var[i],i++);
		valueType t;
		var[ci] = 0;
		for (int i=0;i < divN;i++)
		{
			t = (lst[index((-1)*i)]-aver());
			t = t*t;
			var[ci] += t;
		}
		var[ci] = (var[ci]/(valueType)moveStep());
	}
	if(((flag & SL_TYPE_STDD) == SL_TYPE_STDD)/*&& stdd*/){
		if(isCys)for(sizeType i =1;i<nMax;stdd[i-1]=stdd[i],i++);		
		stdd[ci] = sqrt(vari());
	}
	lst[ci] = v;
#ifdef _DEBUG
	//if (usDebug == 1 ) lstCheck.push_back(v);
#endif
	return ci;
err:
	::MessageBox(NULL,_T("CLSmartLine类“push操作”非法，对象没有构造 ！"),_T("CLSmartLine Alert"),MB_ICONERROR);
	throw std::logic_error("CLSmartLine类“push操作”非法，对象没有构造 ！");
	return -1;
}
const  CLSmartLine::valueType*  CLSmartLine::zeroAvrLstEnd(){
	if(flag & SL_TYPE_AVER)for(sizeType i = ci+1; i < nMax ; i++ )avr[i]=0;
	return avrLst();
}
const  CLSmartLine::valueType*  CLSmartLine::zeroDifLstEnd(){
	if(flag & SL_TYPE_DIFF)for(sizeType i = ci+1; i < nMax ; i++ )dif[i]=0;
	return difLst();
}
const  CLSmartLine::valueType*  CLSmartLine::zeroVarLstEnd(){
	if((flag & SL_TYPE_VARI) == SL_TYPE_VARI)for(sizeType i = ci+1; i < nMax ; i++ )var[i]=0;
	return varLst();
}
const  CLSmartLine::valueType*  CLSmartLine::zeroStddLstEnd(){
	if((flag & SL_TYPE_STDD) == SL_TYPE_STDD)for(sizeType i = ci+1; i < nMax ; i++ )stdd[i]=0;
	return stddLst();
}

void CLSmartLine::dumpVlst()const
{
	std::cout <<  std::endl << _T("CLSmartLine dump line base:") << std::endl;
	if(lst.data()){
		std::cout << _T(",") << lst[0];
		for(sizeType i = 0; i < nMax ; i++ ) std::cout<< _T(", ") << lst[i] ;
		std::cout << std::endl;
	}
}

void CLSmartLine::dumpAvrLst()const
{
	std::cout <<  std::endl << _T("CLSmartLine dump line aver:") << std::endl;
	if((flag & SL_TYPE_AVER) /*&& avr */){ 
		std::cout << _T(",") << avr[0];
		for(sizeType i = 1; i < nMax ; i++ ){ std::cout << _T(", ") << avr[i];}
		std::cout << std::endl;
	}
}

void CLSmartLine::dumpDifLst()const
{
	std::cout <<  std::endl << _T("CLSmartLine dump line diff:") << std::endl;
	if((flag & SL_TYPE_DIFF) /*&& dif */){ 
		std::cout << _T(",") << dif[0];
		for(sizeType i = 1; i < nMax ; i++ ){ std::cout << _T(", ") << dif[i];}
		std::cout << std::endl;
	}
}

void CLSmartLine::dumpVariLst()const
{
	std::cout <<  std::endl << _T("CLSmartLine dump line vari:") << std::endl;
	if(((flag & SL_TYPE_VARI) == SL_TYPE_VARI)/* && var */){ 
		std::cout << _T(",") << var[0];
		for(sizeType i = 1; i < nMax ; i++ ){ std::cout << _T(", ") << var[i];}
		std::cout << std::endl;
	}
}

void CLSmartLine::dumpStddLst()const
{
	std::cout <<  std::endl << _T("CLSmartLine dump line stdd:") << std::endl;
	if(((flag & SL_TYPE_STDD) == SL_TYPE_STDD) /*&& stdd */){ 
		std::cout << _T(",") << stdd[0];
		for(sizeType i = 1; i < nMax ; i++ ){ std::cout << _T(", ") << stdd[i];}
		std::cout << std::endl;
	}
}

void CLSmartLine::dumpAllLst() const
{
	std::cout <<  std::endl << _T("CLSmartLine dump line all:") << std::endl;
	if(nMax > 0){ 
		for(sizeType i = 0; i < nMax ; i++ ){ 
			std::cout << _T("    [ ") << (lst.data()?lst[i]:0.0) << _T(", ") 
				<< (((flag & SL_TYPE_AVER) /*&& avr */)?avr[i]:0.0)	<< _T(", ") 
				<< ((((flag & SL_TYPE_VARI) == SL_TYPE_VARI)/* && var */)?var[i]:0.0) << _T(", ") 
				<< ((((flag & SL_TYPE_STDD) == SL_TYPE_STDD) /*&& stdd */)?stdd[i]:0.0) << _T(", ") 
				<< (((flag & SL_TYPE_DIFF)/* && dif */)?dif[0]:0.0) << _T(" ]\r\n");
		}
		std::cout << _T("    [BASE, AVER, VARI, STDD, DIFF]") <<std::endl;
	}
}

CLSmartLine & CLSmartLine::reverse()
{
	if (nMax > 0) {
		valueType * ps, *pe, t;
		if (lst.size()) {
			ps = lst.data();
			pe = &lst[nMax - 1];
			for (; ps < pe ; ps++,pe--){t = *ps; *ps = *pe; *pe = t;}
		}
		if (avr.size()) {
			ps = avr.data();
			pe = &avr[nMax - 1];
			for (; ps < pe; ps++, pe--) { t = *ps; *ps = *pe; *pe = t; }
		}
		if (dif.size()) {
			ps = dif.data();
			pe = &dif[nMax - 1];
			for (; ps < pe; ps++, pe--) { t = *ps; *ps = *pe; *pe = t; }
		}
		if (var.size()) {
			ps = var.data();
			pe = &var[nMax - 1];
			for (; ps < pe; ps++, pe--) { t = *ps; *ps = *pe; *pe = t; }
		}
		if (stdd.size()) {
			ps = stdd.data();
			pe = &stdd[nMax - 1];
			for (; ps < pe; ps++, pe--) { t = *ps; *ps = *pe; *pe = t; }
		}
	}
	return *this;
}
