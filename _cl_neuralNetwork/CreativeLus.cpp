#include "CreativeLusEx.h"


PCStr const _lpDefineMsgBoxTitle = ("Bpnn struct define alert");
PCStr const _lpSamSetsMsgBoxTitle = ("Bpnn sample sets alert");
PCStr const _lpBpnnToolMsgBoxTitle = ("Neural network tool alert");

BpnnSamSets& BpnnSamSets::normalizationOrStandardization(const TransTypeVec& flagVecter)
{
	VLF tmp;
	Uint totalDim = intputDimension() + targetDimension();
	//if (flagVecter.size() != totalDim)
	//	CLString().format(("\r\nThe dimension of transformed data is different from that of input and output vectors.\r\nFlagVec(%lld) != InputVec(%lld) + OutputVec(%lld)\r\n"),
	//		flagVecter.size(), intputDimension(), targetDimension()).printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	transModeRec.clear();
	for (Uint i = 0; i < totalDim; i++)//对每一列做变换
	{
		auto& pmod = (transModeRec[i]);
		pmod.dimIndex = i;
		if (i < flagVecter.size()) {
			switch (flagVecter[i]) {
			case STT_Null: {} break;
			case STT_Normalize: {
				pmod.dimType = STT_Normalize;
				Float vmax = -FLT_MAX, vmin = FLT_MAX, v;
				for (Uint j = 0; j < size(); j++) {
					v = (*this)[j][i];
					//if (abs(v) > 1e5)Sleep(0);
					if (v > vmax)vmax = v;
					if (v < vmin)vmin = v;
				}
				if ((vmax - vmin) == 0)
					for (Uint j = 0; j < size(); j++) {
						(*this)[j][i] = 0.5;//全部为0
					}
				else
					for (Uint j = 0; j < size(); j++) {
						auto pv = &((*this)[j][i]);
						*pv = (*pv - vmin) / (vmax - vmin);
					};
				pmod.vmax = vmax;
				pmod.vmin = vmin;
			} break;
			case STT_NormalizeEx: {
				pmod.dimType = STT_NormalizeEx;
				Float vmax = -FLT_MAX, vmin = FLT_MAX, v;
				for (Uint j = 0; j < size(); j++) {
					v = (*this)[j][i];
					//if (abs(v) > 1e5)Sleep(0);
					if (v > vmax)vmax = v;
					if (v < vmin)vmin = v;
				}
				if ((vmax - vmin) == 0)
					for (Uint j = 0; j < size(); j++) {
						(*this)[j][i] = 0;//全部为0
					}
				else
					for (Uint j = 0; j < size(); j++) {
						auto pv = &((*this)[j][i]);
						*pv = (*pv - vmin) / (vmax - vmin) * 2 - 1;
					};
				pmod.vmax = vmax;
				pmod.vmin = vmin;
			} break;
			case STT_Standart: {
				pmod.dimType = STT_Standart;
				Float vaver = 0, nsi = 0, v = 0, vSd = 0;
				for (Uint j = 0; j < size(); j++) {
					v += (*this)[j][i];
					nsi++;
				}
				if (nsi > 0) {
					vaver = v / nsi;
					for (Uint j = 0; j < size(); j++) {
						v = (*this)[j][i] - vaver;
						v = v * v;
						vSd += v;
					}
					vSd = sqrt(vSd / nsi);
					pmod.vAver = vaver;
					pmod.vStandardDeviation = vSd;
				}
			}  break;
			default: {}
				   break;
			};
		}
	}
	return *this;
}

Uint BpnnSamSets::getTransModRecSize() const
{
	return transModeRec.size();
}

TransModelDb BpnnSamSets::getTransModRec() const
{
	return transModeRec;
}

TransModelDb CreativeLus::BpnnSamSets::getTransModRec(PCStr lpFile, Bool binMode)
{
	assert(lpFile != nullptr);
	BpnnSamSets tag;
	readSamSetsFromFile(tag, lpFile, binMode);
	return std::move(tag.getTransModRec());
}

BpnnSamSets& BpnnSamSets::clearTransModRec()
{
	transModeRec.clear();
	return *this;
}

BpnnSamSets& BpnnSamSets::setTransModRec(const TransModelDb& rec)
{
	transModeRec = rec;
	return *this;
}

