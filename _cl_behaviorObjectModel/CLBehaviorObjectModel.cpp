#include "CLBehaviorObjectModel.h"

CLBhvObjUnit::CLBhvObjUnit()
{
	thisBhvCounts = 0;
	uid  = 0;
	pFather = 0;
	linkPower = 1;
}

CLBhvObjUnitR CLBhvObjUnit::reset()
{
	if(pFather)pFather->unRegiste(*this);
	uid = 0;
	inputVector.clear();
	outputVector.clear();
	transMatrix.clear();
	midMatrixA.clear();
	subLst.clear();	
	return *this;
}

CLBhvObjUnitR CLBhvObjUnit::dumpStruct( int itype /*= 0*/ )
{
	if ( itype == 0)
	{
		_tprintf_s(_T("\r\n[%lld] "),getUid());
		for (size_t i=0;i<subLst.size();i++)
			_tprintf_s(_T("-->(%lld)"),pFather ? getSubUt(i).getUid() : 0);
	}
	return *this;
}

CLBhvObjUnitR CLBhvObjUnit::trigger( const VTV& _inputVector )
{
	size_t si = subLst.size();
	inputVector = _inputVector;
	if(si == 0){		
		midMatrixA.clear();
		midMatrixA.add_row(inputVector);
		outputVector = (midMatrixA * transMatrix)[0];
		return *this;
	}
	if(pFather){
		outputVector = inputVector ;
		for(size_t i=0;i<si;i++){
			outputVector = getSubUt(i).trigger(outputVector).getOutputVector();
		}
	}
	return *this;
}


CLBhvObjUnitR CLBhvObjUnit::learning()
{
	transMatrix = (midMatrixA.inv() *  midMatrixB) ;
	return *this;
}

CLBhvObjUnitR CLBhvObjUnit::getSubUt( size_t index )
{
	 return pFather->getUt(subLst.at(index));
}

CLBhvObjUnitR CLBhvObjUnit::setInputVector( size_t nSi,valueType v1,... )
{
	valueType p = v1;
	inputVector.clear();	
	va_list ap;
	va_start(ap,v1);
	for (size_t i = 0;i < nSi;i++){		
		inputVector.push_back( p );
		p = va_arg(ap,valueType);
	}
	va_end(ap);
	return *this;
}

CLBhvObjUnitR CLBhvObjUnit::setOutputVector( size_t nSi,valueType v1,... )
{
	valueType p = v1;
	outputVector.clear();	
	va_list ap;
	va_start(ap,v1);
	for (size_t i = 0;i < nSi;i++){
		outputVector.push_back( p );
		p = va_arg(ap,valueType);
	}
	va_end(ap);
	return *this;
}

CLBhvObjUnitR CLBhvObjUnit::registe()
{
	if(pFather)pFather->registe(*this);
	return *this;
}

CLBhvObjUnitR CLBhvObjUnit::unRegiste()
{
	if(pFather)pFather->unRegiste(*this);
	return *this;
}

CLBhvObjUnit::~CLBhvObjUnit()
{

}

CLBhvObjManager::CLBhvObjManager( size_t _maxStroeUnit /*= MAXBOU_COUNTS*/ )
{	
	pRemember = new byte[maxStroeSets = _maxStroeUnit];
	ZeroMemory(pRemember,sizeof(byte)*maxStroeSets);
	size_t si = maxStroeSets*8;
	pBOStore = new CLBhvObjUnit[si];
	for (size_t i = 0;i < si;i++)
		pBOStore[i].setFather(this);
}

CLBhvObjManager::~CLBhvObjManager()
{
	if (pRemember){delete[] pRemember;pRemember = 0;}
	if (pBOStore){delete[] pBOStore;pBOStore = 0;}
}

CLBhvObjUnitR CLBhvObjManager::getUt( UID uid )
{
	assert( uid/8 <= MAXBOU_SETTS && pBOStore != NULL );
	return pBOStore[uid];
}

UID CLBhvObjManager::getNewUid()
{
	for (size_t i = 0 ;i < MAXBOU_SETTS;i++)
	{
		for (size_t j=0 ;j<8;j++)
		{
			if(pRemember[i] & (1<<j))
				continue;
			else{ return i*8 + j + 1;}
		}		
	}
	return 0;
}

BOOL CLBhvObjManager::registe( CLBhvObjUnitR obj )
{
	if( obj.isRegiste() == TRUE ) return FALSE;
	UID nid = getNewUid();
	if( setNewUid(nid ,TRUE) == 0){ obj.setUid(nid); return TRUE;}
	else return FALSE;
}

BOOL CLBhvObjManager::unRegiste( CLBhvObjUnitR obj )
{
	if( obj.isRegiste() == FALSE) return FALSE;	
	if( setNewUid(obj.getUid(),FALSE) == 0){ obj.setUid(0); return TRUE;}
	else return FALSE;
}

int CLBhvObjManager::setNewUid( UID uid ,BOOL setting )
{
	if(uid == 0)  return -1;
	uid -=1;
	size_t i = uid / 3;
	if( i > MAXBOU_SETTS) return -1;
	size_t j = uid % 3;
	assert(pRemember!=NULL);
	if( setting ){
		if( pRemember[i]  &  (1<<(j)) ) { return 1; }
		else { pRemember[i] |= (1<<(j));return 0; }
	}else{
		if( pRemember[i]  &  (1<<(j)) ) {  pRemember[i] ^= (1<<(j));return 0;}
		else { return 1; }
	}
}

CLBhvObjManager CLBhvObjManager::resize( size_t _newMaxStroeSets )
{
	return *this;
}