#define _extend_Interface_ ((CLBpExtend*)(extend == 0 ? (extend = new CLBpExtend((CLBpKernel*)kernel,*this)):extend))->
#define _kernel_Interface_ ((CLBpKernel*)kernel)->
Bpnn::Bpnn()
	:extend(NULL)
{
	kernel = new CLBpKernel;
	if (kernel == NULL)
		CLString(("错误：神经网络计算核心因未知异常导致未能构造！")).messageBoxRef(_lpBpnnToolMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
}

Bpnn::Bpnn(const Bpnn& bpnn)
	:extend(NULL)
{
	kernel = new CLBpKernel;
	if (kernel == NULL)
		CLString(("错误：神经网络计算核心因未知异常导致未能构造！")).messageBoxRef(_lpBpnnToolMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	*this = bpnn;
}

Bpnn& Bpnn::operator=(const Bpnn& bpnn)
{
	*(CLBpKernel*)kernel = *(CLBpKernel*)(bpnn.kernel);
	return *this;
}

Bpnn::~Bpnn()
{
	if (extend)
		delete ((CLBpExtend*)extend), extend = 0;
	if (kernel)
		delete ((CLBpKernel*)kernel), kernel = 0;
}

Bpnn::MonitorExample::MonitorExample()
	:m_printCounts(0), m_bkChars(0), pTick(new CLTick) {}

Bpnn::MonitorExample::~MonitorExample()
{
	delete (CLTick*)pTick, pTick = 0;
}

void Bpnn::MonitorExample::reset()
{
	m_printCounts = 0;
	m_bkChars = 0;
}

void Bpnn::MonitorExample::print(BPNN_CBF_MONITOR_PARAM)
{
	if (inf) {
		if (max < 0) {
			printfbc(m_bkChars, ("%s \n"), inf);
			m_bkChars = 0;
		}
		else {
			double lipt = ((CLTick*)pTick)->getSpendTime();
			
			if (lipt >= 0.5 || c == max) {
				((CLTick*)pTick)->timingStart();
				Float cc = (Float(c) / max)*100;
				double aver = lipt / (c - m_printCounts);
				double spend = aver * c;
				double leave = aver * (double(max) - c);
				m_bkChars = printfbc(m_bkChars, ("%s [%g%%] %d : %d. Estimated time [ %.4fs, (%.4fs - %.4fs), %.4fs ]."),
					inf, cc, c, max, spend + leave, spend, leave, aver);
				m_printCounts = c;				
			}

			/*CLTick& tik = *(CLTick*)pTick;
			if (c != 0 && (c == 1 || c == max || cc != m_printCounts)) {
				if (c <= 1)
					tik.timingStart();
				double spend = tik.getSpendTime();
				double aver = spend / c;
				double leave = aver * (double(max) - c);
				m_bkChars = printfbc(m_bkChars, ("%s [%d%%] %d : %d. Estimated time [ %.4fs, (%.4fs - %.4fs), %.4fs ]."),
					inf, cc, c, max, spend + leave, spend, leave, aver);
				m_printCounts = cc;
			}*/
		}
	}
};



Bpnn& Bpnn::detachExtend()
{
	if (extend) {
		delete ((CLBpExtend*)extend);
		extend = nullptr;
	}
	return *this;
}

Bpnn& Bpnn::setDropout(Uint repeatTrainTimes, const DropoutLayerDef& def) {
	return _extend_Interface_ setDropout(repeatTrainTimes, def), * this;
}

Bpnn& Bpnn::setBatchNormalization(Uint miniBatch, const BnLayerIndexList& bnLayerList)
{
	return _extend_Interface_ setBatchNormalization(miniBatch, bnLayerList), * this;
}

Bpnn& Bpnn::setNetUseTo(EBP_UT type)
{
	return _extend_Interface_ setNetUseTo(type), * this;
}
//设置损失函数类型。
Bpnn& Bpnn::setLossFunc(EBP_LS lossId) {
	return _kernel_Interface_ setLossFunc(lossId), * this;
}

Bpnn& Bpnn::setWbiDefault(Float W) {
	return _extend_Interface_ setWbiDefault(W), * this;
}
Bpnn& Bpnn::setWbiDefault(Float A, Float B) {
	return _extend_Interface_ setWbiDefault(A, B), * this;
}

Bool  Bpnn::getOutput(VLF& out_yi) const
{
	return _kernel_Interface_ getOutput(out_yi);
}

Bpnn& Bpnn::setUseRandSample(Bool _isUse) {
	return _extend_Interface_ setUseRandSample(_isUse), * this;
}

Bpnn& Bpnn::buildNet(PCBMonitor _pCbFun, PVoid _pIns) {
	return _extend_Interface_ buildNet(_pCbFun, _pIns), * this;
}

Bpnn& Bpnn::setTrainState(EBP_TPC currentState)
{
	return _extend_Interface_ setTrainState(currentState), * this;
}

EBP_TPC Bpnn::getTrainState() const
{
	return extend ? ((CLBpExtend*)extend)->m_trainProcControl() : TPC_Enable;
}

Bool  Bpnn::train(VLF* pOutEa, VLF* pOutLs, VLF* pOutMc, PCBMonitor _pCbFun, PVoid _pIns) {
	return _extend_Interface_ train(pOutEa, pOutLs, pOutMc, _pCbFun, _pIns);
}

Bool  Bpnn::writeBpnnToFile(PCStr lpFileFullPathName, Bool binMode, Bool encrypteMod) {
	return _kernel_Interface_ writeBpnnToFile(lpFileFullPathName, binMode, encrypteMod);
}

Bool Bpnn::readBpnnFormFile(PCStr lpFile, Bool binMode) {
	return _kernel_Interface_ readBpnnFormFile(lpFile, binMode);
}

Bpnn& Bpnn::setSampSets(const BpnnSamSets& tag) {
	return _extend_Interface_ setSampSets(tag), * this;
}

Float Bpnn::getCorrectRate(const BpnnSamSets* predict, Uint nCounst, Bool useRandom, EBP_CRT crtype) {
	return _extend_Interface_ getCorrectRate(predict, nCounst, useRandom,(Byte)crtype);
}

Bpnn& Bpnn::setCorrectRateEvaluationModel(Float correctRate, const BpnnSamSets* predict, Uint nCounst, Bool useRandom, EBP_CRT crtype) {
	return _extend_Interface_ setCorrectRateEvaluationModel(correctRate, predict, nCounst, useRandom, (Byte)crtype), * this;
}
//检查模型是否是以正确率评价训练收敛的；
Bool Bpnn::isCorrectRateEvaluationModel() const {
	if (extend)
		return ((CLBpExtend*)extend)->isCorrectRateEvaluationModel();
	return false;
}

Bpnn& Bpnn::setSampleBatchCounts(Uint nCounts, Bool UseRandomSamp) {
	return _extend_Interface_ setSampleBatchCounts(nCounts, UseRandomSamp), * this;
}

Float Bpnn::predict(const VLF& inputVec, VLF* resultVec, VLF* tagVec) {
	return _kernel_Interface_ predict(inputVec, resultVec, tagVec);
}

Float Bpnn::predict(const Float* inputData, Uint inputDimension, VLF* resultVec, VLF* tagVec)
{
	return _kernel_Interface_ predict(inputData, inputDimension, resultVec, tagVec);
}

Bpnn& Bpnn::predict()
{
	return _kernel_Interface_ predict(), * this;
}

void Bpnn::makeIndependentDataBuf(VLF& yiData) const
{
	return _kernel_Interface_ makeIndependentDataBuf(yiData);
}

Float Bpnn::predictWithIndependentData(Float* yiData, const VLF& inputVec, VLF* _out_resultVec, VLF* tagVec) 
{
	return _kernel_Interface_ predictWithIndependentData(yiData, inputVec, _out_resultVec,tagVec);
}
Float Bpnn::predictWithIndependentData(VLF& yiData, const VLF& inputVec, VLF* _out_resultVec, VLF* tagVec) 
{
	return _kernel_Interface_ predictWithIndependentData(yiData.data(), inputVec, _out_resultVec, tagVec);
}

Bpnn& Bpnn::setStructure(const BpnnStructDef& mod) {
	return _extend_Interface_ setStructure(mod), * this;
}
Bool Bpnn::exportGraphNetStruct(PCStr outFileName, Bool bUseDetailMode) {
	return _extend_Interface_ exportGraphNetStruct(outFileName, bUseDetailMode);
}

void* Bpnn::showWbDataDistribution(Int x, Int y, Uint cx, Uint cy, Uint sectionCounts) const
{
	return isEncrypted() ? nullptr : _kernel_Interface_ showWbDataDistribution(x, y, cx, cy,sectionCounts);
}

void Bpnn::exportWbDataDistribution(VLF& retRange, VLF& retFreq, Uint sectionCounts, Float* vmin, Float* vmax) const {
	retRange.clear();
	retFreq.clear();
	if (!isEncrypted())
		_kernel_Interface_ exportWbDataDistribution(retRange, retFreq, sectionCounts, vmin, vmax);
}


Uint Bpnn::neuronCounts() const {	
	return isEncrypted() ? 0 : _kernel_Interface_ neuronCounts();
}
Uint Bpnn::neuronCountsInLayer(Uint lay) const
{
	return layerCounts() > lay ? _kernel_Interface_ LayNnCounts(lay):0;
}
Uint Bpnn::inputDimension() const {
	return _kernel_Interface_ inputDimension();
}
Uint Bpnn::outputDimension() const {
	return _kernel_Interface_ outputDimension();
}
Uint Bpnn::layerCounts() const {
	return  isEncrypted() ? 0 : _kernel_Interface_ layerCounts();
}
Uint Bpnn::hideLayerCounts() const {
	return  isEncrypted() ? 0 : _kernel_Interface_ hideLayerCounts();
}

Bpnn& Bpnn::setMultiThreadSupport(Bool bOpen) {
	return _extend_Interface_ setMultiThreadSupport(bOpen), * this;
}

Bpnn& Bpnn::setGpuAcceleratedSupport(Bool bOpen) {
	return _extend_Interface_ setGpuAcceleratedSupport(bOpen), * this;
}

Bpnn& Bpnn::openGraphFlag(Bool bOpen) {
	return _extend_Interface_ openGraphFlag(bOpen), * this;
}

Bpnn& Bpnn::showGraphParam(Uint maxDataToShow, Int posX, Int posY) {
	return _extend_Interface_ showGraphParam(maxDataToShow, posX, posY), * this;
}

Bpnn& Bpnn::showGraphNetStruct(Bool isShowDetail, Int posX, Int posY) {
	return _extend_Interface_ showGraphNetStruct(isShowDetail, posX, posY), * this;
}

Bool Bpnn::exportGraphCorrectRate(PCStr lpfileName) {
	return _extend_Interface_ exportGraphCorrectRate(lpfileName);
}
Bool Bpnn::exportGraphEr(PCStr lpfileName) {
	return _extend_Interface_ exportGraphEr(lpfileName);
}
Bool Bpnn::exportGraphLs(PCStr lpfileName) {
	return _extend_Interface_ exportGraphLs(lpfileName);
}
Bool Bpnn::exportGraphMc(PCStr lpfileName) {
	return _extend_Interface_ exportGraphMc(lpfileName);
}

//取得内部样本集的数据对个数
Uint Bpnn::getSampleCounts() const {
	if (extend)
		return ((CLBpExtend*)extend)->getSampleCounts();
	return 0;
}
Uint Bpnn::getRunTimes() const {
	if (extend)
		return ((CLBpExtend*)extend)->getRunTimes();
	return 0;
}
Uint Bpnn::getRunTimesTutal() const
{
	if (extend)
		return ((CLBpExtend*)extend)->getRunTimesTutal();
	return 0;
}
Float Bpnn::getEr() const {
	if (extend)
		return ((CLBpExtend*)extend)->getEr();
	return 0;
}
Float Bpnn::getLs() const {
	if (extend)
		return ((CLBpExtend*)extend)->getLs();
	return 0;
}
Float Bpnn::getLsOld() const
{
	if (extend)
		return ((CLBpExtend*)extend)->g_ls_old;
	return 0;
}
Float Bpnn::getMc() const {
	if (extend)
		return ((CLBpExtend*)extend)->getMc();
	return 0;
}
Float Bpnn::getMcOld() const
{
	if (extend)
		return ((CLBpExtend*)extend)->g_mc_old;
	return 0;
}
Float Bpnn::getDEr() const {
	if (extend)
		return ((CLBpExtend*)extend)->getDEr();
	return 0;
}
Float Bpnn::getAccuracy() const {
	if (extend)
		return ((CLBpExtend*)extend)->getAccuracy();
	return 0;
}
Uint Bpnn::getMaxTimes() const {
	if (extend)
		return ((CLBpExtend*)extend)->getMaxTimes();
	return 0;
}

Float Bpnn::getSavedCorrectRate() const {
	if (extend)
		return ((CLBpExtend*)extend)->getSavedCorrectRate();
	return 0;
}

Bool Bpnn::isEncrypted() const
{
	ULONGLONG rtf = 21536;
	try {
		_kernel_Interface_ isEncrypted();
	}
	catch (ULONGLONG e) {
		rtf = e ? e : 0;
	}
	return rtf ? true : false;
}

Bpnn& Bpnn::setName(PCStr lpName) {
	return _kernel_Interface_ setName(lpName), * this;
}

PCStr Bpnn::getName() const {
	//if (!extend)return "";
	return _kernel_Interface_ getName();
}

Bpnn& Bpnn::reset() {
	if (extend)((CLBpExtend*)extend)->reset();
	return _kernel_Interface_ reset(), * this;
}

Bpnn& Bpnn::setLayer(Uint hideLayerNumbers, Uint hidePerLayerNumbers) {
	return _extend_Interface_ setLayer(hideLayerNumbers, hidePerLayerNumbers), * this;
}

Bpnn& Bpnn::setParam(Float ls, Float er_accuracy, Float mc) {
	return _extend_Interface_ setParam(ls, er_accuracy, mc), * this;
}

Bpnn& Bpnn::setMaxTimes(Uint iMaxTimes) {
	return _extend_Interface_ setMaxTimes(iMaxTimes), * this;
}

Bpnn& Bpnn::setAdam(Bool open, Float stepl)
{
	return _extend_Interface_ setAdam(open, stepl), * this;
}

Bpnn& Bpnn::setAutoFitLsAndMc(PCBAutoFitLsAndMc pFAutoFit, PVoid pIns) {
	return _extend_Interface_ setAutoFitLsAndMc(pFAutoFit, pIns), * this;
}
Bpnn& Bpnn::setInput(const Float* inputArray, Uint dataDim) {
	return _kernel_Interface_ setInput(inputArray, dataDim), * this;
}
Bpnn& Bpnn::setInput(const VLF& inputArray) {
	return _kernel_Interface_ setInput(inputArray.data(), inputArray.size()), * this;
}

Bpnn& Bpnn::setTransFunc(EBP_TF iBpTypeHide, EBP_TF iBpTypeOut) {
	return _extend_Interface_ setTransFunc(iBpTypeHide, iBpTypeOut), * this;
}

//内部数据集增加一个样本对，标将指向最后一个数据


//对内部样本数据集做归一化或标准化，变换规则由一个TransTypeVec向量表明，每一维度对应一种变换处理方式。


//取得样本文件，保存一份原内部集合对象的副本到bkSet中，bkSet = NULL 不复制；
//binMode = true表示读取二进制文件（该模式用于大量数据情况）
//当采用二进制方式时，指定的文件的扩展名将被自动替换为读取固定扩展名".bpnnSampSets"文件，以确保读取固定的二进制数据流文件；


//保存样本到文件，binMode = true表示2进制文件形式保存（该模式用于大量数据的导出）
//当采用二进制方式时，文件的扩展名将被修改为固定的".bpnnSampSets"并输出文件，以确保写入固定的二进制数据流文件；


//数据变换记录

BpnnSamSets& BpnnSamSets::changeDimension(Uint _ivDim, Uint _ovDim)
{
	clear();
	if (_ivDim == 0) {
		CLString(("\nCan't change input data vector dimension to 0.\n")).printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	}
	if (_ovDim == 0) {
		CLString(("\nCan't change target data vector dimension to 0.\n")).printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	}
	ivDim = _ivDim;
	tvDim = _ovDim;
	return *this;
}

Uint BpnnSamSets::size() const
{
	return intputDimension() > 0 ? ivdata.size() / intputDimension() : 0;
}

BpnnSamSets& BpnnSamSets::resize(Uint newSize, Float defaultInValue, Float defaultTagValue, Uint newInDim, Uint newTagDim)
{
	if ((newInDim > 0 && newInDim != intputDimension()) || (newTagDim > 0 && newTagDim != targetDimension()))
		changeDimension(newInDim, newTagDim);
	if (size() != newSize) {
		ivdata.resize(size_t(newSize) * intputDimension(), defaultInValue);
		tvdata.resize(size_t(newSize) * targetDimension(), defaultTagValue);
	}
	return *this;
}

BpnnSamSets::BpnnSamSets()
{
	ivDim = tvDim = 0;
}

BpnnSamSets::BpnnSamSets(Uint _ivDim, Uint _ovDim)
{
	ivDim = _ivDim; tvDim = _ovDim;
}

BpnnSamSets::~BpnnSamSets()
{
}

Uint BpnnSamSets::intputDimension() const
{
	return ivDim;
}

Uint BpnnSamSets::targetDimension() const
{
	return tvDim;
}

Uint BpnnSamSets::dimension() const
{
	return intputDimension() + targetDimension();
}

BpnnSamSets& BpnnSamSets::clear()
{
	transModeRec.clear();
	ivdata.clear();
	tvdata.clear();
	return *this;
}
BpnnSamSets& BpnnSamSets::reset()
{
	clear();
	ivDim = tvDim = 0;
	releaseStdVector(ivdata);
	releaseStdVector(tvdata);
	return *this;
}

Bool BpnnSamSets::writeToFile(PCStr file, Bool binMode)
{
	return writeSamSetsToFile(*this, file, binMode);
}

Bool BpnnSamSets::readFromFile(PCStr lpFile, Bool binMode)
{
	return readSamSetsFromFile(*this, lpFile, binMode);
}

BpnnSamSets& BpnnSamSets::addSample(const VLF& inputArray, const VLF& targetArray)
{
	/*if (intputDimension() == 0 || targetDimension() == 0) {
		if (inputArray.size() == 0) {
			CLString(("\nSample input data vector dimension can't be 0.\n")).printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
		}
		if (targetArray.size() == 0) {
			CLString(("\nSample target data vector dimension can't be 0.\n")).printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
		}
		ivDim = inputArray.size();
		tvDim = targetArray.size();
	}
	if (inputArray.size() != intputDimension()) {
		CLString().format(("\nInput data vector dimension(%d) can not match the sample iv dimension(%d).\n"), intputDimension(), inputArray.size())
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	}
	if (targetArray.size() != targetDimension()) {
		CLString().format(("\nTarget data vector dimension(%d) can not match the sample ov dimension(%d).\n"), targetDimension(), targetArray.size())
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	}
	ivdata.insert(ivdata.end(), inputArray.cbegin(), inputArray.cend());
	tvdata.insert(tvdata.end(), targetArray.cbegin(), targetArray.cend());
	return *this;*/
	return addSample(inputArray.data(), inputArray.size(), targetArray.data(), targetArray.size());
}

BpnnSamSets& CreativeLus::BpnnSamSets::addSample(const Float* inputArray, Uint inputArrayDim, const Float* targetArray, Uint targetArrayDim)
{
	if (intputDimension() == 0 || targetDimension() == 0) {
		if (inputArrayDim == 0) {
			CLString(("\nSample input data vector dimension can't be 0.\n")).printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
		}
		if (targetArrayDim == 0) {
			CLString(("\nSample target data vector dimension can't be 0.\n")).printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
		}
		ivDim = inputArrayDim;
		tvDim = targetArrayDim;
	}
	if (inputArrayDim != intputDimension()) {
		CLString().format(("\nInput data vector dimension(%d) can not match the sample iv dimension(%d).\n"), intputDimension(), inputArrayDim)
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	}
	if (targetArrayDim != targetDimension()) {
		CLString().format(("\nTarget data vector dimension(%d) can not match the sample ov dimension(%d).\n"), targetDimension(), targetArrayDim)
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	}
	auto pos = ivdata.size();
	ivdata.resize(pos + inputArrayDim);
	memcpy_s(ivdata.data() + pos, inputArrayDim * sizeof(Float), inputArray, inputArrayDim * sizeof(Float));
	pos = tvdata.size();
	tvdata.resize(pos + targetArrayDim);
	memcpy_s(tvdata.data() + pos, targetArrayDim * sizeof(Float), targetArray, targetArrayDim * sizeof(Float));
	return *this;
}

BpnnSamSets& BpnnSamSets::addSample(const BpnnSamPair& samPair)
{
	return addSample(samPair.inputVec(), samPair.targetVec());
}

BpnnSamSets& BpnnSamSets::setSample(Uint samIndex, const VLF& inputArray, const VLF& targetArray)
{
	/*if (size() <= samIndex)
		CLString(("\nBpnnSamSets is out of range.\n")).printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_out_of_range();
	if (inputArray.size() != intputDimension()) {
		CLString().format(("\nInput data vector dimension(%d) can not match the sample iv dimension(%d).\n"), intputDimension(), inputArray.size())
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	}
	if (targetArray.size() != targetDimension()) {
		CLString().format(("\nTarget data vector dimension(%d) can not match the sample ov dimension(%d).\n"), targetDimension(), targetArray.size())
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	}
	auto pair = operator[](samIndex);
	memcpy_s(pair.iv(), intputDimension() * sizeof(Float), inputArray.data(), intputDimension() * sizeof(Float));
	memcpy_s(pair.tv(), targetDimension() * sizeof(Float), targetArray.data(), targetDimension() * sizeof(Float));
	return *this;*/
	return setSample(samIndex, inputArray.data(), inputArray.size(), targetArray.data(), targetArray.size());
}

BpnnSamSets& CreativeLus::BpnnSamSets::setSample(Uint samIndex, const Float* inputArray, Uint inputArrayDim, const Float* targetArray, Uint targetArrayDim)
{
	if (size() <= samIndex)
		CLString(("\nBpnnSamSets is out of range.\n")).printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_out_of_range();
	if (inputArrayDim != intputDimension()) {
		CLString().format(("\nInput data vector dimension(%d) can not match the sample iv dimension(%d).\n"), intputDimension(), inputArrayDim)
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	}
	if (targetArrayDim != targetDimension()) {
		CLString().format(("\nTarget data vector dimension(%d) can not match the sample ov dimension(%d).\n"), targetDimension(), targetArrayDim)
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	}
	auto pair = operator[](samIndex);
	memcpy_s(pair.iv(), intputDimension() * sizeof(Float), inputArray, intputDimension() * sizeof(Float));
	memcpy_s(pair.tv(), targetDimension() * sizeof(Float), targetArray, targetDimension() * sizeof(Float));
	return *this;
}

BpnnSamSets& BpnnSamSets::setSample(Uint samIndex, const BpnnSamPair& samPair)
{
	return setSample(samIndex, samPair.inputVec(), samPair.targetVec());
}

void BpnnSamSets::copyAndTargetDimToIntervalClassification(BpnnSamSets& newSampSets, Float classFlagMax, Float classFlagMin, const VLF& intervalTable) const{
	/*if (targetDimension() > intervalTable.size())
		CLString().format("\nTarget data vector dimension(%d) can not match the Interval Classification Table size(%zd).\n", targetDimension(), intervalTable.size())
		.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();*/
	newSampSets.reset();
	
	Float maxflag = max(classFlagMax, classFlagMin);
	Float minflag = min(classFlagMax, classFlagMin);
	auto rag = intervalTable;
	sort(rag.begin(), rag.end());
	VLF targetArray(targetDimension() * (rag.size() + 1));
	for (size_t i = 0; i < intputDimension(); i++)
	{
		newSampSets.transModeRec[i] = transModeRec.find(i)->second;
	}
	TransModel ut;
	for (size_t i = intputDimension(),si = intputDimension() + targetDimension() * (rag.size() + 1); i < si; i++)
	{
		ut.dimIndex = i;
		newSampSets.transModeRec[i] = ut;
	}
	for (size_t i = 0; i < size(); i++)
	{
		auto itv =  this->tv(i);
		auto iiv =  this->iv(i);
		targetArray.clear();
		for (size_t k = 0; k < targetDimension(); k++)
		{
			auto v = itv[k];
			for (size_t j = 0; j < rag.size() + 1; j++)
			{
				Float vflag;
				if (j == 0 && v <= rag[j])
					vflag = (maxflag);
				else if (j == rag.size() && v > rag[j - 1])
					vflag = (maxflag);
				else if (v > rag[j - 1] && v <= rag[j])
					vflag = (maxflag);
				else
					vflag = (minflag);
				targetArray.push_back(vflag);
			}
		}
		newSampSets.addSample(iiv, intputDimension(), targetArray.data(), targetArray.size());
	}
}

BpnnSamPair BpnnSamSets::operator[](const Uint i)
{
	Uint si = size();
	if (i < si) {
		auto id = intputDimension();
		auto od = targetDimension();
		return BpnnSamPair(&ivdata[size_t(i) * id], id, &tvdata[size_t(i) * od], od);
	}
	throw runtime_error("BpnnSamSets out of range!");
}

const Float* BpnnSamSets::iv(const Uint i) const
{
	return &ivdata[size_t(i) * intputDimension()];
}

const Float* BpnnSamSets::tv(const Uint i) const
{
	return &tvdata[size_t(i) * targetDimension()];
}

const Float* BpnnSamSets::ivData() const
{
	return ivdata.data();
}
const Float* BpnnSamSets::tvData() const
{
	return tvdata.data();
}

const VLF& BpnnSamSets::ivDataVec() const {
	return ivdata;
}
const VLF& BpnnSamSets::tvDataVec() const {
	return tvdata;
}

void BpnnSamSets::getDimVecData(VLF& reseult, Uint dimVecIndex) {
	reseult.clear();	
	if (dimVecIndex >= dimension()) {
		CLString().format("\nBpnnSamSets get DimVecData(index=%u) is out of dimension range(%u).\n", dimVecIndex, dimension())
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_out_of_range();
	}
	else {
		const Float* pi;
		Uint dim;
		if (dimVecIndex < intputDimension()) {
			 pi = ivData() + dimVecIndex;
			 dim = intputDimension();
		}
		else {  
			pi = tvData() + dimVecIndex - intputDimension(); 
			dim = targetDimension();
		}
		reseult.resize(size());
		for (Uint i = 0, si = size(); i < si; i++)
		{
			reseult[i] = *pi;
			pi += dim;
		}
	}
}
//取得输入向量集的某一个维度的分量数据,索引重0开始
void BpnnSamSets::getIntputDimVecData(VLF& reseult, Uint dimVecIndex) {
	reseult.clear();
	if (dimVecIndex >= intputDimension()) {
		CLString().format("\nBpnnSamSets get intput DimVecData(index=%u) is out of dimension range(%u).\n", dimVecIndex, intputDimension() )
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_out_of_range();
	}
	else {
		auto pi = ivData() + dimVecIndex;
		auto dim = intputDimension();
		reseult.resize(size());
		for (Uint i = 0, si = size(); i < si; i++)
		{
			reseult[i] = *pi;
			pi += dim;
		}
	}
}
//取得输出向量集的某一个维度的分量数据,索引重0开始
void BpnnSamSets::getTargetDimVecData(VLF& reseult, Uint dimVecIndex) {
	reseult.clear();
	if (dimVecIndex >= targetDimension()) {
		CLString().format("\nBpnnSamSets get target DimVecData(index=%u) is out of dimension range(%u).\n", dimVecIndex, targetDimension())
			.printf().messageBoxRef(_lpSamSetsMsgBoxTitle, MB_ICONERROR).throw_out_of_range();
	}
	else {
		auto pi = tvData() + dimVecIndex - intputDimension();
		auto dim = targetDimension();
		reseult.resize(size());
		for (Uint i = 0, si = size(); i < si; i++)
		{
			reseult[i] = *pi;
			pi += dim;
		}
	}
}

void _showAcceleratorDeviceAmp(Bool show_all = false, Bool old_format = false) {
#if UseCppAmp > 0
	std::vector<accelerator> accls;
	try {
	accls = accelerator::get_all();
	}
	catch (const std::exception & e) {
		printf(e.what());
	}
	if (!show_all)
	{
		accls.erase(std::remove_if(accls.begin(), accls.end(), [](accelerator& a)
			{
				return (a.device_path == accelerator::cpu_accelerator) || (a.device_path == accelerator::direct3d_ref);
			}), accls.end());
	}

	if (accls.empty())
	{
		std::wcout << "No accelerators found that are compatible with C++ AMP" << std::endl << std::endl;
		return;
	}
	std::cout << std::endl << "Show " << (show_all ? "all " : "") << "AMP Devices (";
#if defined(_DEBUG)
	std::cout << "DEBUG";
#else
	std::cout << "RELEASE";
#endif
	std::cout << " build)" << std::endl;
	std::wcout << "Found " << accls.size()
		<< " accelerator device(s) that are compatible with C++ AMP:" << std::endl;
	Int n = 0;
	if (old_format)
	{
		std::for_each(accls.cbegin(), accls.cend(), [=, &n](const accelerator& a)
			{
				std::wcout << "  " << ++n << ": " << a.description
					<< ", has_display = " << (a.has_display ? "true" : "false")
					<< ", is_emulated = " << (a.is_emulated ? "true" : "false")
					<< std::endl;
			});
		std::wcout << std::endl;
		return;
	}

	std::for_each(accls.cbegin(), accls.cend(), [=, &n](const accelerator& a)
		{
			std::wcout << "  " << ++n << ": " << a.description << " "
				<< std::endl << "       device_path                       = " << a.device_path
				<< std::endl << "       dedicated_memory                  = " << std::setprecision(4) << float(a.dedicated_memory) / (1024.0f * 1024.0f) << " Mb"
				<< std::endl << "       has_display                       = " << (a.has_display ? "true" : "false")
				<< std::endl << "       is_debug                          = " << (a.is_debug ? "true" : "false")
				<< std::endl << "       is_emulated                       = " << (a.is_emulated ? "true" : "false")
				<< std::endl << "       supports_double_precision         = " << (a.supports_double_precision ? "true" : "false")
				<< std::endl << "       supports_limited_double_precision = " << (a.supports_limited_double_precision ? "true" : "false")
				<< std::endl << "       supports_cpu_shared_memory        = " << (a.supports_cpu_shared_memory ? "true" : "false")
				<< std::endl;
		});
	std::wcout << std::endl;
#endif
}

void showAcceleratorDeviceAmp(Uint type)
{
#if UseCppAmp > 0
	if (type == 1)
		return _showAcceleratorDeviceAmp(false, true);
	if (type == 2)
		return _showAcceleratorDeviceAmp(true, false);
	CLString cmd;
	while (CLString::getCharCmd(cmd.store(256), 255, ("退出Esc，确认Enter，请输入 /a 或 /o [直接Enter] ： "))) {
		Bool show_all = false;
		Bool old_format = false;
		cmd.trim();
		if (cmd.compare("/a") == 0)
		{
			show_all = true;
		}
		else if (cmd.compare("/o") == 0)
		{
			show_all = false;
			old_format = true;
		}
		else if (cmd.size() == 0) {
			show_all = false;
			old_format = true;
		}
		else
			continue;

		_showAcceleratorDeviceAmp(show_all, old_format);
		system("pause");
	}
	return;
#else
	CLString("函数showAcceleratorDeviceAmp()暂未开放功能！").messageBox(_lpBpnnToolMsgBoxTitle, MB_ICONINFORMATION | MB_OK);
	return;
#endif	
}

Int CreativeLus::rand_i_0_x(const Int x) {
	return RAND_I_0_X(x);
}

Int CreativeLus::rand_i_a_b(const Int a, const Int b) {
	return  RAND_I_A_B(a, b);
}

Int CreativeLus::rand_i_ai_b(const Int  a, const Int  b) {
	return  RAND_I_Ai_B(a, b);
}

Int CreativeLus::rand_i_ai_bi(const Int  a, const Int  b) {
	return  RAND_I_Ai_Bi(a, b);
}

Int CreativeLus::rand_i_a_bi(const Int  a, const Int  b) {
	return  RAND_I_A_Bi(a, b);
}

Float CreativeLus::rand_f_0_1() {
	return  RAND_F_0_1();
}

Float CreativeLus::rand_f_a_b(const Float  a, const Float  b) {
	return RAND_F_A_B(a, b);
}

static random_device rd;
static mt19937 gen(rd());
void CreativeLus::rand_initData(Float* pData, Uint dataSize,EBP_IT itType, const RandInitParam& paramLst) {
	auto si = paramLst.size();
	Float param1 = si > 0 ? paramLst[0]:0;
	Float param2 = si > 1 ? paramLst[1]:0;
	switch (itType)
	{
	case	IT_Const: {
		if (si == 0) {
			for (Uint i = 0; i < dataSize; i++)pData[i] = param1;
		}
		else {
			for (Uint i = 0; i < dataSize; i++)pData[i] = paramLst[(i%si)];
		}
	}break;//常量
	case	IT_Uniform: {
		uniform_real_distribution<Float> _dst(min(param1, param2), max(param1, param2));
		for (Uint i = 0; i < dataSize; i++)pData[i] = _dst(gen);
	}break;//均值分布
	case	IT_Gaussian: {
		normal_distribution<Float> _dst(param1, param2 <= 0 ? 1 : param2);
		for (Uint i = 0; i < dataSize; i++)pData[i] = _dst(gen);
	}break;//高斯分布（即正太分布）
	case	IT_Xavier: {
		normal_distribution<Float> _dst(0.0, sqrt(2.0 / (Int(param1 < 1.0 ? 1.0 : param1) + Int(param2 < 1.0 ? 1.0 : param2))));
		for (Uint i = 0; i < dataSize; i++)pData[i] = _dst(gen);
	} break;//参数由0均值，标准差为sqrt(2 / (fan_in + fan_out))的正态分布；在tanh激活函数上有很好的效果，但不适用于Relu激活函数。
	case	IT_Msra: {
		normal_distribution<Float> _dst(0.0, sqrt(2.0 / Float(dataSize)));
		for (Uint i = 0; i < dataSize; i++)pData[i] = _dst(gen);
	} break;//均值为0，标准差为sqrt（2/输入的个数n）的高斯分布；它特别适合 Relu激活函数。
	default:
		for (Uint i = 0; i < dataSize; i++)pData[i] = param1;
		break;
	}
}
using namespace rapidjson;
Bool Bpnn::executeByJson(Bpnn& bp, BpnnInterfaceStore* iStore, PCStr jsonString)
{
	if (iStore)iStore->release();
	if (strlen(jsonString) == 0)
		return false;
	rapidjson::GenericDocument<ASCII<>> doc;
	if (!doc.Parse(jsonString).HasParseError())
	{
		auto ie = doc.FindMember("enable");
		if (ie != doc.MemberEnd()
			&& (*ie).value.IsInt()) {
			if ((*ie).value.GetInt() <= 0)
				return true;
		}
//#define traceProgram() cout << ++i << ". \"" << key << "\" " <<endl;
//#define traceProgram2() cout<< "    " << ++i2 << ". " << key2 << " ; " <<endl;
#define traceProgram()
#define traceProgram2() 
		Uint i = 0; 
		PCStr key = nullptr;
		vector<BpnnSamSets> sets;
		vector<BpnnStructDef> defs;
		for (auto iv = doc.MemberBegin(); iv != doc.MemberEnd(); ++iv,++i) {
			if (iv == ie)
				continue;
			auto& vl = (*iv).value;
			auto name = (*iv).name.GetString();
			if (_stricmp(name, key = "bpnnSamSets") == 0
				&& vl.IsArray()) {
				traceProgram();
				const auto& ary = vl.GetArray();
				sets.clear();
				sets.resize(ary.Size());
				for (size_t k = 0; k < ary.Size(); k++)
				{
					auto& it = sets[k];
					it.clear();
					if (!ary[k].IsString())
						continue;
					auto vl2 = ary[k].GetString();
					if (strlen(vl2) < 2)
						continue;					
					it.readFromFile(vl2,true);
				}
			}
			else if (_stricmp(name, key = "bpnnStructDef") == 0
				&& vl.IsArray()) {
				traceProgram();
				defs.clear();
				defs.resize(vl.Size());
				//拆分单元
				for (size_t i = 0; i < vl.Size(); i++)
				{
					auto& def = defs[i];
					def.clear();
					const auto& vl2 = vl[i];
					//优先执行的时脚本检查
					if (((vl2.FindMember("addScript"))->value.FindMember("enable"))->value.GetInt() != 0) {
						const auto& v = (*(vl2.FindMember("addScript"))).value;
						const auto it = v.FindMember("script");
						BpnnStructScript scp;
						if (it != v.MemberEnd()) { //sub: scrip
							const auto& sary = it->value;							
							if (sary.IsArray()) {
								auto nlayers = sary.Size();
								for (size_t i = 0; i < nlayers; i++) // lay
								{
									BpnnStructScriptLayer lay;
									const auto& vlay = sary[i];
									if (vlay.IsArray()) {
										auto nsets = vlay.Size();
										for (size_t j = 0; j < nsets; j++) // set obj
										{
											const auto& vset = vlay[j];
											FilterMapStruct filterMap;
											if (vset.HasMember("filterMap") && vset["filterMap"].IsArray()) {
												const auto& fmdata = vset["filterMap"].GetArray();
												for (size_t k = 0; k < fmdata.Size(); k++) {
													filterMap.push_back(fmdata[k].GetUint());
												}
											}
											WiInitDef wi;
											if (vset.HasMember("wi") && vset["wi"].IsArray()) {
												const auto& fmdata = vset["wi"].GetArray();
												for (size_t k = 0; k < fmdata.Size(); k++) {
													wi.push_back(fmdata[k].GetFloat());
												}
											}
											BiInitDef bi;
											if (vset.HasMember("bi") && vset["bi"].IsArray()) {
												const auto& fmdata = vset["bi"].GetArray();
												for (size_t k = 0; k < fmdata.Size(); k++) {
													bi.push_back(fmdata[k].GetFloat());
												}
											}
											BpnnStructScriptSet set = {
		Uint(vset.HasMember("upIndex") && vset["upIndex"].IsInt() ? abs(vset["upIndex"].GetInt()) : 0), //上一层要链接的组的编号
		EBP_SCP(vset.HasMember("type") && vset["type"].IsInt() ? abs(vset["type"].GetInt()) : SCP_Fc), //本组的类型
		/*
		定义本组脚本的链接和输出映射方式；
		filterMap 写法：
			SCP_Fc : filterMap = { 输出个数} ;
			SCP_Trans : filterMap = {} ;
			SCP_Pool : filterMap = { filter宽，filter高，X步幅，Y步幅} ;
			SCP_Conv : filterMap = { filter宽，filter高，输出map个数，填充padding，X步幅，Y步幅} ;
			SCP_ConvSep : filterMap = { filter宽，filter高，填充padding，X步幅，Y步幅} ;
			SCP_Combin : filterMap = { 结合次数 } ;
			SCP_Softmax : filterMap = { 输出分类个数} ;
		*/
		 filterMap,
		 wi, //权值bi初始化参数
		 bi, //阈值bi初始化参数
		Int(vset.HasMember("trFunc") && vset["trFunc"].IsInt() ? vset["trFunc"].GetInt() : -1),//激活函数类型，当值为小于0表示使用对应生成类型的默认值
		Int(vset.HasMember("wcFunc") && vset["wcFunc"].IsInt() ? vset["wcFunc"].GetInt() : -1),//权值组合函数类型，当值为小于0表示使用对应生成类型的默认值
		Int(vset.HasMember("flag") && vset["flag"].IsInt() ? vset["flag"].GetInt() : -1),//组的标记（保留），当值为小于0表示使用对应生成类型的默认值
											};
											lay.push_back(std::move(set));
										}
										if (lay.size() > 0)
											scp.push_back(std::move(lay));
									}									
								}
							}
						}
						InputFilterMap ifm;
						if (v.HasMember("inputFilterMap") && v["inputFilterMap"].IsArray()) {
							const auto& fmdata = v["inputFilterMap"].GetArray();
							for (size_t k = 0; k < fmdata.Size(); k++) {
								ifm.push_back(fmdata[k].GetUint());
							}
						}
						def.addScript(scp, ifm, v.HasMember("logout") && v["logout"].IsInt() ? bool(v["logout"].GetInt()) : false);
					}//在执行模式定义
					else if (((vl2.FindMember("mod"))->value.FindMember("enable"))->value.GetInt() != 0) {
						const auto& v = (*(vl2.FindMember("mod"))).value;
						const auto it = v.FindMember("struct");
						if (it != v.MemberEnd()) { //sub: scrip
							const auto& sary = it->value;
							if (sary.IsArray()) {
								auto nlayers = sary.Size();
								for (size_t i = 0; i < nlayers; i++) // lay
								{
									BSLayer lay;
									const auto& vlay = sary[i];
									if (vlay.IsArray()) {
										auto nsets = vlay.Size();
										for (size_t j = 0; j < nsets; j++) // set obj
										{
											const auto& vset = vlay[j];
											LinkPos linkPos;
											if (vset.HasMember("linkPos") && vset["linkPos"].IsArray()) {
												const auto& fmdata = vset["linkPos"].GetArray();
												for (size_t k = 0; k < fmdata.Size(); k++) {
													linkPos.push_back(fmdata[k].GetInt());
												}
											}
											LinkWij wi;
											if (vset.HasMember("wi") && vset["wi"].IsArray()) {
												const auto& fmdata = vset["wi"].GetArray();
												for (size_t k = 0; k < fmdata.Size(); k++) {
													wi.push_back(fmdata[k].GetFloat());
												}
											}
											LinkBi bi;
											if (vset.HasMember("bi") && vset["bi"].IsArray()) {
												const auto& fmdata = vset["bi"].GetArray();
												for (size_t k = 0; k < fmdata.Size(); k++) {
													bi.push_back(fmdata[k].GetFloat());
												}
											}
											BSB bsb(
												Uint(vset.HasMember("nnCounts") && vset["nnCounts"].IsInt() ? abs(vset["nnCounts"].GetInt()) : 1), //上一层要链接的组的编号
												EBP_TF(vset.HasMember("trFunc") && vset["trFunc"].IsInt() ? abs(vset["trFunc"].GetInt()) : TF_Sigmoid), //本组的类型		
												linkPos,//从1开始的编号
												EBP_WC(vset.HasMember("wcFunc") && vset["wcFunc"].IsInt() ? abs(vset["wcFunc"].GetInt()) : WC_Add), //本组的类型
												wi, //权值bi初始化参数
												bi //阈值bi初始化参数};
											);
											lay.push_back(std::move(bsb));
										}
										if (lay.size() > 0)
											def.push_back(std::move(lay));
									}
								}
							}
						}
					}//最后在执行文件读取
					else if (strlen(key = (*(vl2.FindMember("readFromFile"))).value.GetString()) > 2) {
						def.readFromFile(key);
					}
					//存在写出路径就写出
					if (strlen(key = (*(vl2.FindMember("writeToFile"))).value.GetString()) > 2) {
						def.writeToFile(key);
					}
				}
			}
			else if (_stricmp(name, key = "reset") == 0
				&& vl.IsNumber()) {
				traceProgram();
				if (vl.GetInt() > 0)
					bp.reset();
			}
			else if (_stricmp(name, key = "setSampSets") == 0
				&& vl.IsInt()) {
				traceProgram();
				if (!iStore || vl.GetInt() < 0 || sets.size() <= size_t(abs(vl.GetInt())))
					continue;
				auto pset = new BpnnSamSets(sets[vl.GetInt()]);//堆上分配对象，并传到外界
				bp.setSampSets(*pset);
				(*iStore)["trainSampSets"] = (PIBpnnBase)pset;
			}
			else if (_stricmp(name, key = "setStructure") == 0
				&& vl.IsInt()) {
				traceProgram();
				if (!iStore || vl.GetInt() < 0 || defs.size() <= size_t(abs(vl.GetInt())))
					continue;
				auto pdef = new BpnnStructDef(defs[vl.GetInt()]);
				(*iStore)["bpnnStructDef"] = (PIBpnnBase)pdef;
				bp.setStructure(*pdef);
			}
			else if (_stricmp(name, key = "setLayer") == 0
				&& vl.IsArray()) {
				traceProgram();
				const auto& ary = vl.GetArray();
				if (ary.Size() < 2 || ary[0].GetInt() < 1 || ary[1].GetInt() < 1)
					continue;
				bp.setLayer(ary[0].GetInt(), ary[1].GetInt());
			}
			else if (_stricmp(name, key = "buildNet") == 0
				&& vl.IsInt()) {
				traceProgram();
				if (vl.GetInt() > 0)
					bp.buildNet();
			}
			else if (_stricmp(name, key = "readBpnnFormFile") == 0
				&& vl.IsObject()) {
				traceProgram();
				const auto& vl2 = vl.GetObjectA();
				if (
					(vl2.MemberCount() < 3) ||
					(!vl2.HasMember("enable")) || (vl2["enable"].GetInt() < 1) ||
					(!vl2.HasMember("file")) || (!vl2["file"].IsString()) || (vl2["file"].GetStringLength() < 2) ||
					(!vl2.HasMember("binMode")) || (!vl2["binMode"].IsInt())
					)
					continue;
				bp.readBpnnFormFile(vl2["file"].GetString(), bool(abs(vl2["binMode"].GetInt())));
			}
			else if (_stricmp(name, key = "setName") == 0
				&& vl.IsString()) {
				traceProgram();
				bp.setName(vl.GetString());
			}
			else if (_stricmp(name, key = "setNetUseTo") == 0
				&& vl.IsInt()) {
				traceProgram();
				if(vl.GetInt() >= 0)
					bp.setNetUseTo(EBP_UT(vl.GetInt()));
			}
			else if (_stricmp(name, key = "setAutoFitLsAndMc") == 0
				&& vl.IsArray()) {		
				traceProgram();
				struct FitCallbk:IBpnnBase {
					Float subRate = 0.95f;
					Float minRate = 0.05f;
					void autoFitFunc(BPNN_CBF_AUTOFIT_PARAM) {
						if (bp.getRunTimes() != 1)
							return;
						const Uint muti = bp.getMaxTimes() * 1;
						if (bp.getRunTimesTutal() / muti >= 1
							&& bp.getRunTimesTutal() % muti < bp.getMaxTimes())
							ls *= subRate;
						if (bp.getLsOld() * minRate > ls)
							ls = bp.getLsOld() * minRate;
					};
					virtual ~FitCallbk(){}
				};
				auto ary = vl.GetArray();
				if (ary.Size() >= 1 && (ary[0].GetInt() >= 0)) {
					if(ary[0].GetInt() == 0)
						bp.setAutoFitLsAndMc(nullptr, nullptr);
					else {
						if (!iStore)continue;
						FitCallbk* ck = new FitCallbk;
						ck->subRate = ary.Size() >= 2 ? min(abs(ary[1].GetFloat()), 1) : 0.95f;
						ck->minRate = ary.Size() >= 3 ? min(abs(ary[2].GetFloat()), 1) : 0.05f;
						BPNN_PCB_AUTOFIT_MAKE(pFunc, FitCallbk::autoFitFunc);
						bp.setAutoFitLsAndMc(pFunc, ck);
						(*iStore)["autoFitCallBack"] = PIBpnnBase(ck);

					}
				}
			}
			else if (_stricmp(name, key = "setBatchNormalization") == 0
				&& vl.IsArray()) {
				traceProgram();
				auto ary = vl.GetArray();
				if (ary.Size() >= 1 && (ary[0].GetInt() >= 0)) {
					if (ary[0].GetInt() == 0)
						bp.setBatchNormalization(0);
					else {
						BnLayerIndexList vlst;
						for (size_t i = 1; i < ary.Size(); i++)
							vlst.push_back(ary[i].GetUint());
						bp.setBatchNormalization(ary[0].GetInt(), vlst);
					}
				}
			}
			else if (_stricmp(name, key = "setCorrectRateEvaluationModel") == 0
				&& vl.IsArray()) {
				traceProgram();
				auto ary = vl.GetArray();
				if (ary.Size() >= 1 && (ary[0].GetFloat() >= 0)) {
					if (ary[0].GetFloat() <= 0.0001)
						bp.setCorrectRateEvaluationModel(0);
					else {
						BpnnSamSets* pset = 0;
						if (iStore && ary[1].GetInt() >= 0
							&& ary[1].GetInt() < (Int)sets.size()) {
							pset = new BpnnSamSets(sets[ary[1].GetInt()]);
							(*iStore)["testSampSets"] = (PIBpnnBase)pset;
						}
						bp.setCorrectRateEvaluationModel(
							min(abs(ary[0].GetFloat()), 1),
							pset ? pset : nullptr,
							ary.Size() >= 3 ? abs(ary[2].GetInt()) : 0,
							ary.Size() >= 4 ? bool(abs(ary[3].GetInt())) : false,
							EBP_CRT(ary.Size() >= 5 ? abs(ary[4].GetInt()) : CRT_MeanSquareLoss)
						);
						
					}
				}
			}
			else if (_stricmp(name, key = "setDropout") == 0
				&& vl.IsArray()) {
				traceProgram();
				auto ary = vl.GetArray();
				if (ary.Size() >= 1 && (ary[0].GetInt() >= 0)) {
					bp.setDropout(ary[0].GetInt(),
						{ DRP(ary.Size() >= 2 ? ary[1].GetUint() : 0,
							ary.Size() >= 3 ? ary[2].GetUint() : 0,
							ary.Size() >= 4 ? ary[3].GetFloat() : 0)
						});
				}
			}			
			else if (_stricmp(name, key = "setLossFunc") == 0
				&& vl.IsInt()) {
				traceProgram();
				if (vl.GetInt() >= 0)
					bp.setLossFunc(EBP_LS(abs(vl.GetInt())));
			}
			else if (_stricmp(name, key = "setMaxTimes") == 0
				&& vl.IsInt()) {
				traceProgram();
				if (vl.GetInt() >= 0)
					bp.setMaxTimes(vl.GetInt());
			}
			else if (_stricmp(name, key = "setMultiThreadSupport") == 0
				&& vl.IsInt()) {
				traceProgram();
				if (vl.GetInt() >= 0)
					bp.setMultiThreadSupport(abs(vl.GetInt()));
			}
			else if (_stricmp(name, key = "setAdam") == 0
			&& vl.IsArray()) {
				traceProgram();
				const auto& ary = vl.GetArray();
				if (ary.Size() < 1 || ary[0].GetFloat() < 0.0f)
					continue;
				bp.setAdam(ary[0].GetFloat() > VtEpslon, ary.Size() >= 2 ? ary[1].GetFloat() : 0.001);
			}
			else if (_stricmp(name, key = "setParam") == 0
				&& vl.IsArray()) {
				traceProgram();
				const auto& ary = vl.GetArray();
				if (ary.Size() < 3 || ary[0].GetFloat() < 0.0f)
					continue;
				bp.setParam(abs(ary[0].GetFloat()), abs(ary[1].GetFloat()), abs(ary[2].GetFloat()));
			}
			else if (_stricmp(name, key = "setSampleBatchCounts") == 0
			&& vl.IsArray()) {
				traceProgram();
				const auto& ary = vl.GetArray();
				if (ary.Size() < 2 || ary[0].GetInt() < 0)
					continue;
				bp.setSampleBatchCounts(ary[0].GetInt(), bool(ary[1].GetInt()));
			}
			else if (_stricmp(name, key = "setTransFunc") == 0
			&& vl.IsArray()) {
				traceProgram();
				const auto& ary = vl.GetArray();
				if (ary.Size() < 2 || ary[0].GetInt() <0)
					continue;
				bp.setTransFunc(EBP_TF(ary[0].GetInt()), EBP_TF(abs(ary[1].GetInt())));
			}
			else if (_stricmp(name, key = "setUseRandSample") == 0
				&& vl.IsInt()) {
				traceProgram();
				if (vl.GetInt() >= 0)					
					bp.setUseRandSample(vl.GetInt());
			}
			else if (_stricmp(name, key = "setWbiDefault") == 0
				&& vl.IsArray()) {
				traceProgram();
				if (vl.GetArray().Size() < 2)
					continue;
				auto vmin = min(vl.GetArray()[0].GetFloat(), vl.GetArray()[1].GetFloat());
				auto vmax = max(vl.GetArray()[0].GetFloat(), vl.GetArray()[1].GetFloat());
				bp.setWbiDefault(vmin, vmax);
			}
			else if (_stricmp(name, key = "openGraphFlag") == 0
				&& vl.IsInt()) {
				traceProgram();
				if (vl.GetInt() >= 0)
					bp.openGraphFlag(vl.GetInt());
			}	
			else if (_stricmp(name, key = "train") == 0
				&& vl.IsInt()) {
				traceProgram();
				if (vl.GetInt() > 0)
					bp.train();
			}
			else if (_stricmp(name, key = "writeBpnnToFile") == 0
				&& vl.IsObject()) {
				traceProgram();
				const auto vl2 = vl.GetObjectA();
				if (
					(vl2.MemberCount() < 4) ||
					(!vl2.HasMember("enable")) || (vl2["enable"].GetInt() <= 0)  ||
					(!vl2.HasMember("file")) || (!vl2["file"].IsString()) || (vl2["file"].GetStringLength() < 2) ||
					(!vl2.HasMember("binMode")) || (!vl2["binMode"].IsInt()) ||
					(!vl2.HasMember("encrypteMod")) || (!vl2["encrypteMod"].IsInt())
					)
					continue;
				bp.writeBpnnToFile(vl2["file"].GetString(), 
					bool(abs(vl2["binMode"].GetInt())),
					bool(abs(vl2["encrypteMod"].GetInt())));
			}
			else if (_stricmp(name, key = "showGraphParam") == 0
				&& vl.IsArray()) {
				traceProgram();
				const auto& ary = vl.GetArray();
				if (ary.Size() < 3 || (ary[0].GetInt()) < 0)continue;
				bp.showGraphParam(ary[0].GetInt(), ary[1].GetInt(), ary[2].GetInt());
			}
			else if (_stricmp(name, key = "showGraphNetStruct") == 0
				&& vl.IsArray()) {
				traceProgram();
				const auto& ary = vl.GetArray();
				if (ary.Size() < 3 || (ary[0].GetInt()) < 0)continue;
				bp.showGraphNetStruct(ary[0].GetInt(), ary[1].GetInt(), ary[2].GetInt());
			}
			else if (_stricmp(name, key = "showWbDataDistribution") == 0
				&& vl.IsArray()) {
				traceProgram();
				const auto& ary = vl.GetArray();
				if (ary.Size() < 3 || (ary[0].GetInt()) < 0)continue;
				bp.showWbDataDistribution( ary[1].GetInt(), ary[2].GetInt(),700,300, ary[0].GetInt() == 0 ? 50 : ary[0].GetInt());
			}
			else if (_stricmp(name, key = "exportGraphCorrectRate") == 0
				&& vl.IsString()) {
				traceProgram();
				if (vl.GetStringLength() < 2)
					continue;
				bp.exportGraphCorrectRate(vl.GetString());
			}
			else if (_stricmp(name, key = "exportGraphEr") == 0
				&& vl.IsString()) {
				traceProgram();
				if (vl.GetStringLength() < 2)
					continue;
				bp.exportGraphEr(vl.GetString());
			}
			else if (_stricmp(name, key = "exportGraphNetStruct") == 0
				&& vl.IsString()) {
				traceProgram();
				if (vl.GetStringLength() < 2)
					continue;
				bp.exportGraphNetStruct(vl.GetString(),true);
			}
		}
		return true;
	}
	else return false;
}
Bool Bpnn::executeByJson(BpnnInterfaceStore* iStore, PCStr jsonString) {
	return Bpnn::executeByJson(*this, iStore,  jsonString);
}
Bool Bpnn::executeByJsonFile(Bpnn& bp, BpnnInterfaceStore* iStore, PCStr jsonFile)
{
	if (iStore)iStore->release();
	CLString file(jsonFile);
	if (!file.fileExists())
		return false;
	if(!file.readFile(file))
		return false;
	return executeByJson(bp, iStore, file.getASCII());
}
Bool Bpnn::executeByJsonFile(BpnnInterfaceStore* iStore, PCStr jsonFile) {
	return Bpnn::executeByJsonFile(*this, iStore, jsonFile);
}
DRP::DRP(Uint layer, Float _rate)
{
	startlayer = endLayer = layer;
	rate = _rate;
}

DRP::DRP(Uint _startlayer, Uint _endLayer, Float _rate)
{
	startlayer = min(_startlayer, _endLayer);
	endLayer = max(_startlayer, _endLayer);
	rate = _rate;
}

DRP::~DRP()
{
}

TransModel::TransModel() { reset(); }

void TransModel::reset()
{
	dimIndex = 0;//维度编号
	dimType = STT_Null;//维度的变换类型
	vmax = -VtFloatMax;//最小值
	vmin = VtFloatMax;//最小值
	vAver = 0;//均值
	vStandardDeviation = 1;//标准差
}
TransModel::~TransModel()
{
}
Float TransModel::forward(Float org) const {
	switch (dimType) {
	case STT_Normalize:  return (org - vmin)/(vmax - vmin) ;//数据做归一化到(0,1)
	case STT_NormalizeEx:  return (org - vmin) / (vmax - vmin) * 2 - 1;//数据做归一化到(-1,1)
	case STT_Standart: return (org - vAver) / vStandardDeviation; //数据做标准化到(0,1)标准分布
	default:
		return org;
	}
}

Float TransModel::backward(Float tag) const {
	switch (dimType) {
	case STT_Normalize:  return tag * (vmax - vmin) + vmin;//数据做归一化到(0,1)
	case STT_NormalizeEx:  return (tag + 1)/2* (vmax - vmin) + vmin ;//数据做归一化到(-1,1)
	case STT_Standart: return  tag *  vStandardDeviation + vAver; //数据做标准化到(0,1)标准分布
	default:
		return tag;
	}
}

Float TransModelDb::forward(Float org, Uint index) const {
	auto i = find(index);
	if (i == cend())
		throw std::out_of_range("TransModelDb forward para out of range!");
	return i->second.forward(org);
}

Float TransModelDb::backward(Float tag, Uint index) const {
	auto i = find(index);
	if (i == cend())
		throw std::out_of_range("TransModelDb forward para out of range!");
	return i->second.backward(tag);
}
void TransModelDb::forward(const VLF& org, VLF& tag, Uint alignStartIndex) const {
	tag.resize(org.size());
	for (size_t i = 0; i < org.size(); i++)
	{
		tag[i] = forward(org[i], alignStartIndex++);
	}
}
void TransModelDb::backward(const VLF& tag, VLF& org, Uint alignStartIndex) const {
	org.resize(tag.size());
	for (size_t i = 0; i < tag.size(); i++)
	{
		org[i] = backward(tag[i], alignStartIndex++);
	}
}

WbDef::WbDef()
{
	reset();
}
WbDef::WbDef(Uint _wiSize, const WiInitDef& _wi, const BiInitDef& _bi,Bool _bNotUpdate)
{
	wiSize = _wiSize; wi = _wi; bi = _bi; bNotUpdate = _bNotUpdate;
}
WbDef::~WbDef()
{
}
void BpnnStructDef::preCheck()
{
	//前向检查
	Uint thisLayindex = size() > 0 ? size() - 1 : 0;
	if (thisLayindex > 0) {
		Bool isBck = false;
		Bool isFord = false;
		CLString str2(("\r\n存在下列向前链接悬空或完全未链接的神经元："));

		auto& lay1 = at((Uint)thisLayindex - 1);
		auto& lay2 = at((Uint)thisLayindex);
		Int imin2 = -1, imax2 = -1;
		Int sj1 = getNeuronCountsInLayer((Uint)thisLayindex - 1);
		for (Uint j = 0, sj = getNeuralBlockCountsInLayer((Uint)thisLayindex);
			j < sj; j++)
		{
			auto& pnode = lay2[j];
			Uint k = 0, sk = pnode.linkPos.size();
			if (sk == 0) {
				if (imin2 < 0)
					imax2 = imin2 = Int(j);
				else if (Int(j) > imax2)
					imax2 = Int(j);
				isFord = true;
				continue;
			}
			for (; k < sk; k++)
			{
				Int clk = pnode.linkPos[k];
				if (clk > sj1) {
					if (imin2 < 0)
						imax2 = imin2 = Int(j);
					else if (Int(j) > imax2)
						imax2 = Int(j);
					isFord = true;
					break;
				}
			}
		}
		if (imin2 >= 0) {
			str2 << ("\r\n") << thisLayindex << ("层：[ ") << imin2 << (" - ") << imax2 << (" ]");
		}
		if (isFord) {
			str2.printf().messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
		}
	}
}

WbDef& BpnnStructDef::makeWbDefine(WbDef& def, Uint wiCounts, const WiInitDef& wi, const BiInitDef& bi, const Bool thisWbIsNotUpdate)
{
	def.reset();
	def.bi = bi;
	def.bi.resize(WbDef_MaxBi, 0);
	if (bi.size() == 0) {
		def.bi[0] = IT_Const;
	}
	if (wi.size() == 0) {
		def.wi.resize(WbDef_MinWi, 0);
		def.wi[0] = IT_Const;		
	}
	else if (wi.size() == 1) {
		def.wi = wi;
		def.wi.resize(WbDef_MinWi, 0);
	}
	else {
		def.wi = wi;
	}
	def.wiSize = wiCounts < 1 ? 1 : wiCounts;
	def.bNotUpdate = thisWbIsNotUpdate;
	return def;
}

BpnnStructDef::BpnnStructDef() {}

BpnnStructDef::BpnnStructDef(const BSInitList& _Ilist)
	:vector(_Ilist) {
	valid();
}

BpnnStructDef::~BpnnStructDef()
{
}

void BpnnStructDef::clear()
{
	vector<BSLayer>::clear();
	gWb.clear();
}

BpnnStructDef& BpnnStructDef::operator=(const BSInitList& _Ilist)
{
	vector<BSLayer>::operator=(_Ilist);
	return valid();
}

Bool tagvIsRepeat(VLF::iterator* _tag, VLF* _vec) {
	auto& vec = *_vec;
	auto& tag = *_tag;
	Bool isExit = false;
	auto i = vec.begin();
	for (; i < vec.end(); ++i)
	{
		if (i == tag) {
			isExit = true;
			break;
		}
	}
	if (!isExit) {
		return false;
	}
	for (auto c = vec.begin(); c != tag; ++c)
	{
		if (*c == *i) {
			return true;
		}
	}
	return false;
}

Bool BpnnStructDef::readFromFile(PCStr file) {
	auto rt = readBpnnStructDefFromFile(*this, file);
	valid();
	return rt;
}

BpnnStructDef& BpnnStructDef::writeToFile(PCStr file) {
	return writeBpnnStructDefToFile(*this, file);
}

Uint BpnnStructDef::getNewLength(Uint baseLen, Uint kernelLen, Uint padding, Uint stride)
{
	return (baseLen + 2 * padding - kernelLen) / stride + 1;
}

BpnnStructDef& BpnnStructDef::addOneConvolutionSeparable(Uint appendhidelayerIndex, Uint _mapBaseIndex,
	Uint& baseLayer_Xsi, Uint& baseLayer_Ysi, const ConvolutionKernel& ckMetrix, const WiInitDef& wi, const BiInitDef& bi,
	EBP_TF transFunc, EBP_WC wcFunc)
{
	Uint X_Stride = ckMetrix.size() > 4 ? ckMetrix[4] > 0 ? ckMetrix[4] : 1 : 1;
	Uint Y_Stride = ckMetrix.size() > 5 ? ckMetrix[5] > 0 ? ckMetrix[5] : 1 : 1;
	if (X_Stride >= baseLayer_Xsi || Y_Stride >= baseLayer_Ysi) {
		CLString(("错误：卷积核扫描步幅尺度已大于基层尺寸!")).messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
	}
	Uint cksi = ckMetrix.size();
	if (cksi < 3) {
		CLString(("错误：输入的卷积核,定义不够明确(缺少必要数据)!")).messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
	}
	Int cols = ckMetrix[1], rows = ckMetrix[2];
	if (cols < 1 || rows < 1 ) {
		CLString(("错误：输入的卷积核,filter宽度或高度无效!")).messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
	}
	Uint padding = ckMetrix.size() > 3 ? ckMetrix[3]: 0;
	if (Int(padding) * 2 + 1 > min(cols, rows)) {
		CLString(("错误：输入的边缘填充宽度,padding大于卷积核的半长或半宽!")).messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
	}
	Int inDepN = 1;
	Int outDepN = ckMetrix[0] < 1 ? 1 : ckMetrix[0];
	if (size() <= appendhidelayerIndex)
		resize((Uint)appendhidelayerIndex + 1);
	auto& lc = at(appendhidelayerIndex);
	Int cxi = getNewLength(baseLayer_Xsi, cols, padding, X_Stride);
	Int cyi = getNewLength(baseLayer_Ysi, rows, padding, Y_Stride);
	Int baSi = lc.size();
	lc.resize(lc.size() + (Uint)cxi * cyi * outDepN);//增加节点
	Int wbSi = gWb.size();
	if (outDepN > 0)
		gWb.resize((Uint)wbSi + outDepN);
	Int upSi = LONG_MAX;
	if (appendhidelayerIndex > 0) {
		upSi = getNeuronCountsInLayer(appendhidelayerIndex - 1);
	}
	Int jjd = -rows / 2, jju = (rows + 1) / 2;
	Int iid = -cols / 2, iiu = (cols + 1) / 2;
	RAND_SEED_EX();
	for (Int outDep = 0; outDep < outDepN; outDep++)
	{
		//构造共享权值结构
		Int wbi = -1;
		auto& pwb = gWb[wbi = wbSi + outDep];
		makeWbDefine(pwb, cols * rows * inDepN, wi, bi);

		Uint mapBaseIndex = _mapBaseIndex + outDep * baseLayer_Xsi * baseLayer_Ysi;

		//初始化神经元
		for (Int jc = 0; jc < cyi; jc++)
		{
			for (Int ic = 0; ic < cxi; ic++)
			{
				auto cci = baSi + jc * cxi + ic;
				auto& ub = lc[(Uint)cci];
				ub.reset();
				ub.nnCounts = 1;
				ub.transFuncType = transFunc;
				ub.wcFuncType = wcFunc;
				ub.linkPos.reserve((Uint)cols * rows * inDepN);
				ub.globleWbId = wbi;

				Int cyc = rows / 2 + (Int)jc * Y_Stride - padding;
				Int cxc = cols / 2 + (Int)ic * X_Stride - padding;

				for (Int jj = jjd; jj < jju; jj++)
				{
					for (Int ii = iid; ii < iiu; ii++)
					{
						if (
							(cyc + jj < 0 || cyc + jj >= (Int)baseLayer_Ysi) ||  // zero pad操作
							(cxc + ii < 0 || cxc + ii >= (Int)baseLayer_Xsi)
							)
						{
							ub.linkPos.push_back(-1); // zero pad操作
						}
						else {
							Int ci = mapBaseIndex + (cyc + jj) * baseLayer_Xsi + (cxc + ii) + 1;
							if (ci > upSi) {
								CLString().format(("\r\nBpnnStructDef Error: layer = %d, Neuron = %d,\r\nforward link out of range ( %d > Max[%d] ).\r\n"), appendhidelayerIndex, cci, ci, upSi)
									.printf().messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
							}
							ub.linkPos.push_back(ci);//扁平化的全局标签
						}
					}
				}
				if (ub.linkPos.size() == 0) {
					CLString().format(("\r\nBpnnStructDef Error: layer = %d, Neuron = %d,\r\nforward link size cann't be 0.\r\n"), appendhidelayerIndex, cci)
						.printf().messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
				}
			}
		}
		baSi += (cxi * cyi);
	}
	preCheck();
	baseLayer_Ysi = (Uint)cyi;
	baseLayer_Xsi = (Uint)cxi;
	return *this;
}

BpnnStructDef& BpnnStructDef::addOneConvolution(Uint appendhidelayerIndex, Uint _mapBaseIndex, Uint& baseLayer_Xsi, Uint& baseLayer_Ysi,
	const ConvolutionKernel& ckMetrix, const WiInitDef& wi, const BiInitDef& bi, EBP_TF transFunc, EBP_WC wcFunc)
{
	Uint X_Stride = ckMetrix.size() > 5 ? ckMetrix[5] > 0 ? ckMetrix[5] : 1 : 1;
	Uint Y_Stride = ckMetrix.size() > 6 ? ckMetrix[6] > 0 ? ckMetrix[6] : 1 : 1;
	if (X_Stride > baseLayer_Xsi || Y_Stride > baseLayer_Ysi) {
		CLString(("错误：卷积核扫描步幅尺度已大于基层尺寸!")).messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
	}
	Uint cksi = ckMetrix.size();
	if (cksi < 4) {
		CLString(("错误：输入的卷积核,定义不够明确,至少需要4个有效数据!")).messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
	}
	Int cols = ckMetrix[1], rows = ckMetrix[2];
	//不再限值卷积核为基数维度
	if (cols < 1 || rows < 1 ) {
		CLString(("错误：输入的卷积核,filter宽度或高度小于1!")).messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
	}
	Uint padding = ckMetrix.size() > 4 ? ckMetrix[4] : 0;
	if (Int(padding) * 2 + 1 > min(cols, rows)) {
		CLString(("错误：输入的边缘填充宽度,padding大于卷积核的半长或半宽!")).messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
	}
	Int outDepN = ckMetrix[3] < 1 ? 1 : ckMetrix[3];
	Int inDepN = ckMetrix[0] < 1 ? 1 : ckMetrix[0];
	if (size() <= appendhidelayerIndex)
		resize((Uint)appendhidelayerIndex + 1);
	auto& lc = at(appendhidelayerIndex);
	Int cxi = getNewLength(baseLayer_Xsi, cols, padding, X_Stride);
	Int cyi = getNewLength(baseLayer_Ysi, rows, padding, Y_Stride);
	Int baSi = lc.size();
	lc.resize(lc.size() + (Uint)cxi * cyi * outDepN);//增加节点
	Int wbSi = gWb.size();
	if (outDepN > 0)
		gWb.resize((Uint)wbSi + outDepN);
	Int upSi = LONG_MAX;
	if (appendhidelayerIndex > 0) {
		upSi = getNeuronCountsInLayer(appendhidelayerIndex - 1);
	}
	Int jjd = -rows / 2, jju = (rows + 1) / 2;
	Int iid = -cols / 2, iiu = (cols + 1) / 2;
	RAND_SEED_EX();
	for (Int outDep = 0; outDep < outDepN; outDep++)
	{
		//构造共享权值结构
		Int wbi = -1;
		auto& pwb = gWb[wbi = wbSi + outDep];
		makeWbDefine(pwb, cols * rows * inDepN, wi, bi);

		//初始化神经元
		for (Int jc = 0; jc < cyi; jc++)
		{
			for (Int ic = 0; ic < cxi; ic++)
			{
				auto cci = baSi + jc * cxi + ic;
				auto& ub = lc[(Uint)cci];
				ub.reset();
				ub.nnCounts = 1;
				ub.transFuncType = transFunc;
				ub.wcFuncType = wcFunc;
				ub.linkPos.reserve((Uint)cols * rows * inDepN);
				ub.globleWbId = wbi;

				Uint mapBaseIndex = _mapBaseIndex;
				Int cyc = rows / 2 + (Int)jc * Y_Stride - padding;
				Int cxc = cols / 2 + (Int)ic * X_Stride - padding;
				for (Int dep = 0; dep < inDepN; dep++) {//多通道叠加					
					for (Int jj = jjd; jj < jju; jj++)
					{
						for (Int ii = iid; ii < iiu; ii++)
						{
							if (
								(cyc + jj < 0 || cyc + jj >= (Int)baseLayer_Ysi) ||  // zero pad操作
								(cxc + ii < 0 || cxc + ii >= (Int)baseLayer_Xsi)
								)
							{
								ub.linkPos.push_back(-1); // zero pad操作
							}
							else {
								Int ci = mapBaseIndex + (cyc + jj) * baseLayer_Xsi + (cxc + ii) + 1;
								if (ci > upSi) {
									CLString().format(("\r\nBpnnStructDef Error: layer = %d, Neuron = %d,\r\nforward link out of range ( %d > Max[%d] ).\r\n"), appendhidelayerIndex, cci, ci, upSi)
										.printf().messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
								}
								ub.linkPos.push_back(ci);//扁平化的全局标签
							}
						}
					}
					mapBaseIndex += baseLayer_Xsi * baseLayer_Ysi;
				}
				if (ub.linkPos.size() == 0) {
					CLString().format(("\r\nBpnnStructDef Error: layer = %d, Neuron = %d,\r\nforward link size cann't be 0.\r\n"), appendhidelayerIndex, cci)
						.printf().messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
				}
			}
		}
		baSi += (cxi * cyi);
	}
	preCheck();
	baseLayer_Ysi = (Uint)cyi;
	baseLayer_Xsi = (Uint)cxi;
	return *this;
}

BpnnStructDef& BpnnStructDef::addOnePooling(Uint appendhidelayerIndex, Uint _mapBaseIndex,
	Uint& baseLayer_Xsi, Uint& baseLayer_Ysi, const PoolingMap& poolMap,
	EBP_WC wcFunc, EBP_TF transFunc)
{
	if (poolMap.size() < 3) {
		CLString(("错误：池化映射定义结构体的数据不够!...")).messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
	}
	Uint depIO = poolMap[0] < 1 ? 1 : poolMap[0];
	Uint cols = poolMap[1] < 1 ? 1 : poolMap[1];
	Uint rows = poolMap[2] < 1 ? 1 : poolMap[2];
	Uint poolingXstep = poolMap.size() >= 4 ? (poolMap[3] < 1 ? 1 : poolMap[3]) : cols;
	Uint poolingYstep = poolMap.size() >= 5 ? (poolMap[4] < 1 ? 1 : poolMap[4]) : rows;
	Uint X_Stride = (poolingXstep == 0 ? 1 : poolingXstep), Y_Stride = (poolingYstep == 0 ? 1 : poolingYstep);
	if (X_Stride > baseLayer_Xsi || Y_Stride > baseLayer_Ysi) {
		CLString(("错误：池化扫描步幅尺度已大于基层尺寸!...")).messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
	}
	if (size() <= appendhidelayerIndex)
		resize((Uint)appendhidelayerIndex + 1);
	auto& lc = at(appendhidelayerIndex);
	Uint cxi = getNewLength(baseLayer_Xsi, cols, 0, poolingXstep);
	Uint cyi = getNewLength(baseLayer_Ysi, rows, 0, poolingYstep);

	Uint baSi = lc.size();
	lc.resize(lc.size() + (Uint)cxi * cyi * depIO);//增加节点	
	Int wbi = -1;
	if (depIO > 0) {
		gWb.resize(gWb.size() + 1);
		//构造共享权值结构
		auto& pwb = gWb[wbi = (Int)gWb.size() - 1];
		makeWbDefine(pwb, cols * rows * 1, { IT_Const,1 }, { IT_Const,0 },true);
	}
	Int upSi = LONG_MAX;
	if (appendhidelayerIndex > 0) {
		upSi = getNeuronCountsInLayer(appendhidelayerIndex - 1);
	}
	Uint mapBaseIndex = _mapBaseIndex;
	for (Uint dep = 0; dep < depIO; dep++)
	{
		//初始化神经元
		for (Uint jc = 0; jc < cyi; jc++)
		{
			for (Uint ic = 0; ic < cxi; ic++)
			{
				auto cci = baSi + jc * cxi + ic;
				auto& ub = lc[cci];
				ub.reset();
				ub.nnCounts = 1;
				ub.transFuncType = transFunc;
				ub.wcFuncType = wcFunc;
				ub.linkPos.reserve((Uint)cols * rows);
				ub.bi.push_back(0);//偏移值初值为0
				//ub.updateFlag = (FG_WB_NotUpdate);//更新标志设为均不跟新
				ub.globleWbId = wbi;

				Uint cyc = (Uint)jc * Y_Stride;
				Uint cxc = (Uint)ic * X_Stride;
				for (Uint jj = 0; jj < rows; jj++)
				{
					for (Uint ii = 0; ii < cols; ii++)
					{
						if ((cyc + jj >= baseLayer_Ysi) || (cxc + ii >= baseLayer_Xsi))
							ub.linkPos.push_back(-1);
						else {
							Int ci = mapBaseIndex + (cyc + jj) * baseLayer_Xsi + (cxc + ii) + 1;
							if (ci > upSi) {
								CLString().format(("\r\nBpnnStructDef Error: layer = %d, Neuron = %d,\r\nforward link out of range ( %d > Max[%d] ).\r\n"), appendhidelayerIndex, cci, ci, upSi)
									.printf().messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
							}
							ub.linkPos.push_back(ci);//扁平化的全局标签
						}
					}
				}
				if (ub.linkPos.size() == 0) {
					CLString().format(("\r\nBpnnStructDef Error: layer = %d, Neuron = %d,\r\nforward link size cann't be 0.\r\n"), appendhidelayerIndex, cci)
						.printf().messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_logic_error();
				}
			}
		}
		mapBaseIndex += (baseLayer_Ysi * baseLayer_Xsi);//各通道全局节点号偏移一个通道深度
		baSi += (cxi * cyi);
	}
	preCheck();
	baseLayer_Ysi = (Uint)cyi;
	baseLayer_Xsi = (Uint)cxi;
	return *this;
}

BpnnStructDef& BpnnStructDef::addOneFullConnect(Uint appendhidelayerIndex, Uint _mapBaseIndex,
	Uint baseLayer_XYZsi, Uint thisTimesNodes, const WiInitDef& wi, const BiInitDef& bi,
	EBP_TF transFunc, EBP_WC wcFunc, Bool isNoUpdateFlag)
{
	if (size() <= appendhidelayerIndex)
		resize((Uint)appendhidelayerIndex + 1);
	auto& lc = at(appendhidelayerIndex);
	Int baSi = lc.size();
	lc.resize((Uint)baSi + 1);//增加节点
	Uint mapBaseIndex = _mapBaseIndex;

	auto& ub = lc[baSi];
	ub.reset();
	ub.nnCounts = appendhidelayerIndex < 1 ? 1 : thisTimesNodes == 0 ? at(appendhidelayerIndex - 1).size() : thisTimesNodes;
	ub.transFuncType = transFunc;
	ub.wcFuncType = wcFunc;
	ub.linkPos.resize((Uint)baseLayer_XYZsi);
	for (Uint i = 0, si = baseLayer_XYZsi; i < si; i++)
	{
		ub.linkPos[i] = mapBaseIndex + i + 1;
	}
	/*if (isNoUpdateFlag)
		ub.updateFlag = (FG_WB_NotUpdate);
	else
		ub.updateFlag = 0;*/
	WbDef wbd;
	makeWbDefine(wbd, baseLayer_XYZsi, wi, bi, isNoUpdateFlag);
	//CLBpExtend wbp;
	Float cbi;
	ub.linkWij.resize(wbd.wiSize);
	CLBpKernel::createWbByWbDef(ub.linkWij.data(), &cbi, wbd);
	ub.bi.clear(); ub.bi.push_back(cbi);
	preCheck();
	return *this;
}

BpnnStructDef& BpnnStructDef::addOneTransmit(Uint appendhidelayerIndex, Uint mapBaseIndex, Uint baseLayer_Xsi, Uint baseLayer_Ysi, Uint baseLayer_DepthSi, EBP_TF transFunc)
{
	return addOnePooling(appendhidelayerIndex, mapBaseIndex, baseLayer_Xsi, baseLayer_Ysi, { baseLayer_DepthSi,1,1 }, WC_Add, transFunc);
}

BpnnStructDef& BpnnStructDef::addOneConbinate(Uint appendhidelayerIndex, Uint mapBaseIndex, Uint baseLayer_SizeSpan, Uint baseLayer_SizeSpanTimes)
{
	if (size() <= appendhidelayerIndex)
		resize((Uint)appendhidelayerIndex + 1);
	auto& lc = at(appendhidelayerIndex);
	Uint SizeSpanTimes = baseLayer_SizeSpanTimes < 1? 1: baseLayer_SizeSpanTimes;
	Uint SizeSpan = baseLayer_SizeSpan < 1 ? 1 : baseLayer_SizeSpan;
	Uint baSi = lc.size();
	lc.resize(baSi + (Uint)SizeSpan);//增加节点
	Uint wbSi = gWb.size();
	gWb.resize((Uint)wbSi + 1);
	Uint upSi = LONG_MAX;
	if (appendhidelayerIndex > 0) {
		upSi = getNeuronCountsInLayer(appendhidelayerIndex - 1);
	}
	//构造共享权值Wb
	Int wbi = -1;
	auto& pwb = gWb[wbi = (Int)gWb.size() - 1];
	WiInitDef wid;
	wid.resize(SizeSpanTimes + 2, 1.0);
	wid[0] = IT_Const;
	makeWbDefine(pwb, SizeSpanTimes+1, wid, { IT_Const,0 },true);
	for (Uint i = 0; i < SizeSpan; i++)
	{
		auto& ub = lc[baSi + i];
		ub.reset();
		ub.nnCounts = 1;
		ub.transFuncType = TF_Purelin;
		//ub.linkPos = { (Int)i + 1,(Int)classifiers + 1 };
		ub.linkPos.resize(SizeSpanTimes + 1, 0);
		for (Uint j = 0; j < SizeSpanTimes + 1; j++)ub.linkPos[j] = mapBaseIndex + i + j * SizeSpan + 1;
		ub.wcFuncType = WC_Add;
		ub.globleWbId = wbi;
		//ub.updateFlag = (FG_WB_NotUpdate);
	}
	preCheck();
	return *this;
}

BpnnStructDef& BpnnStructDef::addSoftmaxClassifierToEnd(Uint mapBaseIndex, Uint baseLayer_XYZsi, Uint classifiers)
{
	addOneFullConnect(size(), mapBaseIndex, baseLayer_XYZsi, classifiers, { IT_Const,1 }, { IT_Const,0 }, TF_Exp, WC_Add, true);
	addOneTransmit(size(), 0, classifiers, 1, 1, TF_Ln);//增加一个传递层填充层，做loge变换
	addOneFullConnect(size() - 1, 0, classifiers, 1, { IT_Const, 1 }, { IT_Const, 0 }, TF_Ln,WC_Add, true);//增加一个求和单元，做loge变换

	//增加输出层
	resize(size() + 1);
	auto& lout = at(size() - 1);
	lout.resize(classifiers);
	//构造共享权值Wb
	Int wbSi = gWb.size();
	gWb.resize((Uint)wbSi + 1);
	Int wbi = -1;
	auto& pwb = gWb[wbi = (Int)gWb.size() - 1];
	makeWbDefine(pwb, 2, { IT_Const,1,-1 }, { IT_Const,0 },true);
	for (Uint i = 0; i < classifiers; i++)
	{
		auto& ub = lout[i];
		ub.reset();
		ub.nnCounts = 1;
		ub.transFuncType = TF_Exp;
		ub.linkPos = { (Int)i + 1,(Int)classifiers + 1 };
		ub.wcFuncType = WC_Add;
		ub.globleWbId = wbi;
		//ub.updateFlag = (FG_WB_NotUpdate);
	}
	preCheck();
	return *this;
}

typedef vector<FilterMapStruct> FilterMapLayer;
#define _out_error(thisLayer,thisSet,upLayer,upSet) \
	(CLString().format("\n神经网络构建脚本定义有误：\n脚本层%d,第%d描述组所链接到的%d层第%d描述组未找到！",(thisLayer)+1,(thisSet),(upLayer)+1,(upSet))\
	.printf().messageBoxRef(_lpDefineMsgBoxTitle,MB_ICONERROR).throw_invalid_argument())
BpnnStructDef& _addScript(BpnnStructDef& mod, const BpnnStructScript& script, const InputFilterMap& inputFilterMap,Bool logoutConstructInfo) {
	if (inputFilterMap.size() < 2)
		CLString().format("\n神经网络增加脚本方法的输入映射参数定义数据个数至少为2个，当前为%d个！", (Uint)inputFilterMap.size()).printf().messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
	Uint inIndex = inputFilterMap.size() > 0 ? inputFilterMap[0]: 0;
	Uint inDp = inputFilterMap.size() > 1 ? (inputFilterMap[1] > 0 ? inputFilterMap[1] : 1) : 1;
	Uint inWt = inputFilterMap.size() > 2 ? (inputFilterMap[2] > 0 ? inputFilterMap[2] : 1) : 1;
	Uint inHt = inputFilterMap.size() > 3 ? (inputFilterMap[3] > 0 ? inputFilterMap[3] : 1) : 1;
	Uint inSi = inWt * inHt * inDp;

	WiInitDef widef = { IT_Uniform,-1,1 }; //权值bi初始化参数
	BiInitDef bidef = { IT_Const,0 }; //阈值bi初始化参数
	Int layIndex = Int(mod.size()) - 1;
	FilterMapLayer upRang, thisRang;
	PoolingMap pmap;
	ConvolutionKernel cmap;
	if(logoutConstructInfo)printf("\nBpnnStructDef create by script: input= [ %d , %d x %d x %d ]",
		inputFilterMap[0], inputFilterMap[1], inputFilterMap.size() > 2 ? inputFilterMap[2] : 1, inputFilterMap.size() > 3 ? inputFilterMap[3] : 1);
	for (Int l = 0; l < (Int)script.size(); l++)
	{
		auto& lay = script[l];
		if (lay.size() == 0)
			CLString().format("\n神经网络构建脚本定义有误：\n%d层脚本层定义数据为空！", l + 1).printf().messageBoxRef(_lpDefineMsgBoxTitle, MB_ICONERROR).throw_invalid_argument();
		
		++layIndex;
		if (logoutConstructInfo)printf("\nLayer= %d: ", layIndex);
		Uint startI = 0, endI = 0;
		thisRang.clear();
		thisRang.resize(lay.size());
		for (Int s = 0; s < (Int)lay.size(); s++)
		{
			auto& set = lay[s];
			Uint setSi = 0;
			Uint wt = 1;
			Uint ht = 1;
			Uint deep = 1;
			PCStr lp = "";
			switch (set.type)
			{
			case SCP_Fc: {
				mod.addOneFullConnect(
					layIndex,
					l == 0 ? inIndex : set.upIndex < upRang.size() ? upRang[set.upIndex][0] : 0,
					l == 0 ? inSi : set.upIndex < upRang.size() ? upRang[set.upIndex][1] : 1,
					setSi = set.filterMap.size() > 0 ? (set.filterMap[0] > 0 ? set.filterMap[0] : 1) : 1,
					set.wi.size() > 0 ? set.wi : widef,
					set.bi.size() > 0 ? set.bi : bidef,
					EBP_TF(set.trFunc < 0 ? TF_Purelin : set.trFunc),
					EBP_WC(set.wcFunc < 0 ? WC_Add : set.wcFunc),
					set.flag < 0 ? false:(Bool)set.flag
				);
				deep = setSi;
				lp = "SCP_Fc";
			}break;
			case SCP_Trans: {
				wt = l == 0 ? inWt : set.upIndex < upRang.size() ? upRang[set.upIndex][3] : (_out_error(l, s, l - 1, set.upIndex), 1);
				ht = l == 0 ? inHt : set.upIndex < upRang.size() ? upRang[set.upIndex][4] : (_out_error(l, s, l - 1, set.upIndex), 1);
				deep = l == 0 ? inDp : set.upIndex < upRang.size() ? upRang[set.upIndex][5] : (_out_error(l, s, l - 1, set.upIndex), 1);
				mod.addOneTransmit(
					layIndex,
					l == 0 ? inIndex : set.upIndex < upRang.size() ? upRang[set.upIndex][0] : 0,
					wt ,
					ht ,
					deep ,
					EBP_TF(set.trFunc < 0 ? TF_Purelin : set.trFunc)
				);
				setSi = wt * ht * deep;
				lp = "SCP_Trans";
			}break;
			case SCP_Pool: {
				wt = l == 0 ? inWt : set.upIndex < upRang.size() ? upRang[set.upIndex][3] : (_out_error(l, s, l - 1, set.upIndex), 1);
				ht = l == 0 ? inHt : set.upIndex < upRang.size() ? upRang[set.upIndex][4] : (_out_error(l, s, l - 1, set.upIndex), 1);
				deep = l == 0 ? inDp : set.upIndex < upRang.size() ? upRang[set.upIndex][5] : (_out_error(l, s, l - 1, set.upIndex), 1);
				Uint kwt = set.filterMap.size() > 0 ? (set.filterMap[0] > 0 ? set.filterMap[0] : 1) : 1;
				Uint kht = set.filterMap.size() > 1 ? (set.filterMap[1] > 0 ? set.filterMap[1] : 1) : 1;
				pmap = {
					deep,
					kwt,
					kht,
					set.filterMap.size() > 2 ? (set.filterMap[2] > 0 ? set.filterMap[2] : 1) : kwt,
					set.filterMap.size() > 3 ? (set.filterMap[3] > 0 ? set.filterMap[3] : 1) : kht,
				};
				mod.addOnePooling(
					layIndex,
					l == 0 ? inIndex : set.upIndex < upRang.size() ? upRang[set.upIndex][0] : 0,
					wt,
					ht,
					pmap,
					EBP_WC(set.wcFunc < 0 ? WC_Add : set.wcFunc),
					EBP_TF(set.trFunc < 0 ? TF_Purelin : set.trFunc)
				);
				setSi = wt * ht * deep;
				lp = "SCP_Pool";
			}break;
			case SCP_Conv: {
				wt = l == 0 ? inWt : set.upIndex < upRang.size() ? upRang[set.upIndex][3] : (_out_error(l, s, l - 1, set.upIndex), 1);
				ht = l == 0 ? inHt : set.upIndex < upRang.size() ? upRang[set.upIndex][4] : (_out_error(l, s, l - 1, set.upIndex), 1);
				cmap = {
					l == 0 ? inDp : set.upIndex < upRang.size() ? upRang[set.upIndex][5] : (_out_error(l,s,l - 1, set.upIndex), 1),
					set.filterMap.size() > 0 ? (set.filterMap[0] > 0 ? set.filterMap[0] : 1) : 1,
					set.filterMap.size() > 1 ? (set.filterMap[1] > 0 ? set.filterMap[1] : 1) : 1,
					deep = set.filterMap.size() > 2 ? (set.filterMap[2] > 0 ? set.filterMap[2] : 1) : 1,
					set.filterMap.size() > 3 ? (set.filterMap[3]) : 0,
					set.filterMap.size() > 4 ? (set.filterMap[4] > 0 ? set.filterMap[4] : 1) : 1,
					set.filterMap.size() > 5 ? (set.filterMap[5] > 0 ? set.filterMap[5] : 1) : 1,
				};
				mod.addOneConvolution(
					layIndex,
					l == 0 ? inIndex : set.upIndex < upRang.size() ? upRang[set.upIndex][0] : 0,
					wt,
					ht,
					cmap,
					set.wi.size() > 0 ? set.wi : widef,
					set.bi.size() > 0 ? set.bi : bidef,					
					EBP_TF(set.trFunc < 0 ? TF_Tanh : set.trFunc),
					EBP_WC(set.wcFunc < 0 ? WC_Convolution : set.wcFunc)
				);
				setSi = wt * ht * deep;
				lp = "SCP_Conv";
			}break;
			case SCP_ConvSep: {
				wt = l == 0 ? inWt : set.upIndex < upRang.size() ? upRang[set.upIndex][3] : (_out_error(l, s, l - 1, set.upIndex), 1);
				ht = l == 0 ? inHt : set.upIndex < upRang.size() ? upRang[set.upIndex][4] : (_out_error(l, s, l - 1, set.upIndex), 1);
				cmap = {
					deep = l == 0 ? inDp : set.upIndex < upRang.size() ? upRang[set.upIndex][5] : (_out_error(l,s,l - 1, set.upIndex), 1),
					set.filterMap.size() > 0 ? (set.filterMap[0] > 0 ? set.filterMap[0] : 1) : 1,
					set.filterMap.size() > 1 ? (set.filterMap[1] > 0 ? set.filterMap[1] : 1) : 1,
					set.filterMap.size() > 3 ? (set.filterMap[3]) : 0,
					set.filterMap.size() > 4 ? (set.filterMap[4] > 0 ? set.filterMap[4] : 1) : 1,
					set.filterMap.size() > 5 ? (set.filterMap[5] > 0 ? set.filterMap[5] : 1) : 1,
				};
				mod.addOneConvolutionSeparable(
					layIndex,
					l == 0 ? inIndex : set.upIndex < upRang.size() ? upRang[set.upIndex][0] : 0,
					wt,
					ht,
					cmap,
					set.wi.size() > 0 ? set.wi : widef,
					set.bi.size() > 0 ? set.bi : bidef,
					EBP_TF(set.trFunc < 0 ? TF_Tanh : set.trFunc),
					EBP_WC(set.wcFunc < 0 ? WC_Convolution : set.wcFunc)
				);
				setSi = wt * ht * deep;
				lp = "SCP_ConvSep";
			}break;
			case SCP_Combin: {
				wt = l == 0 ? inWt : set.upIndex < upRang.size() ? upRang[set.upIndex][3] : (_out_error(l, s, l - 1, set.upIndex), 1);
				ht = l == 0 ? inHt : set.upIndex < upRang.size() ? upRang[set.upIndex][4] : (_out_error(l, s, l - 1, set.upIndex), 1);
				deep = l == 0 ? inDp : set.upIndex < upRang.size() ? upRang[set.upIndex][5] : (_out_error(l, s, l - 1, set.upIndex), 1);
				mod.addOneConbinate(
					layIndex,
					l == 0 ? inIndex : set.upIndex < upRang.size() ? upRang[set.upIndex][0] : 0,
					setSi = l == 0 ? inSi  : set.upIndex < upRang.size() ? upRang[set.upIndex][1] : (_out_error(l, s, l - 1, set.upIndex), 1),
					set.filterMap.size() > 0 ? (set.filterMap[0] > 0 ? set.filterMap[0] : 1) : 1
				);
				lp = "SCP_Combin";
			}break;
			case SCP_Softmax: {
				mod.addSoftmaxClassifierToEnd(
					0,
					deep = l == 0 ? inSi : mod.getNeuronCountsInLayer(mod.size() - 1),
					setSi = set.filterMap.size() > 0 ? (set.filterMap[0]) : 1
				);
				lp = "SCP_Softmax";
			}break;
			default:
				break;
			}
			if (logoutConstructInfo)printf("\n       -> set= %d: upLink= %d, range= [ %d - %d ], size= (%d), map= ( %d x %d x %d ), %s",s, set.upIndex, startI, startI + setSi - 1, setSi, wt, ht, deep,lp);
			thisRang[s] = { startI ,setSi , startI + setSi - 1, wt,ht,deep };
			startI += setSi;
		}
		upRang = thisRang;
	}
	if (logoutConstructInfo)printf("\nScript end.\n");
	return mod;
}

BpnnStructDef& BpnnStructDef::addScript(const BpnnStructScript& script, const InputFilterMap& inputFilterMap,Bool logoutConstructInfo)
{
	return _addScript(*this, script, inputFilterMap, logoutConstructInfo);
}

Uint BpnnStructDef::getNeuronCountsInLayer(Uint layId)
{
	Uint ss = 0;
	for (Uint i = 0, si = getNeuralBlockCountsInLayer(layId); i < si; i++)
	{
		ss += at(layId).at(i).nnCounts;
	}
	return ss;
}

Uint BpnnStructDef::getNeuralBlockCountsInLayer(Uint layId)
{
	return at(layId).size();
}

BpnnStructDef& BpnnStructDef::valid()
{
	//层检查
	for (auto i = begin(); i != end(); )
	{
		if ((*i).size() == 0) {
			i = erase(i);
		}
		else ++i;
	}
	//节点检查
	for (auto i = begin(), i2 = i + 1; i2 != end(); ++i, ++i2)
	{
		Int allNodes = 0;
		//算总结点
		for (auto j = i->begin(); j != i->end(); ++j) {
			allNodes += j->nnCounts;
		}
		for (auto j = i2->begin(); j != i2->end(); ++j) {
			for (auto k = j->linkPos.begin(); k != j->linkPos.end(); )
			{
				if (*k == 0) {//移除0
					k = j->linkPos.erase(k);
				}
				else if (*k > allNodes) { //大于上层总数
					k = j->linkPos.erase(k);
				}
				else if (*k > 0 && tagvIsRepeat((VLF::iterator*) & k, (VLF*) & (j->linkPos))) {//值对象在前面已经出现过了重复了
					k = j->linkPos.erase(k);
				}
				else ++k;
			}
		}
	}
	return *this;
}

Uint BpnnStructDef::neuronCounts() const
{
	Uint allInCreate = 0;
	for (Uint i = 0, si = size(); i < si; i++)
	{
		auto& pc = at(i);
		for (Uint j = 0, sj = pc.size(); j < sj; j++) {
			allInCreate += pc[j].nnCounts;
		}
	}
	return allInCreate;
}

Uint BpnnStructDef::getNeuralBlockCounts() const
{
	Uint allInCreate = 0;
	for (Uint i = 0, si = size(); i < si; i++)
	{
		allInCreate += at(i).size();
	}
	return allInCreate;
}

Uint BpnnStructDef::layerCounts() const
{
	return vector<BSLayer>::size();
}

BpnnSamPair::BpnnSamPair() {
	piv = pov = 0;
	ivDim = tvDim = 0;
}

BpnnSamPair::BpnnSamPair(Float* _iv, Uint _ivDim, Float* _ov, Uint _ovDim)
{
	piv = _iv, pov = _ov;
	ivDim = _ivDim, tvDim = _ovDim;
}

BpnnSamPair::~BpnnSamPair()
{
}

Uint BpnnSamPair::intputDimension() const
{
	return ivDim;
}
Uint BpnnSamPair::targetDimension() const
{
	return tvDim;
}

void BpnnSamPair::clear()
{
	if (iv() && ivDim)
		ZeroMemory(iv(), sizeof(Float) * ivDim);
	if (tv() && tvDim)
		ZeroMemory(tv(), sizeof(Float) * tvDim);
}

Float& BpnnSamPair::operator[](Uint i)
{
	if (i < ivDim)
		return iv()[i];
	else if (i < ivDim + tvDim)
		return tv()[i - ivDim];
	throw runtime_error("out of rang");
}

VLF BpnnSamPair::inputVec() const
{
	if (iv() && intputDimension()) {
		VLF a(intputDimension());
		memcpy_s(&a[0], intputDimension() * sizeof(Float), iv(), intputDimension() * sizeof(Float));
		return a;
	}
	else return VLF();
}
VLF BpnnSamPair::targetVec() const
{
	if (tv() && targetDimension()) {
		VLF a(targetDimension());
		memcpy_s(&a[0], targetDimension() * sizeof(Float), tv(), targetDimension() * sizeof(Float));
		return a;
	}
	else return VLF();
}

Float* BpnnSamPair::iv() const
{
	return piv;
}
Float* BpnnSamPair::tv() const
{
	return pov;
}

BSB::BSB()
	:nnCounts(1), transFuncType((Byte)EBP_TF::TF_Sigmoid), wcFuncType((Byte)EBP_WC::WC_Add), /*updateFlag(0), */globleWbId(-1) {}

BSB::BSB(Uint _nnCounts, EBP_TF _transFunc)
	: nnCounts(_nnCounts), transFuncType((Byte)_transFunc), wcFuncType((Byte)EBP_WC::WC_Add), /*updateFlag(0), */globleWbId(-1) {}

BSB::BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos)
	: nnCounts(_nnCounts), transFuncType((Byte)_transFunc), linkPos(_linkForwordNodePos), wcFuncType((Byte)EBP_WC::WC_Add), /*updateFlag(0), */globleWbId(-1) {}

BSB::BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos, EBP_WC _wcType)
	: nnCounts(_nnCounts), transFuncType((Byte)_transFunc), linkPos(_linkForwordNodePos), wcFuncType((Byte)_wcType), /*updateFlag(0), */globleWbId(-1) {}

BSB::BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos, EBP_WC _wcType, const LinkWij& _linkForwordNodeWij)
	: nnCounts(_nnCounts), transFuncType((Byte)_transFunc), linkPos(_linkForwordNodePos), wcFuncType((Byte)_wcType), linkWij(_linkForwordNodeWij), /*updateFlag(0), */globleWbId(-1) {}

BSB::BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos, EBP_WC _wcType, const LinkWij& _linkForwordNodeWij, const LinkBi& _bi)
	: nnCounts(_nnCounts), transFuncType((Byte)_transFunc), linkPos(_linkForwordNodePos), wcFuncType((Byte)_wcType), linkWij(_linkForwordNodeWij), bi(_bi), /*updateFlag(0), */globleWbId(-1) {}

BSB::~BSB() {}

void BSB::reset()
{
	nnCounts = (1), transFuncType = ((Byte)EBP_TF::TF_Sigmoid), wcFuncType = ((Byte)EBP_WC::WC_Add), linkPos.clear(), linkWij.clear(), bi.clear(), /*updateFlag(0), */globleWbId = (-1);
}

IBpnnBase::~IBpnnBase()
{
}

BpnnStructScript::BpnnStructScript()
{
}

BpnnStructScript::BpnnStructScript(const initializer_list<BpnnStructScriptLayer>& list)
	:std::vector<BpnnStructScriptLayer>(list)
{
}

BpnnStructScript::~BpnnStructScript()
{
}

BpnnInterfaceStore::BpnnInterfaceStore()
{
}

void BpnnInterfaceStore::release()
{
	for (auto& i : *this)
		delete i.second, i.second = nullptr;
	clear();
}

BpnnInterfaceStore::~BpnnInterfaceStore()
{
	release();
}
