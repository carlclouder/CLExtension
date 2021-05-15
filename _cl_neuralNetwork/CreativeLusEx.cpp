
#include "CreativeLusEx.h"

#define make_link(nn,lkSize) \
	((nn).link = (vm_link_Data).size(),\
	(vm_link_Data).resize((nn).link + (lkSize),0),\
	(&(vm_link_Data)[(nn).link]))
#define make_linkBk(nn,lkSize,plkBk,plkBkw) \
	((nn).linkBk = vm_linkBk_Data.size(),\
	vm_linkBk_Data.resize((nn).linkBk + (lkSize),0),\
	(nn).linkBkSi = (lkSize),(plkBk) = (&vm_linkBk_Data[(nn).linkBk]),\
	vm_linkBkw_Data.resize((nn).linkBk + (lkSize),0),\
	(plkBkw) = (&vm_linkBkw_Data[(nn).linkBk]))

#define pbkLayNn0( nn ) (&(vm_neuron)[(vm_layInfo)[(nn).id_lay + 1].iLayNnStartIndex])
#define pFwLayNn0( nn ) (&(vm_neuron)[(vm_layInfo)[(nn).id_lay - 1].iLayNnStartIndex])
#define link_size( nn ) (((vm_wbpack)[(nn).wb]).wjiSi)
#define linki_size( index ) (((vm_wbpack)[Nn(index).wb]).wjiSi)
#define plink( nn ) (&(vm_link_Data)[(nn).link])
#define plinki( index ) (&(vm_link_Data)[Nn(index).link])
#define linkBk_size( nn ) ((nn).linkBkSi)
#define linkBkW_size( nn ) linkBk_size( nn )
#define plinkBk( nn ) (&vm_linkBk_Data[(nn).linkBk])
#define plinkBkW( nn )  (&vm_linkBkw_Data[(nn).linkBk])

#define pWji(nn) (&(vm_wji_Data)[(vm_wbpack)[(nn).wb].wji_Index+1])
#define pWjii(index) (&(vm_wji_Data)[(vm_wbpack)[Nn(index).wb].wji_Index+1])
#define pWji_dt(nn) (&vm_wji_dt_Data[(vm_wbpack)[(nn).wb].wji_Index+1])
#define pWji_dt_old(nn) (&vm_wji_dt_old_Data[(vm_wbpack)[(nn).wb].wji_Index+1])
#define wji_size(nn) (vm_wbpack[(nn).wb].wjiSi)
#define wji_dt_size(nn) wji_size(nn)
#define wji_dt_old_size(nn) wji_size(nn)

#define id_index(nn) ((nn).id_index)
#define bpnn_id_index(nn) id_index(nn)

#define Nn(index) (vm_neuron[(index)])
#define bpnn_Nn(index) (bpnn->vm_neuron[(index)])
#define pGrad(nn) (&vm_grad[id_index(nn) * vm_grad_span])
#define pGradi(index) (&vm_grad[(index) * vm_grad_span])
#define bpnn_pGrad(bpnn,nn) (&(bpnn)->vm_grad[bpnn_id_index(nn) * ((bpnn)->vm_grad_span)])
#define pYi(nn) (&vm_yi[ id_index(nn) * (vm_xy_span)])
#define pYii(index) (&vm_yi[ (index) * vm_xy_span])
#define pYi2(nn) (&vm_yi_predict[ id_index(nn) * (vm_yi_span_predict)])
#define pYi2i(index) (&vm_yi_predict[ (index) * (vm_yi_span_predict)])
#define bpnn_pYi(nn) (&pyi[ bpnn_id_index(nn) * xy_span])
#define bpnn_pYii(index) (&pyi[ (index) * xy_span])
#define yi0(nn) ((vm_yi0)[id_index(nn)])
#define yi0i(index) ((vm_yi0)[(index)])
#define pYi0i(index) (&vm_yi0[(index)])
#define bpnn_yi0(bpnn,nn) ((bpnn)->(vm_yi0)[bpnn_id_index(nn)])
#define pXi(nn) (&vm_xi[ id_index(nn) * (vm_xy_span)])
#define pXii(index) (&vm_xi[(index) * vm_xy_span])
#define bpnn_pXi(nn) (pxi?&pxi[ bpnn_id_index(nn) * xy_span]:nullptr)
#define bpnn_pXii(index) (pxi?&pxi[ (index) * xy_span]:nullptr)
#define xi0(nn) ((vm_xi0)[id_index(nn)])
#define bpnn_xi0(bpnn,nn) ((bpnn)->(vm_xi0)[bpnn_id_index(nn)])

#define Wb(nn) ((vm_wbpack)[(nn).wb])
#define Wbi(index) ((vm_wbpack)[Nn(index).wb])


#define make_wb_Wji_bi(wb,wiSize) \
	((wb).wji_Index = (vm_wji_Data).size(),\
	(vm_wji_Data).resize((wb).wji_Index + (wiSize) + 1),\
	((wb).wjiSi = (wiSize)),(&(vm_wji_Data)[(wb).wji_Index + 1]))
#define wb_pWji(wb)  (&(vm_wji_Data)[(wb).wji_Index + 1])
#define wb_Wji_size(wb)  ((wb).wjiSi)
#define wb_pWji_dt(wb)  (&vm_wji_dt_Data[(wb).wji_Index + 1])
#define wb_Wji_dt_size(wb)  wb_Wji_size(wb)
#define wb_pWji_dt_old(wb)  (&vm_wji_dt_old_Data[(wb).wji_Index + 1])
#define wb_Wji_dt_old_size(wb)  wb_Wji_size(wb)
#define wb_bi(wb) (vm_wji_Data[(wb).wji_Index])
#define wb_bi_dt(wb) (vm_bi_dt_Data[(wb).index])
#define wb_bi_dt_old(wb) (vm_bi_dt_old_Data[(wb).index])

//wij输出文件的文件头字符串
static PCStr const _lpBpNnWijFileFlag = "BPNN out put data file! Please,do not modify! Designed by Cailuo.";
static PCStr const _lpBpNnEncryptedHear = "BPNN out put data file is encrypted! Please,do not modify! Designed by Cailuo.\r\n\r\n";
static PCStr const _lpBpNnModeFileFlag = "BPNN out put hide layers define file! Please,do not modify! Designed by Cailuo.";
static PCStr const _lpBpnnMsgBoxTitle = "Neural network alert";

//CLString tem;
static const BpnnToolInfo toolInfo = {
	{"Version               ","1.0.5"},
	{"Author                ","Cailuo"},
	{"Company               ","Cailuo"},
	{"Country               ","China"},
	{"Region                ","Shenzhen"},

	{"OriginalFilename      ",
#ifdef _WIN64
#ifdef NDEBUG
	"CreativeLus_x64"
#else
	"CreativeLus_x64d"
#endif
#else
#ifdef NDEBUG
	"CreativeLus_x86"
#else
	"CreativeLus_x86d"
#endif
	#endif
	},
	{"OriginalExtensionName ","dll"},
	{"System                ","Windows"},
	{"SystemMinimumSupport  ","Windows Nt"},
	{"Platform              ",
#ifdef _WIN64
	"x64"
#else
	"x86"
#endif
	},
	{"CharacterSet          ","gbk"},
	{"CharacterWidth        ","MuiltChar/Ascii"},
	{"Language              ","Chinese"},
	{"RuntimeLib            ",
#ifdef NDEBUG
	"MT"
#else
	"MTd"
#endif
	},
	{"CompileDate           ",__DATE__},
	{"CompileTime           ",__TIME__},
	{"CompileEnvironment    ","Microsoft Visual Studio 2019 (v142)"},
	{"CompileLanguage       ","C++17"},
	{"SupportMultiThreading ","true"},
	{"SupportGpuAcceleration",
#if UseCppAmp > 0
	"true"
#else
	"false"
#endif
	},
	{"IsDoublePrecision     ",(sizeof(Float) == 4 ? "false" : "true")},
	{"Level                 ","0"},
	{"Debug                 ",
#ifdef NDEBUG
	"Release"
#else
	"Debug"
#endif
	},

};

CLBpExtend& CLBpExtend::reset()
{
	//kernel.reset();
	setTrainState();
	pNetFront = 0, pNetBack = 0;
	bOpenGraph = false;
	hidePerLayerNumbers = BPNN_DFT_PERLAYERROOTNUM;
	hideLayerNumbers = BPNN_DFT_LAYERNUM;
	g_ls_old = g_ls = BPNN_DFT_LEARNSTEP;
	g_mc_old = g_mc = BPNN_DFT_MOMENTUM;
	g_accuracy = BPNN_DFT_ER;
	bIsCheckLinkBk = false;
	bIsCheckKeepGoWhenNanInf = true;
	m_KeepGoWhenNanInfTimes = 0;
	m_runTimesFromBuild = 0;
	//bAutoFit = 0;
	bSetParam = 0;
	pFAutoFit = nullptr;
	pAutoFitOperateIns = nullptr;
	g_Er = g_Er_old = g_ls = g_mc = g_ls_old = g_mc_old
		= g_accuracy = g_DEr = g_DEr_old
		= A = B = 0;
	hideLayerNumbers = hidePerLayerNumbers = maxTimes = runTimes
		= g_baseLow = g_infiSmalls = 0;
	g_CorrectRate = g_CorrectRate_old = 0;
	setAutoFitLsAndMc();
	bSetParam = false;	
	setTransFunc();
	A = -1;
	B = 1;
	vm_samSets = 0;
	setUseRandSample();
	setMultiThreadSupport();
	setGpuAcceleratedSupport();
	bShowNetAlert = true;
	bSuspendAlert = true;
	setSampleBatchCounts();
	setNetUseTo();
	setCorrectRateEvaluationModel();
	setDropout();
	setAdam();
	clearAllDataContainer();
	releaseTrainDataContainer();
	return *this;
}

Uint CLBpExtend::getSampleCounts() const { return vm_samSets ? vm_samSets->size() : 0; }

Uint CLBpExtend::getRunTimes() const { return runTimes; }

Uint CLBpExtend::getRunTimesTutal() const
{
	return m_runTimesFromBuild;
}

Float CLBpExtend::getEr() const { return g_Er; }

Float CLBpExtend::getLs() const { return g_ls; }

Float CLBpExtend::getMc() const { return g_mc; }

Float CLBpExtend::getDEr() const { return g_DEr; }

Float CLBpExtend::getAccuracy() const { return g_accuracy; }

Uint CLBpExtend::getMaxTimes() const { return maxTimes; }

Float CLBpExtend::getSavedCorrectRate() const
{
	return g_CorrectRate;
}

PCStr CLBpKernel::getName() const { return bpnnName.c_str(); }

void* CLBpKernel::showWbDataDistribution(Int x, Int y, Uint cx, Uint cy, Uint sectionCounts) const
{
	if (vm_wji_Data.size() == 0)
		return nullptr;
	std::map<double, double> data;
	double vmin, vmax;
	//std::map<double, double>::const_iterator i, i2;
	//ag:
	dataToDistribution(data, vm_wji_Data, sectionCounts,&vmin,&vmax);
	//i2 = data.cbegin(); ++i2;
	/*for (i = data.cbegin(); i!= data.cend() && i2!=data.cend();++i,++i2)
	{
		if ((i->second < i2->second * 0.6) || (i->second * 0.6 > i2->second)) {
			sectionCounts *= 2; 
			goto ag;
		}
	}*/
	auto pt = new CLShowTool;
	auto bkrec = CLShowTool::setDefaultSimpleLineRect({ 0,0,long(cx),long(cy) });
	/*pt->writeSimpleDataDistribution(vm_wji_Data, sectionCounts,
		CLString().format("%s Wb Data Distribution < size= %u, sec= %u >",
			getName(), vm_wji_Data.size(), max(1,sectionCounts)).string(),
		x,y,false,CLGOLD,CLYELLOW,3);*/
	if (sectionCounts != 0)
		pt->writeSimpleVerLine2D(data, CLString().format("%s Wb Data Distribution < size= %u, sec= %u/%u >",
			getName(), vm_wji_Data.size(), data.size(), sectionCounts).string(),
			x, y, vmin, vmax, false, CLGOLD, CLYELLOW, 3);
	else
		pt->writeSimpleVerLine2D(data, CLString().format("%s Wb Data Distribution < size= %u, sec= %u（自适应）>",
			getName(), vm_wji_Data.size(), data.size()).string(),
			x, y, vmin, vmax, false, CLGOLD, CLYELLOW, 3);
	CLShowTool::setDefaultSimpleLineRect(bkrec);
	CLShowTool::createAndShowInNewThread(0, pt, true);
	//auto pt = st.copyToShowInNewThread();
	//auto pt = st;
	if (pt) {
		return (void*)(HWND(*pt));
	}
	else
		return nullptr;
}

void CLBpKernel::exportWbDataDistribution(VLF& retRange, VLF& retFreq, Uint sectionCounts, Float* vmin, Float* vmax) const
{
	if (vm_wji_Data.size() == 0) {
		retRange.clear(), retFreq.clear(); 
		if (vmin)*vmin = 0; 
		if (vmax)*vmax = 0;
		return;
	}
	dataToDistribution(retRange, retFreq, vm_wji_Data, sectionCounts, vmin, vmax);
}

CLBpExtend& CLBpExtend::setMaxTimes(Uint iMaxTimes) { 
	maxTimes = (iMaxTimes == 0 ? (vm_samSets ? vm_samSets->size() : 1) : iMaxTimes);
	return *this; 
}

CLBpExtend& CLBpExtend::setAutoFitLsAndMc(/*Bool bOpen, */Bpnn::PCBAutoFitLsAndMc _pFAutoFit,PVoid pIns) { 
	//bAutoFit = bOpen; 
	pFAutoFit = _pFAutoFit;
	pAutoFitOperateIns = pFAutoFit ? pIns : nullptr;
	return *this; 
}

CLBpExtend& CLBpExtend::setWbiDefault(Float W) { A = W; B = W; return *this; }

CLBpExtend& CLBpExtend::setWbiDefault(Float _A, Float _B) { A = _A; B = _B; return *this; }

CLBpExtend& CLBpExtend::setUseRandSample(Bool _isUse) { train_useRandom = _isUse; return *this; }

CLBpExtend& CLBpExtend::autoFitParam()
{
	//if (!bAutoFit)return *this;
	if (pFAutoFit) {
		Float _ls = getLs(), _mc = getMc();
		if (pAutoFitOperateIns) {
			Bpnn::PMCBAutoFitLsAndMc pcbm = *(Bpnn::PMCBAutoFitLsAndMc*)&pFAutoFit;
			(((Bpnn*)pAutoFitOperateIns)->*pcbm)(_ls, _mc, bp);
		}
		else {
			pFAutoFit(_ls, _mc, bp);
		}
		if (_ls > 0) g_ls = _ls;
		if (_mc >= 0 && _mc < 1)g_mc = _mc;
	}
	return *this;
	//以下代码废弃---------------------------------------
	Bool isChange = true;
	if (g_Er_old <= g_Er) {//涨
		g_baseLow = 0;
		if (g_accuracy < Abs(g_DEr) - Abs(g_DEr_old)) {
			g_ls = 0.99 * g_ls;
			if (g_ls < 0.01 * g_accuracy)
				g_ls = 0.01 * g_accuracy;
			g_mc -= (BPNN_DFT_MOMENTUM / 20);
			if (g_mc < 0)
				g_mc = 0;
		}
		else
			isChange = false;
	}
	else {//跌
		if (Abs(g_DEr) < Abs(g_DEr_old)) {
			g_ls = min(1.01 * g_ls, g_ls + Abs(g_Er - g_Er_old));
			g_baseLow++;
			if (g_baseLow < (*vm_samSets).size() * 2) {
				Float maxlg = min(3 * g_ls_old, g_Er / 100);
				if (g_ls > maxlg)
					g_ls = maxlg;
				g_mc += (BPNN_DFT_MOMENTUM / 20);
				if (g_mc > BPNN_DFT_MOMENTUM)
					g_mc = BPNN_DFT_MOMENTUM;
			}
			else {
				g_mc -= (BPNN_DFT_MOMENTUM / 20);
				if (g_mc < 0)
					g_mc = 0;
			}

			//陷入极小值后处理
			if ((Abs(g_DEr) < 0.000001) && (Abs(g_DEr) / Abs(g_Er) < 0.00000001)) {
				g_infiSmalls++;
				if (g_infiSmalls > (*vm_samSets).size()) {
					g_infiSmalls = 0;
					g_mc = BPNN_DFT_MOMENTUM;
					for (Uint i = 0, si = kernel.layerCounts(); i < si; ++i) {
						auto lay = kernel.LayStartNn(i);
						auto laySi = kernel.LayNnCounts(i);
						for (Uint j = 0, sj = laySi; j < sj; ++j) {
							auto& nn = lay[j];
							auto link = plink(nn);
							auto Wji = pWji(nn);
							auto Wji_dt_old = pWji_dt_old(nn);
							for (Uint k = 0, sk = link_size(nn); k < sk; k++)
							{
								Int upi = link[k];
								if (upi < 0)continue;
								auto v = Wji[upi] * g_ls * RAND_F_A_B(0, 3);
								if (Abs(v) > abs(Wji_dt_old[upi]))
									Wji_dt_old[upi] = v;
							}
							auto v = wb_bi(Wb(nn)) * g_ls * RAND_F_A_B(0, 3);
							if (Abs(v) > abs(wb_bi_dt_old(Wb(nn))))
								wb_bi_dt_old(Wb(nn)) = v;
						}
					}
				}
			}
			else g_infiSmalls = 0;
		}
		else if (g_accuracy < Abs(g_DEr) - Abs(g_DEr_old)) {
			g_baseLow = 0;
			g_ls = 0.99 * g_ls;
			if (g_ls < 0.01 * g_accuracy)
				g_ls = 0.01 * g_accuracy;
			g_mc -= (BPNN_DFT_MOMENTUM / 20);
			if (g_mc < 0)
				g_mc = 0;
		}
		else
			isChange = false;
	}
	return *this;
}

Float CLBpExtend::Er(Bool bUseLastForwardCalc)
{
	Float er = 0, ter = 0;
	auto layindex0 = kernel.lastLayStartNnIndex();
	Uint ovSi = kernel.outputDimension();
	if (bUseLastForwardCalc) {
		for (Uint l = 0, sl = vm_samUsage->size(), jsi = min(vm_samSets->targetDimension(), ovSi); l < sl; l++)
		{
			Uint ci = (*vm_samUsage)[l];
			auto ov = (*vm_samSets).tv(ci);
			er = 0;
			for (Uint j = 0; j < jsi; j++)
			{
				er += kernel.loss(pYii(layindex0 + j)[l], ov[j]);
			}
			(_isnan((er)) ? (throw std::invalid_argument("ErNan"), FALSE) : (isinf((er)) ? (throw std::invalid_argument("ErInf"), FALSE) : TRUE));
			ter += er;
		}
	}
	else {
		for (Uint l = 0, sl = vm_samUsage->size(), iDim = (*vm_samSets).intputDimension(), jsi = min((*vm_samSets).targetDimension(), kernel.outputDimension()); l < sl; l++)
		{
			Uint ci = (*vm_samUsage)[l];
			auto ov = (*vm_samSets).tv(ci);
			kernel.setInput((*vm_samSets).iv(ci), iDim).predict();
			er = 0;
			for (Uint j = 0; j < jsi; j++)
			{
				er += kernel.loss(yi0i(layindex0 + j), ov[j]);
			}
			//CLCheckNanInfMsg(er, 错误：误差计算溢出，请减小学习率并重试！);
			(_isnan((er)) ? (throw std::invalid_argument("ErNan"), FALSE) : (isinf((er)) ? (throw std::invalid_argument("ErInf"), FALSE) : TRUE));
			ter += er;
		}
	}
	ter /= (vm_samUsage->size() == 0 ? 1.0 : ((Float)(vm_samUsage->size())));
	g_Er_old = g_Er;
	g_Er = ter;
	g_DEr_old = g_DEr;
	g_DEr = g_Er - g_Er_old;
	return g_Er;
}

Float CLBpExtend::getDefaultW()
{
	if (B == A)return A;
	Float a = min(A,B), b = max(A,B);
	return RAND_F_A_B(a, b);
}

Bool CLBpExtend::exportGraph(PCStr lpfileName, Int pos)
{
	auto pi = logoutLine.find(pos);
	if (pi == logoutLine.end())
		return false;
	auto pw = (CLShowTool*)pi->second;
	if (pw) {
		if (!pw->isWindow())
			return false;
		if (::IsIconic(*pw))
			return false;
		Sleep(1000);
		return pw->windowsToBmp(lpfileName);
	}
	else return true;
}

//设置神经网络训练采用多线程技术支持（默认状态是关闭）
CLBpExtend& CLBpExtend::setMultiThreadSupport(Float percentageOfThreadsStarted)
{
	if (percentageOfThreadsStarted < VtEpslon) {
		if (work.isRunning())
			work.close();
		m_mutiTrdSupportPerc = 0;
	}
	else 
		m_mutiTrdSupportPerc = min(percentageOfThreadsStarted,1);
	return *this;
}

CLBpExtend& CLBpExtend::setGpuAcceleratedSupport(Bool bOpen)
{
	bAmpSupport = bOpen;
	return *this;
}

CLBpExtend& CLBpExtend::openGraphFlag(Bool bOpen)
{
	if (bOpen == true) {
		er.clear(), ls.clear(), mc.clear();
	}
	if (bOpenGraph == true && bOpen == false) {
		releasBitmapBuf();
	}
	bOpenGraph = bOpen;
	return *this;
}

CLBpExtend& CLBpExtend::showGraphParam(Uint maxToShow, Int posX, Int posY)
{
	CLRect rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcWorkArea, NULL);
	auto bk = CLShowTool::getDefaultSimpleLineRect();
	Int y = 0;
	if (isCorrectRateEvaluationModel()) {
		y = rcWorkArea.Height() / 4;
	}
	else {
		y = rcWorkArea.Height() / 3;
	}
	if (y < bk.Height()) {
		auto cnew = bk;
		cnew.bottom = cnew.top + y;
		CLShowTool::setDefaultSimpleLineRect(cnew);
	}
	else y = bk.Height();
	if (isCorrectRateEvaluationModel() && cr.size() > 2) {
		CLShowTool* scr;
		auto pci = logoutLine.find(Ps_CrTool);
		if (pci == logoutLine.end()) {
			scr = new CLShowTool;
			logoutLine[Ps_CrTool] = scr;
			scr->setAutoDeleteInNewThread(false);
		}
		else scr = (CLShowTool*)pci->second;
		
		if (maxToShow > 0) {
			if (cr.size() > maxToShow)cr.erase(cr.begin(), cr.begin() + (cr.size() - maxToShow - 1));
		}
		CLString tit(300);
		scr->writeSimpleLine(cr, tit.format(("%s Correct Rate = %.2f%%"), kernel.getName(), (cr.at(cr.size() - 1)) * 100.0).string(), posX, posY, 0, (double)cr.size(), CLVIOLET);
		if (!scr->isWindow())
			CLShowTool::createAndShowInNewThread(0, scr, true);
	}
	if (er.size() > 2) {
		CLShowTool* sea;
		auto pci = logoutLine.find(Ps_EaTool);
		if (pci == logoutLine.end()) {
			sea = new CLShowTool;
			logoutLine[Ps_EaTool] = sea;
			sea->setAutoDeleteInNewThread(false);
		}
		else sea = (CLShowTool*)pci->second;
		if (maxToShow > 0) {
			if (er.size() > maxToShow)er.erase(er.begin(), er.begin() + (er.size() - maxToShow - 1));
		}
		CLString tit(300);
		sea->writeSimpleLine(er, tit.format(("%s Er = %.9f"), kernel.getName(), (er.at(er.size() - 1))).string(), posX, posY + (isCorrectRateEvaluationModel() ? y : 0), 0, (double)er.size(), CLRED);
		if (!sea->isWindow())
			CLShowTool::createAndShowInNewThread(0, sea, true);
	}
	if (pFAutoFit && ls.size() > 2 && mc.size() > 2) {
		CLShowTool * sls, * smc;
		auto pci = logoutLine.find(Ps_LsTool);
		if (pFAutoFit && pci == logoutLine.end()) {
			sls = new CLShowTool;
			logoutLine[Ps_LsTool] = sls;
			sls->setAutoDeleteInNewThread(false);
		}
		else sls = (CLShowTool*)pci->second;
		pci = logoutLine.find(Ps_McTool);
		if (pFAutoFit && pci == logoutLine.end()) {
			smc = new CLShowTool;
			logoutLine[Ps_McTool] = smc;
			smc->setAutoDeleteInNewThread(false);
		}
		else smc = (CLShowTool*)pci->second;
		
		if (maxToShow > 0) {
			if (ls.size() > maxToShow)ls.erase(ls.begin(), ls.begin() + (ls.size() - maxToShow - 1));
			if (mc.size() > maxToShow)mc.erase(mc.begin(), mc.begin() + (mc.size() - maxToShow - 1));
		}
		CLString tit(300);
		//if (pFAutoFit)
		sls->writeSimpleLine(ls, tit.format(("%s Ls = %.9f"), kernel.getName(), (ls.at(ls.size() - 1))).string(), posX, posY + (isCorrectRateEvaluationModel() ? y * 2 : y * 1), 0, (double)ls.size(), CLGREEN);
		//if (pFAutoFit)
		smc->writeSimpleLine(mc, tit.format(("%s Mc = %.9f"), kernel.getName(), (mc.at(mc.size() - 1))).string(), posX, posY + (isCorrectRateEvaluationModel() ? y * 3 : y * 2), 0, (double)mc.size(), CLORANGE);
		if (pFAutoFit && !sls->isWindow())
			CLShowTool::createAndShowInNewThread(0, sls, true);
		if (pFAutoFit && !smc->isWindow())
			CLShowTool::createAndShowInNewThread(0, smc, true);
	}
	CLShowTool::setDefaultSimpleLineRect(bk);
	return *this;
}

CLBpExtend& CLBpExtend::showGraphNetStruct(Bool isShowDetail, Int posX, Int posY)
{
	if (kernel.layerCounts() >= Minimum_Layers) {
		auto nnsi = kernel.neuronCounts();
		if (nnsi > 10000) {
			if (bShowNetAlert) {
				if (IDYES == CLString().format(("网络的神经元数量(%d)过多，无法绘制网络的二维结构图！\n下次不再显示提示（Yes/No）"), nnsi).messageBox(_lpBpnnMsgBoxTitle, MB_YESNO | MB_ICONWARNING)) {
					bShowNetAlert = false;
				}
			}
			return *this;
		}
		CLShowTool* sst = 0, * sstd = 0;
		CLString tit(300);
		auto pcst = logoutLine.find(Ps_StructTool);
		if (pcst == logoutLine.end()) {
			sst = new CLShowTool;
			logoutLine[Ps_StructTool] = sst;
			sst->setAutoDeleteInNewThread(false);
		}
		else sst = (CLShowTool*)pcst->second;
		
		getBitmapData(hBitmapInfo, m_BtmapFileHdr, pBitmapInfo, m_bitmapBufSi, 0);
		sst->writeSimpleBitmap(m_BtmapFileHdr, pBitmapInfo, tit.format(("%s Net Structure"), kernel.getName()).string(), posX, posY + 0);
		if (!sst->isWindow())
			CLShowTool::createAndShowInNewThread(0, sst, true);

		if (isShowDetail) {
			auto pcstd = logoutLine.find(Ps_StructDMcTool);
			if (pcstd == logoutLine.end()) {
				sstd = new CLShowTool;
				logoutLine[Ps_StructDMcTool] = sstd;
				sstd->setAutoDeleteInNewThread(false);
			}
			else sstd = (CLShowTool*)pcstd->second;			
			getBitmapData(hBitmapInfo, m_BtmapFileHdr, pBitmapInfo, m_bitmapBufSi, 1);
			sstd->writeSimpleBitmap(m_BtmapFileHdr, pBitmapInfo, tit.format(("%s Net Detail Structure"), kernel.getName()).string(), posX, posY + 350);
			if (!sstd->isWindow())
				CLShowTool::createAndShowInNewThread(0, sstd,true);
		}
	}
	return *this;
}

Bool CLBpExtend::exportGraphCorrectRate(PCStr lpfileName)
{
	return exportGraph(lpfileName, Ps_CrTool);
}

Bool CLBpExtend::exportGraphEr(PCStr lpfileName)
{
	return exportGraph(lpfileName, Ps_EaTool);
}

Bool CLBpExtend::exportGraphLs(PCStr lpfileName)
{
	return exportGraph(lpfileName, Ps_LsTool);
}

Bool CLBpExtend::exportGraphMc(PCStr lpfileName)
{
	return exportGraph(lpfileName, Ps_McTool);
}

void CLBpKernel::setName(PCStr lpName)
{
	bpnnName = lpName ? lpName : "";
}

CLBpExtend::CLBpExtend(CLBpKernel* pkernel, const Bpnn& _bp)
	:kernel(*pkernel),
	work(this),
	vm_layInfo(pkernel->vm_layInfo),
	vm_globleInfo(pkernel->vm_globleInfo),
	vm_wbpack(pkernel->vm_wbpack),
	vm_wji_Data(pkernel->vm_wji_Data),
	vm_neuron(pkernel->vm_neuron),
	vm_link_Data(pkernel->vm_link_Data),
	vm_yi0(pkernel->vm_yi0),
	vm_bnData(pkernel->vm_bnData),
	bp(_bp)
{
	//if (kernel == nullptr)
	//	CLString(("错误：神经网络计算内核因未知异常导致未能构造，所以网络扩展创建失败！")).messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	reset();
}

CLBpExtend::~CLBpExtend()
{
	//清理外显对象
	for (auto i = logoutLine.begin(); i != logoutLine.end(); ++i)
	{
		auto pw = (CLShowTool*)i->second;
		if (pw) {
			if (pw->isWindow()) {
				if (pw->getAutoDeleteInNewThread() == FALSE) //窗口状态 且 非自删除
					pw->setAutoDeleteInNewThread(TRUE); //指定为自删除
			}
			else delete pw;
		}
		i->second = 0;
	}
	setMultiThreadSupport(0);//可以不用这一步	
	setGpuAcceleratedSupport(false);
	releasBitmapBuf();
}

CLBpExtend& CLBpExtend::setLayer(Uint _hideLayerNumbers, Uint _hidePerLayerNumbers)
{
	hideLayerNumbers = max(1, _hideLayerNumbers);
	hidePerLayerNumbers = max(1, _hidePerLayerNumbers);
	return *this;
}

CLBpExtend& CLBpExtend::setParam(Float ls, Float er_accuracy, Float mc)
{
	g_ls_old = g_ls = Abs(ls);
	g_mc_old = g_mc = Abs(mc);
	g_accuracy = Abs(er_accuracy);
	bSetParam = true;
	return *this;
}

inline Uint CLBpKernel::nnLayerIndex(Uint index) {
	for (Uint lay = 0, si = layerCounts(); lay < si; lay++)
	{
		if (vm_layInfo[lay].iLayNnStartIndex <= index && index <= vm_layInfo[lay].iLayNnEndIndex)
			return lay;
	}
	CLString().format("\nnnLayerIndex(): current neuron index(%d) is out of range( >= %d)!", index, neuronCounts()).printf()
		.messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR | MB_OK).throw_runtime_error();
	return UINT_MAX;
}

inline Uint CLBpKernel::wbLayerIndex(Uint index) {
	for (Uint lay = 0, si = layerCounts(); lay < si; lay++)
	{
		if (vm_layInfo[lay].iLayWbStartIndex <= index && index < (vm_layInfo[lay].iLayWbStartIndex + vm_layInfo[lay].iLayWbCounts))
			return lay;
	}
	CLString().format("\nwbLayerIndex(): current wb index(%d) is out of range( >= %d)!", index, (Uint)vm_wbpack.size()).printf()
		.messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR | MB_OK).throw_runtime_error();
	return UINT_MAX;
}

CLBpKernel& CLBpKernel::setInput(const Float* inputArray, Uint dataDim)
{
	if (dataDim < inputDimension())
		CLString().format("\n错误：输入向量维度(%d)小于样本输入维度(%d)!", dataDim, inputDimension()).printf().messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	inVec = inputArray;
	return *this;
}

CLBpKernel& CLBpKernel::setInput(const VLF& inputArray)
{
	return setInput(inputArray.data(), inputArray.size());
}

void CLBpKernel::_predict(Float* pxi, Float* pyi, const Float* inVec)
{
#if UsePFunc > 0
	auto pFunc = vm_bnMi > 0 ? &CLBpKernel::_Xi_bn : &CLBpKernel::_Yi;
	for (Uint lay = 0, si = layerCounts(); lay < si; lay++)//正向执行
	{
		auto posSi = LayEndNnIndex(lay);
		if (lay == 0)
			for (Uint index = LayStartNnIndex(lay); index <= posSi; index++)
				(this->*pFunc)(index, Nn(index), pxi, pyi, inVec, 1, 0, nullptr, 1, 0);
		else
			for (Uint index = LayStartNnIndex(lay); index <= posSi; index++)
				(this->*pFunc)(index, Nn(index), pxi, pyi, nullptr, 1, 0, nullptr, 1, 0);
	}
#else
	if (vm_bnMi > 0)
		for (Uint lay = 0, laysi = layerCounts(); lay < laysi; lay++)//正向执行
		{
			auto posSi = LayEndNnIndex(lay);
			if (lay == 0)
				for (Uint index = LayStartNnIndex(lay); index <= posSi; index++)
					_Xi_bn(index, Nn(index), pxi, pyi, inVec);
			else
				for (Uint index = LayStartNnIndex(lay); index <= posSi; index++)
					_Xi_bn(index, Nn(index), pxi, pyi, nullptr);
		}
	else
		for (Uint lay = 0, laysi = layerCounts(); lay < laysi; lay++)//正向执行
		{
			auto posSi = LayEndNnIndex(lay);
			if (lay == 0)
				for (Uint index = LayStartNnIndex(lay); index <= posSi; index++)
					_Yi(index, Nn(index), pxi, pyi, inVec);
			else
				for (Uint index = LayStartNnIndex(lay); index <= posSi; index++)
					_Yi(index, Nn(index), pxi, pyi, nullptr);
		}
#endif
}

CLBpKernel& CLBpKernel::predict()
{
	return _predict(nullptr, vm_yi0.data(), inVec), * this;
}

void CLBpKernel::updateTransFunc(Uint hideTransType, Uint outTransType)
{
	for (Uint i = 0, slayer = layerCounts(); i < slayer; i++)
	{
		auto index0 = LayStartNnIndex(i);
		if (i >= lastLayIndex()) {
			for (Uint j = 0, jsi = LayNnCounts(i); j < jsi; j++) {
				setTransFunc(index0 + j, outTransType);
			}
		}
		else {
			for (Uint j = 0, jsi = LayNnCounts(i); j < jsi; j++) {
				setTransFunc(index0 + j, hideTransType);
			}
		}
	}
}

CLBpExtend& CLBpExtend::updateDropout()
{
	if (dpRepeatTrainTimes == 0)
		return *this;
	if (kernel.vm_bnMi > 0)
		return *this;
	if (dpRepeatTrainTimesC++ == 0) {
		vm_drop.clear();
		vm_drop.resize(kernel.layerCounts(), 0.0);
		for (auto& i : dpDefineTbl)
		{
			if (i.first >= kernel.layerCounts() - 1)
				break;
			vm_drop[i.first] = i.second;
			auto lay = kernel.LayStartNn(i.first);
			auto index = kernel.LayStartNnIndex(i.first);
			auto laySi = kernel.LayNnCounts(i.first);
			for (Uint j = 0, sj = laySi; j < sj; j++)
			{
				auto& nn = lay[j];
				if (Wb(nn).bFlag & FG_WB_NotUpdate)
					continue;
				else {
					Float cr = RAND_F_0_1();
					if (cr <= i.second) {
						nn.bitFlag |= FG_NN_Dropout;
					}
					else {
						nn.bitFlag &= (~FG_NN_Dropout);
					}
				}
			}
		}
	}
	if (dpRepeatTrainTimesC >= dpRepeatTrainTimes)
		dpRepeatTrainTimesC = 0;
	return *this;
}

CLBpExtend& CLBpExtend::buildBpnnInfo()
{
	(kernel.vm_layInfo).clear();
	(kernel.vm_globleInfo).iLayCounts = (kernel.vm_globleInfo).iNCounts = 0;
	if (mode != nullptr && mode->size()) {
		(kernel.vm_globleInfo).iLayCounts = mode->size();
		(kernel.vm_layInfo).resize((kernel.vm_globleInfo).iLayCounts);
		Uint cs = 0;
		for (Uint i = 0, thisNcounts = 0; i < (kernel.vm_globleInfo).iLayCounts; i++)
		{
			auto& inf = (kernel.vm_layInfo)[i];
			auto& lay = mode->at(i);
			inf.bIsEndLay = i >= (kernel.vm_globleInfo).iLayCounts - 1 ? true : false;
			inf.iLayIndex = i;
			inf.iLayNnCounts = 0;
			thisNcounts = 0;
			for (auto& ib : lay) {
				thisNcounts += ib.nnCounts;
			}
			inf.iLayNnCounts = thisNcounts;
			inf.iLayNnStartIndex = cs;
			inf.iLayNnEndIndex = cs + thisNcounts - 1;
			cs += thisNcounts;
		}
		(kernel.vm_globleInfo).iNCounts = cs;
	}
	else {
		(kernel.vm_globleInfo).iLayCounts = hideLayerNumbers + 1;
		(kernel.vm_globleInfo).iNCounts = hideLayerNumbers * hidePerLayerNumbers + vm_samSets->targetDimension();
		(kernel.vm_layInfo).resize((kernel.vm_globleInfo).iLayCounts);
		for (Uint i = 0, cs = 0; i < (kernel.vm_globleInfo).iLayCounts; i++)
		{
			auto& inf = (kernel.vm_layInfo)[i];
			inf.bIsEndLay = i >= (kernel.vm_globleInfo).iLayCounts - 1 ? true : false;
			inf.iLayIndex = i;
			inf.iLayNnCounts = inf.bIsEndLay ? vm_samSets->targetDimension() : hidePerLayerNumbers;
			inf.iLayNnStartIndex = cs;
			inf.iLayNnEndIndex = cs + inf.iLayNnCounts - 1;
			cs += inf.iLayNnCounts;
		}
	}
	return *this;
}

BpnnToolInfo CreativeLus::getBpnnToolInfo() {
	return toolInfo;
}

CLBpExtend& CLBpExtend::setDropout(Uint _repeatTrainTimes, const DropoutLayerDef& def)
{
	dpDefineTbl.clear();
	dpRepeatTrainTimes = _repeatTrainTimes;
	dpRepeatTrainTimesC = 0;
	if (dpRepeatTrainTimes < 1)
		return *this;
	for (const DRP& dp : def) {
		if (!BETWEEN(dp.rate, 0.05, 0.95))
			continue;
		for (Uint i = dp.startlayer; i <= dp.endLayer; i++)
		{
			dpDefineTbl[i] = dp.rate;
		}
	}
	return *this;
}

CLBpExtend& CLBpExtend::setBatchNormalization(Uint miniBatch, const BnLayerIndexList& bnLayerList)
{
	if (miniBatch == 0) {
		vm_bnData.clear();
		kernel.vm_bnMi = 0;
		bnDefineTbl.clear();
	}
	else {
		if (kernel.vm_bnMi != miniBatch)
			vm_bnData.clear();
		kernel.vm_bnMi = miniBatch;
		bnDefineTbl.clear();
		for (const auto& i : bnLayerList) {
			bnDefineTbl[i] = max(1, kernel.vm_bnMi);
		}
	}
	return *this;
}

CLBpExtend& CLBpExtend::setNetUseTo(Byte type)
{
	switch (type)
	{	
	case EBP_UT::UT_Classify:
		m_netUseToType = EBP_UT::UT_Classify;
		setSampleBatchCounts(1, 0);
		break;
	default:
		m_netUseToType = EBP_UT::UT_Approach;
		setSampleBatchCounts(0, 0);
		break;
	}
	return *this;
}

CLBpExtend& CLBpExtend::setTransFunc(Byte iBpTypeHide, Byte iBpTypeOut)
{
	hideLayerTrsFunType = iBpTypeHide;
	outLayerTrsFunType = iBpTypeOut;
	return kernel.updateTransFunc(hideLayerTrsFunType, outLayerTrsFunType), * this;
}

//损失函数实现--------------------------------------------------
#define LOSS_FUNC( NAME ) \
	Float ls_##NAME(const Float y,const Float t,const Bool isDev)
#define LOSS_FUNC_G( NAME ) \
	Float ls_##NAME##_amp(const Float y,const Float t,const Bool isDev) restrict(amp)

LOSS_FUNC(MeanSquareLoss) {
	if (isDev) {
		return (y - t);
	}
	else {
		return (y - t) * (y - t) / 2;
	}
}

LOSS_FUNC_G(MeanSquareLoss) {
	if (isDev) {
		return (y - t);
	}
	else {
		return (y - t) * (y - t) / 2;
	}
}

void CLBpKernel::setLossFunc(Byte lossId)
{
	vm_LossFuncType = lossId;
	switch (vm_LossFuncType)
	{
	case EBP_LS::LS_MeanSquareLoss:
		pLossFunc = ls_MeanSquareLoss;
		break;
	default:
		vm_LossFuncType = EBP_LS::LS_MeanSquareLoss;
		pLossFunc = ls_MeanSquareLoss;
		break;
	}
}

inline void CLBpKernel::clearContainer(UINT createCounts) {
	if (createCounts == 0)
		(vm_globleInfo).reset(), (vm_layInfo).clear();
	(vm_wbpack).clear();
	(vm_wji_Data).clear();

	(vm_neuron).clear(), (vm_neuron).resize(createCounts);
	(vm_yi0).clear(), (vm_yi0).resize(createCounts);
	(vm_link_Data).clear();
	vm_bnData.clear();
}

Bool CLBpKernel::getOutput(VLF& out_yi) const
{
	out_yi.clear();
	if (layerCounts() < Minimum_Layers)
		return false;
	out_yi.resize(outputDimension());
	memcpy_s(out_yi.data(), outputDimension() * sizeof(Float), lastLayYi0Start(), outputDimension() * sizeof(Float));
	return true;
}

Bool CLBpKernel::_getOutput(const Float* pyiData, VLF& out_yi) const
{
	out_yi.clear();
	if (layerCounts() < Minimum_Layers)
		return false;
	out_yi.resize(outputDimension());
	memcpy_s(out_yi.data(), outputDimension() * sizeof(Float), &pyiData[lastLayStartNnIndex()], outputDimension() * sizeof(Float));
	return true;
}

Float CLBpExtend::lossDerv(const Float y, const Float t)
{
	return (kernel.pLossFunc)(y, t, true);
}

//显示回掉
void bpnnCallBack(PVoid _pCbFun, PVoid _pIns, Int c, Int max, PCStr info = ("")) {
	if (_pCbFun) {
		if (_pIns) {
			Bpnn::PMCBMonitor pCbFun = *(Bpnn::PMCBMonitor*)(void**) & _pCbFun;
			//memcpy(&pCbFun, &_pCbFun, sizeof(Bpnn::PMCBMonitor));
			((Bpnn*)_pIns->*pCbFun)(c, max, info);
		}
		else
			(*(Bpnn::PCBMonitor)_pCbFun)(c, max, info);
	}
}

void CLBpExtend::releasBitmapBuf() {
	if (hBitmapInfo) {
		::GlobalUnlock(hBitmapInfo);
		::GlobalFree(hBitmapInfo);
		hBitmapInfo = 0;
		pBitmapInfo = 0;
		m_bitmapBufSi = 0;
	}
}

Bool CLBpExtend::convergenceHasBeenAchieved()
{
	if (isCorrectRateEvaluationModel()) {
		switch (m_CorrectRateType)
		{
		case CRT_MaxValuePosMatch:
		case CRT_MinValuePosMatch:
			if ((runTimes % (maxTimes / 5)) == 0)
				if (getCorrectRate(this->predict_samSets, predict_useSamCounts, predict_useRandom, m_CorrectRateType) >= m_CorrectRate)//样本正确率达到
					return true;
			break;
		default:
			//if (getEr() <= g_accuracy)//误差精度达到
			//if ((getEr() <= g_accuracy) && ((runTimes % (maxTimes / 10)) == 0) )//误差精度达到
			if ((runTimes % (maxTimes / 30)) == 0 )//误差精度达到
				if (getCorrectRate(this->predict_samSets, predict_useSamCounts, predict_useRandom, m_CorrectRateType) >= m_CorrectRate)//样本正确率达到
					return true;
			break;
		}
	}
	else if (getEr() <= g_accuracy) {//误差精度达到
		return true;
	}
	return false;
}

Bool CLBpExtend::checkNeuronLinkSuspended(PVoid _pCbFun, PVoid _pIns)
{
	if (kernel.layerCounts() < Minimum_Layers)
		return false;
	Bool isFord = false;
	CLString str2(("\r\nThere are suspended or totally unrelated neurons \r\nwith forward links in the following range:"));
	for (Uint i = 1, si = kernel.layerCounts(); i < si; i++) {
		auto lay1 = kernel.LayStartNn(i - 1);
		auto lay1Si = kernel.LayNnCounts(i - 1);
		auto lay2 = kernel.LayStartNn(i);
		auto lay2Si = kernel.LayNnCounts(i);
		Int upBaseSi = neuronCountsTillLayer(i);
		Int imin2 = -1, imax2 = -1;
		Int sj1 = lay1Si;
		for (Uint j = 0, sj = lay2Si; j < sj; j++)
		{
			auto nn = &lay2[j];
			Uint k = 0, sk = link_size(*nn);
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
				Int clk = plink(*nn)[k];
				if (clk - upBaseSi >= sj1) {
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
			str2 << ("\r\nLayer ") << i << (": [ ") << imin2 << (" - ") << imax2 << (" ]");
		}
	}
	if (isFord) {
		str2 << ("\r\n");
		bpnnCallBack(_pCbFun, _pIns, -1, -1, str2());
		str2.messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	}
	else if (isFord) {
		str2 << ("\r\n");
		bpnnCallBack(_pCbFun, _pIns, -1, -1, str2);
		str2.messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	}
	return true;
}

Bool CLBpExtend::prepairTrainSamUsageData()
{
	//获取数量
	Uint useMax = 0;
	Uint tsi = (*train_samSets).size();
	if (train_useSamCounts == 0 || train_useSamCounts > tsi)
		useMax = tsi;
	else
		useMax = train_useSamCounts;
	//构造使用集
	if (train_useRandom) {
		if (useMax == tsi) {
			if (train_samUsage.size() != tsi) {
				train_samUsage.resize(useMax);
				for (Uint i = 0; i < useMax; i++)
				{
					train_samUsage[i] = i;
				}
			}
		}
		else {
			train_samUsage.resize(useMax);
			for (Uint i = 0; i < useMax; i++)
			{
				Uint j = RAND_I_Ai_Bi(0, tsi - 1);
				train_samUsage[i] = j;
			}
		}
	}
	else {
		if (train_samUsage.size() == 0) {
			train_samUsage.resize(useMax);
			for (Uint i = 0; i < useMax; i++)
			{
				train_samUsage[i] = i;
			}
		}
		else {
			if (train_samUsage.size() != tsi) {
				Uint lasti = *train_samUsage.crbegin();
				Uint samSi = (*train_samSets).size();
				train_samUsage.resize(useMax);
				for (Uint i = 0; i < useMax; i++)
				{
					lasti++;
					if (lasti >= samSi)
						lasti = 0;
					train_samUsage[i] = lasti;
				}
			}
		}
	}
	if (train_samUsage.size() > 0) {
		return true;
	}
	return  false;
}

void CLBpExtend::checkWbPackShareLinkRange(PVoid _pCbFun, PVoid _pIns)
{
	//Wb反响链接检查
	for (Uint i = 0, si = kernel.layerCounts(); i < si; i++)
	{
		auto lay = kernel.LayStartNn(i);
		auto laySi = kernel.LayNnCounts(i);
		auto index0 = kernel.LayStartNnIndex(i);
		for (Uint j = 0, sj = laySi; j < sj; j++)
		{
			auto& nn = lay[j];
			auto& wb = Wb(nn);
			auto index = index0 + j;
			if (wb.nnIds > index)
				wb.nnIds = index;
			if (wb.nnIde < index + 1)
				wb.nnIde = index + 1;
		}
		bpnnCallBack(_pCbFun, _pIns, i + 1, si, ("Net is checking share link range."));
	}
}

void CLBpExtend::checkWbPackShareBnData(PVoid _pCbFun, PVoid _pIns)
{
	//处理bn层
	vm_bnLayOpen.clear();
	if (kernel.vm_bnMi > 0) {
		vm_bnData.resize(vm_wbpack.size());		
		if (bnDefineTbl.size() == 0) {
			vm_bnLayOpen.resize(kernel.layerCounts(), false);
			for (size_t i = 0,si = vm_wbpack.size();i<si;i++ )
			{
				auto& wb = vm_wbpack[i];
				auto& bn = vm_bnData[wb.index];	
				bn.index = -1;
				if (!(wb.bFlag & FG_WB_NotUpdate)) {
					wb.bFlag |= FG_WB_BatchNorm;
					bn.index = wb.index;
					bn.createEx();
					if (bn.isNeedToResetSize(wb.nnIde - wb.nnIds , vm_xy_span)) {
						bn.resetSize(wb.nnIds, kernel.vm_bnMi, wb.nnIde - wb.nnIds , vm_xy_span);
					}
					vm_bnLayOpen[kernel.wbLayerIndex(wb.index)] = true;
				}
				else {
					wb.bFlag &= (~FG_WB_BatchNorm);
					bn.releaseEx();
				}
				bpnnCallBack(_pCbFun, _pIns, i + 1, si, ("Net is checking batchNorm data."));
			}
		}
		else {
			vm_bnLayOpen.resize(kernel.layerCounts(), false);
			for (size_t i = 0, si = vm_wbpack.size(); i < si; i++)
			{
				auto& wb = vm_wbpack[i]; 
				auto& bn = vm_bnData[wb.index];
				bn.index = -1;
				if (!(wb.bFlag & FG_WB_NotUpdate)) {
					auto lay = kernel.nnLayerIndex(wb.nnIds);
					if (bnDefineTbl.find(lay) == bnDefineTbl.cend()) {// no bn
						wb.bFlag &= (~FG_WB_BatchNorm);
					}
					else {// has bn
						vm_bnLayOpen[lay] = true;
						wb.bFlag |= FG_WB_BatchNorm;	
						bn.index = wb.index;
						bn.createEx();
						if (bn.isNeedToResetSize(wb.nnIde - wb.nnIds , vm_xy_span)) {
							bn.resetSize(wb.nnIds, kernel.vm_bnMi, wb.nnIde - wb.nnIds , vm_xy_span);
						}
					}
				}
				else {
					wb.bFlag &= (~FG_WB_BatchNorm);
					bn.releaseEx();
				}
				bpnnCallBack(_pCbFun, _pIns, i + 1, si, ("Net is checking batchNorm data."));
			}
		}
	}
	else {
		vm_bnData.clear();
		bnDefineTbl.clear();
		for (auto& wb : vm_wbpack)
			wb.bFlag &= (~FG_WB_BatchNorm);
	}
}

Float CLBpKernel::loss(const Float y, const Float t) const{
	return pLossFunc(y, t, false);
}

wbpack* CLBpExtend::newWb() {
	Uint id = (kernel.vm_wbpack).size();
	(kernel.vm_wbpack).resize(id + 1);
	auto p = &*(kernel.vm_wbpack).rbegin();
	p->index = id;
	return p;
}

CLBpExtend& CLBpExtend::buildNet(Bpnn::PCBMonitor _pCbFun, PVoid _pIns){
	clearAllDataContainer();
	if (!(vm_samSets != nullptr && (*vm_samSets).size() > 0)) {
		bpnnCallBack(_pCbFun, _pIns, -1, -1, ("Net train stoped. Sample sets is null."));
		CLString(("错误：网络训练样本未设置,无法获取输入输出维度数据！\r\n无法构建神经网络。")).messageBoxRef(_lpBpnnMsgBoxTitle, MB_OK | MB_ICONERROR)
			.throw_runtime_error();
	}
	(kernel.vm_inputDim) = vm_samSets->intputDimension();
	(kernel.vm_outputDim) = vm_samSets->targetDimension();
	if ((kernel.vm_inputDim) == 0 || (kernel.vm_outputDim) == 0) {
		(kernel.vm_inputDim) = (kernel.vm_outputDim) = 0;
		bpnnCallBack(_pCbFun, _pIns, -1, -1, ("Bpnn buildNet: Sample sets io vector size is 0!"));
		CLString(("错误：样本的输入或输出向量的维度为0！\r\n无法构建神经网络。")).messageBoxRef(_lpBpnnMsgBoxTitle, MB_OK | MB_ICONERROR)
			.throw_runtime_error();
	}
	if (!(((mode != nullptr) && mode->size() >= 2) || (hideLayerNumbers * hidePerLayerNumbers > 0))) {
		bpnnCallBack(_pCbFun, _pIns, -1, -1, ("Bpnn buildNet: Define data is defect!"));
		CLString(("错误：网络未设置结构定义描述对象，也没有定义结构层数据！\r\n无法构建神经网络。")).messageBoxRef(_lpBpnnMsgBoxTitle, MB_OK | MB_ICONERROR)
			.throw_runtime_error();
	}

	//bpnnCallBack(_pCbFun, _pIns, -1, -1, ("Net build started."));
	Int cInCreate = 0, allInCreate = 0;

	RAND_SEED_EX();
	Int fordNum = 0;
	//必须先构建信息结构------------------------
	buildBpnnInfo();
	//构造必须的数据
	clearAllDataContainer(kernel.neuronCounts());
	//构建结构-----------------------------
	Uint cs = 0;
	if (mode == 0 || (*mode).size() == 0) {
		allInCreate = kernel.neuronCounts();
		fordNum = kernel.inputDimension();
		WbDef wbdef(fordNum, { (Float)EBP_IT::IT_Uniform,A,B }, { (Float)EBP_IT::IT_Uniform,A,B });
		for (Uint i = 0; i < hideLayerNumbers + 1; i++)
		{
			//auto lay = &kernel.getLayer(i);
			auto lay = kernel.LayStartNn(i);
			auto laySi = kernel.LayNnCounts(i);
			Int upNnSiBase = neuronCountsTillLayer(i);
			if (i != (hideLayerNumbers)) {//构造隐含层
				for (Uint j = 0; j < hidePerLayerNumbers; j++)
				{
					auto& nn = lay[j];
					CLBpKernel::fitId(nn, cs, i, j);
					kernel.setTransFunc(cs, hideLayerTrsFunType);
					kernel.setWcFunc(cs, EBP_WC::WC_Add);
					cs++;
					auto& wb = *newWb();
					nn.wb = wb.index;
					wbdef.wiSize = fordNum;
					kernel.createWbByWbDef(wb, wb.index, wbdef);
					auto link = make_link(nn, fordNum);
					for (Int k = 0; k < fordNum; k++)
						link[k] = upNnSiBase + k;
					bpnnCallBack(_pCbFun, _pIns, ++cInCreate, allInCreate, ("Net is building hide layer."));
				}
			}
			else { //构造输出层
				auto outDim = kernel.outputDimension();
				for (Uint j = 0; j < outDim; j++)
				{
					auto& nn = lay[j];
					CLBpKernel::fitId(nn, cs, i, j);
					kernel.setTransFunc(cs, outLayerTrsFunType);
					kernel.setWcFunc(cs, EBP_WC::WC_Add);
					cs++;
					auto& wb = *newWb();
					nn.wb = wb.index;
					wbdef.wiSize = fordNum;
					kernel.createWbByWbDef(wb, wb.index, wbdef);
					auto link = make_link(nn, fordNum);
					for (Int k = 0; k < fordNum; k++)
						link[k] = upNnSiBase + k;
					bpnnCallBack(_pCbFun, _pIns, ++cInCreate, allInCreate, ("Net is building output layer."));
				}
			}
			fordNum = laySi;
		}//end for i
	}
	else {
		if ((*mode).size() < 2) {
			clearAllDataContainer(0);
			bpnnCallBack(_pCbFun, _pIns, -1, -1, ("Bpnn buildNet: BpnnStructDef define is less than two layers!"));
			CLString(("错误：BpnnStructDef结构定义描述对象所定义的结构层数少于两层，至少包含一个隐层和一个输出层！")).
				messageBoxRef(_lpBpnnMsgBoxTitle, MB_OK | MB_ICONERROR).throw_runtime_error();
		}
		allInCreate = 0;
		for (Uint i = 0, si = (*mode).size(); i < si; i++)
		{
			auto& pc = (*mode)[i];
			for (Uint j = 0, sj = pc.size(); j < sj; j++) {
				allInCreate += pc[j].nnCounts;
			}
		}
		//构造全局wb
		(kernel.vm_wbpack).resize((*mode).gWb.size());
		for (Uint i = 0, si = (*mode).gWb.size(); i < si; i++)
		{
			auto& wb = (kernel.vm_wbpack)[i];
			auto& pwb = (*mode).gWb[i];
			kernel.createWbByWbDef(wb, i, pwb);
			bpnnCallBack(_pCbFun, _pIns, i + 1, si, ("Net is building globle Wb Struct."));
		}
		Uint alllayers = (*mode).size();
		fordNum = kernel.inputDimension();
		VLI tempLink;
		tempLink.reserve((kernel.vm_layInfo).getLayerMaxNnCountsInNet());
		VLF tempWji;
		tempWji.reserve((kernel.vm_layInfo).getLayerMaxNnCountsInNet());
		for (Uint i = 0; i < alllayers; i++)
		{
			Int upNnSiBase = neuronCountsTillLayer(i);
			auto pl = &(*mode)[i];
			Uint cui = 0, ctt = 0;
			for (Uint j = 0; j < pl->size(); j++) {
				ctt += pl->at(j).nnCounts;
			}
			auto lay = kernel.LayStartNn(i);
			auto laySi = kernel.LayNnCounts(i);
			for (Uint j = 0; j < pl->size(); j++)
			{
				auto plb = &pl->at(j);
				Int gwbi = plb->globleWbId;
				for (Uint k = 0; k < plb->nnCounts; k++)
				{
					auto& nn = lay[cui];
					CLBpKernel::fitId(nn, cs, i, cui);
					kernel.setTransFunc(cs, plb->transFuncType);
					kernel.setWcFunc(cs, plb->wcFuncType);
					cs++;
					cui++;
					//nn.bitFlag = plb->updateFlag;
					Float _bi = 0;
					if (gwbi < 0) {
						wbpack& wb = *newWb();
						nn.wb = wb.index;
						if (plb->bi.size() == 0)
							_bi = getDefaultW();
						else
							_bi = plb->bi.at(0);
					}
					else {
						nn.wb = gwbi;
					}
					tempLink.clear();
					tempWji.clear();
					if (plb->linkPos.size() > 0) {	//按 规则链接						
						Uint m = 0, mcc = 0, msib = plb->linkPos.size();
						for (; m < msib; m++)
						{
							Int vc = plb->linkPos[m];
							if (vc >= 0)vc -= 1;
							if (vc >= fordNum)	//	//检查有没出现超过范围
								continue;
							if (msib < 50) {//小于50个才检查，超过就忽略
								Bool isSkip = false;
								for (Uint l = 0, lsi = tempLink.size(); l < lsi && vc >= 0; ++l) {//检查有没出现重复元素
									if (tempLink[l] == upNnSiBase + vc) {
										isSkip = true;
										break;
									}
								}
								if (isSkip)//找到就跳过，避免重复下标
									continue;
							}
							if (gwbi < 0) {
								tempWji.push_back(plb->linkWij.size() > 0 ? plb->linkWij[m] : getDefaultW());
							}
							tempLink.push_back(vc >= 0 ? upNnSiBase + vc : -1);
							mcc++;
						}
					}
					else {	//向全部上层节点链接
						for (Int m = 0; m < fordNum; m++)
						{
							if (gwbi < 0) {
								tempWji.push_back(getDefaultW());
							}
							tempLink.push_back(upNnSiBase + m);
						}
					}
					//构造
					auto link = make_link(nn, tempLink.size());
					memcpy_s(link, tempLink.size() * sizeof(Int), tempLink.data(), tempLink.size() * sizeof(Int));
					if (gwbi < 0) {
						Uint wjiSi = tempWji.size();
						auto& wb = Wb(nn);
						auto wji = make_wb_Wji_bi(wb, wjiSi);
						memcpy_s(wji, tempWji.size() * sizeof(Float), tempWji.data(), tempWji.size() * sizeof(Float));
						wb_bi(wb) = _bi;
					}
					//本节点完成
					bpnnCallBack(_pCbFun, _pIns, ++cInCreate, allInCreate,
						((i < (*mode).size()) ?
						("Net is building hide layer.") :
							("Net is building output layer."))
					);
				}
			}
			fordNum = laySi;
		}//end for i
	}

	//更新权值层范围数据
	kernel.updateLayWbRange();

	if (kernel.outputDimension() != kernel.LayNnCounts(kernel.lastLayIndex())) {
		auto nn = kernel.LayNnCounts(kernel.lastLayIndex()), od = kernel.outputDimension();
		clearAllDataContainer(0);
		bpnnCallBack(_pCbFun, _pIns, -1, -1, ("Bpnn buildNet: Mismatch between output vector dimension and sample target dimension!"));
		CLString(250).format(("错误：神经网络输出向量维度(%d)与样本目标向量维度(%d)不相同！"), nn, od)
			.messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_range_error();
	}
	//Wb反响链接检查
	bpnnCallBack(_pCbFun, _pIns, -1, -1, 
		CLString().format(("Net construct completed. Neurons: %d, layers: %d."), kernel.neuronCounts(),kernel.layerCounts()).string()
	);
	return *this;
}

Bool CLBpExtend::train(VLF* pOutEa, VLF* pOutLs, VLF* pOutMc, Bpnn::PCBMonitor _pCbFun, PVoid _pIns)
{
	Bpnn::PMCBMonitor pCbFun = *(Bpnn::PMCBMonitor*)& _pCbFun;//回调函数指针    
	Bpnn::PCBMonitor pCbFunSt = (Bpnn::PCBMonitor)_pCbFun;
	Bpnn* m_pInstance = reinterpret_cast<Bpnn*>(_pIns);    //调用对象 
	if (pOutEa)pOutEa->clear();
	if (pOutLs)pOutLs->clear();
	if (pOutMc)pOutMc->clear();

	runTimes = 0;

	Int c = 0, cb = -1;

	if (!(vm_samSets != nullptr && (*vm_samSets).size() > 0)) {
		bpnnCallBack(_pCbFun, _pIns, -1, -1, ("Net training stoped. Sample sets is null."));
		CLString(("警告：未设置训练样本数据集，将跳过本次训练过程！")).messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONWARNING);
		return true;
	}
	if (kernel.inputDimension() != vm_samSets->intputDimension()) {
		bpnnCallBack(_pCbFun, _pIns, -1, -1, ("Net training stoped. Mismatch between model input dimension and sample input dimension!"));
		CLString(250).format(("警告：神经网络输入向量维度(%d)与样本的输入向量维度(%d)不相同，无法执行训练！请调整样本数据集的输入向量维度后重试。"),
			kernel.inputDimension(), vm_samSets->intputDimension()).messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONWARNING);
		return true;
	}
	if (vm_samSets->targetDimension() != kernel.outputDimension()) {
		bpnnCallBack(_pCbFun, _pIns, -1, -1, ("Net training stoped. Mismatch between model output dimension and sample target dimension!"));
		CLString(250).format(("警告：神经网络输出向量维度(%d)与样本的目标向量维度(%d)不相同，无法执行训练！请调整样本数据集的目标向量维度后重试。"),
			kernel.outputDimension(), vm_samSets->targetDimension()).messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONWARNING);
		return true;
	}
	if (bOpenGraph) {
		_er.clear(), _ls.clear(), _mc.clear(), _cr.clear();
		_er.reserve(maxTimes + 1);
		//if (bAutoFit)
		if (pFAutoFit)
			_ls.reserve(maxTimes + 1), _mc.reserve(maxTimes + 1);
		if (isCorrectRateEvaluationModel())
			_cr.reserve(maxTimes + 1);
	}

	Bool rt = false;
	if (!doPrepair(_pCbFun, _pIns))	//	没准备好直接退出
		goto end1;
	if (bAmpSupport) {//Gpu加速		
#if UseCppAmp > 0
		Bool isBad = false;
		try {
			rt = __runByHardwareSpeedup2(pOutEa, pOutLs, pOutMc, _pCbFun, _pIns);
		}
		catch (const std::runtime_error & e) {
			printf(e.what());
			isBad = true;
		}
		if (isBad)
			goto else1;
#else
		auto yn = CLString("警告：当前BpnnTool工具并不支持大规模并行(GPU)加速，是否采用替代方案？\nYes    ：采用多线程\nNo     ：采用单线程\nCancel ：退出训练")
			.messageBox(_lpBpnnMsgBoxTitle, MB_ICONWARNING | MB_YESNOCANCEL);
		setGpuAcceleratedSupport(false);
		if (yn == IDYES) {
			setMultiThreadSupport(1);
			goto else1;
		}
		else if (yn == IDNO)
		{
			setMultiThreadSupport(0);
			goto else1;
		}
		else
		{
			goto end1;
		}
#endif
	}
	else {
	else1:
		if(m_KeepGoWhenNanInfTimes == 0)
			vm_wji_bk = vm_wji_Data;
		auto cid = GetCurrentThreadId();
		for (;;)
		{
			if (m_trainProcControl == TPC_Disable)
				goto end1;
			else if (m_trainProcControl == TPC_Pause) {
				while (m_trainProcControl != TPC_Resume) {
					if (m_trainProcControl == TPC_Disable)
						goto end1;
					Sleep(0);
				}
			}
			try { //捕获异常，重新执行训练
				rt = _trainOnce();
			}
			catch (const std::invalid_argument& ep) {
				//精确判断异常类型及其值
				if (ep.what() == string("ErNan") 
					|| ep.what() == string("ErInf")
					|| ep.what() == string("checkErValid")
					) {	
					const Float dlay = RAND_F_A_B(0.1, 0.3);
					int rid = IDYES;
					if (bIsCheckKeepGoWhenNanInf) {
						char _str[MAX_PATH];
						sprintf_s(_str, "\
模型训练过程出现数据异常，模型将减小学习率ls = ( %g -> %g )来修复，并重算（ 第 %d 次 ）？\
\nYes：     确定重算，并且不再提示该信息窗\
\nNo：      确定重算\
\nCancel：取消\n\
\nEpoch info:\
\nTimes: %d\
\nError: %g\
\nCoRat: %.2f %%\
"
						, getLs(), getLs() * dlay, m_KeepGoWhenNanInfTimes + 1
						,getRunTimes(),getEr(),getSavedCorrectRate());
						rid = messageBoxTimeoutA(0, _str,_lpBpnnMsgBoxTitle, MB_ICONWARNING|MB_YESNOCANCEL,10*1000);
						if (rid == IDTIMEOUT) { rid = IDYES; }
						else if (rid == IDYES) { bIsCheckKeepGoWhenNanInf = false; }
					}
					if (rid == IDCANCEL || m_KeepGoWhenNanInfTimes >= 15) {
						bpnnCallBack(_pCbFun, _pIns, -1, -1, "[Error]: Data exception occurred during training!");
						if (m_KeepGoWhenNanInfTimes >= 15) {
							messageBoxTimeoutA(0,"提示：经过多次学习率调整，模型仍然发生数据异常现象，请检查：\n1、传递函数类型是否发散\n2、权值初始化设置影响\n3、或通过调整模型结构改变计算特性", 
								_lpBpnnMsgBoxTitle, MB_ICONERROR | MB_OK, 60 * 1000);
						}
						m_KeepGoWhenNanInfTimes = 0;		
						m_runTimesFromBuild = 0;
						throw std::runtime_error(ep.what());
					}
					else {
						m_KeepGoWhenNanInfTimes++;
						bpnnCallBack(_pCbFun, _pIns, -1, -1, 
							CLString().format("[Waring]: Times= %d'st.The model automatically adjust learning rate ( %g -> %g ) and retry!", 
								m_KeepGoWhenNanInfTimes,getLs(), getLs() * dlay).string());
						//cout << vm_wji_bk[0] << "," << vm_wji_bk[1] << "," << vm_wji_bk[2] << endl;;
						setParam(getLs() * dlay, getAccuracy(), getMc());
						//vm_wji_Data = vm_wji_bk;
						//权值随机加减5%
						for (size_t i = 0, si = vm_wji_bk.size(); i < si; i++)
						{
							auto& ia = vm_wji_bk[i];
							vm_wji_Data[i] = ia = RAND_F_A_B(ia * 0.95, ia * 1.05);
						}
						vm_bnData.clear();
						bIsCheckLinkBk =false;
						auto trt = train(pOutEa, pOutLs, pOutMc, _pCbFun, _pIns);
						//m_KeepGoWhenNanInfTimes--;
						return trt;
					}
				}
			};

			if (pOutEa)
				pOutEa->push_back(getEr());
			if (pOutLs)
				pOutLs->push_back(getLs());
			if (pOutMc)
				pOutMc->push_back(getMc());
			if (bOpenGraph) {
				_er.push_back(getEr());
				if (pFAutoFit)
					_ls.push_back(getLs()), _mc.push_back(getMc());
				if (isCorrectRateEvaluationModel())
					_cr.push_back(getSavedCorrectRate());
			}
			if (pCbFunSt) {//外显步骤计算
				if (m_pInstance) {
					if (pCbFun)(m_pInstance->*pCbFun)(Int(runTimes), Int(maxTimes), "Net is training."); //类对象内部函数作为外显回调的情况
				}
				else {
					(*pCbFunSt)(Int(runTimes), Int(maxTimes), "Net is training."); //全局或静态函数作为外显回调的情况
				}
			}
			if (rt)
				goto end1;
			if (runTimes >= maxTimes)
			{
				rt = false; goto end1;
			}
			if (!prepairTrainSamUsageData()) {//为下一循环做准备
				rt = false; goto end1;
			}
		}//end for
	}
end1:
	setTrainState(TPC_Enable);
	if (bOpenGraph) {
		er.insert(er.end(), _er.begin(), _er.end());
		if (pFAutoFit) {
			ls.insert(ls.end(), _ls.begin(), _ls.end());
			mc.insert(mc.end(), _mc.begin(), _mc.end());
		}
		if (isCorrectRateEvaluationModel())
			cr.insert(cr.end(), _cr.begin(), _cr.end());
	}
	if (rt) {		
		//达到评价标准释放多余空间
		releaseTrainDataContainer();
		releaseStdVector(_er); 
		releaseStdVector(_ls);
		releaseStdVector(_mc); 
		releaseStdVector(_cr);
		bpnnCallBack(_pCbFun, _pIns, -1, -1,
			isCorrectRateEvaluationModel() ?
			CLString().format("Net training epoch completed with achieve accuracy. CorrectRate(%.2f%%) >= TagCorrectRate(%.2f%%)", getSavedCorrectRate() * 100, m_CorrectRate * 100).string() :
			CLString().format("Net training epoch completed with achieve accuracy. Er(%g) <= Accuracy(%g)", getEr(), getAccuracy()).string()
		);
	}
	else {
		bpnnCallBack(_pCbFun, _pIns, -1, -1, "Net training epoch completed.");
	}
	return rt;
}

Bool CLBpExtend::doPrepair(PVoid _pCbFun, PVoid _pIns) {
	//检查 
	if (kernel.layerCounts() < Minimum_Layers)
		return false;
	if (!prepairTrainSamUsageData())
		return false;
	vm_samSets = train_samSets;
	vm_samUsage = &train_samUsage;

	//后向检查
	if (bIsCheckLinkBk == false) {
		vm_bi_dt_old_Data.clear(), vm_bi_dt_old_Data.resize((kernel.vm_wbpack).size());
		vm_wji_dt_Data.clear(), vm_wji_dt_Data.resize((kernel.vm_wji_Data).size());
		vm_wji_dt_old_Data.clear(), vm_wji_dt_old_Data.resize((kernel.vm_wji_Data).size());

		checkWbPackShareLinkRange(_pCbFun, _pIns);

		if (!checkNeuronLinkSuspended(_pCbFun, _pIns))
			return false;		

		bIsCheckLinkBk = true;

		//Float wji[] = { -0.87,0.37,0.001,0.51,-0.702,-0.764,-0.31,0.5,0.75,-0.24 };
		//memcpy_s(vm_wji_Data.data(), sizeof(wji), wji, sizeof(wji));
	}

	//做一次数据容器准备
	auto sl = kernel.layerCounts();
	auto samn = (*vm_samSets).size();
	if (train_useSamCounts > 0 && train_useSamCounts <= samn)
		samn = train_useSamCounts;
	if (vm_grad.size() / (Uint)kernel.neuronCounts() < samn) {
		buildTrainDataContainer(-1, samn);
	}
	if (vm_xi.size() / (Uint)kernel.neuronCounts() < samn) {
		buildTrainDataContainer(samn, -1);
	}
	//清除dropout标记
	dpRepeatTrainTimesC = 0;
	for (Uint i = 0, si = kernel.layerCounts(); i < si; i++)
	{
		auto lay = kernel.LayStartNn(i);
		auto laySi = kernel.LayNnCounts(i);
		for (Uint j = 0, sj = laySi; j < sj; j++)
		{
			lay[j].bitFlag &= (~FG_NN_Dropout);
		}
	}
	vm_drop.clear();
	vm_drop.resize(kernel.layerCounts(), 0.0);

	checkWbPackShareBnData(_pCbFun, _pIns);	

	//设置运行模式，基于samUsage，samSets
	Uint ncore = work.getCpuCoreCounts();
	if (ncore < 4 || (kernel.vm_globleInfo.iMaxLayNCounts + 1 < ncore && vm_samUsage->size() + 1 < ncore)) {
		setMultiThreadSupport(0);
	}
	work.method = work.getWorkType(vm_samUsage->size());
	

	if (Uint(m_mutiTrdSupportPerc * ncore) >= 4) {
		//启动线程
		if (!work.isRunning()) {
			if (!work.start(m_mutiTrdSupportPerc)) {
				Int id = CLString(("设备CPU核心数可能不支持多线程模式分析，是否采用单线程继续分析 [Yes/No] ！")).messageBox(_lpBpnnMsgBoxTitle, MB_ICONWARNING | MB_YESNO, 0);
				if (id == IDYES) {
					setMultiThreadSupport(0);
				}
				else return false;
			}
		}
		work.ws_samSi = vm_samUsage->size();
		work.ws_wbSi = kernel.vm_wbpack.size();
		if (kernel.vm_bnMi > 0) //bn模式变换线程组到bn模式
			work.setPermissions(PERS_TURN_BN);
		else
			work.setPermissions(PERS_TURN_NORM);
	}

	//设置原子梯度模式
	if (Uint(m_mutiTrdSupportPerc * ncore) >= 4 && work.method == 1) {
		//多线程且为节点行多线程启动梯度原子模型
		vector<vector<Uint>> linkbk((kernel.vm_layInfo).getLayerMaxNnCountsInNet());
		for (Uint i = 1, si = kernel.layerCounts(); i < si; i++)
		{
			bpnnCallBack(_pCbFun, _pIns, i, si - 1, ("Net is checking back link."));
			auto lay = kernel.LayStartNn(i - 1);
			auto laySi = kernel.LayNnCounts(i - 1);
			auto lay2 = kernel.LayStartNn(i);
			auto lay2index0 = kernel.LayStartNnIndex(i);
			auto layindex0 = kernel.LayStartNnIndex(i - 1);
			auto lay2Si = kernel.LayNnCounts(i);
			linkbk.clear(); linkbk.resize(laySi);
			auto baseNnSi = neuronCountsTillLayer(i);
			for (Uint j = 0, sj = lay2Si; j < sj; j++)
			{
				auto& p2 = lay2[j];
				auto pnn2_link = plink(p2);
				for (Uint k = 0, sk = link_size(p2); k < sk; k++)
				{
					auto ik = pnn2_link[k];//全局码
					if (ik < 0)
						continue;
					auto& plbk = linkbk[size_t(ik) - baseNnSi];
					plbk.push_back(Uint(lay2index0 + j));
				}
			}
			//设置
			for (Uint j = 0, sj = laySi; j < sj; j++) {
				if (linkbk[j].size() > 1)
					vm_grad[layindex0 + j].setUseAtomic(true);
				else
					vm_grad[layindex0 + j].setUseAtomic(false);
			}
		}

	}
	else {
		//不采用原子模型（提高效率）
		for (auto& i : vm_grad) {
			i.setUseAtomic(false);
		}
	}

	//设置一次内部数据
	setFordParam(&vm_xi[0], &vm_yi[0], vm_xy_span, &vm_neuronExData, vm_grad_span);
		
	gErEquitTimes = 0;

	return true;
}

#if UseCnnTestOpen > 0
int ctimes = 1;
std::string cnn_filePath = "D:/Documents/Desktop/cnn/";
Float* pTagYi = nullptr;
Float* pTagXi = nullptr;
CLAtomic<Float>* pTagGrad = nullptr;
Float* pTagWji1 = nullptr;
template<class T>
Bool cmpPrintf(Float Er, Uint lay, const char* inf, T* pTag, T* pyi, Uint cmpSi, Bool isDot = false, Uint pyiXspan = 0, Uint pyiYspan = 0) {
	CLString str; Uint lj = 0;
	str.format("\n%s error, lay = %d: ", inf, lay);
	for (Uint i = 0; i < cmpSi; i++)
	{
		T& t = pTag[i];
		T& y = pyi[i];
		if (isDot) {
			t = pTag[(i / pyiXspan) + (i % pyiXspan) * pyiYspan];
		}
		Float dt = t == Float(y) ? 0 : y != 0.0f ? abs((t - Float(y)) / y) : 999;
		if (dt > Er) {
			str << "\n pos= " << i + 1 << " / " << cmpSi << " : pnn(" << Float(t) << ") != pyi(" << Float(y) << ") delta(" << (t - Float(y)) << ") Er(" << dt * 100 << "%) > " << Er * 100 << "%";
			if (++lj >= 10)
				break;
		}
	}
	if (lj)
		cout << str() << endl;
	return lj ? true : false;
}

#endif

Bool CLBpExtend::_trainOnce()
{
#if UseCnnTestOpen > 0
	if (ctimes == 1) {
		CLFileBinSerial bin;
		if (bin.open(string(cnn_filePath + "cnn_weight.cnn").c_str())) {
			bin.read((char*)kernel.vm_wji_Data.data(), kernel.vm_wji_Data.size() * sizeof(Float));
		}
		else if (bin.open(string(cnn_filePath + "cnn_weight.cnn").c_str(), true)) {
			bin.write((const char*)kernel.vm_wji_Data.data(), kernel.vm_wji_Data.size() * sizeof(Float));
		}

		if (bin.open(string(cnn_filePath + "cnn_yi1.cnn").c_str())) {
			if(!pTagXi)pTagXi = new Float[kernel.neuronCounts() * xy_span];
			if(!pTagYi)pTagYi = new Float[kernel.neuronCounts() * xy_span];
			if(!pTagGrad)pTagGrad = new CLAtomic<Float>[vm_grad.size()];
			if(!pTagWji1)pTagWji1 = new Float[vm_wji_Data.size()];
			bin.read((char*)pTagXi, kernel.neuronCounts() * xy_span * sizeof(Float));
			bin.read((char*)pTagYi, kernel.neuronCounts() * xy_span * sizeof(Float));
			bin.read((char*)pTagGrad, vm_grad.size() * sizeof(CLAtomic<Float>));
			bin.read((char*)pTagWji1, vm_wji_Data.size() * sizeof(Float));
		}
	}
#endif
	
	updateDropout();

	if (!(Uint(m_mutiTrdSupportPerc * work.getCpuCoreCounts()) >= 4)) {
		//单线程
		auto samSi = vm_samUsage->size();

#if UseXyTestOpen > 0
		cout << "\n\nLay Forward---------------------------------------------------------------------------------------\n";
#endif
		
		//前向执行
		if (kernel.vm_bnMi > 0) {
			for (Uint lay = 0, laySi = kernel.layerCounts(); lay < laySi; lay++)//正向执行,第一层
			{
				
				auto posSi = kernel.LayNnCounts(lay);
				if (vm_bnLayOpen[lay] != 0) {//本层采用bn非drop方案
					//1）：前向输入数据
					forward_lay_bn_xi(lay, 0, posSi, 0, samSi);

#if UseXyTestOpen > 0
					auto index0 = kernel.LayStartNnIndex(lay);
					cout << "\n" << lay << ": xi = [ ";
					for (Uint ti = 0; ti < posSi; ti++) {
						cout << pxi[(index0 + ti) * xy_span] << ", ";
					}
					cout << " ]";
#endif
					//2）：更新权值的u和a2
					auto& layInfo = kernel.vm_layInfo[lay];
					forward_lay_bn_UpdateParam(layInfo.iLayWbStartIndex, layInfo.iLayWbStartIndex + layInfo.iLayWbCounts);

#if UseBnTestOpen > 0
					cout << "\n" << lay << ": bn = [ ";
					for (Uint ti = layInfo.iLayWbStartIndex; ti < layInfo.iLayWbStartIndex + layInfo.iLayWbCounts; ti++) {
						auto& bn = vm_bnData[ti];
						cout << "\n  ( ri= " << bn.ri << ", bt= " << bn.bt << ", Eu= " << bn.Eu << ", Ea2= " << bn.Ea2 << ", mi= " << bn.mi << ", ci= " << bn.ex->ci << ", nSiT= " << bn.ex->nSiTimes
							<< " ), xi_hat = " << bn.ex->xi_h[bn.ex->ci] <<  " -> yi_h = " << bn.getYi_h(ti);
					}
					cout << "\n   ]";
#endif

					//3）：提取真实变换的Yi
					forward_lay_bn_yi(lay, 0, posSi, 0, samSi);

#if UseXyTestOpen > 0
					cout << "\n" << lay << ": yi = [ ";
					for (Uint ti = 0; ti < posSi; ti++) {
						cout << pyi[(index0 + ti) * xy_span] << ", ";
					}
					cout << " ]\n";
#endif

				}
				else { //采用非bn非drop方案					
					forward_lay_bn_no(lay, 0, posSi, 0, samSi);
				}
			}
		}
		else {
			forward(0, samSi);
		}

		++m_runTimesFromBuild;
		++runTimes;
		
		Er();

		if (convergenceHasBeenAchieved())
			return true;

		autoFitParam();//自适应调整参数

		zeroGradData();

		//梯度传递
		if (kernel.vm_bnMi > 0) {
#if UseXyTestOpen > 0
			cout << "\n\nbackard: \n";
#endif

			for (Int lay = Int(kernel.layerCounts()) - 1; lay >= 0; lay--) //反向修正
			{
				auto posSi = kernel.LayNnCounts(lay);
				if (vm_bnLayOpen[lay] != 0) { //本层采用bn非drop方案
					//1）：压入梯度
					gradient_lay_bn_pushGrad(lay, 0, posSi, 0, samSi);

#if UseXyTestOpen > 0
					auto index0 = kernel.LayStartNnIndex(lay);
					cout << "\n" << lay << ": grad_in = [ ";
					for (Uint ti = 0; ti < posSi; ti++) {
						cout << vm_grad[(index0 + ti) * xy_span]() << ", ";
					}
					cout << " ]";
#endif

					//2）：构造梯度传递参数
					auto& layInfo = kernel.vm_layInfo[lay];
					gradient_lay_bn_createParam(layInfo.iLayWbStartIndex, layInfo.iLayWbStartIndex + layInfo.iLayWbCounts);

#if UseBnTestOpen > 0
					cout << "\n" << lay << ": grad_in = [ ";
					for (Uint ti = layInfo.iLayWbStartIndex; ti < layInfo.iLayWbStartIndex + layInfo.iLayWbCounts; ti++) {
						auto& bn = vm_bnData[ti];
						cout << "\n  ( dl_da2= " << bn.ex->dl_da2 << ", dl_du= " << bn.ex->dl_du << ", u= " << bn.ex->u << ", a2= " << bn.ex->a2 << " )";
					}
					cout << "\n   ]";
#endif

					//dy[pos] * ri* dsqrta2 + dl_da2 * 2.0 * (xi[pos] - u) / si + dl_du / si;
					//3）：传递梯度到上层
					gradient_lay_bn_sendGrad(lay, 0, posSi, 0, samSi);

#if UseXyTestOpen > 0
					cout << "\n" << lay << ": grad_out = [ ";
					for (Uint ti = 0; ti < posSi; ti++) {
						cout << vm_grad[(index0 + ti) * xy_span]() << ", ";
					}
					cout << " ]";
#endif

					//4）：更新bn数据参数
					gradient_lay_bn_UpdateParam(layInfo.iLayWbStartIndex, layInfo.iLayWbStartIndex + layInfo.iLayWbCounts);
					
#if UseBnTestOpen > 0
					cout << "\n" << lay << ": update r,b = [ ";
					for (Uint ti = layInfo.iLayWbStartIndex; ti < layInfo.iLayWbStartIndex + layInfo.iLayWbCounts; ti++) {
						auto& bn = vm_bnData[ti];
						cout << "\n( ri= " << bn.ex->ri << ", bt= " << bn.ex->bt << " ),";
					}
					cout << "\n   ]\n";
					if (getRunTimes() > 30)
						Sleep(0);
#endif
				}
				else {
					gradient_lay_bn_no(lay, 0, posSi, 0, samSi);
				}
			}			
		}
		else {
			gradient(0, samSi);			
		}

		//更新权值
		modify_wi_and_bi(samSi, 0, (Uint)kernel.vm_wbpack.size());

		checkErValid();

#if UseCnnTestOpen > 0
		if (ctimes++ == 1) {
			CLFileBinSerial bin;
			if (bin.open(string(cnn_filePath + "cnn_yi1.cnn").c_str())) {
			}
			else if (bin.open(string(cnn_filePath + "cnn_yi1.cnn").c_str(), true)) {
				bin.write((const char*)pxi, kernel.neuronCounts() * xy_span * sizeof(Float));
				bin.write((const char*)pyi, kernel.neuronCounts() * xy_span * sizeof(Float));
				bin.write((const char*)vm_grad.data(), vm_grad.size() * sizeof(CLAtomic<Float>));
				bin.write((const char*)vm_wji_Data.data(), vm_wji_Data.size() * sizeof(Float));
			}
			Sleep(0);
		}
#endif

		return false; //返回假
	}
	else {	//	多线程

		checkMultiThreadStartup();

		if (kernel.vm_bnMi > 0) {
			for (Uint lay = 0, laySi = kernel.layerCounts(); lay < laySi; lay++)//正向执行,第一层
			{
				work.pclaySize = kernel.LayNnCounts(lay);
				work.clayIndex = lay;
				if (vm_bnLayOpen[lay] != 0) { //本层采用bn非drop方案
					//1）：前向输入数据
					work.setPermissions(PERS_BN_FORD_XI);
					//2）：更新权值的u和a2
					work.setPermissions(PERS_BN_FORD_UPDATE);
					//3）：提取真实变换的Yi
					work.setPermissions(PERS_BN_FORD_YI);
				}
				else {
					work.setPermissions(PERS_BN_FORD_NO);
				}
			}
		}
		else {
			if (work.method == 0) {
				work.setPermissions(PERS_FORD);
			}
			else {
				//启动向前循环			
				for (Uint lay = 0, laysi = kernel.layerCounts(); lay < laysi; lay++) //反向修正
				{
					//work.pclay = kernel.LayStartNn(i);
					work.pclaySize = kernel.LayNnCounts(lay);
					work.clayIndex = lay;
					work.setPermissions(PERS_FORD);
				}
			}
		}

#if UseCnnTestOpen > 0
		if (ctimes == 1) {
			for (Uint lay = 0, laysi = kernel.layerCounts(); lay < laysi; lay++) //反向修正
			{
				auto cmppos = kernel.LayStartNnIndex(lay) * xy_span;
				auto cmpsi = kernel.LayNnCounts(lay) * xy_span;
				if(pTagXi && pxi)cmpPrintf(0.0001, lay, "Xi check", &pTagXi[cmppos], &pxi[cmppos], cmpsi);
				if(pTagYi)cmpPrintf(0.0001, lay, "Yi check", &pTagYi[cmppos], &pyi[cmppos], cmpsi);
			}
			Sleep(0);
		}
#endif

		++m_runTimesFromBuild;
		++runTimes;

		Er();
		
		if (convergenceHasBeenAchieved())
			return true;

		autoFitParam();//自适应调整参数

		zeroGradData();

		if (kernel.vm_bnMi > 0) {
			for (Int lay = Int(kernel.layerCounts()) - 1; lay >= 0; lay--) //反向修正
			{
				work.pclaySize = kernel.LayNnCounts(lay);
				work.clayIndex = lay;
				if (vm_bnLayOpen[lay] != 0) { //本层采用bn非drop方案
					//1）：压入梯度
					work.setPermissions(PERS_BN_GRAD_PUSH);
					//2）：构造梯度传递参数
					work.setPermissions(PERS_BN_GRAD_CREATE);
					//3）：传递梯度到上层
					work.setPermissions(PERS_BN_GRAD_SEND);
					//4）：更新bn数据参数
					work.setPermissions(PERS_BN_GRAD_UPDATE);
				}
				else {
					work.setPermissions(PERS_BN_GRAD_NO);
				}
			}
		}
		else {
			if (work.method == 0) {
				work.setPermissions(PERS_GRAD);
			}
			else {
				//启动反响梯度修正
				for (Int lay = kernel.layerCounts() - 1; lay >= 0; lay--) //反向修正
				{
					work.pclaySize = kernel.LayNnCounts(lay);
					work.clayIndex = lay;
					work.setPermissions(PERS_GRAD);
				}
			}
		}

		work.setPermissions(PERS_MODIFY);

		//检查Er变化
		checkErValid();

#if UseCnnTestOpen > 0
		if (ctimes++ == 1) {
			auto pgrad = vm_grad.data();
			for (Int lay = kernel.layerCounts()-1; lay >= 0; lay--) //反向修正
			{
				auto cmppos = kernel.LayStartNnIndex(lay) * vm_grad_span;
				auto cmpsi = kernel.LayNnCounts(lay) * vm_grad_span;
				if (pTagGrad)cmpPrintf(0.0001, lay, "Grad check", &pTagGrad[cmppos], &pgrad[cmppos], cmpsi);
			}
			if (pTagWji1)cmpPrintf(0.0001, 0, "Wji2 check", pTagWji1, vm_wji_Data.data(), vm_wji_Data.size());
			Sleep(0);
		}
#endif

		return false; //返回假
	}
	return false;
}

Uint CLBpExtend::getNeuronMenSize()
{
	return sizeof(neuron);
}

static const int encDeep = 10;
Bool CLBpKernel::writeBpnnToFile(PCStr lpFileFullPathName, Bool binMode, Bool _encrypteMod) {
	if (!lpFileFullPathName) {
		return false;
	}
	if (binMode == false) {
		const ULONGLONG limitSi = 16 * 1024 * 1024;
		CLString tt(limitSi);
		
		tt % (encrypteMod ? _lpBpNnEncryptedHear : _encrypteMod ? _lpBpNnEncryptedHear : _lpBpNnWijFileFlag);
		if (!CLString::createDirectoryByFileName(lpFileFullPathName) ||
			tt.writeToFile(lpFileFullPathName, CREATE_ALWAYS) == FALSE)
			return false;
		tt.closeFile();
		tt % ("\r\n\r\n\r\n#网络头部数据，意义注释：\r\n#[ 损失函数类型，所有神经元总数，网络层数，输入向量维度，输出向量维度，Wb结构个数，数据生成时间戳，每层详细信息... ]\r\n")
			<< vm_LossFuncType
			<< CLComma << neuronCounts()
			<< CLComma << layerCounts()
			<< CLComma << inputDimension()
			<< CLComma << outputDimension()
			<< CLComma << (vm_wbpack.size())
			<< CLComma << CLTime::getLocalTime_ll();
		for (const auto& lay : vm_layInfo)
		{
			tt << CLComma << lay.bIsEndLay;
			tt << CLComma << lay.iLayIndex;
			tt << CLComma << lay.iLayNnCounts;
			tt << CLComma << lay.iLayNnStartIndex;
			tt << CLComma << lay.iLayNnEndIndex;
		}
		tt << ("\r\n\r\n\r\n#Wb参数结构库，意义注释：\r\n#每行为一个Wb结构定义 [序号，阈值bi，权值个数Wn，权值向量< w1, w2 , ... , wN >]");
		class _Exeption {
		public:
			Bool flag;
			_Exeption(Bool flag = true) {
				this->flag = flag;
			}
		};
		tt << CLRet;
		//写权值
		try {
			for (Uint i = 0, si = (vm_wbpack).size(); i < si; i++)
			{
				auto& p = vm_wbpack[i];
				tt << i << CLComma << p.bFlag << CLComma << wb_bi(p) << CLComma << wb_Wji_size(p);
				for (Uint j = 0, sj = wb_Wji_size(p); j < sj; j++)
				{
					tt << CLComma << wb_pWji(p)[j];
				}
				//tt.writeLineToFile();
				tt << CLRet;
				if (tt.size() > limitSi)
					throw _Exeption(false);
			}

			tt << ("\r\n\r\n\r\n#网络结构数据，意义注释：\r\n#每行为一个神经元定义 [ 本层所含神经元总数，本神经元层编号，本神经元位置编号，激活函数类别TransFunc，权值组合类型WcFuncType，更新标记，本神经元对前层神经元的链接标记向量的维度N_link，链接向量< l1 , l2, ... , lN >，权值结构编号 ]");
			tt << CLRet;
			//写节点
			for (Uint i = 0; i < layerCounts(); i++)
			{
				auto p = LayStartNn(i);
				auto pSi = LayNnCounts(i);
				Uint nns = pSi;
				for (Uint j = 0; j < pSi; j++)
				{
					neuron* p1 = &p[j];
					tt << nns << CLComma << (i) << CLComma << (j) << CLComma << p1->transFuncType << CLComma << p1->wcFuncType << CLComma << p1->bitFlag;
					//写链接
					tt << CLComma << (link_size(*p1));
					for (Uint k = 0; k < link_size(*p1); k++)
					{
						tt << CLComma << (plink(*p1)[k]);
					}
					tt << CLComma << (Wb(*p1).index);
					tt << CLRet;
					if (tt.size() > limitSi)
						throw _Exeption(false);
				}
			}
			//tt.closeFile();
		}
		catch (const _Exeption & e) {
			::DeleteFileA(lpFileFullPathName);
			auto rt = ::messageBoxTimeoutA(0, "要保存的模型文本数据大于16MB，无法采用文本文件保存。是否改用二进制文件保存模型？ \nYes：用二进制方式继续保存 \nNo：不保存文件", 
				_lpBpnnMsgBoxTitle, MB_ICONINFORMATION | MB_YESNO, 60 * 1000);
			if (rt == IDNO)
				return e.flag;
			else
				goto bin2;
		}
		if (encrypteMod || _encrypteMod) {
			CLString stem,enc;
			stem = encrypteMod ? encrypteMod : RAND_I_A_MAX(10);
			tt.insert(0, stem);
			enc.encrypteString(tt, encDeep);
			enc.writeToFile(lpFileFullPathName, OPEN_EXISTING);
			enc.closeFile();
		}
		else { 
			tt.writeToFile(lpFileFullPathName,OPEN_EXISTING); 
			tt.closeFile();
		}
	}
	else {
		bin2:
		CLString file = lpFileFullPathName;
		file.replaceExtName((".bpnn"));
		if (file.fileExists())
			DeleteFile(file);
		CLFileBinSerial tt(file, true);
		if (tt.is_open() == false)
			return false;
		tt.seekg(ios::beg);
		tt << vm_LossFuncType
			<< neuronCounts()
			<< layerCounts()
			<< inputDimension()
			<< outputDimension()
			<< (vm_wbpack).size()
			<< CLTime::getLocalTime_ll();
		//写入层信息
		for (Uint i = 0; i < layerCounts(); i++)
		{
			tt << vm_layInfo[i];
		}
		//写入加密信息
		tt << (encrypteMod > 0 ? encrypteMod : _encrypteMod ? RAND_I_A_MAX(10) : 0ui64);
		//写权值
		for (Uint i = 0, si = vm_wbpack.size(); i < si; i++)
		{
			auto& p = vm_wbpack[i];
			auto wjiSi = wb_Wji_size(p);
			tt << i << p.bFlag << wb_bi(p) << wjiSi;
			for (Uint j = 0, sj = wb_Wji_size(p); j < sj; j++)
			{
				tt << wb_pWji(p)[j];
			}
		}
		//写节点
		for (Uint i = 0; i < layerCounts(); i++)
		{
			auto p = LayStartNn(i);
			auto pSi = LayNnCounts(i);
			Uint nns = pSi;
			for (Uint j = 0; j < nns; j++)
			{
				neuron* p1 = &p[j];
				tt << nns << (i) << (j)
					<< p1->transFuncType << p1->wcFuncType << p1->bitFlag;
				//写链接
				tt << (link_size(*p1));
				for (Uint k = 0; k < link_size(*p1); k++)
				{
					tt << (plink(*p1)[k]);
				}
				tt << (Wb(*p1).index);
			}
		}
	}
	return true;
};

Bool CLBpKernel::readBpnnFormFile(PCStr lpFile, Bool binMode)
{
	Bool rt = false;
	Int i = 0, ci = 0;
	TIMESTAMP ctime;
	Uint wijSi = 0;
	Uint unitCount = 0;
	Int cLayer = 0, cPos = 0;
	Uint cs = 0;
	Uint wsi = 0;
	Uint laynns = 0;
	Uint linkSi = 0;
	Uint wbSi = 0;
	Uint wbi = 0;
	CLString file = lpFile;
	VLF tempWji;
	VLI tempLink;
	Bool isEnc = false;
	CLString stren;
	Uint eni = 0;
	Uint enSi = 0;
	if (binMode == false) {
		if (file.fileExists() == FALSE)
			return false;
		encrypteMod = 0;
		clearContainer();
		CLString tt(500);
		while (
			isEnc ? (++eni < enSi ? ((tt = stren.vtAt(eni)),true) : false):
			(tt.readLineFromFile(i++ == 0 ? file.string() : nullptr) != -1)			
			) {
			CLRemoveNotes(tt);
			if (tt.size() == 0)
				continue;
			if (ci == 0) { //文件启动判断
				//CLString stemp = _lpBpNnEncryptedHear;
				//stemp.trim();
				if (tt == _lpBpNnWijFileFlag) {
					//rt = false; break;
				}
				else if (tt == CLString(_lpBpNnEncryptedHear).trim()) {
					isEnc = true;
					tt.closeFile();
					tt.readFile(file.string());
					tt.trimLeft(_lpBpNnEncryptedHear);
					stren.unEncrypteString(tt,encDeep);
					stren.split(CLRet);
					enSi = stren.vtSize();
					encrypteMod = atoll(stren.vtAt(0));
					eni = 0;
				}
				else {
					rt = false; break;
				}
				ci = 1;
			}
			else if (ci == 1) {//读取全局数据
				tt* CLComma >> vm_LossFuncType >> vm_globleInfo.iNCounts >> vm_globleInfo.iLayCounts >> vm_inputDim >> vm_outputDim >> wbSi >> ctime;
				setLossFunc(vm_LossFuncType);
				vm_layInfo.resize(layerCounts());
				for (auto& lay : vm_layInfo)
				{
					tt >> lay.bIsEndLay;
					tt >> lay.iLayIndex;
					tt >> lay.iLayNnCounts;
					tt >> lay.iLayNnStartIndex;
					tt >> lay.iLayNnEndIndex;
				}
				clearContainer(neuronCounts());
				if (wbSi > 0)
					vm_wbpack.resize(wbSi), ci = 3;//读取wb
				else
					ci = 2;
			}
			else if (ci == 3)//取得wb包
			{

				auto& pwb = vm_wbpack[wbi];
				pwb.reset();
				wsi = 0;
				Float _bi = 0;
				tt* CLComma >> pwb.index >> pwb.bFlag >> _bi >> wsi;
				tempWji.resize(wsi);
				for (Uint wi = 0; wi < wsi; wi++)
				{
					tt >> tempWji[wi];
				}
				auto pwji = make_wb_Wji_bi(pwb, wsi);
				memcpy_s(pwji, wsi * sizeof(Float), tempWji.data(), wsi * sizeof(Float));
				wb_bi(pwb) = _bi;
				if (++wbi >= wbSi)
					ci = 2;
			}
			else if (ci == 2) {//读取输入节点数据
				laynns = 0;
				tt* CLComma >> laynns >> cLayer >> cPos;
				auto& nn = LayStartNn(cLayer)[cPos];
				fitId(nn, cs, cLayer, cPos);
				tt >> nn.transFuncType >> nn.wcFuncType >> nn.bitFlag;
				setTransFunc(cs, nn.transFuncType);
				setWcFunc(cs, nn.wcFuncType);
				cs++;
				linkSi = 0;
				tt >> linkSi;
				tempLink.resize(linkSi);
				for (Uint k = 0; k < linkSi; k++)
				{
					tt >> tempLink[k];
				}
				auto link = make_link(nn, linkSi);
				memcpy_s(link, linkSi * sizeof(Int), tempLink.data(), linkSi * sizeof(Int));
				tt >> nn.wb;
				if (nn.wb > vm_wbpack.size()) {
					clearContainer();
					CLString(("权值参数结构指针被修改，文件已失效！")).messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR | MB_YESNO).throw_runtime_error();
				}
				ci = 2;
				unitCount++;
			}
		}
		tt.closeFile();
	}
	else {
		file.replaceExtName((".bpnn"));
		if (file.fileExists() == FALSE)
			return false;
		clearContainer();
		CLFileBinSerial tt(file);
		if (tt.is_open() == false)
			return false;
		tt.seekg(ios::beg);

		tt >> vm_LossFuncType
			>> vm_globleInfo.iNCounts
			>> vm_globleInfo.iLayCounts
			>> vm_inputDim
			>> vm_outputDim
			>> wbSi
			>> ctime;
		setLossFunc(vm_LossFuncType);
		//层信息
		vm_layInfo.resize(layerCounts());
		for (Uint i = 0; i < layerCounts(); i++)
		{
			tt >> vm_layInfo[i];
		}		
		//读入加密信息
		tt >> encrypteMod;

		clearContainer(neuronCounts());
		if (wbSi > 0)
			vm_wbpack.resize(wbSi);
		else
			ci = 2;
		for (wbi = 0; wbi < wbSi; wbi++)//取得wb包
		{
			auto& pwb = vm_wbpack[wbi];
			pwb.reset();
			wsi = 0;
			Uint i;
			Float _bi = 0;
			tt >> i >> pwb.bFlag >> _bi >> wsi;
			pwb.index = i;
			tempWji.resize(wsi);
			for (Uint wi = 0; wi < wsi; wi++)
			{
				tt >> tempWji[wi];
			}
			auto pwji = make_wb_Wji_bi(pwb, wsi);
			memcpy_s(pwji, wsi * sizeof(Float), tempWji.data(), wsi * sizeof(Float));
			wb_bi(pwb) = _bi;
		}
		//取得节点
		for (unitCount = 0; unitCount < neuronCounts(); unitCount++) {
			laynns = 0;
			tt >> laynns >> cLayer >> cPos;
			auto& nn = (LayStartNn(cLayer))[cPos];
			fitId(nn, cs, cLayer, cPos);
			tt >> nn.transFuncType >> nn.wcFuncType >> nn.bitFlag;
			setTransFunc(cs, nn.transFuncType);
			setWcFunc(cs, nn.wcFuncType);
			cs++;
			linkSi = 0;
			tt >> linkSi;
			tempLink.resize(linkSi);
			for (Uint k = 0; k < linkSi; k++)
			{
				tt >> tempLink[k];
			}
			auto link = make_link(nn, linkSi);
			memcpy_s(link, linkSi * sizeof(Int), tempLink.data(), linkSi * sizeof(Int));
			tt >> nn.wb;
			if (nn.wb > vm_wbpack.size()) {
				clearContainer();
				CLString(("权值参数结构指针损坏，文件已失效！")).messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR | MB_OK).throw_runtime_error();
			}
		}
	}

	//更新权值层范围数据
	updateLayWbRange();

	if (cs == neuronCounts()) {
		rt = true;
	}
	else {
		clearContainer();
		rt = false;
	}
	return rt;
}

Bool writeSamSetsToFile(const BpnnSampSets& org, PCStr file, Bool binMode)
{
	if (!file || !CLString::createDirectoryByFileName(file)) {
		return false;
	}
	Bool rt = false;
	if (binMode) {//二进制
		if (org.size() == 0)
			return false;
		CLString filet = file;
		filet.replaceExtName((".bpnnSampSets"));
		if (filet.fileExists())
			DeleteFile(filet);
		fstream afile;
		afile.open(filet(), ios::app | ios::out | ios::binary);
		if (afile.is_open() == false)
			return false;
		afile.seekg(ios::beg);
		auto si = org.size();
		afile.write((const char*)&si, sizeof(si));
		auto sin = org.intputDimension();
		afile.write((const char*)&sin, sizeof(sin));
		auto sio = org.targetDimension();
		afile.write((const char*)&sio, sizeof(sio));
		if (si > 0) {
			afile.write((const char*)org.ivData(), sizeof(Float) * si * sin);
			afile.write((const char*)org.tvData(), sizeof(Float) * si * sio);
		}
		si = org.getTransModRecSize();
		afile.write((const char*)&si, sizeof(si));
		if (si > 0) {
			auto recdb = org.getTransModRec();
			for (auto& i : recdb){
				afile.write((const char*)&i.second, sizeof(i.second));
			}
		}
		afile.close();
		rt = true;
	}
	else {
		CLString tt(1000);
		rt = tt.format(("#Bpnn data sample set text file.\r\n%lld,%lld\r\n"), org.size(),org.getTransModRecSize()).writeToFile(file, CREATE_ALWAYS);
		for (Uint i = 0; i < org.size(); i++)
		{
			auto iv = (org.iv(i));
			auto ov = (org.tv(i));
			tt.empty();
			for (Uint j = 0, sj = org.intputDimension(); j < sj; j++)
			{
				if (j == 0)
					tt << sj;
				tt << CLComma << (iv[j]);
			}
			tt << CLVerL;
			for (Uint j = 0, sj = org.targetDimension(); j < sj; j++)
			{
				if (j == 0)
					tt << sj;
				tt << CLComma << (ov[j]);
			}
			rt = tt.writeLineToFile();
		}
		if (org.getTransModRecSize() > 0) {
			auto recdb = org.getTransModRec();
			for (auto& i : recdb)
			{
				auto& rec = i.second;
				tt.empty() << rec.dimIndex << CLComma << rec.dimType << CLComma << rec.vAver
					<< CLComma << (rec.vmax <= -9e11f ? (-9e11f): rec.vmax) << CLComma << (rec.vmin >= 9e11f ? (9e11f) : rec.vmin) << CLComma << rec.vStandardDeviation;
				rt = tt.writeLineToFile();
			}
		}
		tt.closeFile();
	}
	return rt ? true : false;
}

Bool readSamSetsFromFile(BpnnSampSets& tag, PCStr lpFile, Bool binMode)
{
	assert(lpFile != nullptr);
	tag.reset();
	if (binMode) {//二进制
		CLString filet = lpFile;
		filet.replaceExtName((".bpnnSampSets"));
		if (!filet.fileExists())
			return false;
		fstream afile;
		afile.open(filet(), ios::in | ios::binary);
		if (afile.is_open() == false)
			return false;
		afile.seekg(ios::beg);
		auto si = tag.size();
		afile.read((char*)&si, sizeof(si));
		auto ivDim = si;
		afile.read((char*)&ivDim, sizeof(ivDim));
		auto tvDim = si;
		afile.read((char*)&tvDim, sizeof(tvDim));
		if (si > 0 && ivDim > 0 && tvDim > 0) {
			tag.resize(si, 0, 0, ivDim, tvDim);
			afile.read((char*)tag[0].iv(), sizeof(Float) * si * ivDim);
			afile.read((char*)tag[0].tv(), sizeof(Float) * si * tvDim);
		}
		afile.read((char*)&si, sizeof(si));
		if (si > 0) {
			BpnnSampTransModelRecord recdb;
			BpnnTransModelUnit mod;
			for (Uint i = 0; i < si; i++){
				afile.read((char*)&mod, sizeof(BpnnTransModelUnit));
				recdb[mod.dimIndex] = mod;
			}
			tag.setTransModRec(recdb);
		}
		afile.close();
		return tag.size() == 0 ? false : true;
	}
	CLString filet = lpFile;
	if (!filet.fileExists())
		return false;
	CLString tt,tti,ttv;
	Uint i = 0, nsi = 0, njs = 0, njs2 = 0, ci = 0, ct = 0,nsiRecMod = 0;
	Float v = 0;
	VLF ivi, ivo;
	BpnnSampTransModelRecord recdb; 
	BpnnTransModelUnit rec;
	while (tt.readLineFromFile(i++ == 0 ? lpFile : nullptr) >= 0) {
		CLRemoveNotes(tt);
		if (tt.size() == 0)
			continue;
		if (ct == 0) {
			tt* CLComma >> nsi >> nsiRecMod;
			ct = 1;
		}
		else if (ct == 1) {
			if (tt.split("|").size() != 2) { continue; }
			tti = tt.vtAt(0);
			ttv = tt.vtAt(1);
			tti* CLComma >> njs;
			if (njs > 0) {
				auto& iv = ivi;
				iv.resize(njs, 0);
				for (Uint k = 0; k < njs; k++){
					tti >> iv[k];
				}
				ttv* CLComma >> njs2;
				if (njs2 > 0) {
					auto& ov = ivo;
					ov.resize(njs2, 0);
					for (Uint k = 0; k < njs2; k++){
						ttv >> ov[k];
					}
					if (ci == 0)
						tag.resize(nsi, 0, 0, njs, njs2);
					tag.setSample(ci, ivi, ivo);
					if (++ci >= nsi) {
						if (nsiRecMod > 0)
							ct = 2,ci = 0; 
						else 
							break;
					}
				}
			}			
			else break;
		}
		else if (ct == 2) {
			rec.reset();
			tt* CLComma >> rec.dimIndex >> rec.dimType >> rec.vAver
				>> rec.vmax >> rec.vmin >> rec.vStandardDeviation;
			recdb[rec.dimIndex] = rec;
			if (++ci >= nsiRecMod) {
				tag.setTransModRec(recdb);
				break;
			}
		}
	}
	return tag.size() == 0 ? false : true;
}

CLBpExtend& CLBpExtend::setSampSets(const BpnnSampSets& tag)
{
	vm_samSets = train_samSets = &tag;
	return *this;
}

Float CLBpExtend::getCorrectRate(const BpnnSampSets* tag, Uint nCounst, Bool useRandom, Byte crtype)
{
	if (tag == 0) {
		if (vm_samSets == 0) {
			g_CorrectRate_old = g_CorrectRate;
			return g_CorrectRate = 0.0;
		}
		tag = &(*vm_samSets);
	}
	Uint sl = tag->size();
	if (sl == 0) {
		g_CorrectRate_old = g_CorrectRate;
		return g_CorrectRate = 0.0;
	}
	if (nCounst > 0 && nCounst < sl) {
		if (useRandom) {
			RAND_SEED_EX();
			predict_samUsage.resize(nCounst);
			for (Uint i = 0; i < nCounst; i++)
			{
				predict_samUsage[i] = RAND_I_Ai_B(0, sl);
			}
		}
		sl = nCounst;
	}
	else useRandom = false;
	Float er = 0;
	auto index = kernel.LayStartNnIndex(kernel.lastLayIndex());
	auto pnnSi = kernel.LayNnCounts(kernel.lastLayIndex());
	Uint ovSi = pnnSi;
	Float yes = 0;
	if ((Uint(m_mutiTrdSupportPerc * work.getCpuCoreCounts()) >= 4) && sl >= 2) {//多线程并且预测样本数量大于2时候才使用多线程模式
		if (vm_yi_predict.size() / (Uint)kernel.neuronCounts() < sl) {
			vm_yi_predict.resize(sl * kernel.neuronCounts());
			vm_yi_span_predict = sl;
		}
		if (useRandom) {
		}
		else {
			predict_samUsage.resize(sl);
			for (Uint i = 0; i < sl; i++)
			{
				predict_samUsage[i] = i;
			}
		}

		checkMultiThreadStartup();

		vm_samSets = tag;
		vm_samUsage = &predict_samUsage;
		work.method = work.getWorkType(work.ws_samSi = sl);
		setFordParam(nullptr, &vm_yi_predict[0], vm_yi_span_predict, nullptr, 0);

		//采用预测模式的多线程过程
		EWS_PERSID thisPers = kernel.vm_bnMi > 0 ? PERS_BN_FORD_PREDICT : PERS_FORD_PREDICT;
		if (work.method == 0) {
			work.setPermissions(thisPers);
		}
		else {
			//启动向前循环			
			for (Uint i = 0, si = kernel.layerCounts(); i < si; i++) //反向修正
			{
				work.pclaySize = kernel.LayNnCounts(i);
				work.clayIndex = i;
				work.setPermissions(thisPers);
			}
		}

		switch (crtype) {
		case CRT_MaxValuePosMatch: {
			for (Uint l = 0, jsi = min(tag->targetDimension(), ovSi); l < sl; l++)
			{
				Uint ci = (*vm_samUsage)[l];
				auto ov = tag->tv(ci);
				int pos_t = -1;
				int pos_y = -2;
				Float max_value_t = -VtFloatMax;
				Float max_value_y = -VtFloatMax;
				for (Uint j = 0; j < jsi; j++) {
					auto py = pYi2i(index + j);
					if (py[l] > max_value_y) {
						max_value_y = py[l];
						pos_y = j;
					}
					if (ov[j] > max_value_t) { //值大于
						max_value_t = ov[j];
						pos_t = j;
					}
				}
				if (pos_y == pos_t) {
					yes++;
				}
			}
		}break;
		case CRT_MinValuePosMatch: {
			for (Uint l = 0, jsi = min(tag->targetDimension(), ovSi); l < sl; l++)
			{
				Uint ci = (*vm_samUsage)[l];
				auto ov = tag->tv(ci);
				int pos_t = -1;
				int pos_y = -2;
				Float max_value_t = VtFloatMax;
				Float max_value_y = VtFloatMax;
				for (Uint j = 0; j < jsi; j++) {
					auto py = pYi2i(index + j);
					if (py[l] < max_value_y) {
						max_value_y = py[l];
						pos_y = j;
					}
					if (ov[j] < max_value_t) {
						max_value_t = ov[j];
						pos_t = j;
					}
				}
				if (pos_y == pos_t) {
					yes++;
				}
			}
		}break;
		default: {
			for (Uint l = 0, jsi = min(tag->targetDimension(), ovSi); l < sl; l++)
			{
				Uint ci = (*vm_samUsage)[l];
				auto ov = tag->tv(ci);
				er = 0;
				for (Uint j = 0; j < jsi; j++)
				{
					auto py = pYi2i(index + j);
					er += kernel.loss(py[l], ov[j]);
				}
				if (er <= g_accuracy)//损失小于精度，评价为分类成功
					yes++;
			}
		}break;
		}

		vm_samSets = train_samSets;
		vm_samUsage = &train_samUsage;
		work.method = work.getWorkType(work.ws_samSi = vm_samUsage->size());
		setFordParam(&vm_xi[0], &vm_yi[0], vm_xy_span, &vm_neuronExData, vm_grad_span);
	}
	else {
		switch (crtype) {
		case CRT_MaxValuePosMatch: {
			auto py0 = pYi0i(index);
			for (Uint l = 0, jsi = tag->targetDimension(), iDim = tag->intputDimension(); l < sl; l++)
			{
				Uint ci = useRandom ? predict_samUsage[l] : l;
				auto ov = tag->tv(ci);
				kernel.setInput(tag->iv(ci), iDim).predict();
				int pos_t = -1;
				int pos_y = -2;
				Float max_value_t = -VtFloatMax;
				Float max_value_y = -VtFloatMax;
				for (Uint j = 0; j < jsi; j++) {
					if (py0[j] > max_value_y) {
						max_value_y = py0[j];
						pos_y = j;
					}
					if (ov[j] > max_value_t) {
						max_value_t = ov[j];
						pos_t = j;
					}
				}
				if (pos_y == pos_t) {
					yes++;
				}
			}
		}break;
		case CRT_MinValuePosMatch: {
			auto py0 = pYi0i(index);
			for (Uint l = 0, jsi = tag->targetDimension(), iDim = tag->intputDimension(); l < sl; l++)
			{
				Uint ci = useRandom ? predict_samUsage[l] : l;
				auto ov = tag->tv(ci);
				kernel.setInput(tag->iv(ci), iDim).predict();
				int pos_t = -1;
				int pos_y = -2;
				Float max_value_t = VtFloatMax;
				Float max_value_y = VtFloatMax;
				for (Uint j = 0; j < jsi; j++) {
					if (py0[j] < max_value_y) {
						max_value_y = py0[j];
						pos_y = j;
					}
					if (ov[j] < max_value_t) {
						max_value_t = ov[j];
						pos_t = j;
					}
				}
				if (pos_y == pos_t) {
					yes++;
				}
			}
		}break;
		default: {
			for (Uint l = 0, jsi = tag->targetDimension(), iDim = tag->intputDimension(); l < sl; l++)
			{
				Uint ci = useRandom ? predict_samUsage[l] : l;
				auto ov = tag->tv(ci);
				kernel.setInput(tag->iv(ci), iDim).predict();
				Float er = 0;
				for (Uint j = 0; j < jsi; j++)
				{
					er += kernel.loss(yi0i(index + j), ov[j]);
				}
				if (er <= g_accuracy)//损失小于精度，评价为分类成功
					yes++;
			}
		}break;
		}
	}

	g_CorrectRate_old = g_CorrectRate;
	g_CorrectRate = yes / Float(sl);
	return g_CorrectRate;
}

CLBpExtend& CLBpExtend::setCorrectRateEvaluationModel(Float correctRate, const BpnnSampSets* _predict, Uint _nCounst, Bool _useRandom, Byte crtype)
{
	m_CorrectRateType = crtype;
	m_CorrectRate = correctRate < 0 ? 0 : correctRate > 1 ? 1 : correctRate;
	if (isCorrectRateEvaluationModel()) {
		this->predict_samSets = _predict;
		predict_useSamCounts = _nCounst;
		predict_useRandom = _useRandom;
	}
	else {
		this->predict_samSets = 0;
		predict_useSamCounts = 0;
		predict_useRandom = 0;
	}
	return *this;
}

Bool CLBpExtend::isCorrectRateEvaluationModel() const
{
	return m_CorrectRate > 0 ? true : false;
}

CLBpExtend& CLBpExtend::setSampleBatchCounts(Uint nCounts, Bool UseRandomSamp)
{
	setUseRandSample(UseRandomSamp);
	train_useSamCounts = nCounts;
	return *this;
}

Float CLBpKernel::predict(const VLF& inputVec, VLF* _resultVec, VLF* tagVec)
{
	return predict(inputVec.data(), inputVec.size(), _resultVec, tagVec);
}

Float CLBpKernel::predict(const Float* inputData, Uint inputDimension, VLF* _resultVec, VLF* tagVec)
{
	if (layerCounts() < Minimum_Layers) {
		CLString(("\n错误：神经网络模型尚未准备就绪，无法执行预测！\n原因：模型可能从未构造并训练，也可能未加载过任何已有的完整模型。\n"))
			.printf().messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	}
	setInput(inputData, inputDimension).predict();
	if (_resultVec)
		getOutput(*_resultVec);
	if (tagVec == nullptr)
		return VtFloatMax;
	else
		return Er(tagVec->data(), tagVec->size());
}

void CLBpKernel::makeIndependentDataBuf(VLF& yiData) const
{
	yiData.clear();
	if (layerCounts() < Minimum_Layers) {
		CLString(("\n错误：神经网络模型尚未准备就绪，无法构造独立数据区结构！\n原因：模型可能从未构造并训练，也可能未加载过任何已有的完整模型。\n"))
			.printf().messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	}
	yiData.resize(neuronCounts());
}

Float CLBpKernel::predictWithIndependentData(Float* yiData, const VLF& inputVec, VLF* _out_resultVec, VLF* tagVec)
{
	if (layerCounts() < Minimum_Layers) {
		//throw logic_error("Bpnn is not ready!");
		CLString(("\n错误：神经网络模型尚未准备就绪，无法执行采用独立数据区的预测！\n原因：模型可能从未构造并训练，也可能未加载过任何已有的完整模型。\n"))
			.printf().messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	}
	_predict(nullptr, yiData, inputVec.data());
	if (_out_resultVec)
		_getOutput(yiData, *_out_resultVec);
	if (tagVec == nullptr)
		return VtFloatMax;
	else
		return _Er(yiData, tagVec->data(), tagVec->size());
}

CLBpExtend& CLBpExtend::setStructure(const BpnnStructDef& mod)
{
	if (mod.size() < 2)
		CLString(("\r\n错误：结构定义对象的层数应不少于2层（至少包含一个隐层和一个输出层）！\r\n")).printf().messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	mode = &mod;
	return *this;
}

PCStr getTransFuncName(Byte type) {
#define GTFNI(name) case TF_##name: return (#name)
	switch (type)
	{
		GTFNI(Sigmoid);
		GTFNI(Purelin);
		GTFNI(Step);
		GTFNI(Relu);
		GTFNI(Tanh);
		GTFNI(LeakyRelu);
		GTFNI(PRelu);
		GTFNI(ELU);
		GTFNI(SELU);
		GTFNI(SRelu);
		GTFNI(HardSigmoid);
		GTFNI(HardTanh);
		GTFNI(LeCunTanh);
		GTFNI(ArcTan);
		GTFNI(SoftSign);
		GTFNI(SoftPlus);
		GTFNI(Signum);
		GTFNI(BentPurelin);
		GTFNI(SymmetricalSigmoid);
		GTFNI(LogLog);
		GTFNI(Gaussian);
		GTFNI(Absolute);
		GTFNI(Sinusoid);
		GTFNI(Cos);
		GTFNI(Sinc);
	default:
		return ("Unkown transFunc name.");
	}
}

Bool drawText(HDC dc, PCStr str, Int szlen, Int x, Int y, Int alignFlag)
{
	if (alignFlag < 0 || alignFlag > 9)alignFlag = 0;
	if (alignFlag == 0) { return TextOut(dc, x, y, str, szlen); }
	switch (alignFlag) {
	case 1: SetTextAlign(dc, TA_LEFT | TA_TOP | TA_NOUPDATECP); break;
	case 2: SetTextAlign(dc, TA_CENTER | TA_TOP | TA_NOUPDATECP); break;
	case 3: SetTextAlign(dc, TA_RIGHT | TA_TOP | TA_NOUPDATECP); break;
	case 4: SetTextAlign(dc, TA_LEFT | TA_BASELINE | TA_NOUPDATECP); break;
	case 5: SetTextAlign(dc, TA_CENTER | TA_BASELINE | TA_NOUPDATECP); break;
	case 6: SetTextAlign(dc, TA_RIGHT | TA_BASELINE | TA_NOUPDATECP); break;
	case 7: SetTextAlign(dc, TA_LEFT | TA_BOTTOM | TA_NOUPDATECP); break;
	case 8: SetTextAlign(dc, TA_CENTER | TA_BOTTOM | TA_NOUPDATECP); break;
	case 9: SetTextAlign(dc, TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP); break;
	default:return false;
	}
	return TextOut(dc, x, y, str, szlen);
}

#define YSide 300
#define XSide 400
#define Ynn 260
#define Xnn 1000

#define SZOFF 20

void CLBpExtend::drawNode(Uint _lay, Uint _pos, Int upNnBase, Int type, HDC hp, Int r, Int lwidth, Int lheight, Int layernns, Int nlayers, Uint siFord, Int pr,
	neuron* nn, Int iStyle, Int iWide, COLORREF cls, HBRUSH hbr,
	Float wmin, Float wmax, Float wqmin, Float wqmax, Bool isDetail
) {
	Int cx = XSide + lwidth * (type == 0 ? 0 : _lay + 1) + lwidth / 2;
	Int cy = YSide + (lheight - 2 * YSide) / layernns * (_pos + 0.5);

	Float wq = 0, wax = -VtFloatMax, win = VtFloatMax;
	if (isDetail)
		wq = getMaxMinWij(*nn, win, wax);
	COLORREF cPen; Float deep;
	HPEN hpc = 0, hpb = 0;
	HBRUSH hBru = hbr, hBrub = hbr;

	if (!isDetail || type == 0 || type == 2) {
		hBrub = SelectBrush(hp, hbr);
		hpc = CreatePen(iStyle, iWide, cls);
		hpb = SelectPen(hp, hpc);
	}

	if (type == 0) {//输入层
		MoveToEx(hp, cx - r - 3 * r, cy, 0);
		LineTo(hp, cx - r, cy);
	}
	else if (type == 2) {//输出层
		MoveToEx(hp, cx + r + 3 * r, cy, 0);
		LineTo(hp, cx + r, cy);
		if (isDetail) {
			SelectPen(hp, hpb);
			SelectPen(hp, hBrub);
			if (hpc)DeleteObject(hpc), hpc = 0;
			if (hBru)DeleteObject(hBru), hBru = 0;
		}
		Int playernns = siFord;
		Int cxp = XSide + lwidth * (_lay)+lwidth / 2;
		for (Uint i = 0; i < link_size(*nn); i++)
		{
			if (plink(*nn)[i] < 0)
				continue;
			if (isDetail) {
				Float w = pWji(*nn)[i];
				deep = Abs(w) / max(abs(wmax), abs(wmin));
#define MinColorV 245
#define MinLineWide 1
#define MaxLineWide 5
				cPen = RGB(
					(w >= 0 ? deep * (230 - 225) + 225 : deep * (50 - MinColorV) + MinColorV),
					(w >= 0 ? deep * (55 - MinColorV) + MinColorV : deep * (80 - MinColorV) + MinColorV),
					(w >= 0 ? deep * (60 - MinColorV) + MinColorV : deep * (230 - 255) + 255)
				);
				hpc = CreatePen(PS_DOT, deep * MaxLineWide + MinLineWide, cPen);
				hpb = SelectPen(hp, hpc);
			}
			Int cyp = YSide + (lheight - 2 * YSide) / playernns * (plink(*nn)[i] - upNnBase + 0.5);
			MoveToEx(hp, cxp + pr, cyp, 0);
			LineTo(hp, cx - r, cy);
			if (isDetail) {
				SelectPen(hp, hpb);
				if (hpc)DeleteObject(hpc), hpc = 0;
			}
		}
	}
	else {//隐藏节点
		Int playernns = siFord;
		Int cxp = XSide + lwidth * (_lay)+lwidth / 2;
		for (Uint i = 0; i < link_size(*nn); i++)
		{
			if (plink(*nn)[i] < 0)
				continue;
			if (isDetail) {
				Float w = pWji(*nn)[i];
				deep = Abs(w) / max(abs(wmax), abs(wmin));
				cPen = RGB(
					(w >= 0 ? deep * (230 - 225) + 225 : deep * (50 - MinColorV) + MinColorV),
					(w >= 0 ? deep * (55 - MinColorV) + MinColorV : deep * (80 - MinColorV) + MinColorV),
					(w >= 0 ? deep * (60 - MinColorV) + MinColorV : deep * (230 - 255) + 255)
				);
				hpc = CreatePen(PS_DOT, deep * MaxLineWide + MinLineWide, cPen);
				hpb = SelectPen(hp, hpc);
			}
			Int cyp = YSide + (lheight - 2 * YSide) / playernns * (plink(*nn)[i] - upNnBase + 0.5);
			MoveToEx(hp, cxp + pr, cyp, 0);
			LineTo(hp, cx - r, cy);
			if (isDetail) {
				SelectPen(hp, hpb);
				if (hpc)DeleteObject(hpc), hpc = 0;
			}
		}
	}
	if (isDetail) {
		deep = (wqmax - wq) / wqmax * 200 + 55;
		if (type == 0) {//输入层
			hpc = CreatePen(iStyle, iWide, cls);
			hBrub = SelectBrush(hp, hbr);
		}
		else if (type == 2) {//输出层
			hpc = CreatePen(iStyle, wq / wqmax * 15 + 2, cls);
			hBru = CreateSolidBrush(RGB(deep, deep, deep));
		}
		else {
			hBru = CreateSolidBrush(RGB(deep, deep, deep));
			hpc = CreatePen(PS_DOT, wq / wqmax * 15 + 2, RGB(deep - 55, deep - 55, deep - 55));
		}
		hBrub = SelectBrush(hp, hBru);
		hpb = SelectPen(hp, hpc);
	}
	//画圈
	Ellipse(hp, cx - r, cy - r, cx + r, cy + r);
	//画横
	MoveToEx(hp, cx - r, cy, 0);
	LineTo(hp, cx + r, cy);

	//写传递函数
	HFONT hfont = CreateFont(30, 15, 0, 0, FW_EXTRALIGHT, false, false, false,
		GB2312_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_MODERN, ("宋体"));
	HFONT hfontb = SelectFont(hp, hfont);
	auto pfcn = type == 0 ? ("-->") : getTransFuncName(nn->transFuncType);
	drawText(hp, pfcn, _tcslen(pfcn), cx, cy + SZOFF, 2);
	TCHAR str[30];
	if (type == 0) {//输入层
		_stprintf_s(str, 30, ("In  %d "), _pos + 1);
	}
	else if (type == 2) {//输出层
		_stprintf_s(str, 30, ("Out  %d "), _pos + 1);
	}
	else {
		_stprintf_s(str, 30, ("%d  %d"), _lay + 1, _pos + 1);
	}
	//写编号
	drawText(hp, str, _tcslen(str), cx, cy - SZOFF, 8);
	MoveToEx(hp, cx, cy, 0);
	LineTo(hp, cx, cy - r);

	SelectFont(hp, hfontb);
	SelectPen(hp, hpb);
	SelectPen(hp, hBrub);
	if (hpc)DeleteObject(hpc);
	if (hBru)DeleteObject(hBru);
	if (hfont)DeleteObject(hfont);
}

void CLBpExtend::checkErValid()
{
	if (g_Er == g_Er_old && g_Er > g_accuracy) {
		if (++gErEquitTimes > min(maxTimes / 100, 1000)) {
			gErEquitTimes = 0;
			//CLString().format("\n网络训练停止变化，需调整参数后重试！\n当前 Er = %f ", g_Er).messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR);
			//CLString().format("\n网络训练停止变化，需调整参数后重试！当前（ Er = %f ）", getEr()).printf();
			throw std::invalid_argument(__func__);
		}
	}
	else if (isCorrectRateEvaluationModel()){
		if (m_runTimesFromBuild > 1e5 && runTimes > 0.8 * maxTimes && getSavedCorrectRate() < 0.01) {
			//CLString().format("\n网络训练停止变化，需调整参数后重试！当前（ 正确率 = %g%% ）", getSavedCorrectRate() * 100).printf();
			throw std::invalid_argument(__func__);
		}
	}
	else {
		gErEquitTimes = 0;
	}
}

void CLBpExtend::checkMultiThreadStartup()
{
	if (!work.isRunning()) {//检查
		CLString(("训练方法采用了多线程模式，但在这之前系统却未能启动多线程模块！")).messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR)
			.throw_runtime_error();
	}
}

Bool CLBpExtend::getBitmapData(HANDLE& hBitmapInfo, BITMAPFILEHEADER& fileHdr, BITMAPINFO*& pdata, Uint& bufSize, Bool bUseDetailMode) {
	CLRect rect;
	{
		//计算大小
		Int maxlays = kernel.layerCounts() + 1, maxnns = 0;
		for (Uint i = 0; i < kernel.layerCounts(); i++)
		{
			if (Int(kernel.LayNnCounts(i)) > maxnns)
				maxnns = Int(kernel.LayNnCounts(i));
		}
		rect.bottom = maxnns * Ynn + YSide * 2;
		rect.right = maxlays * Xnn + XSide * 2;
	}
	//创建设备描述表
	//HDC hs = ::GetWindowDC(hTagWnd);
	HDC hs = ::GetDC(NULL);
	//创建兼容的设备描述表
	HDC hp = CreateCompatibleDC(hs);
	// 创建与设备描述表兼容的位图
	HBITMAP hbitmap = CreateCompatibleBitmap(hs, rect.Width(), rect.Height());
	// 把位图选到设备描述表中
	SelectObject(hp, hbitmap);
	{
		//画图，图白
		LOGBRUSH lbrush;
		lbrush.lbColor = 0xFFFFF0;//护眼黄
		lbrush.lbHatch = HS_CROSS;
		lbrush.lbStyle = BS_HATCHED;
		HBRUSH hbrWite = CreateBrushIndirect(&lbrush);
		if (!::FillRect(hp, (const RECT*)&rect, hbrWite))
			CLString().getLastErrorMessageBoxExceptSucceed(GetLastError());
		DeleteObject(hbrWite);

		HFONT hfont = CreateFont(60, 30, 0, 0, FW_THIN, false, false, false,
			GB2312_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, ("宋体"));
		HFONT hfontb = SelectFont(hp, hfont);
		CLString str;
		str.format(("神经网络结构图，网络名称：< %s > "), kernel.getName());
		auto brt = drawText(hp, str(), str.size(), 1, 1, 1);
		if (!brt)
			str.getLastErrorMessageBoxExceptSucceed(GetLastError());
		str.format(("Neurons=%d, HideLayers=%d, InputDim=%d, OutputDim=%d"),
			kernel.neuronCounts(),
			kernel.hideLayerCounts(),
			kernel.inputDimension(),
			kernel.outputDimension()
		);
		brt = drawText(hp, str(), str.size(), 1, 60, 1);
		if (!brt)
			str.getLastErrorMessageBoxExceptSucceed(GetLastError());
		if (bUseDetailMode) {
			str = ("说明：线条越粗颜色越深表示权重比例越大，红色表示链接权值为正，蓝色表示链接权值为负。");
			brt = drawText(hp, str(), str.size(), 1, 120, 1);
			if (!brt)
				str.getLastErrorMessageBoxExceptSucceed(GetLastError());
		}
		SelectFont(hp, hfontb);
		DeleteObject(hfont);

		//计算最小最大权
		Float wmin = VtFloatMax, wmax = -VtFloatMax, wqmin = VtFloatMax, wqmax = -VtFloatMax;
		if (bUseDetailMode)
			for (Uint i = 0, si = kernel.layerCounts(); i < si; i++)
			{
				auto lay = kernel.LayStartNn(i);
				auto laySi = kernel.LayNnCounts(i);
				for (Uint j = 0, sj = laySi; j < sj; j++)
				{
					auto wq = getMaxMinWij(lay[j], wmin, wmax);
					if (wq > wqmax)wqmax = wq;
					if (wq < wqmax)wqmin = wq;
				}
			}

#define RHide 80
#define ROut  85
		neuron ut;
		Int type = 0;
		for (Uint j = 0; j < (kernel.inputDimension()); j++)
		{
			CLBpKernel::fitId(ut, 0, 0, j);
			drawNode(0, j,
				0, type, hp, (type == 0 ? 85 : (type == 2 ? 85 : RHide)),
				Xnn, rect.Height(), (kernel.inputDimension()), kernel.layerCounts(), 0,
				(type == 0 ? 85 : (type == 2 ? RHide : RHide)),
				&ut,
				(type == 0 ? PS_DOT : (type == 2 ? PS_DOT : PS_SOLID)),
				(type == 0 ? 3 : (type == 2 ? 3 : 2)),
				(type == 0 ? /*0xD2691E*/ CLVIOLET : (type == 2 ? 0x1C86EE : 0x4B0082)),
				(type == 0 ? (HBRUSH)GRAY_BRUSH : (type == 2 ? (HBRUSH)DKGRAY_BRUSH : (HBRUSH)NULL_BRUSH)),
				wmin, wmax, wqmin, wqmax,
				bUseDetailMode
			);
		}
		for (Uint i = 0; i < kernel.layerCounts(); i++)
		{
			auto p = i == 0 ? (kernel.inputDimension()) : (kernel.LayNnCounts(i - 1));
			//auto pc = &kernel.getLayer(i);
			auto pc = kernel.LayStartNn(i);
			auto pcSi = kernel.LayNnCounts(i);
			type = (i == (kernel.layerCounts() - 1)) ? 2 : 1;
			Int upBaseSi = neuronCountsTillLayer(i);
			for (Uint j = 0; j < pcSi; j++)
			{
				drawNode(i, j,
					upBaseSi, type, hp, (type == 0 ? 85 : (type == 2 ? 85 : RHide)),
					Xnn, rect.Height(), pcSi, kernel.layerCounts(), p,
					(type == 0 ? 85 : (type == 2 ? RHide : RHide)),
					&pc[j],
					(type == 0 ? PS_DOT : (type == 2 ? PS_DOT : PS_SOLID)),
					(type == 0 ? 3 : (type == 2 ? 3 : 2)),
					(type == 0 ? /*0xD2691E*/ CLVIOLET : (type == 2 ? 0x1C86EE : 0x4B0082)),
					(type == 0 ? (HBRUSH)GRAY_BRUSH : (type == 2 ? (HBRUSH)DKGRAY_BRUSH : (HBRUSH)NULL_BRUSH)),
					wmin, wmax, wqmin, wqmax,
					bUseDetailMode
				);
			}
		}

	}
	DeleteDC(hs);
	DeleteDC(hp);
	HDC hdc = 0;
	DWORD dwbmbitssize, dwdibsize;
	BITMAP bitmap; //位图属性结构
	BITMAPFILEHEADER bmfhdr; //位图文件头结构
	BITMAPINFOHEADER bi; //位图信息头结构
	LPBITMAPINFOHEADER lpbi; //指向位图信息头结构
	HANDLE hdib = NULL, hpal = NULL, holdpal = NULL;
	//设置位图信息头
	GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bitmap.bmWidth;
	bi.biHeight = bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwbmbitssize = ((bitmap.bmWidth * 24 + 31) / 32) * 4 * bitmap.bmHeight;
	if (bufSize < dwbmbitssize + sizeof(BITMAPINFOHEADER)) {
		if (hBitmapInfo) {
			::GlobalUnlock(hBitmapInfo);
			::GlobalFree(hBitmapInfo);
			hBitmapInfo = 0;
			pdata = 0;
			bufSize = 0;
		}
		hBitmapInfo = GlobalAlloc(GHND, (bufSize = dwbmbitssize + sizeof(BITMAPINFOHEADER)));
		pdata = (LPBITMAPINFO)GlobalLock(hBitmapInfo);
	}
	else {
		memset(pdata, 0, bufSize);
	}
	lpbi = (LPBITMAPINFOHEADER)pdata;
	*lpbi = bi;
	// 处理调色板 
	hpal = GetStockObject(DEFAULT_PALETTE);
	if (hpal)
	{
		hdc = ::GetDC(NULL);
		holdpal = ::SelectPalette(hdc, (HPALETTE)hpal, false);
		::RealizePalette(hdc);
	}
	// 获取该调色板下新的像素值
	::GetDIBits(hdc, hbitmap, 0, (Uint)bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
	// 位图文件头
	bmfhdr.bfType = 0x4d42;
	dwdibsize = 54 + dwbmbitssize;
	bmfhdr.bfSize = dwdibsize;
	bmfhdr.bfReserved1 = 0;
	bmfhdr.bfReserved2 = 0;
	bmfhdr.bfOffBits = 54;
	fileHdr = bmfhdr;
	::DeleteObject(hbitmap);
	return true;
}

Bool CLBpExtend::exportGraphNetStruct(PCStr outFileName, Bool bUseDetailMode)
{
	if (kernel.layerCounts() < Minimum_Layers)
		return false;
	auto nnsi = kernel.neuronCounts();
	if (nnsi > 10000) {
		if (bShowNetAlert) {
			if (IDYES == CLString().format(("网络的神经元数量(%d)过多，无法绘制网络的二维结构图！\n下次不再显示提示（Yes/No）"), nnsi).messageBox(_lpBpnnMsgBoxTitle, MB_YESNO | MB_ICONWARNING)) {
				bShowNetAlert = false;
			}
		}
		return false;
	}
	auto htop = GetDesktopWindow();
	CLString file, path = outFileName, name = outFileName;
	if (!path.deleteLastStrFromPath().filePathExists())
		if (!path.createDirectory())
			return ::MessageBox(htop, CLString(("路径不存在:\n"), (PCStr)(path), 0).string(), ("错误提示"), MB_ICONERROR), FALSE;
	name.getLastStrByPath();
	auto pos = name.rfind(('.'));
	if (pos > 0)
		name.leftSave(pos);
	else
		return ::MessageBox(htop, CLString(("文件名有误:\n"), (PCStr)(outFileName), 0).string(), ("错误提示"), MB_ICONERROR), FALSE;
	file = path + name + (".bmp");
	if (file.fileExists()) {
		DeleteFile(file);
	}
	CLRect rect; Uint nnn = 0;
	{
		//计算大小
		Int maxlays = kernel.layerCounts() + 1, maxnns = 0;

		for (Uint i = 0; i < kernel.layerCounts(); i++)
		{
			auto laySi = kernel.LayNnCounts(i);
			if (Int(laySi) > maxnns)
				maxnns = Int(laySi);
			nnn += laySi;
		}
		rect.bottom = maxnns * Ynn + YSide * 2;
		rect.right = maxlays * Xnn + XSide * 2;
	}
	//创建设备描述表
	//HDC hs = ::GetWindowDC(hTagWnd);
	HDC hs = ::GetDC(NULL);
	//创建兼容的设备描述表
	HDC hp = CreateCompatibleDC(hs);
	// 创建与设备描述表兼容的位图
	HBITMAP hbitmap = CreateCompatibleBitmap(hs, rect.Width(), rect.Height());
	// 把位图选到设备描述表中
	SelectObject(hp, hbitmap);
	{
		//画图，图白
		LOGBRUSH lbrush;
		lbrush.lbColor = 0xFFFFF0;//护眼黄
		lbrush.lbHatch = HS_CROSS;
		lbrush.lbStyle = BS_HATCHED;
		HBRUSH hbrWite = CreateBrushIndirect(&lbrush);
		if (!::FillRect(hp, (const RECT*)&rect, hbrWite))
			CLString().getLastErrorMessageBoxExceptSucceed(GetLastError());
		DeleteObject(hbrWite);

		HFONT hfont = CreateFont(60, 30, 0, 0, FW_THIN, false, false, false,
			GB2312_CHARSET, OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_MODERN, ("宋体"));
		HFONT hfontb = SelectFont(hp, hfont);
		CLString str;

		str.format(("神经网络结构图，网络名称：< %s > "), kernel.getName());
		auto brt = drawText(hp, str(), str.size(), 1, 1, 1);
		if (!brt)
			str.getLastErrorMessageBoxExceptSucceed(GetLastError());
		str.format(("Neurons=%d, HideLayers=%d, InputDim=%d, OutputDim=%d"),
			nnn,
			kernel.hideLayerCounts(),
			kernel.inputDimension(),
			kernel.outputDimension()
		);
		brt = drawText(hp, str(), str.size(), 1, 60, 1);
		if (!brt)
			str.getLastErrorMessageBoxExceptSucceed(GetLastError());
		if (bUseDetailMode) {
			str = ("说明：线条越粗颜色越深表示权重比例越大，红色表示链接权值为正，蓝色表示链接权值为负。");
			brt = drawText(hp, str(), str.size(), 1, 120, 1);
			if (!brt)
				str.getLastErrorMessageBoxExceptSucceed(GetLastError());
		}
		SelectFont(hp, hfontb);
		DeleteObject(hfont);

		//计算最小最大权
		Float wmin = VtFloatMax, wmax = -VtFloatMax, wqmin = VtFloatMax, wqmax = -VtFloatMax;
		if (bUseDetailMode)
			for (Uint i = 0, si = kernel.layerCounts(); i < si; i++)
			{
				auto lay = kernel.LayStartNn(i);
				auto laySi = kernel.LayNnCounts(i);
				for (Uint j = 0, sj = laySi; j < sj; j++)
				{
					auto wq = getMaxMinWij(lay[j], wmin, wmax);
					if (wq > wqmax)wqmax = wq;
					if (wq < wqmax)wqmin = wq;
				}
			}

#define RHide 80
#define ROut  85
		neuron ut; Int type = 0;
		for (Uint j = 0; j < (kernel.inputDimension()); j++)
		{
			CLBpKernel::fitId(ut, 0, 0, j);
			drawNode(0, j,
				0, type, hp, (type == 0 ? 85 : (type == 2 ? 85 : RHide)),
				Xnn, rect.Height(), (kernel.inputDimension()), kernel.layerCounts(), NULL,
				(type == 0 ? 85 : (type == 2 ? RHide : RHide)),
				&ut,
				(type == 0 ? PS_DOT : (type == 2 ? PS_DOT : PS_SOLID)),
				(type == 0 ? 3 : (type == 2 ? 3 : 2)),
				(type == 0 ? /*0xD2691E*/ CLVIOLET : (type == 2 ? 0x1C86EE : 0x4B0082)),
				(type == 0 ? (HBRUSH)GRAY_BRUSH : (type == 2 ? (HBRUSH)DKGRAY_BRUSH : (HBRUSH)NULL_BRUSH)),
				wmin, wmax, wqmin, wqmax,
				bUseDetailMode
			);
		}
		for (Uint i = 0; i < kernel.layerCounts(); i++)
		{
			auto p = (i == 0 ? (kernel.inputDimension()) : (kernel.LayNnCounts(i - 1)));
			auto pc = kernel.LayStartNn(i);
			auto pcSi = kernel.LayNnCounts(i);
			type = (i == (kernel.layerCounts() - 1)) ? 2 : 1;
			Int upBaseSi = neuronCountsTillLayer(i);
			for (Uint j = 0; j < pcSi; j++)
			{
				drawNode(i, j,
					upBaseSi, type, hp, (type == 0 ? 85 : (type == 2 ? 85 : RHide)),
					Xnn, rect.Height(), pcSi, kernel.layerCounts(), p,
					(type == 0 ? 85 : (type == 2 ? RHide : RHide)),
					&pc[j],
					(type == 0 ? PS_DOT : (type == 2 ? PS_DOT : PS_SOLID)),
					(type == 0 ? 3 : (type == 2 ? 3 : 2)),
					(type == 0 ? /*0xD2691E*/ CLVIOLET : (type == 2 ? 0x1C86EE : 0x4B0082)),
					(type == 0 ? (HBRUSH)GRAY_BRUSH : (type == 2 ? (HBRUSH)DKGRAY_BRUSH : (HBRUSH)NULL_BRUSH)),
					wmin, wmax, wqmin, wqmax,
					bUseDetailMode
				);
			}
		}

	}
	// 把设备描述表拷贝到内存设备描述表中
	DeleteDC(hs);
	DeleteDC(hp);
	HDC hdc = 0;
	DWORD dwbmbitssize, dwdibsize;
	BITMAP bitmap; //位图属性结构
	BITMAPFILEHEADER bmfhdr; //位图文件头结构
	BITMAPINFOHEADER bi; //位图信息头结构
	LPBITMAPINFOHEADER lpbi; //指向位图信息头结构
	HANDLE hdib = NULL, hpal = NULL, holdpal = NULL;
	//设置位图信息头
	GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bitmap.bmWidth;
	bi.biHeight = bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwbmbitssize = ((bitmap.bmWidth * 24 + 31) / 32) * 4 * bitmap.bmHeight;
	hdib = GlobalAlloc(GHND, dwbmbitssize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);
	*lpbi = bi;
	// 处理调色板 
	hpal = GetStockObject(DEFAULT_PALETTE);
	if (hpal)
	{
		hdc = ::GetDC(NULL);
		holdpal = ::SelectPalette(hdc, (HPALETTE)hpal, false);
		::RealizePalette(hdc);
	}
	// 获取该调色板下新的像素值
	::GetDIBits(hdc, hbitmap, 0, (Uint)bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
	// 位图文件头
	bmfhdr.bfType = 0x4d42;
	dwdibsize = 54 + dwbmbitssize;
	bmfhdr.bfSize = dwdibsize;
	bmfhdr.bfReserved1 = 0;
	bmfhdr.bfReserved2 = 0;
	bmfhdr.bfOffBits = 54;
	FILE* fl = NULL;
	if (CLString::createDirectoryByFileName(file) &&
		0 == _tfopen_s(&fl, file.string(), ("wb")) && fl != 0 && lpbi != 0) {
		fwrite((void*)&bmfhdr, sizeof(BITMAPFILEHEADER), 1, fl);
		fwrite((void*)lpbi, dwdibsize, 1, fl);
		fclose(fl);
	}
	::DeleteObject(hbitmap);
	::GlobalUnlock(hdib);
	::GlobalFree(hdib);
	return TRUE;
}

CLBpExtend& CLBpExtend::setAdam(Bool open, Float stepL /*, Float belta2*/)
{
	m_adamAlpha = open ? min(max(stepL, 1e-5), 0.3) : 0;
	return *this;
}

CLBpExtend& CLBpExtend::setTrainState(EBP_TPC currentState)
{
	m_trainProcControl_postTrd = GetCurrentThreadId();
	m_trainProcControl = currentState;
	return *this;
}


//传递函数实现--------------------------------------------------
#define TRANS_FUNC( NAME ) \
	Float tf_##NAME(const Float x,const Bool dt)
#define TRANS_FUNC_G( NAME ) \
	Float tf_##NAME##_amp(Float x,Bool dt) restrict(amp)
#define GPUCPU 
#define GPU fast_math::

TRANS_FUNC(Sigmoid) {
	if (dt == 0) {
		return 1.0 / (1.0 + GPUCPU exp((-1.0) * x));
	}
	else {
		auto v = trFunc(Sigmoid, x);
		return v * (1.0 - v);
	}
}
TRANS_FUNC(Purelin) {
	if (dt == 0) {
		return x;
	}
	else {
		return 1;
	}
}
TRANS_FUNC(Step) {
	if (dt == 0) {
		if (x >= 0)return 1;
		else return 0;
	}
	else {
		return 0;
	}
}
TRANS_FUNC(Relu) {
	if (dt == 0) {
		if (x >= 0)return x;
		else return 0;
	}
	else {
		if (x >= 0)return 1;
		else return 0;
	}
}
TRANS_FUNC(Tanh) {
	if (dt == 0) {
		return GPUCPU tanh(x);
	}
	else {
		auto v = GPUCPU tanh(x);
		return 1.0 - double(v) * v;
	}
}
TRANS_FUNC(LeakyRelu) {
	if (dt == 0) {
		if (x >= 0)return x;
		else return 0.01 * x;
	}
	else {
		if (x >= 0)return 1;
		else return 0.01;
	}
}
TRANS_FUNC(PRelu) {
	if (dt == 0) {
		if (x >= 0)return x;
		else return 0.7 * x;
	}
	else {
		if (x >= 0)return 1;
		else return 0.7;
	}
}
TRANS_FUNC(ELU) {
	if (dt == 0) {
		if (x >= 0)return x;
		else return 0.7 * (GPUCPU double(exp(x)) - 1);
	}
	else {
		if (x >= 0)return 1;
		else return 0.7 * GPUCPU exp(x) - trFunc(ELU, x) + 0.7;
	}
}
TRANS_FUNC(SELU) {
	if (dt == 0) {
		if (x >= 0)return x;
		else return 1.67326 * (GPUCPU double(exp(x)) - 1.0);
	}
	else {
		if (x >= 0)return 1.0507;
		else return 1.0507 * 1.67326 * GPUCPU exp(x) - 1.0507 * (trFunc(ELU, x) + 1.67326);
	}
}
TRANS_FUNC(SRelu) {
	if (dt == 0) {
		if (x >= 1)return 1 + 0.4 * (x - 1.0);
		else if (x <= -1)return -1 + 0.7 * (x - (-1.0));
		else return  x;
	}
	else {
		if (x >= 1)return 0.4;
		else if (x <= -1)return 0.7;
		else return  1;
	}
}
TRANS_FUNC(HardSigmoid) {
	if (dt == 0) {
		if (x > 2.5)return 1;
		else if (x < -2.5)return 0;
		else return  0.2 * x + 0.5;
	}
	else {
		if (x > 2.5)return 0;
		else if (x < -2.5)return 0;
		else return  0.2;
	}
}
TRANS_FUNC(HardTanh) {
	if (dt == 0) {
		if (x > 1)return 1;
		else if (x < -1)return -1;
		else return  x;
	}
	else {
		if (x > 1)return 0;
		else if (x < -1)return 0;
		else return  1;
	}
}
TRANS_FUNC(LeCunTanh) {
	if (dt == 0) {
		return  1.769 * GPUCPU tanh(0.66666 * x);
	}
	else {
		return  1.7619 * 0.66666 * (1 - GPUCPU pow(GPUCPU tanh(0.66666 * x), 2));
	}
}
TRANS_FUNC(ArcTan) {
	if (dt == 0) {
		return  GPUCPU pow(GPUCPU tan(x), -1);
	}
	else {
		return  1 / (x * x + 1);
	}
}
TRANS_FUNC(SoftSign) {
	if (dt == 0) {
		return  x / (1 + Abs(x));
	}
	else {
		return  1 / GPUCPU pow(Abs(x) + 1, 2);
	}
}
TRANS_FUNC(SoftPlus) {
	if (dt == 0) {
		return  GPUCPU log(1 + GPUCPU exp(x));
	}
	else {
		return  1 / (1 + GPUCPU exp((-1.0) * x));
	}
}
TRANS_FUNC(Signum) {
	if (dt == 0) {
		if (x > 0)return 1;
		else if (x < 0)return -1;
		else return  0;
	}
	else {
		return  0;
	}
}
TRANS_FUNC(BentPurelin) {
	if (dt == 0) {
		return  (GPUCPU pow(x * x + 1, 0.5) + 1) * 0.5 + x;
	}
	else {
		return  x / (2 * GPUCPU pow(x * x + 1, 0.5)) + 1;
	}
}
TRANS_FUNC(SymmetricalSigmoid) {
	if (dt == 0) {
		return  GPUCPU tanh(x * 0.5);
	}
	else {
		return  0.5 * (1 - GPUCPU pow(GPUCPU tanh(x * 0.5), 2));
	}
}
TRANS_FUNC(LogLog) {
	if (dt == 0) {
		return  1 - GPUCPU exp((-1) * GPUCPU exp(x));
	}
	else {
		return  GPUCPU exp(x - GPUCPU exp(x));
	}
}
TRANS_FUNC(Gaussian) {
	if (dt == 0) {
		return  GPUCPU exp((-1) * GPUCPU pow(x, 2));
	}
	else {
		return  -2 * x * trFunc(Gaussian, x);
	}
}
TRANS_FUNC(Absolute) {
	if (dt == 0) {
		return  GPUCPU Abs(x);
	}
	else {
		if (x > 0)return 1;
		else if (x < 0)return -1;
		else return  1;
	}
}
TRANS_FUNC(Sinusoid) {
	if (dt == 0) {
		return  GPUCPU sin(x);
	}
	else {
		return  GPUCPU cos(x);
	}
}
TRANS_FUNC(Cos) {
	if (dt == 0) {
		return  GPUCPU cos(x);
	}
	else {
		return  (-1) * GPUCPU sin(x);
	}
}
TRANS_FUNC(Sinc) {
	if (dt == 0) {
		if (x == 0)return 1;
		else return  GPUCPU sin(x) / x;
	}
	else {
		if (x == 0)return 0;
		else return  GPUCPU cos(x) / x - GPUCPU sin(x) / (x * x);
	}
}
TRANS_FUNC(Exp) {
	return  GPUCPU exp(x);
}
TRANS_FUNC(Ln) {
	if (dt == 0) {
		//if (x <= 0)	throw std::runtime_error("Ln(0) error.");
		return GPUCPU log(x > 0 ? x : VtEpslon);
	}
	else {
		//if (x == 0)	throw std::runtime_error("Division 0 error.");
		return  1.0 / (x > 0 ? x : VtEpslon);
	}
}
TRANS_FUNC(Reciprocal) {
	//if (x == 0)	throw std::runtime_error("Division 0 error.");
	if (x == 0)	return VtEpslon;
	if (dt == 0) {
		return 1.0 / x;
	}
	else {
		return  -1.0 / (double(x) * x);
	}
}

#if UseCppAmp > 0
TRANS_FUNC_G(Sigmoid) {
	if (dt == 0) {
		return 1.0 / (1.0 + GPU exp((-1.0) * x));
	}
	else {
		//Float v = (1.0 + GPU exp((-1.0) * x));
		Float v = GPU exp((-1.0) * x);
		return v / ((1.0 + v) * (1.0 + v));
	}
}
TRANS_FUNC_G(Purelin) {
	if (dt == 0) {
		return x;
	}
	else {
		return 1;
	}
}
TRANS_FUNC_G(Step) {
	if (dt == 0) {
		if (x >= 0)return 1;
		else return 0;
	}
	else {
		return 0;
	}
}
TRANS_FUNC_G(Relu) {
	if (dt == 0) {
		if (x >= 0)return x;
		else return 0;
	}
	else {
		if (x >= 0)return 1;
		else return 0;
	}
}
TRANS_FUNC_G(Tanh) {
	if (dt == 0) {
		return GPU tanh(x);
	}
	else {
		Float v = GPU tanh(x);
		return 1 - v * v;
	}
}
TRANS_FUNC_G(LeakyRelu) {
	if (dt == 0) {
		if (x >= 0)return x;
		else return 0.01 * x;
	}
	else {
		if (x >= 0)return 1;
		else return 0.01;
	}
}
TRANS_FUNC_G(PRelu) {
	if (dt == 0) {
		if (x >= 0)return x;
		else return 0.7 * x;
	}
	else {
		if (x >= 0)return 1;
		else return 0.7;
	}
}
TRANS_FUNC_G(ELU) {
	if (dt == 0) {
		if (x >= 0)return x;
		else return 0.7 * (GPU exp(x) - 1);
	}
	else {
		if (x >= 0)return 1;
		else {
			Float v;
			if (x >= 0)v = x;
			else v = 0.7 * (GPU exp(x) - 1);
			return	0.7 * GPU exp(x) - v + 0.7;
		}
	}
}
TRANS_FUNC_G(SELU) {
	if (dt == 0) {
		if (x >= 0)return x;
		else return 1.67326 * (GPU exp(x) - 1);
	}
	else {
		if (x >= 0)return 1.0507;
		else {
			Float v;
			if (x >= 0)v = x;
			else v = 1.67326 * (GPU exp(x) - 1);
			return  1.0507 * 1.67326 * GPU exp(x) - 1.0507 * (v + 1.67326);
		}
	}
}
TRANS_FUNC_G(SRelu) {
	if (dt == 0) {
		if (x >= 1)return 1 + 0.4 * (x - 1.0);
		else if (x <= -1)return -1 + 0.7 * (x - (-1.0));
		else return  x;
	}
	else {
		if (x >= 1)return 0.4;
		else if (x <= -1)return 0.7;
		else return  1;
	}
}
TRANS_FUNC_G(HardSigmoid) {
	if (dt == 0) {
		if (x > 2.5)return 1;
		else if (x < -2.5)return 0;
		else return  0.2 * x + 0.5;
	}
	else {
		if (x > 2.5)return 0;
		else if (x < -2.5)return 0;
		else return  0.2;
	}
}
TRANS_FUNC_G(HardTanh) {
	if (dt == 0) {
		if (x > 1)return 1;
		else if (x < -1)return -1;
		else return  x;
	}
	else {
		if (x > 1)return 0;
		else if (x < -1)return 0;
		else return  1;
	}
}
TRANS_FUNC_G(LeCunTanh) {
	if (dt == 0) {
		return  1.769 * GPU tanh(0.66666 * x);
	}
	else {
		return  1.7619 * 0.66666 * (1 - GPU pow(GPU tanh(0.66666 * x), 2));
	}
}
TRANS_FUNC_G(ArcTan) {
	if (dt == 0) {
		return  GPU pow(GPU tan(x), -1);
	}
	else {
		return  1 / (x * x + 1);
	}
}
TRANS_FUNC_G(SoftSign) {
	if (dt == 0) {
		return  x / (1 + Abs(x));
	}
	else {
		return  1 / GPU pow(Abs(x) + 1, 2);
	}
}
TRANS_FUNC_G(SoftPlus) {
	if (dt == 0) {
		return  GPU log(1 + GPU exp(x));
	}
	else {
		return  1 / (1 + GPU exp((-1.0) * x));
	}
}
TRANS_FUNC_G(Signum) {
	if (dt == 0) {
		if (x > 0)return 1;
		else if (x < 0)return -1;
		else return  0;
	}
	else {
		return  0;
	}
}
TRANS_FUNC_G(BentPurelin) {
	if (dt == 0) {
		return  (GPU pow(x * x + 1, 0.5) + 1) * 0.5 + x;
	}
	else {
		return  x / (2 * GPU pow(x * x + 1, 0.5)) + 1;
	}
}
TRANS_FUNC_G(SymmetricalSigmoid) {
	if (dt == 0) {
		return  GPU tanh(x * 0.5);
	}
	else {
		return  0.5 * (1 - GPU pow(GPU tanh(x * 0.5), 2));
	}
}
TRANS_FUNC_G(LogLog) {
	if (dt == 0) {
		return  1 - GPU exp((-1) * GPU exp(x));
	}
	else {
		return  GPU exp(x - GPU exp(x));
	}
}
TRANS_FUNC_G(Gaussian) {
	if (dt == 0) {
		return  GPU exp((-1) * GPU pow(x, 2));
	}
	else {
		return  -2 * x * (GPU exp((-1) * GPU pow(x, 2)));
	}
}
TRANS_FUNC_G(Absolute) {
	if (dt == 0) {
		return  Abs(x);
	}
	else {
		if (x > 0)return 1;
		else if (x < 0)return -1;
		else return  1;
	}
}
TRANS_FUNC_G(Sinusoid) {
	if (dt == 0) {
		return  GPU sin(x);
	}
	else {
		return  GPU cos(x);
	}
}
TRANS_FUNC_G(Cos) {
	if (dt == 0) {
		return  GPU cos(x);
	}
	else {
		return  (-1) * GPU sin(x);
	}
}
TRANS_FUNC_G(Sinc) {
	if (dt == 0) {
		if (x == 0)return 1;
		else return  GPU sin(x) / x;
	}
	else {
		if (x == 0)return 0;
		else return  GPU cos(x) / x - GPU sin(x) / (x * x);
	}
}
TRANS_FUNC_G(Exp) {
	return  GPU exp(x);
}
TRANS_FUNC_G(Ln) {
	if (dt == 0) {
		//if (x <= 0)	throw std::runtime_error("Ln(0) error.");
		return GPU log(x > 0 ? x : VtEpslon);
	}
	else {
		//if (x == 0)	throw std::runtime_error("Division 0 error.");
		return  1.0 / (x > 0 ? x : VtEpslon);
	}
}
TRANS_FUNC_G(Reciprocal) {
	//if (x == 0)	throw std::runtime_error("Division 0 error.");
	if (x == 0)	return VtEpslon;
	if (dt == 0) {
		return 1.0 / x;
	}
	else {
		return  -1.0 / (x * x);
	}
}


Float transFuncAmp(const Uint _transFuncType, const Float x) restrict(amp)
{
#define setPTFAMP(Name) case TF_##Name: return tf_##Name##_amp(x,false);
	switch (_transFuncType) {
		setPTFAMP(Sigmoid);
		setPTFAMP(Purelin);
		setPTFAMP(Step);
		setPTFAMP(Relu);
		setPTFAMP(Tanh);
		setPTFAMP(LeakyRelu);
		setPTFAMP(PRelu);
		setPTFAMP(ELU);
		setPTFAMP(SELU);
		setPTFAMP(SRelu);
		setPTFAMP(HardSigmoid);
		setPTFAMP(HardTanh);
		setPTFAMP(LeCunTanh);
		setPTFAMP(ArcTan);
		setPTFAMP(SoftSign);
		setPTFAMP(SoftPlus);
		setPTFAMP(Signum);
		setPTFAMP(BentPurelin);
		setPTFAMP(SymmetricalSigmoid);
		setPTFAMP(LogLog);
		setPTFAMP(Gaussian);
		setPTFAMP(Absolute);
		setPTFAMP(Sinusoid);
		setPTFAMP(Cos);
		setPTFAMP(Sinc);
		setPTFAMP(Exp);
		setPTFAMP(Ln);
	default:
		return 0.0;
	}
	return 0.0;
}
Float transFuncDevAmp(const Uint _transFuncType, const Float x) restrict(amp)
{
#define setPTFAMPDev(Name) case TF_##Name: return tf_##Name##_amp(x,true);
	switch (_transFuncType) {
		setPTFAMPDev(Sigmoid);
		setPTFAMPDev(Purelin);
		setPTFAMPDev(Step);
		setPTFAMPDev(Relu);
		setPTFAMPDev(Tanh);
		setPTFAMPDev(LeakyRelu);
		setPTFAMPDev(PRelu);
		setPTFAMPDev(ELU);
		setPTFAMPDev(SELU);
		setPTFAMPDev(SRelu);
		setPTFAMPDev(HardSigmoid);
		setPTFAMPDev(HardTanh);
		setPTFAMPDev(LeCunTanh);
		setPTFAMPDev(ArcTan);
		setPTFAMPDev(SoftSign);
		setPTFAMPDev(SoftPlus);
		setPTFAMPDev(Signum);
		setPTFAMPDev(BentPurelin);
		setPTFAMPDev(SymmetricalSigmoid);
		setPTFAMPDev(LogLog);
		setPTFAMPDev(Gaussian);
		setPTFAMPDev(Absolute);
		setPTFAMPDev(Sinusoid);
		setPTFAMPDev(Cos);
		setPTFAMPDev(Sinc);
		setPTFAMPDev(Exp);
		setPTFAMPDev(Ln);
	default:
		return 0.0;
	}
	return 0.0;
}
Float transFuncDevAmp(const Uint _transFuncType, const Float y, const Float x) restrict(amp) {
	//有先选用Y模式
	switch (_transFuncType) {
	case TF_Purelin:return 1;
	case TF_Sigmoid:return y * (1 - y);
	case TF_Tanh:return 1 - y * y;
	case TF_Relu:return y >= 0 ? 1 : 0;
	default:
		return transFuncDevAmp(_transFuncType, x);
	}
	return 0.0;
}

typedef Float(*PTransFuncAmp)(Float x, Bool dt);
Float transFuncAmp(const Uint _transFuncType, const Float x) restrict(amp);
Float transFuncDevAmp(const Uint _transFuncType, const Float x, const Float y) restrict(amp);

Bool CLBpExtend::__runByHardwareSpeedup(VLF* pOutEa, VLF* pOutLs, VLF* pOutMc, Bpnn::PCBMonitor _pCbFun, PVoid _pIns)
{
	//设置默认加速器
	std::vector<accelerator> accs = accelerator::get_all();
	accelerator acc_chosen = accs[0];
	accelerator acc_wrap;
	for (size_t i = 0; i < accs.size(); i++) {
		if (accs[i].device_path == accelerator::cpu_accelerator)
			continue;
		if (accs[i].device_path == accelerator::direct3d_warp)
			acc_wrap = accs[i];
		if (accs[i].dedicated_memory > acc_chosen.dedicated_memory) {
			acc_chosen = accs[i];
		}
	}
	if (acc_chosen.dedicated_memory == 0) {
		CLString str(("\nGPU acceleration is not available, the calculation mode will be changed automatically!\n"));
		auto idd = str.printf().messageBox(_lpBpnnMsgBoxTitle, MB_ICONWARNING | MB_YESNO);
		if (idd == IDYES)
			str.throw_runtime_error();
		else
			return false;
	}
	accelerator::set_default(acc_chosen.device_path);

	accelerator::set_default(acc_wrap.device_path);
	acc_chosen = acc_wrap;

	accelerator_view av = acc_chosen.create_view(queuing_mode::queuing_mode_automatic);

	//外显数据
	Bpnn::PMCBMonitor pCbFun = 0;//回调函数指针    
	Bpnn::PCBMonitor pCbFunSt = (Bpnn::PCBMonitor)_pCbFun;
	memcpy(&pCbFun, &_pCbFun, sizeof(Bpnn::PMCBMonitor));
	Bpnn* m_pInstance = reinterpret_cast<Bpnn*>(_pIns);    //调用对象 

	vector<Uint> __uPack;
	__uPack.push_back(kernel.layerCounts() - 1);
	auto ivs = vm_samSets->ivDataVec().cbegin();
	auto ive = vm_samSets->ivDataVec().cend();
	auto ovs = vm_samSets->tvDataVec().cbegin();
	auto ove = vm_samSets->tvDataVec().cend();
	concurrency::array<Float, 2> vi_samIv(concurrency::extent<2>(vm_samSets->size(), vm_samSets->intputDimension()), ivs, ive, av);
	concurrency::array<Float, 2> vi_samOv(concurrency::extent<2>(vm_samSets->size(), vm_samSets->targetDimension()), ovs, ove, av);
	__uPack.push_back(vm_samUsage->size());
	for (size_t i = 0; i < vm_samUsage->size(); i++)__uPack.push_back((*vm_samUsage)[i]);
	vector<BpnnGlobleInfo> vm_globleInfov(1); vm_globleInfov[0] = kernel.vm_globleInfo;
	__uPack.push_back(kernel.vm_globleInfo.iLayCounts); __uPack.push_back(kernel.vm_globleInfo.iNCounts);
	for (size_t i = 0; i < kernel.vm_globleInfo.iLayCounts; i++) {
		auto& li = kernel.vm_layInfo[i];
		__uPack.push_back(li.iLayIndex); //层号0开始
		__uPack.push_back(li.iLayNnCounts);//本层神经元个数
		__uPack.push_back(li.iLayNnStartIndex); //起始全局编号
		__uPack.push_back(li.iLayNnEndIndex); //终点全局编号	
		__uPack.push_back(li.bIsEndLay);//是否是尾层
	}
	//make wb--------------------------------------
	concurrency::array<Float, 1> vi_wji_Data(kernel.vm_wji_Data.size(), kernel.vm_wji_Data.begin(), av);
	concurrency::array<Float, 1> vi_wji_dt_Data(vm_wji_dt_Data.size(), vm_wji_dt_Data.begin(), av);
	concurrency::array<Float, 1> vi_wji_dt_old_Data(vm_wji_dt_old_Data.size(), vm_wji_dt_old_Data.begin(), av);
	concurrency::array<Float, 1> vi_bi_dt_old_Data(vm_bi_dt_old_Data.size(), vm_bi_dt_old_Data.begin(), av);

	//make nuaron-----------------------------------	
	concurrency::array<Float, 2> vi_yi(concurrency::extent<2>(kernel.neuronCounts(), vm_xy_span), vm_yi.begin(), av);
	concurrency::array<Uint, 2> vi_neuronExData(concurrency::extent<2>(kernel.neuronCounts(), vm_grad_span), vm_neuronExData.begin(), av);
	concurrency::array<Float, 2> vi_grad(concurrency::extent<2>(kernel.neuronCounts(), vm_grad_span), vm_grad.begin(), av);
	concurrency::array<Float, 2> vi_xi(concurrency::extent<2>(kernel.neuronCounts(), vm_xy_span), vm_xi.begin(), av);
	__uPack.push_back(vm_grad_span);
	__uPack.push_back(vm_xy_span);
	__uPack.push_back(kernel.vm_wbpack.size());
	for (size_t i = 0, si = kernel.vm_wbpack.size(); i < si; i++) {
		auto& li = kernel.vm_wbpack[i];
		__uPack.push_back(li.index);
		//Uint flag;//跟新标记
		__uPack.push_back(li.nnIds);//起始节点全局号
		__uPack.push_back(li.nnIde);//终了节点全局号
		//Float bi;
		__uPack.push_back(li.wji_Index);
		__uPack.push_back(li.wjiSi);
	}
	for (size_t i = 0, si = kernel.vm_neuron.size(); i < si; i++) {
		auto& li = kernel.vm_neuron[i];
		__uPack.push_back(li.id_index);
		__uPack.push_back(li.id_lay);
		__uPack.push_back(li.id_pos);
		__uPack.push_back(li.wcFuncType);
		__uPack.push_back(li.transFuncType);
		__uPack.push_back(li.wb);
		__uPack.push_back(li.link);
		//__uPack.push_back(li.linkSi);
		__uPack.push_back(li.linkBk);
		__uPack.push_back(li.linkBkSi);
		__uPack.push_back(li.flag);
	}
	__uPack.push_back(kernel.vm_link_Data.size());
	for (size_t i = 0, si = kernel.vm_link_Data.size(); i < si; i++) {
		auto lk = kernel.vm_link_Data[i];
		if (lk < 0)
			__uPack.push_back(UINT_MAX);
		else
			__uPack.push_back(lk);
	}
	__uPack.push_back(vm_linkBk_Data.size());
	for (size_t i = 0; i < vm_linkBk_Data.size(); i++) {
		__uPack.push_back(vm_linkBk_Data[i]);
	}
	__uPack.push_back(vm_linkBkw_Data.size());
	for (size_t i = 0; i < vm_linkBkw_Data.size(); i++) {
		__uPack.push_back(vm_linkBkw_Data[i]);
	}
	concurrency::array<Uint, 1> vi_uPack(__uPack.size(), __uPack.begin(), av);

	vector<Uint> vm_index(kernel.vm_layInfo.getLayerMaxNnCountsInNet());
	for (Uint i = 0, si = kernel.vm_layInfo.getLayerMaxNnCountsInNet(); i < si; i++)vm_index[i] = i;
	concurrency::array<Uint, 1> vi_index(kernel.vm_layInfo.getLayerMaxNnCountsInNet(), vm_index.begin(), av);
	vector<Uint> vm_wbindex(kernel.vm_wbpack.size());
	for (Uint i = 0, si = kernel.vm_wbpack.size(); i < si; i++)vm_wbindex[i] = i;
	concurrency::array<Uint, 1> vi_wbindex(vm_wbindex.size(), vm_wbindex.begin(), av);

	//判断模式
	Uint type = 0;
	if (kernel.vm_layInfo.getLayerMaxNnCountsInNet() * 3 < vm_samSets->size()) {
		//type = 1;
	}

	//#define USE_INDEX id_pos
#define USE_INDEX id_index

	Bool rt = false;
	runTimes = 0;
	Int c = 0, cb = -1;
	Float er = 10;
	CLTick tick;
	for (;;) {
		//ford--------------

		for (Uint i = 0, si = kernel.layerCounts(); i < si; i++)
		{
			//auto& vi_lay = vi_layNnPack[i];
			//auto& vi_layBf = i == 0 ? vi_layNnPack[vi_layNnPack.size() - 1] : vi_layNnPack[i - 1];
			//auto& vi_layxi = vi_layXiPack[i];
			//auto& vi_layyi = vi_layYiPack[i];
			//vi_layxi.discard_data();
			//vi_layyi.discard_data();
			if (type == 0) {
				try {
					concurrency::parallel_for_each(
						vi_index.extent.tile<1>(),
						[&, i](tiled_index<1> t_idx) restrict(amp)
						{
							Uint* vi_lastIndex = &vi_uPack[0];
							Uint* vi_samUsageSi = &vi_uPack[1];
							Uint* vi_samUsage = &vi_uPack[2];
							BpnnGlobleInfo* vi_globleInfo = (BpnnGlobleInfo*)(vi_samUsage + vi_samUsageSi[0]);
							layInfo* vi_LayInfo = (layInfo*)(vi_globleInfo + 1);
							Uint* vi_grad_span = (Uint*)(vi_LayInfo + vi_globleInfo->iLayCounts);
							Uint* vi_xy_span = ((Uint*)(vi_LayInfo + vi_globleInfo->iLayCounts) + 1);
							Uint* vi_wbpackSi = (Uint*)(vi_xy_span + 1);
							wbpack* vi_wbpack = (wbpack*)(vi_wbpackSi + 1);
							neuron* vi_lay = (neuron*)(vi_wbpack + vi_wbpackSi[0]);
							Uint* vi_linkDataSi = (Uint*)(vi_lay + vi_globleInfo->iNCounts);
							Uint* vi_linkData = (Uint*)(vi_linkDataSi + 1);
							//tile_static Uint a;
							Uint pos = vi_index[t_idx];
							Uint clay = i;
							//for (Uint clay = 0, nLay = vi_globleInfo[0].iLayCounts; clay < nLay; clay++) {
							if (pos < vi_LayInfo[clay].iLayNnCounts) {
								Uint index = (clay < 2 ? 0 : vi_LayInfo[clay - 1].iLayNnStartIndex) + pos;
								auto& nn = vi_lay[index];
								auto pWij0 = &vi_wji_Data[vi_wbpack[nn.wb].wji_Index + 1];
								auto plink0 = &vi_linkData[nn.link];
								auto silink = vi_wbpack[nn.wb].wjiSi;

								Float bi = vi_wji_Data[vi_wbpack[nn.wb].wji_Index];
								Float* pxi;
								//Float* pyi = &vi_layyi[nn.USE_INDEX][0];
								Float* pyi = &vi_yi[index * vi_xy_span[0]][0];
								const Float* piv0;
								if (nn.id_lay == 0) {
									for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
										Uint ci = vi_samUsage[l];
										//pxi = &vi_layxi[nn.USE_INDEX][l];
										pxi = &vi_xi[index * vi_xy_span[0]][l];
										piv0 = &vi_samIv[ci][0];
										(*pxi) = 0;
										for (Uint j = 0; j < silink; j++)
										{
											auto il = plink0[j];
											if (il == UINT_MAX)
												continue;
											(*pxi) += (pWij0[j] * piv0[il]);
										}
										(*pxi) += bi;
										pyi[l] = transFuncAmp(nn.transFuncType, (*pxi));//trans
									}
								}
								else {
									//auto p = &vi_layBf[0];
									//auto p = &vi_lay[vi_LayInfo[clay - 1].iLayNnStartIndex];
									auto p = vi_lay;
									//auto pxi0 = &vi_layxi[nn.USE_INDEX][0];
									auto pxi0 = &vi_xi[index * vi_xy_span[0]][0];
									for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
										pxi = &pxi0[l];
										(*pxi) = 0;
										for (Uint j = 0; j < silink; j++)
										{
											auto il = plink0[j];
											if (il == UINT_MAX)
												continue;
											auto& nn = p[il];
											(*pxi) += (pWij0[j] * vi_yi[nn.USE_INDEX * vi_xy_span[0]][l]);
										}
										(*pxi) += bi;
										pyi[l] = transFuncAmp(nn.transFuncType, (*pxi));//trans
									}
								}
							}
							//}
							t_idx.barrier.wait();
						}
					);
				}
				catch (const std::exception & e) {
					printf(e.what());
				}
			}
			else {

			}
			//vi_layxi.synchronize();
			//if(i == si-1)
			//vi_layyi.synchronize();
		}

		runTimes++;

		//yiOut = vi_layYiPack[vi_layYiPack.size() - 1];
		/*yiOut = vi_layYiPack[vi_layYiPack.size() - 1];
		memcpy_s(vm_yi.data() + kernel.LayStartNnIndex(vi_lastIndex[0]) * vm_xy_span,
			kernel.LayNnCounts(vi_lastIndex[0])* vm_xy_span * sizeof(Float),
			yiOut.data(),
			kernel.LayNnCounts(vi_lastIndex[0])* vm_xy_span * sizeof(Float)
		);*/
#define timeEclipse 1
		if (tick.getSpendTime() > timeEclipse) {
			try {
				vm_yi = vi_yi;
			}
			catch (const concurrency::accelerator_view_removed & ex)
			{
				cout << "\nTDR exception received: " << ex.what();
				cout << "\nError code:" << std::hex << ex.get_error_code();
				cout << "\nRemoved reason:" << std::hex
					<< ex.get_view_removed_reason();
			}
			er = Er();
		}
		//vm_yi = vi_yi;
		//concurrency::copy_async(yiOut.begin(), vi_layYiPack[vi_layYiPack.size() - 1]);
		//er = Er();
		if (er <= g_accuracy) {
			//if (false) {
			rt = true;
		}
		else
		{
			//grad-------------
			for (Int i = kernel.layerCounts() - 1; i >= 0; i--)
			{
				//auto& vi_lay = vi_layNnPack[i];
				//auto& vi_layBf = i == kernel.layerCounts() - 1 ? vi_layNnPack[0] : vi_layNnPack[i+1];
				//auto& vi_neuronExData = vi_layNnExData[i];
				//auto& vi_layxi = vi_layXiPack[i];
				//auto& vi_layyi = vi_layYiPack[i];
				//auto& vi_layGrad = vi_layGradPack[i];
				//auto& vi_layGrad2 = i == kernel.layerCounts() - 1 ? vi_layGradPack[0] : vi_layGradPack[i+1];
				//vi_layGrad.discard_data();
				if (type == 0) {
					concurrency::parallel_for_each(
						vi_index.extent.tile<1>(),
						[&, i](tiled_index<1> t_idx) restrict(amp)
						{
							Uint* vi_lastIndex = &vi_uPack[0];
							Uint* vi_samUsageSi = &vi_uPack[1];
							Uint* vi_samUsage = &vi_uPack[2];
							BpnnGlobleInfo* vi_globleInfo = (BpnnGlobleInfo*)(vi_samUsage + vi_samUsageSi[0]);
							layInfo* vi_LayInfo = (layInfo*)(vi_globleInfo + 1);
							Uint* vi_grad_span = (Uint*)(vi_LayInfo + vi_globleInfo->iLayCounts);
							Uint* vi_xy_span = ((Uint*)(vi_LayInfo + vi_globleInfo->iLayCounts) + 1);
							Uint* vi_wbpackSi = (Uint*)(vi_xy_span + 1);
							wbpack* vi_wbpack = (wbpack*)(vi_wbpackSi + 1);
							neuron* vi_lay = (neuron*)(vi_wbpack + vi_wbpackSi[0]);
							Uint* vi_linkDataSi = (Uint*)(vi_lay + vi_globleInfo->iNCounts);
							Uint* vi_linkData = (Uint*)(vi_linkDataSi + 1);
							Uint* vi_linkBkDataSi = (Uint*)(vi_linkData + vi_linkDataSi[0]);
							Uint* vi_linkBkData = (Uint*)(vi_linkBkDataSi + 1);
							Uint* vi_linkBkwDataSi = (Uint*)(vi_linkBkData + vi_linkBkDataSi[0]);
							Uint* vi_linkBkwData = (Uint*)(vi_linkBkwDataSi + 1);

							Uint clay = i;
							Uint pos = vi_index[t_idx];
							if (pos < vi_LayInfo[clay].iLayNnCounts) {
								Uint index = (clay < 2 ? 0 : vi_LayInfo[clay - 1].iLayNnStartIndex) + pos;
								auto& nn = vi_lay[index];
								/*auto pgrad0 = &vi_layGrad[nn.USE_INDEX][0];
								auto pyi0 = &vi_layyi[nn.USE_INDEX][0];
								auto pxi0 = &vi_layxi[nn.USE_INDEX][0];*/
								auto pgrad0 = &vi_grad[nn.USE_INDEX * vi_grad_span[0]][0];
								auto pyi0 = &vi_yi[nn.USE_INDEX * vi_xy_span[0]][0];
								auto pxi0 = &vi_xi[nn.USE_INDEX * vi_xy_span[0]][0];
								if (nn.id_lay >= vi_lastIndex[0]) //当前为输出层
								{
									for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
										Uint ci = vi_samUsage[l];
										pgrad0[l] = ls_MeanSquareLoss_amp(pyi0[l], vi_samOv[ci][nn.id_pos], true) * transFuncDevAmp(nn.transFuncType, pyi0[l], pxi0[l]);
									}
								}
								else if (!(nn.flag & FG_NN_Dropout)) {//节点未dropout
									//Uint bfIndex = vi_LayInfo[clay + 1].iLayNnStartIndex;
									Uint si = nn.linkBkSi;
									auto lkb0 = &vi_linkBkData[nn.linkBk];
									auto lkbw0 = &vi_linkBkwData[nn.linkBk];
									for (Uint i = 0; i < si; i++)//检索所有上层对本节点产生的链接
									{
										auto& p2 = vi_lay[/*bfIndex + */lkb0[i]];
										auto p2grad = &vi_grad[p2.USE_INDEX * vi_grad_span[0]][0];
										auto cw = vi_wji_Data[vi_wbpack[p2.wb].wji_Index + 1 + lkbw0[i]];
										//根据上层节点权值组合方式决定梯度向下层传递的类型及计算方式
										if (p2.wcFuncType == (Uint)EBP_WC::WC_Add) {//线性相加
											for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
												pgrad0[l] = (p2grad[l] * cw) * transFuncDevAmp(nn.transFuncType, pyi0[l], pxi0[l]);
											}
										}
										else if (p2.wcFuncType == (Uint)EBP_WC::WC_Max || p2.wcFuncType == (Uint)EBP_WC::WC_Min) {
											auto savePos = &vi_neuronExData[p2.USE_INDEX][0];
											//最大池化过程产生，若最大池化保存的id和本节点id相同，则向下层传递
											for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
												if (savePos[l] == nn.id_pos)
													pgrad0[l] = (p2grad[l] * cw) * transFuncDevAmp(nn.transFuncType, pyi0[l], pxi0[l]);
												else
													pgrad0[l] = 0;
											}
										}
										else if (p2.wcFuncType == (Uint)EBP_WC::WC_Average) {
											Uint p2linkSi = vi_wbpack[p2.wb].wjiSi;
											for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
												pgrad0[l] = (p2grad[l] / p2linkSi * cw) * transFuncDevAmp(nn.transFuncType, pyi0[l], pxi0[l]);//均值池化过程产生，上层梯度除以分均个数（即对下层链接数）
											}
										}
										else {
											//未知情况，传递0梯度
											for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
												pgrad0[l] = 0;
											}
										}
									}
								}
								else for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
									pgrad0[l] = 0;
								}
							}
							t_idx.barrier.wait();
						}
					);
				}
				else {
				}
			}
			auto _ls_ = g_ls;
			auto _mc_ = g_mc;
			concurrency::parallel_for_each(
				vi_wbindex.extent.tile<1>(),
				[&, _ls_, _mc_](tiled_index<1> t_idx) restrict(amp) {
					Uint* vi_lastIndex = &vi_uPack[0];
					Uint* vi_samUsageSi = &vi_uPack[1];
					Uint* vi_samUsage = &vi_uPack[2];
					BpnnGlobleInfo* vi_globleInfo = (BpnnGlobleInfo*)(vi_samUsage + vi_samUsageSi[0]);
					layInfo* vi_LayInfo = (layInfo*)(vi_globleInfo + 1);
					Uint* vi_grad_span = (Uint*)(vi_LayInfo + vi_globleInfo->iLayCounts);
					Uint* vi_xy_span = ((Uint*)(vi_LayInfo + vi_globleInfo->iLayCounts) + 1);
					Uint* vi_wbpackSi = (Uint*)(vi_xy_span + 1);
					wbpack* vi_wbpack = (wbpack*)(vi_wbpackSi + 1);
					neuron* vi_lay = (neuron*)(vi_wbpack + vi_wbpackSi[0]);
					Uint* vi_linkDataSi = (Uint*)(vi_lay + vi_globleInfo->iNCounts);
					Uint* vi_linkData = (Uint*)(vi_linkDataSi + 1);


					auto& wb = vi_wbpack[t_idx.global[0]];
					auto pWji_dt_old0 = (&vi_wji_dt_old_Data[(wb).wji_Index + 1]);
					auto pWji_dt0 = (&vi_wji_dt_Data[(wb).wji_Index + 1]);
					auto pWji0 = (&(vi_wji_Data)[(wb).wji_Index + 1]);
					auto wjiSi = wb_Wji_size(wb);
					for (Uint i = 0; i < wjiSi; i++)
						pWji_dt0[i] = 0;
					Float bi_dt = 0;
					auto& bi_dt_old = vi_bi_dt_old_Data[(wb).index];
					auto clay = vi_lay[wb.nnIds].id_lay;
					if (clay == 0) {
						for (auto nci = wb.nnIds, ie = wb.nnIde; nci <= ie; nci++)
						{
							auto& nn = vi_lay[nci];
							if (nn.flag & FG_WB_NotUpdate || nn.flag & FG_NN_Dropout)
								continue;
							auto link = &vi_linkData[nn.link];
							auto grad = &vi_grad[nn.id_index][0];
							for (Uint _i = 0; _i < wjiSi; _i++)
							{
								auto i = link[_i];
								if (i == UINT_MAX)
									continue;
								for (Uint l = 0; l < vi_samUsageSi[0]; l++)
									pWji_dt0[_i] += (grad[l] * (vi_samIv[vi_samUsage[l]][i]));
							}
							for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
								bi_dt += grad[l];
							}
						}
					}
					else {
						//auto pnn = &vi_lay[vi_LayInfo[clay - 1].iLayNnStartIndex];
						for (auto nci = wb.nnIds, ie = wb.nnIde; nci <= ie; nci++)
						{
							auto& nn = vi_lay[nci];
							if (nn.flag & FG_WB_NotUpdate || nn.flag & FG_NN_Dropout)
								continue;
							auto link = &vi_linkData[nn.link];
							auto grad = &vi_grad[nn.id_index][0];
							for (Uint _i = 0; _i < wjiSi; _i++)
							{
								auto i = link[_i];
								if (i == UINT_MAX)
									continue;
								auto preyi = &vi_yi[(nn).id_index][0];
								for (Uint l = 0; l < vi_samUsageSi[0]; l++)
									pWji_dt0[_i] += (grad[l] * preyi[l]);
							}
							for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
								bi_dt += grad[l];
							}
						}
					}
					//更新权值
					for (Uint i = 0; i < wjiSi; i++)
					{
						Float deltaWi = (1.0 - _mc_) * _ls_ * pWji_dt0[i] + _mc_ * pWji_dt_old0[i];
						pWji0[i] -= deltaWi;
						pWji_dt_old0[i] = deltaWi;
					}
					//更新阈值
					Float deltaBi = (1.0 - _mc_) * _ls_ * bi_dt + _mc_ * bi_dt_old;
					vi_wji_Data[(wb).wji_Index] -= deltaBi;
					bi_dt_old = deltaBi;
					t_idx.barrier.wait();
				}
			);
			if (tick.getSpendTime() > timeEclipse)
			{
				try {
					kernel.vm_wji_Data = vi_wji_Data;
					vm_wji_dt_old_Data = vi_wji_dt_old_Data;
					vm_bi_dt_old_Data = vi_bi_dt_old_Data;
				}
				catch (const concurrency::accelerator_view_removed & ex)
				{
					cout << "\nTDR exception received: " << ex.what();
					cout << "\nError code:" << std::hex << ex.get_error_code();
					cout << "\nRemoved reason:" << std::hex
						<< ex.get_view_removed_reason();
				}
			}
		}

		if (tick.getSpendTime() > timeEclipse) {
			tick.timingStart();
		}
		//Float er = 0;
		if (pOutEa)pOutEa->push_back(er);
		if (pOutLs)pOutLs->push_back(g_ls);
		if (pOutMc)pOutMc->push_back(g_mc);
		if (bOpenGraph) {
			_ea.push_back(er), _ls.push_back(g_ls), _mc.push_back(g_mc);
			if (isCorrectRateEvaluationModel())
				_cr.push_back(getSavedCorrectRate());
		}
		if (pCbFunSt) {//外显步骤计算
			c = Int(Float(runTimes) / Float(maxTimes) * 100);
			if (rt) {
				cb = -1; c = 100;
			}
			if (c != cb) {
				if (m_pInstance) {
					if (pCbFun)(m_pInstance->*pCbFun)(Int(runTimes), Int(maxTimes), ("Net is in training.")); //类对象内部函数作为外显回调的情况
				}
				else {
					(*pCbFunSt)(Int(runTimes), Int(maxTimes), ("Net is in training.")); //全局或静态函数作为外显回调的情况
				}
				cb = c;
			}
		}
		if (rt)
			goto end1;
		if (runTimes >= maxTimes)
		{
			rt = false; goto end1;
		}
		if (!prepairTrainSamUsageData()) {//为下一循环做准备
			rt = false; goto end1;
		}
	}

end1:
	try {
		vm_yi = vi_yi;
		kernel.vm_wji_Data = vi_wji_Data;
		vm_wji_dt_old_Data = vi_wji_dt_old_Data;
		vm_bi_dt_old_Data = vi_bi_dt_old_Data;
	}
	catch (const concurrency::accelerator_view_removed & ex)
	{
		cout << "\nTDR exception received: " << ex.what();
		cout << "\nError code:" << std::hex << ex.get_error_code();
		cout << "\nRemoved reason:" << std::hex
			<< ex.get_view_removed_reason();
	}

	er = Er();
	if (er <= g_accuracy)
		rt = true;
	//#endif
	return rt;
}

Bool CLBpExtend::__runByHardwareSpeedup2(VLF* pOutEa, VLF* pOutLs, VLF* pOutMc, Bpnn::PCBMonitor _pCbFun, PVoid _pIns)
{
	//设置默认加速器
	std::vector<accelerator> accs = accelerator::get_all();
	accelerator acc_chosen = accs[0];
	for (size_t i = 0; i < accs.size(); i++) {
		if (accs[i].device_path == accelerator::cpu_accelerator)
			continue;
		if (accs[i].dedicated_memory > acc_chosen.dedicated_memory) {
			acc_chosen = accs[i];
		}
	}
	if (acc_chosen.dedicated_memory == 0) {
		CLString str(("\nGPU acceleration is not available, the calculation mode will be changed automatically!\n"));
		auto idd = str.printf().messageBox(_lpBpnnMsgBoxTitle, MB_ICONWARNING | MB_YESNO);
		if (idd == IDYES)
			str.throw_runtime_error();
		else
			return false;
	}
	accelerator::set_default(acc_chosen.device_path);
	accelerator_view av = accelerator().create_view(queuing_mode::queuing_mode_automatic);

	//外显数据
	Bpnn::PMCBMonitor pCbFun = 0;//回调函数指针    
	Bpnn::PCBMonitor pCbFunSt = (Bpnn::PCBMonitor)_pCbFun;
	memcpy(&pCbFun, &_pCbFun, sizeof(Bpnn::PMCBMonitor));
	Bpnn* m_pInstance = reinterpret_cast<Bpnn*>(_pIns);    //调用对象 

#define _Const2 const
	array_view<_Const2 Float, 2> vi_samIv(concurrency::extent<2>(vm_samSets->size(), vm_samSets->intputDimension()), vm_samSets->ivData());
	array_view<_Const2 Float, 2> vi_samOv(concurrency::extent<2>(vm_samSets->size(), vm_samSets->targetDimension()), vm_samSets->tvData());
	array_view<_Const2 Uint, 1> vi_samUsage(vm_samUsage->size(), vm_samUsage->data());
	Uint samUsgSi = vm_samUsage->size();
	array_view<_Const2 Uint, 1> vi_samUsageSi(1, &samUsgSi);
	Float vm_inputTemp = 0;
	array_view<_Const2 Float, 1>  vi_input((kernel.inVec) ? kernel.inputDimension() : 1, (kernel.inVec) ? ((kernel.inVec)) : &vm_inputTemp);
	array_view<_Const2 BpnnGlobleInfo, 1> vi_globleInfo(1, &kernel.vm_globleInfo);
	array_view<_Const2 layInfo, 1> vi_LayInfo(kernel.vm_layInfo.size(), kernel.vm_layInfo.data());

	////make wb--------------------------------------
	array_view<_Const2 wbpack, 1> vi_wbpack(kernel.vm_wbpack.size(), kernel.vm_wbpack.data());
	array_view<Float, 1> vi_wji_Data(kernel.vm_wji_Data.size(), kernel.vm_wji_Data.data());
	array_view<Float, 1> vi_wji_dt_Data(vm_wji_dt_Data.size(), vm_wji_dt_Data.data());
	array_view<Float, 1> vi_wji_dt_old_Data(vm_wji_dt_old_Data.size(), vm_wji_dt_old_Data.data());
	array_view<Float, 1> vi_bi_dt_old_Data(vm_bi_dt_old_Data.size(), vm_bi_dt_old_Data.data());

	////make nuaron-----------------------------------	
	array_view<_Const2 neuron, 1> vi_neuron(kernel.vm_neuron.size(), kernel.vm_neuron.data());
	//array_view<Uint, 2> vi_neuronExData(concurrency::extent<2>(kernel.neuronCounts(), vm_samUsageSi), vm_neuronExData);
	//array_view<Float, 2> vi_grad(concurrency::extent<2>(kernel.neuronCounts(), vm_samUsageSi), vm_grad);
	//array_view<Float, 2> vi_xi(concurrency::extent<2>(kernel.neuronCounts(), vm_samUsageSi), vm_xi);
	array_view<Float, 2> vi_yi(concurrency::extent<2>(kernel.neuronCounts(), vm_xy_span), vm_yi.data());
	array_view<_Const2 Int, 1> vi_linkData(kernel.vm_link_Data.size(), kernel.vm_link_Data.data());
	array_view<_Const2 Uint, 1> vi_linkBkData(vm_linkBk_Data.size(), vm_linkBk_Data.data());
	array_view<_Const2 Uint, 1> vi_linkBkwData(vm_linkBkw_Data.size(), vm_linkBkw_Data.data());
	////end make nuaron-----------------------------------	
	Uint vm_lastIndex = kernel.layerCounts() - 1;
	array_view<_Const2 Uint, 1> vi_lastIndex(1, &vm_lastIndex);
	//vector<array_view<_Const2 neuron, 1>> vi_layNnPack;
	vector<array_view<Uint, 2>> vi_layNnExData;
	vector<array_view<Float, 2>> vi_layXiPack;
	//vector<array_view<Float, 2>> vi_layYiPack;
	//vector<array_view<Float, 2>> vi_layGradPack;
	for (Uint i = 0, si = kernel.layerCounts(); i < si; i++)
	{
		//vi_layNnPack.push_back(array_view<_Const2 neuron, 1>(kernel.LayNnCounts(i), kernel.LayStartNn(i)));
		vi_layNnExData.push_back(array_view<Uint, 2>(concurrency::extent<2>(kernel.LayNnCounts(i), vm_grad_span), &vm_neuronExData.data()[kernel.LayStartNnIndex(i) * vm_grad_span]));
		//vi_layGradPack.push_back(array_view<Float, 2>(concurrency::extent<2>(kernel.LayNnCounts(i), vm_grad_span), &vm_grad.data()[kernel.LayStartNnIndex(i) * vm_grad_span]));
		vi_layXiPack.push_back(array_view<Float, 2>(concurrency::extent<2>(kernel.LayNnCounts(i), vm_xy_span), &vm_xi.data()[kernel.LayStartNnIndex(i) * vm_xy_span]));
		//vi_layYiPack.push_back(array_view<Float, 2>(concurrency::extent<2>(kernel.LayNnCounts(i), vm_xy_span), &vm_yi.data()[kernel.LayStartNnIndex(i) * vm_xy_span]));
	}
	array_view<_Const2 Uint, 1> vi_grad_span(1, &vm_grad_span);
	array_view<_Const2 Uint, 1> vi_xy_span(1, &vm_xy_span);
	array_view<Float, 2> vi_grad(concurrency::extent<2>(kernel.neuronCounts(), vm_grad_span), vm_grad.data());

	//make nuaron-----------------------------------	

	vector<Uint> vm_index(kernel.vm_layInfo.getLayerMaxNnCountsInNet());
	for (Uint i = 0, si = kernel.vm_layInfo.getLayerMaxNnCountsInNet(); i < si; i++)vm_index[i] = i;
	array_view<_Const2 Uint, 1> vi_index(vm_index);
	vector<Uint> vm_wbindex(kernel.vm_wbpack.size());
	for (Uint i = 0, si = kernel.vm_wbpack.size(); i < si; i++)vm_wbindex[i] = i;
	array_view<_Const2 Uint, 1> vi_wbindex(vm_wbindex);

	//判断模式
	Uint type = 0;
	if (kernel.vm_layInfo.getLayerMaxNnCountsInNet() * 3 < vm_samSets->size()) {
		//type = 1;
	}

#define USE_INDEX2 id_pos

	Bool rt = false;
	runTimes = 0;
	Int c = 0, cb = -1;
	Float er = 10;
	CLTick tick;
	for (;;) {
		//ford--------------
		vi_yi.discard_data();
		for (Uint i = 0, si = kernel.layerCounts(); i < si; i++)
		{
			//auto& vi_lay = vi_layNnPack[i];
			//auto& vi_layBf = i == 0 ? vi_layNnPack[vi_layNnPack.size() - 1] : vi_layNnPack[i - 1];
			auto& vi_layxi = vi_layXiPack[i];
			//auto& vi_layyi = vi_layYiPack[i];
			vi_layxi.discard_data();
			//vi_layyi.discard_data();
			if (type == 0) {
				try {
					concurrency::parallel_for_each(
						vi_index.extent.tile<1>(),
						[=](tiled_index<1> t_idx) restrict(amp)
						{
							Uint pos = vi_index[t_idx];
							Uint clay = i;
							//for (Uint clay = 0, nLay = vi_globleInfo[0].iLayCounts; clay < nLay; clay++) {
							if (pos < vi_LayInfo[clay].iLayNnCounts) {
								auto& nn = vi_neuron[pos + (clay < 2 ? 0 : vi_LayInfo[clay - 1].iLayNnStartIndex)];
								auto pWij0 = &vi_wji_Data[vi_wbpack[nn.wb].wji_Index + 1];
								auto plink0 = &vi_linkData[nn.link];
								auto silink = vi_wbpack[nn.wb].wjiSi;

								Float bi = vi_wji_Data[vi_wbpack[nn.wb].wji_Index];
								Float* pxi;
								Float* pyi = &vi_yi[nn.id_index][0];
								//Float* pyi = &vi_yi[index * vi_xy_span[0]][0];
								const Float* piv0;
								if (nn.id_lay == 0) {
									for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
										Uint ci = vi_samUsage[l];
										pxi = &vi_layxi[nn.USE_INDEX2][l];
										//pxi = &vi_xi[index * vi_xy_span[0]][l];
										piv0 = &vi_samIv[ci][0];
										(*pxi) = 0;
										for (Uint j = 0; j < silink; j++)
										{
											auto il = plink0[j];
											if (il < 0)
												continue;
											(*pxi) += (pWij0[j] * piv0[il]);
										}
										(*pxi) += bi;
										pyi[l] = transFuncAmp(nn.transFuncType, (*pxi));//trans
									}
								}
								else {
									auto p = &vi_neuron[0];
									//auto p = &vi_lay[vi_LayInfo[clay - 1].iLayNnStartIndex];
									auto pxi0 = &vi_layxi[nn.USE_INDEX2][0];
									//auto pxi0 = &vi_xi[index * vi_xy_span[0]][0];
									for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
										pxi = &pxi0[l];
										(*pxi) = 0;
										for (Uint j = 0; j < silink; j++)
										{
											auto il = plink0[j];
											if (il < 0)
												continue;
											auto& nn = p[il];
											(*pxi) += (pWij0[j] * vi_yi[nn.id_index][l]);
										}
										(*pxi) += bi;
										pyi[l] = transFuncAmp(nn.transFuncType, (*pxi));//trans
									}
								}
							}
							//}
							t_idx.barrier.wait();
						}
					);
				}
				catch (const std::exception & e) {
					printf(e.what());
				}
			}
			else {

			}
		}

		runTimes++;
#define timeEclipse2 0.1
		if (tick.getSpendTime() > timeEclipse2) {
			try {
				vi_yi.synchronize();
			}
			catch (const concurrency::accelerator_view_removed & ex)
			{
				cout << "\nTDR exception received: " << ex.what();
				cout << "\nError code:" << std::hex << ex.get_error_code();
				cout << "\nRemoved reason:" << std::hex
					<< ex.get_view_removed_reason();
			}
			er = Er();
		}
		if (er <= g_accuracy) {
			rt = true;
		}
		else
		{
			//grad-------------
			vi_grad.discard_data();
			for (Int i = kernel.layerCounts() - 1; i >= 0; i--)
			{
				//auto& vi_lay = vi_layNnPack[i];
				//auto& vi_layBf = i == kernel.layerCounts() - 1 ? vi_layNnPack[0] : vi_layNnPack[i+1];
				auto& vi_neuronExData = vi_layNnExData[i];
				auto& vi_layxi = vi_layXiPack[i];
				if (type == 0) {
					concurrency::parallel_for_each(
						vi_index.extent.tile<1>(),
						[=](tiled_index<1> t_idx) restrict(amp)
						{

							Uint clay = i;
							Uint pos = vi_index[t_idx];
							if (pos < vi_LayInfo[clay].iLayNnCounts) {
								//Uint index = vi_LayInfo[clay].iLayNnStartIndex + pos;
								auto& nn = vi_neuron[pos + (clay < 2 ? 0 : vi_LayInfo[clay - 1].iLayNnStartIndex)];
								auto pgrad0 = &vi_grad[nn.id_index][0];
								auto pyi0 = &vi_yi[nn.id_index][0];
								auto pxi0 = &vi_layxi[nn.USE_INDEX2][0];

								if (nn.id_lay >= vi_lastIndex[0]) //当前为输出层
								{
									for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
										Uint ci = vi_samUsage[l];
										pgrad0[l] = ls_MeanSquareLoss_amp(pyi0[l], vi_samOv[ci][nn.id_pos], true) * transFuncDevAmp(nn.transFuncType, pyi0[l], pxi0[l]);
									}
								}
								else if (!(nn.flag & FG_NN_Dropout)) {//节点未dropout
									//Uint bfIndex = vi_LayInfo[clay + 1].iLayNnStartIndex;
									Uint si = nn.linkBkSi;
									auto lkb0 = &vi_linkBkData[nn.linkBk];
									auto lkbw0 = &vi_linkBkwData[nn.linkBk];
									for (Uint i = 0; i < si; i++)//检索所有上层对本节点产生的链接
									{
										auto& p2 = vi_neuron[lkb0[i]];
										auto p2grad = &vi_grad[p2.id_index][0];
										auto cw = vi_wji_Data[vi_wbpack[p2.wb].wji_Index + 1 + lkbw0[i]];
										//根据上层节点权值组合方式决定梯度向下层传递的类型及计算方式
										if (p2.wcFuncType == (Uint)EBP_WC::WC_Add) {//线性相加
											for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
												pgrad0[l] = (p2grad[l] * cw) * transFuncDevAmp(nn.transFuncType, pyi0[l], pxi0[l]);
											}
										}
										else if (p2.wcFuncType == (Uint)EBP_WC::WC_Max || p2.wcFuncType == (Uint)EBP_WC::WC_Min) {
											auto savePos = &vi_neuronExData[p2.USE_INDEX2][0];
											//最大池化过程产生，若最大池化保存的id和本节点id相同，则向下层传递
											for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
												if (savePos[l] == nn.id_pos)
													pgrad0[l] = (p2grad[l] * cw) * transFuncDevAmp(nn.transFuncType, pyi0[l], pxi0[l]);
												else
													pgrad0[l] = 0;
											}
										}
										else if (p2.wcFuncType == (Uint)EBP_WC::WC_Average) {
											Uint p2linkSi = vi_wbpack[p2.wb].wjiSi;
											for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
												pgrad0[l] = (p2grad[l] / p2linkSi * cw) * transFuncDevAmp(nn.transFuncType, pyi0[l], pxi0[l]);//均值池化过程产生，上层梯度除以分均个数（即对下层链接数）
											}
										}
										else {
											//未知情况，传递0梯度
											for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
												pgrad0[l] = 0;
											}
										}
									}
								}
								else for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
									pgrad0[l] = 0;
								}
							}
							t_idx.barrier.wait();
						}
					);
				}
				else {
				}
			}
			if (tick.getSpendTime() > timeEclipse2) {
				try {
					vi_grad.synchronize();
				}
				catch (const concurrency::accelerator_view_removed & ex)
				{
					cout << "\nTDR exception received: " << ex.what();
					cout << "\nError code:" << std::hex << ex.get_error_code();
					cout << "\nRemoved reason:" << std::hex
						<< ex.get_view_removed_reason();
				}
			}

			auto _ls_ = g_ls;
			auto _mc_ = g_mc;
			concurrency::parallel_for_each(
				vi_wbindex.extent.tile<1>(),
				[=](tiled_index<1> t_idx) restrict(amp) {

					auto& wb = vi_wbpack[t_idx.global[0]];
					auto pWji_dt_old0 = (&vi_wji_dt_old_Data[(wb).wji_Index + 1]);
					auto pWji_dt0 = (&vi_wji_dt_Data[(wb).wji_Index + 1]);
					auto pWji0 = (&(vi_wji_Data)[(wb).wji_Index + 1]);
					auto wjiSi = wb_Wji_size(wb);
					for (Uint i = 0; i < wjiSi; i++)
						pWji_dt0[i] = 0;
					Float bi_dt = 0;
					auto& bi_dt_old = vi_bi_dt_old_Data[(wb).index];
					auto clay = vi_neuron[wb.nnIds].id_lay;
					if (clay == 0) {
						for (auto nci = wb.nnIds, ie = wb.nnIde; nci <= ie; nci++)
						{
							auto& nn = vi_neuron[nci];
							if (nn.flag & FG_WB_NotUpdate || nn.flag & FG_NN_Dropout)
								continue;
							auto link = &vi_linkData[nn.link];
							auto grad = &vi_grad[nn.id_index][0];
							for (Uint _i = 0; _i < wjiSi; _i++)
							{
								auto i = link[_i];
								if (i < 0)
									continue;
								for (Uint l = 0; l < vi_samUsageSi[0]; l++)
									pWji_dt0[_i] += (grad[l] * (vi_samIv[vi_samUsage[l]][i]));
							}
							for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
								bi_dt += grad[l];
							}
						}
					}
					else {
						auto pnn = &vi_neuron[vi_LayInfo[clay - 1].iLayNnStartIndex];
						for (auto nci = wb.nnIds, ie = wb.nnIde; nci <= ie; nci++)
						{
							auto& nn = vi_neuron[nci];
							if (nn.flag & FG_WB_NotUpdate || nn.flag & FG_NN_Dropout)
								continue;
							auto link = &vi_linkData[nn.link];
							auto grad = &vi_grad[nn.id_index][0];
							for (Uint _i = 0; _i < wjiSi; _i++)
							{
								auto i = link[_i];
								if (i < 0)
									continue;
								auto preyi = &vi_yi[(nn).id_index][0];
								for (Uint l = 0; l < vi_samUsageSi[0]; l++)
									pWji_dt0[_i] += (grad[l] * preyi[l]);
							}
							for (Uint l = 0; l < vi_samUsageSi[0]; l++) {
								bi_dt += grad[l];
							}
						}
					}
					//更新权值
					for (Uint i = 0; i < wjiSi; i++)
					{
						Float deltaWi = (1.0 - _mc_) * _ls_ * pWji_dt0[i] + _mc_ * pWji_dt_old0[i];
						pWji0[i] -= deltaWi;
						pWji_dt_old0[i] = deltaWi;
					}
					//更新阈值
					Float deltaBi = (1.0 - _mc_) * _ls_ * bi_dt + _mc_ * bi_dt_old;
					vi_wji_Data[(wb).wji_Index] -= deltaBi;
					bi_dt_old = deltaBi;
					t_idx.barrier.wait();
				}
			);
			if (tick.getSpendTime() > timeEclipse2)
			{
				try {
					vi_wji_Data.synchronize();
					vi_wji_dt_old_Data.synchronize();
					vi_bi_dt_old_Data.synchronize();
				}
				catch (const concurrency::accelerator_view_removed & ex)
				{
					cout << "\nTDR exception received: " << ex.what();
					cout << "\nError code:" << std::hex << ex.get_error_code();
					cout << "\nRemoved reason:" << std::hex
						<< ex.get_view_removed_reason();
				}
			}
		}

		if (tick.getSpendTime() > timeEclipse2) {
			tick.timingStart();
		}
		//Float er = 0;
		if (pOutEa)pOutEa->push_back(er);
		if (pOutLs)pOutLs->push_back(g_ls);
		if (pOutMc)pOutMc->push_back(g_mc);
		if (bOpenGraph) {
			_ea.push_back(er), _ls.push_back(g_ls), _mc.push_back(g_mc);
			if (isCorrectRateEvaluationModel())
				_cr.push_back(getSavedCorrectRate());
		}
		if (pCbFunSt) {//外显步骤计算
			c = Int(Float(runTimes) / Float(maxTimes) * 100);
			if (rt) {
				cb = -1; c = 100;
			}
			if (c != cb) {
				if (m_pInstance) {
					if (pCbFun)(m_pInstance->*pCbFun)(Int(runTimes), Int(maxTimes), ("Net is in training.")); //类对象内部函数作为外显回调的情况
				}
				else {
					(*pCbFunSt)(Int(runTimes), Int(maxTimes), ("Net is in training.")); //全局或静态函数作为外显回调的情况
				}
				cb = c;
			}
		}
		if (rt)
			goto end1;
		if (runTimes >= maxTimes)
		{
			rt = false; goto end1;
		}
		if (!prepairTrainSamUsageData()) {//为下一循环做准备
			rt = false; goto end1;
		}
	}

end1:
	try {
		vi_yi.synchronize();
		vi_wji_Data.synchronize();
		vi_wji_dt_old_Data.synchronize();
		vi_bi_dt_old_Data.synchronize();
	}
	catch (const concurrency::accelerator_view_removed & ex)
	{
		cout << "\nTDR exception received: " << ex.what();
		cout << "\nError code:" << std::hex << ex.get_error_code();
		cout << "\nRemoved reason:" << std::hex
			<< ex.get_view_removed_reason();
	}

	er = Er();
	if (er <= g_accuracy)
		rt = true;
	//#endif
	return rt;
}

#endif

Bool readBpnnStructDefFromFile(BpnnStructDef& mod, PCStr lpFile)
{
	mod.clear();
	CLString file = lpFile;
	if (file.fileExists() == FALSE)
		return false;
	CLString tt(500);
	Bool rt = false;
	Int i = 0, ci = 0;
	Float badv;
	Uint alllays = 0, allBlocks = 0, gWbSi = 0;
	Int clayer = -1, cpos = -1;
	Uint cgwbi = 0;
	mod.clear();//清理自定义模式
	while (tt.readLineFromFile(i++ == 0 ? file.string() : NULL) != -1) {
		CLRemoveNotes(tt);
		if (tt.size() == 0)
			continue;
		if (ci == 0) { //文件启动判断
			if (tt != _lpBpNnModeFileFlag) {
				rt = false; goto Ret1;
			}
			ci = 4;
		}
		else if (ci == 4) {//取全局Wb数据个数
			tt* CLComma >> gWbSi;
			if (gWbSi > 0)
				mod.gWb.resize(gWbSi), ci = 5;
			else
				ci = 1;
		}
		else if (ci == 5) {//取全局Wb数据
			auto& pwb = mod.gWb.at(cgwbi);
			pwb.reset();
			Uint nw;
			tt* CLComma;
			tt >> pwb.bNotUpdate >> nw;
			pwb.bi.resize(nw, 0);
			for (Uint i = 0; i < nw; i++)
			{
				tt >> pwb.bi[i];
			}
			tt >> pwb.wiSize >> nw;
			pwb.wi.resize(nw, 0);
			tt >> pwb.wi[0];
			for (Uint i = 1; i < nw; i++)
			{
				tt >> pwb.wi[i];
			}
			if (++cgwbi >= gWbSi)
				ci = 1;
		}
		else if (ci == 1) {//读取全局层数据
			tt* CLComma >> alllays;
			mod.reserve(alllays);
			if (alllays > 0)
				ci = 2;
		}
		else if (ci == 2) {//读取层组块总数数据
			tt* CLComma >> allBlocks;
			if (allBlocks > 0) {
				if (++clayer >= (Int)mod.size())	//生成新层
					mod.push_back(BSLayer());
				auto pl = &mod.at(mod.size() - 1);
				pl->reserve(allBlocks);
				ci = 3;
				cpos = -1;
			}
		}
		else if (ci == 3) {
			auto pl = &mod.at(clayer);
			++cpos;
			if (cpos == 0)
				pl->resize(allBlocks);
			auto plb = &pl->at(cpos);
			Uint lkSi = 0;
			Int _cpos = 0;
			Int bitype = 0;
			plb->bi.clear();
			plb->linkPos.clear();
			plb->linkWij.clear();
			tt* CLComma >> (plb->nnCounts) >> (plb->transFuncType) >> (plb->wcFuncType) >> lkSi;
			//读取链接
			for (Uint k = 0; k < lkSi; k++)
			{
				tt >> _cpos;
				plb->linkPos.push_back(_cpos);
			}
			tt >> (plb->globleWbId);
			if (plb->globleWbId < 0) {//使用局部值
				tt >> bitype >> (badv) >> (lkSi);
				//都权值
				if (bitype < 0) {
				}
				else plb->bi.push_back(badv);
				//读取权值预设
				tt >> lkSi;
				plb->linkWij.reserve(lkSi);
				for (Uint k = 0; k < lkSi; k++)
				{
					tt >> badv;
					plb->linkWij.push_back(badv);
				}
			}
			if (cpos + 1 >= Int(allBlocks)) {
				ci = 2;
				if (clayer + 1 >= Int(alllays))
					break;
			}
		}
	}
	rt = true;
Ret1:
	tt.closeFile();
	return rt;
}

BpnnStructDef& writeBpnnStructDefToFile(BpnnStructDef& mod, PCStr lpFileFullPathName)
{
	if (!lpFileFullPathName) {
		return mod;
	}
	CLString tt(90000);
	tt% _lpBpNnModeFileFlag << CLRet;
	DeleteFile(lpFileFullPathName);
	if (!CLString::createDirectoryByFileName(lpFileFullPathName) ||
		!tt.writeToFile(lpFileFullPathName, CREATE_ALWAYS))
		return mod;
	Int f1 = 0, f2 = 0, f3 = 0, f4 = 0, f5 = 0;
	//写入总层数
	Uint gWbSi = mod.gWb.size();
	tt% gWbSi << ("    #网络结构的全局参数包个数（#号后面的内容为注释）");
	tt.writeLineToFile();
	for (Uint i = 0; i < gWbSi; i++)
	{
		auto& pl = mod.gWb.at(i);
		tt% pl.bNotUpdate << pl.bi.size();
		tt << CLComma << Byte(pl.bi[0]);
		for (Uint j = 1, jsi = pl.bi.size(); j < jsi; j++)
		{
			tt << CLComma << pl.bi[j];
		}
		tt << CLComma << pl.wiSize;
		tt << CLComma << pl.wi.size();
		tt << CLComma << Byte(pl.wi[0]);
		for (Uint j = 1, jsi = pl.wi.size(); j < jsi; j++)
		{
			tt << CLComma << pl.wi[j];
		}
		if (f1++ == 0) {
			tt << ("    #全局参数定义[bi参数个数，阈值初始化类型，参数1，...，参数n，权值个数，wi参数个数，权值初始化类型，参数1，...，参数n]");
		}
		tt.writeLineToFile();
	}
	Uint alllays = mod.size();
	tt% alllays << ("    #网络结构的隐层总数");
	tt.writeLineToFile();
	f1 = 0;
	for (Uint i = 0; i < alllays; i++)
	{
		auto pl = &mod.at(i);
		//写入每层组块总数
		Uint allblocks = pl->size();
		if (f1++ == 0) {
			tt% allblocks << ("    #本层内有") << allblocks << ("个神经元小组（注：每组内的神经元，对上层的链接方式，传递函数均相同）");
		}
		else
		{
			tt% allblocks;
		}
		tt.writeLineToFile();
		for (Uint j = 0; j < allblocks; j++)
		{
			auto pb = &pl->at(j);
			Uint lkSi = pb->linkPos.size();
			//写入单个块组元
			tt % (pb->nnCounts) << CLComma << (pb->transFuncType) << CLComma << (pb->wcFuncType) << CLComma << (lkSi);
			//写链接
			for (Uint k = 0; k < lkSi; k++)
			{
				tt << CLComma << (pb->linkPos[k]);
			}
			tt << CLComma << (pb->globleWbId);
			if (pb->globleWbId < 0) { //使用局部描述
				tt << CLComma << (pb->bi.size() == 0 ? -1 : 1)
					<< CLComma << (pb->bi.size() > 0 ? (pb->bi.at(0)) : 0.0);
				//写权值
				Uint wiSi = pb->linkWij.size();
				tt << CLComma << (wiSi);
				for (Uint k = 0; k < wiSi; k++)
				{
					tt << CLComma << (pb->linkWij[k]);
				}
				if (f2 == 0 && lkSi > 0) {
					tt << ("    #本组内[ 有") << pb->nnCounts << ("个神经元，激活函数类别，权值组合方式，更新标记，对上层的链接个数N_Link，链接编号向量< l1, l2 , ... , lN > ，共享权重编号，阈值类型，阈值，权值初值个数(当为0时赋随机初值)，权值初值向量< w1, w2 , ... , wN > ]");
					f2++;
				}
				if (f3 == 0 && lkSi == 0) {
					tt << ("    #本组内[ 有") << pb->nnCounts << ("个神经元，激活函数类别，权值组合方式，更新标记，对上层的链接个数N_Link = 0 (无链接编号向量，表示会链接上层所有神经元，权值初值随机）,共享权重编号，阈值类型，阈值，权值初值个数(当为0时赋随机初值)，权值初值向量< w1, w2 , ... , wN > ]");
					f3++;
				}
			}
			else {
				if (f4 == 0 && lkSi > 0) {
					tt << ("    #本组内[ 有") << pb->nnCounts << ("个神经元，激活函数类别，权值组合方式，更新标记，对上层的链接个数N_Link，链接编号向量< l1, l2 , ... , lN > ，共享权重编号 ]");
					f4++;
				}
				if (f5 == 0 && lkSi == 0) {
					tt << ("    #本组内[ 有") << pb->nnCounts << ("个神经元，激活函数类别，权值组合方式，更新标记，对上层的链接个数N_Link = 0，共享权重编号 ]");
					f5++;
				}
			}
			tt.writeLineToFile();
		}
	}
	tt.closeFile();
	return mod;
}

void WbDef::reset()
{
	bi.clear(), wi.clear(), wiSize = 0;
}

void CLBpExtend::clearAllDataContainer(Uint createCounts)
{
	kernel.clearContainer(createCounts);

	vm_xi.clear();
	vm_yi.clear();
	vm_yi_predict.clear();
	vm_grad.clear();
	vm_neuronExData.clear();
	bIsCheckLinkBk = false;
	vm_bi_dt_old_Data.clear();
	vm_wji_dt_Data.clear();
	vm_wji_dt_old_Data.clear();
	vm_wji_bk.clear();
}

void CLBpExtend::releaseTrainDataContainer()
{
	buildTrainDataContainer(0, 0);
	bIsCheckLinkBk = false;
	releaseStdVector(vm_yi_predict);
	releaseStdVector(vm_bi_dt_old_Data);
	releaseStdVector(vm_wji_dt_Data);
	releaseStdVector(vm_wji_dt_old_Data);
	releaseStdVector(vm_wji_bk);
	releaseStdUnorderedMap(vm_neuronExData);
}

void CLBpExtend::buildTrainDataContainer(Int xySpan, Int gradSpan)
{
	if (xySpan > 0) {
		vm_xy_span = xySpan;
		//无损数据迁移
		auto nnsi = kernel.neuronCounts();
		Uint bk_xy_span = vm_xi.size() / nnsi;
		if (bk_xy_span == 0) {
			vm_xi.clear(), vm_xi.resize(vm_xy_span * nnsi, 0.0);
			vm_yi.clear(), vm_yi.resize(vm_xy_span * nnsi, 0.0);
		}
		else if (vm_xy_span != bk_xy_span) {
			auto minSi = min(vm_xy_span, bk_xy_span) * sizeof(Float);
			auto bkxi = vm_xi;
			vm_xi.clear(), vm_xi.resize(vm_xy_span * nnsi, 0.0);
			for (Uint i = 0, si = nnsi; i < si; i++)
				memcpy_s(vm_xi.data() + i * vm_xy_span, minSi, bkxi.data() + i * bk_xy_span, minSi);
			auto bkyi = vm_yi;
			vm_yi.clear(), vm_yi.resize(vm_xy_span * nnsi, 0.0);
			for (Uint i = 0, si = nnsi; i < si; i++)
				memcpy_s(vm_yi.data() + i * vm_xy_span, minSi, bkyi.data() + i * bk_xy_span, minSi);
		}
	}
	else if (xySpan == 0) {
		vm_xy_span = 0;
		releaseStdVector(vm_xi);
		releaseStdVector(vm_yi);
		for (size_t i = 0,si = vm_bnData.size(); i < si; i++)
		{
			vm_bnData[i].releaseEx();
		}
	}
	if (gradSpan > 0) {
		vm_grad_span = gradSpan;
		auto nnsi = kernel.neuronCounts();
		Uint bk_grad_span = vm_grad.size() / nnsi;
		if (bk_grad_span == 0) {
			vm_grad.clear(), vm_grad.resize(vm_grad_span * nnsi, 0.0);
			vm_neuronExData.clear();
			releaseStdUnorderedMap(vm_neuronExData);
			for (Uint i = 0, si = kernel.neuronCounts(); i < si; i++) {
				auto& wct = Nn(i).wcFuncType;
				if (wct == WC_Max || wct == WC_Min) {
					for (Uint l = 0; l < vm_grad_span; l++)
						vm_neuronExData[i * vm_grad_span + l] = { 0,0 };
				}
			}
		}
		else if (vm_grad_span != bk_grad_span) {
			auto minSi = min(vm_grad_span, bk_grad_span) * sizeof(Float);
			auto minSie = min(vm_grad_span, bk_grad_span) * sizeof(Uint);
			auto bkg = vm_grad;
			auto bke = vm_neuronExData;
			vm_grad.clear(), vm_grad.resize(vm_grad_span * nnsi, 0.0);
			vm_neuronExData.clear();
			releaseStdUnorderedMap(vm_neuronExData);
			for (Uint i = 0, si = kernel.neuronCounts(); i < si; i++) {
				auto& wct = Nn(i).wcFuncType;
				if (wct == WC_Max || wct == WC_Min) {
					for (Uint l = 0; l < vm_grad_span; l++)
						vm_neuronExData[i * vm_grad_span + l] = { 0,0 };
				}
			}
			for (Uint i = 0, si = nnsi; i < si; i++) {
				memcpy_s(vm_grad.data() + i * vm_grad_span, minSi, bkg.data() + i * bk_grad_span, minSi);
			}
			for (Uint i = 0, si = vm_neuronExData.size() / bk_grad_span; i < si; i++) {
				for (Uint j = 0, jsi = min(vm_grad_span, bk_grad_span); j < jsi; j++)
				{
					vm_neuronExData[i * jsi + j] = bke[i * bk_grad_span + j];
				}
			}
		}
	}
	else if (gradSpan == 0) {
		vm_grad_span = 0;
		releaseStdVector(vm_grad);
		releaseStdUnorderedMap(vm_neuronExData);
	}
}

void CLBpKernel::fitId(neuron& nn, Uint globleIndex, Uint layIndex, Uint cindex)
{
	/*nn.id_index = globleIndex;
	nn.id_lay = layIndex;
	nn.id_pos = cindex;*/
}

void CLBpExtend::_gradient_o(_gradient_param_def)
{
	//if (index == 9579)
	//	Sleep(0);
	//auto& nn = Nn(index);
	auto grad = pGradi(index);
	auto yi = pYii(index);
	for (Uint l = is; l < ie; l++) {
		Uint ci = (*vm_samUsage)[l];
		grad[l] = lossDerv(yi[l], vm_samSets->tv(ci)[pos]);
#if UseCheckNanInf > 0
		CLCheckNanInfMsg(grad[l](), _gradient() : last lay grad[l] is a error number!);
#endif
	}
	_gradient_h(_gradient_param);
}

void CLBpExtend::_gradient_h(_gradient_param_def)
{
	auto link = plink(nn);
	auto wji = pWji(nn);
	auto grad = pGradi(index);
	auto yi = pYii(index);
	auto xi = pXii(index);

	//本层乘以
	for (Uint l = is; l < ie; ++l) {
		grad[l] = grad[l] * activate_function_Derv(nn.transFuncType, yi[l], xi[l]);//不采用原子操作
#if UseCheckNanInf > 0
		CLCheckNanInfMsg(grad[l](), _gradient() : hide lay grad[l] is a error number!);
#endif
	}

	switch (nn.wcFuncType)
	{
	case WC_Add: {
		for (Uint i = 0, linkSi = link_size(nn); i < linkSi; ++i)//检索所有上层对本节点产生的链接
		{
			auto lk = link[i];
			if (lk < 0)continue;
			auto pUpGrad = pGradi(lk);
			for (Uint l = is; l < ie; ++l)
			{
				pUpGrad[l] += grad[l] * wji[i];
			}
		}
	}break;
	case WC_Average: {
		for (Uint i = 0, linkSi = link_size(nn); i < linkSi; ++i)//检索所有上层对本节点产生的链接
		{
			auto lk = link[i];
			if (lk < 0)continue;
			auto pUpGrad = pGradi(lk);
			for (Uint l = is; l < ie; ++l)
			{
				pUpGrad[l] += (grad[l] / linkSi * wji[i]);
			}
		}
	}break;
	case WC_Max: {
		for (Uint l = is; l < ie; ++l)
		{
			auto& lk = vm_neuronExData[index * vm_grad_span + l];
			auto pUpGrad = pGradi(lk.index);
			pUpGrad[l] += grad[l] * wji[lk.wjiIndex];
		}
	}break;
	case WC_Min: {
		for (Uint l = is; l < ie; ++l)
		{
			auto& lk = vm_neuronExData[index * vm_grad_span + l];
			auto pUpGrad = pGradi(lk.index);
			pUpGrad[l] += grad[l] * wji[lk.wjiIndex];
		}
	}break;
	default:
		CLString().format("\n错误：梯度传递中神经元(id=%d)的权值组合类型当前为(%d)不是预设的类型(0-3)!", index, nn.wcFuncType).printf()
			.messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR | MB_OK).throw_runtime_error();
	}
}

void CLBpExtend::_gradient_h_dp(_gradient_param_def)
{
	if (nn.bitFlag & FG_NN_Dropout) {
		return;
	}
	else _gradient_h(_gradient_param);
}

void CLBpExtend::_gradient_i(_gradient_param_def)
{
	auto link = plink(nn);
	auto wji = pWji(nn);
	auto grad = pGradi(index);
	auto yi = pYii(index);
	auto xi = pXii(index);

	//本层乘以
	for (Uint l = is; l < ie; l++) {
		grad[l] = grad[l] * activate_function_Derv(nn.transFuncType, yi[l], xi[l]);//不采用原子操作
#if UseCheckNanInf > 0
		CLCheckNanInfMsg(grad[l](), _gradient() : input lay grad[l] is a error number!);
#endif
	}
}

void CLBpExtend::_gradient_i_dp(_gradient_param_def)
{
	if (nn.bitFlag & FG_NN_Dropout) {
		return;
	}
	else _gradient_i(_gradient_param);
}

void CLBpExtend::_gradient_o_bn(_gradient_param_def)
{
	auto grad = pGradi(index);
	auto yi = pYii(index);
	for (Uint l = is; l < ie; l++) {
		Uint ci = (*vm_samUsage)[l];
		grad[l] = lossDerv(yi[l], vm_samSets->tv(ci)[pos]);
#if UseCheckNanInf > 0
		CLCheckNanInfMsg(grad[l](), _gradient() : last lay grad[l] is a error number!);
#endif
	}
	_gradient_h_bn(_gradient_param);
}

void CLBpExtend::_gradient_h_bn(_gradient_param_def)
{
	//auto link = plink(nn);
	//auto wji = pWji(nn);
	auto grad = pGradi(index);
	auto yi = pYii(index);
	auto xi = pXii(index);

	//本层乘以
	for (Uint l = is; l < ie; l++) {
		grad[l] = grad[l] * activate_function_Derv(nn.transFuncType, yi[l], xi[l]);//不采用原子操作
#if UseCheckNanInf > 0
		CLCheckNanInfMsg(grad[l](), _gradient() : hide lay grad[l] is a error number!);
#endif
	}
	auto& wb = Wb(nn);
	if (wb.bFlag & FG_WB_BatchNorm) {
		auto pex = vm_bnData[wb.index].ex; 
		for (Uint l = is; l < ie; l++) {
			pex->pushGrad(grad[l], index, l);
		}
	}
}

void CLBpExtend::_gradient_sendOut_bn(_gradient_param_def)
{
	auto link = plink(nn);
	auto wji = pWji(nn);
	auto grad = pGradi(index);
	//auto yi = pYii(index);
	//auto xi = pXii(index);

	auto& wb = Wb(nn);
	if (wb.bFlag & FG_WB_BatchNorm) {
		//auto yi = pYii(index);
		//auto xi = pXii(index);
		auto pex = vm_bnData[wb.index].ex;
		for (Uint l = is; l < ie; l++) {
			grad[l] = pex->gradSendOut(index, l);
#if UseCheckNanInf > 0
			CLCheckNanInfMsg(grad[l](), _gradient_sendOut_bn() : hide lay grad[l] is a error number!);
#endif
		}
	}
	if (lay == 0)//第一层不再传递
		return;
	switch (nn.wcFuncType)
	{
	case WC_Add: {
		for (Uint i = 0, linkSi = link_size(nn); i < linkSi; i++)//检索所有上层对本节点产生的链接
		{
			auto lk = link[i];
			if (lk < 0)continue;
			auto pUpGrad = pGradi(lk);
			for (Uint l = is; l < ie; l++)
			{
				pUpGrad[l] += grad[l] * wji[i];
			}
		}
	}break;
	case WC_Average: {
		for (Uint i = 0, linkSi = link_size(nn); i < linkSi; i++)//检索所有上层对本节点产生的链接
		{
			auto lk = link[i];
			if (lk < 0)continue;
			auto pUpGrad = pGradi(lk);
			for (Uint l = is; l < ie; l++)
			{
				pUpGrad[l] += (grad[l] / linkSi * wji[i]);
			}
		}
	}break;
	case WC_Max: {
		for (Uint l = is; l < ie; l++)
		{
			auto& lk = vm_neuronExData[index * vm_grad_span + l];
			auto pUpGrad = pGradi(lk.index);
			pUpGrad[l] += grad[l] * wji[lk.wjiIndex];
		}
	}break;
	case WC_Min: {
		for (Uint l = is; l < ie; l++)
		{
			auto& lk = vm_neuronExData[index * vm_grad_span + l];
			auto pUpGrad = pGradi(lk.index);
			pUpGrad[l] += grad[l] * wji[lk.wjiIndex];
		}
	}break;
	default:
		CLString().format("\n错误：梯度传递中神经元(id=%d)的权值组合类型当前为(%d)不是预设的类型(0-3)!", index, nn.wcFuncType).printf()
			.messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR | MB_OK).throw_runtime_error();
	}
}

void CLBpExtend::gradient(const Uint is, const Uint ie)
{
	for (Int lay = kernel.layerCounts() - 1; lay >= 0; lay--) //反向修正
	{
		gradient_lay(lay, 0, kernel.LayNnCounts(lay), is, ie);
	}
}

void CLBpExtend::gradient_lay(const Uint lay, const Uint ns, const Uint ne, const Uint is, const Uint ie)
{
#if UsePFunc > 0
	auto pFunc = kernel.isOutlayer(lay) ?
		&CLBpExtend::_gradient_o :
		vm_drop[lay] > 0.0 ?
		(lay == 0 ? &CLBpExtend::_gradient_i_dp : &CLBpExtend::_gradient_h_dp) :
		(lay == 0 ? &CLBpExtend::_gradient_i : &CLBpExtend::_gradient_h)
		;	
	for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
		(this->*pFunc)(index, lay, pos, Nn(index), is, ie);
#else
	if (kernel.isOutlayer(lay))
		for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
			_gradient_o(index, lay, pos, Nn(index), is, ie);
	else if (vm_drop[lay] > 0.0) {
		if (lay == 0)
			for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
				_gradient_i_dp(index, lay, pos, Nn(index), is, ie);
		else
			for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
				_gradient_h_dp(index, lay, pos, Nn(index), is, ie);
	}
	else {
		if (lay == 0)
			for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
				_gradient_i(index, lay, pos, Nn(index), is, ie);
		else
			for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
				_gradient_h(index, lay, pos, Nn(index), is, ie);
	}
#endif
}

void CLBpExtend::gradient_lay_bn_pushGrad(const Uint lay, const Uint ns, const Uint ne, const Uint is, const Uint ie)
{
#if UsePFunc > 0
	auto pFunc = kernel.isOutlayer(lay) ? &CLBpExtend::_gradient_o_bn : &CLBpExtend::_gradient_h_bn;
	for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
		(this->*pFunc)(index, lay, pos, Nn(index), is, ie);
#else
	if(kernel.isOutlayer(lay))
		for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
			_gradient_o_bn(index, lay, pos, Nn(index), is, ie);
	else
		for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
			_gradient_h_bn(index, lay, pos, Nn(index), is, ie);
#endif
}

void CLBpExtend::gradient_lay_bn_no(const Uint lay, const Uint ns, const Uint ne, const Uint is, const Uint ie)
{
#if UsePFunc > 0
	auto pFunc = kernel.isOutlayer(lay) ?
		&CLBpExtend::_gradient_o :
		(lay == 0 ? &CLBpExtend::_gradient_i : &CLBpExtend::_gradient_h)
		;
	for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
		(this->*pFunc)(index, lay, pos, Nn(index), is, ie);
#else
	if (kernel.isOutlayer(lay))
		for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
			_gradient_o(index, lay, pos, Nn(index), is, ie);
	else if(lay == 0)
		for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
			_gradient_i(index, lay, pos, Nn(index), is, ie);
	else
		for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
			_gradient_h(index, lay, pos, Nn(index), is, ie);
#endif
}

void CLBpExtend::gradient_lay_bn_sendGrad(const Uint lay, const Uint ns, const Uint ne, const Uint is, const Uint ie)
{
	for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
	{
		_gradient_sendOut_bn(index, lay, pos, Nn(index), is, ie);
	}
}

void CLBpExtend::zeroGradData()
{
	auto pgrad = vm_grad.data();
	for (size_t i = 0, si = vm_grad.size(); i < si; i++)
		*pgrad = 0.0, pgrad++;
}

void CLBpExtend::modify_wi_and_bi(Uint nSiSams, Uint is, Uint ie)
{
	auto wb = kernel.vm_wbpack.data();
	for (auto i = is; i < ie; i++)
	{
		_modify_wi_and_bi(wb[i], nSiSams);
	}
}

void CLBpExtend::_modify_wi_and_bi(wbpack& wb, Uint nSiSams)
{
	if (wb.bFlag & FG_WB_NotUpdate)
		return;
	auto pWji_dt_old0 = wb_pWji_dt_old(wb);
	auto pWji_dt0 = wb_pWji_dt(wb);
	auto pWji0 = wb_pWji(wb);
	auto wjiSi = wb_Wji_size(wb);
	memset(pWji_dt0, 0, sizeof(Float) * wjiSi);
	Float bi_dt = 0;
	auto& bi_dt_old = wb_bi_dt_old(wb);
	auto clay = kernel.nnLayerIndex(wb.nnIds);
	if (clay == 0) {
		auto samU = &(*vm_samUsage)[0];
		auto& samS = (*vm_samSets);
		for (auto nci = wb.nnIds, ie = wb.nnIde; nci < ie; ++nci)
		{
			auto& nn = kernel.vm_neuron[nci];

			if (nn.bitFlag & FG_NN_Dropout)
				continue;
			auto link = plink(nn);
			auto grad = pGradi(nci);
			for (Uint _i = 0; _i < wjiSi; ++_i)
			{
				auto i = link[_i];
				if (i < 0)
					continue;
				for (Uint l = 0; l < nSiSams; ++l) {
					pWji_dt0[_i] += (grad[l] * (samS.iv(samU[l])[i]));
#if UseCheckNanInf > 0
					CLCheckNanInfMsg(pWji_dt0[_i], modify wji_dt is a error number!);
#endif
				}
			}
			for (Uint l = 0; l < nSiSams; ++l) {
				bi_dt += grad[l];
#if UseCheckNanInf > 0
				CLCheckNanInfMsg(bi_dt, modify bi is a error number!);
#endif
			}
		}
	}
	else {
		auto pnn = &kernel.vm_neuron[0];
		for (auto nci = wb.nnIds, ie = wb.nnIde; nci < ie; ++nci)
		{
			auto& nn = kernel.vm_neuron[nci];
			/*if (nn.bitFlag & FG_WB_NotUpdate)
				continue;*/
				//if (isDrop)
			if (nn.bitFlag & FG_NN_Dropout)
				continue;
			auto link = plink(nn);
			auto grad = pGradi(nci);
			for (Uint _i = 0; _i < wjiSi; ++_i)
			{
				auto i = link[_i];
				if (i < 0)
					continue;
				auto preyi = pYii(i);
				for (Uint l = 0; l < nSiSams; ++l) {
					pWji_dt0[_i] += (grad[l] * preyi[l]);
#if UseCheckNanInf > 0
					CLCheckNanInfMsg(pWji_dt0[_i], modify wji_dt is a error number!);
#endif
				}
			}
			for (Uint l = 0; l < nSiSams; ++l) {
				bi_dt += grad[l];
#if UseCheckNanInf > 0
				CLCheckNanInfMsg(bi_dt, modify bi_dt is a error number!);
#endif
			}
		}
	}
	if (m_adamAlpha < VtEpslon) {
		//更新权值
		for (Uint i = 0; i < wjiSi; i++)
		{
			Float deltaWi = (1.0 - g_mc) * g_ls * pWji_dt0[i] + g_mc * pWji_dt_old0[i];
			pWji_dt_old0[i] = deltaWi;
			pWji0[i] -= deltaWi;
#if UseCheckNanInf > 0
			CLCheckNanInfMsg(pWji0[i], modify wji is a error number!);
#endif
		}
		//更新阈值
		Float deltaBi = (1.0 - g_mc) * g_ls * bi_dt + g_mc * bi_dt_old;
		bi_dt_old = deltaBi;
		wb_bi(wb) -= deltaBi;
#if UseCheckNanInf > 0
		CLCheckNanInfMsg(wb_bi(wb), modify bi is a error number!);
#endif
	}
	else {
		//更新权值
#define _adamDelta1 0.9
#define _adamDelta2 0.999
#define _adamDelta1_1 0.1
#define _adamDelta2_1 0.001
		for (Uint i = 0; i < wjiSi; i++)
		{
			Float& m = pWji_dt_old0[i] = _adamDelta1 * pWji_dt_old0[i] + _adamDelta1_1 * pWji_dt0[i];
			Float& v = pWji_dt_old0[i][1] = _adamDelta2 * pWji_dt_old0[i][1] + _adamDelta2_1 * pWji_dt0[i] * pWji_dt0[i];
			Float lr = m / _adamDelta1_1 / (sqrt(v / _adamDelta2_1) + VtEpslon);
			Float deltaWi = lr * m_adamAlpha;
			pWji0[i] -= deltaWi;
#if UseCheckNanInf > 0
			CLCheckNanInfMsg(pWji0[i], modify wji is a error number!);
#endif
		}
		//更新阈值
		Float& m = bi_dt_old = _adamDelta1 * bi_dt_old + _adamDelta1_1 * bi_dt;
		Float& v = bi_dt_old[1] = _adamDelta2 * bi_dt_old[1] + _adamDelta2_1 * bi_dt * bi_dt;
		Float lr = m / _adamDelta1_1 / (sqrt(v / _adamDelta2_1) + VtEpslon);
		Float deltaBi = lr * m_adamAlpha;
		wb_bi(wb) -= deltaBi;
#if UseCheckNanInf > 0
		CLCheckNanInfMsg(wb_bi(wb), modify bi is a error number!);
#endif		
	}
}

Float CLBpKernel::_wcFunc(_WcFuncParam_def)
{
	auto pWij0 = pWji(nn);
	auto plink0 = plink(nn);
	Uint silink = link_size(nn);
	Float _xi;
	switch (nn.wcFuncType)
	{
	case WC_Add: {
		_xi = 0;
		for (Uint j = 0; j < silink; j++)
		{
			auto il = plink0[j];
			if (il < 0)
				continue;
			_xi += pWij0[j] * preLayYiData0[il * span + l];
		}
	}break;
	case WC_Average: {
		_xi = 0;
		for (Uint j = 0; j < silink; j++)
		{
			auto il = plink0[j];
			if (il < 0)
				continue;
			_xi += pWij0[j] * preLayYiData0[il * span + l];
		}
		_xi /= silink;
	}break;
	case WC_Max: {
		_xi = -VtFloatMax;
		for (Uint j = 0; j < silink; j++)
		{
			auto il = plink0[j];
			if (il < 0)
				continue;
			Float v = pWij0[j] * preLayYiData0[il * span + l];
			if (_xi < v) {
				_xi = v;
				//需要考量层数据
				if (exData) {
					(*exData)[index * exdataSpan + exdatal].set((Uint)il, j);
				}
			}
		}
	}break;
	case WC_Min: {
		_xi = VtFloatMax;
		for (Uint j = 0; j < silink; j++)
		{
			auto il = plink0[j];
			if (il < 0)
				continue;
			Float v = pWij0 [j] * preLayYiData0[il * span + l];
			if (_xi < v) {
				_xi = v;
				if (exData) {
					(*exData)[index * exdataSpan + exdatal].set((Uint)il,j);
				}
			}
		}
	}break;
	default:
		_xi = 0;
		CLString().format("\n错误：神经元(id=%d)的权值组合类型当前为(%d)不是预设的类型(0-3)!", index, nn.wcFuncType).printf()
			.messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR | MB_OK).throw_runtime_error();
		break;
	}
	_xi += wb_bi(Wb(nn));
#if UseCheckNanInf > 0
	CLCheckNanInfMsg(_xi, _wcFunc() is a error number!);
#endif
	return _xi;
}

void CLBpKernel::setWcFunc(const Uint index, const Byte _wcFuncType)
{
	Nn(index).wcFuncType = _wcFuncType;
}

void CLBpKernel::setTransFunc(const Uint index, const Byte  _transFuncType)
{
	Nn(index).transFuncType = _transFuncType;
}

BpnnLayInfo& CLBpKernel::updateLayWbRange() {
	if (vm_layInfo.size() == 0)
		CLString("\nupdateLayWbRange: BpnnLayInfo Struct is not built!\n").printf().messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	if (vm_neuron.size() == 0)
		CLString("\nupdateLayWbRange: Bpnn neurons struct is not built!\n").printf().messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
	for (Uint lay = 0, laysi = vm_layInfo.size(); lay < laysi; lay++) {
		auto& thisLayWbSi = vm_layInfo[lay].iLayWbCounts;
		auto& thislayWbIndex = vm_layInfo[lay].iLayWbStartIndex;
		thisLayWbSi = 0, thislayWbIndex = UINT_MAX;
		auto cindex = thislayWbIndex;
		for (Uint ni = 0, ns = LayStartNnIndex(lay), nsi = LayNnCounts(lay);
			ni < nsi;
			ni++) {
			auto wbi = vm_neuron[ns + ni].wb;
			if (wbi < thislayWbIndex)
				thislayWbIndex = wbi;
			if(cindex != wbi)
				cindex = wbi, thisLayWbSi++;
		}
	}
	Uint layers = vm_layInfo.size();
	vm_globleInfo.iMaxLayNCounts = 0;
	vm_globleInfo.iMinLayNCounts = UINT_MAX;
	for (Uint i = 0; i < layers; i++)
	{
		auto ci = LayNnCounts(i);
		if (ci > vm_globleInfo.iMaxLayNCounts)
			vm_globleInfo.iMaxLayNCounts = ci, vm_globleInfo.iMaxLayIndex = i;
		if (ci < vm_globleInfo.iMinLayNCounts)
			vm_globleInfo.iMinLayNCounts = ci, vm_globleInfo.iMinLayIndex = i;
	}
	return vm_layInfo;
}

void CLBpKernel::_Yi(_Yi_param_def)
{	
	Float _xi;
	auto& xi = pxi0 ? pxi0[index * span + l] : _xi;
	if (inVec) {
		xi = _wcFunc(index, nn, inVec, 1, 0, exData, exdataSpan, exdatal);
	}
	else {
		xi = _wcFunc(index, nn, pyi0, span, l, exData, exdataSpan, exdatal);
	}
	pyi0[index * span + l] = activate_function(nn.transFuncType, xi);
#if UseCheckNanInf > 0
	CLCheckNanInfMsg(pyi0[index * span + l], _Yi() : yi is a error number!);
#endif
}

void CLBpKernel::_Xi_bn(_Yi_param_def)
{	
	auto& wb = Wb(nn);
	if (wb.bFlag & FG_WB_BatchNorm) {
		Float _xi;
		auto& xi = pxi0 ? pxi0[index * span + l] : _xi;
		if (inVec) {
			xi = _wcFunc(index, nn, inVec, 1, 0, exData, exdataSpan, exdatal);
		}
		else {
			xi = _wcFunc(index, nn, pyi0, span, l, exData, exdataSpan, exdatal);
		}

		if (pxi0) {
			vm_bnData[wb.index].putXi(xi, true, index, l);
		}
		else {
			pyi0[index * span + l] = activate_function(nn.transFuncType,
				(vm_bnData[wb.index].putXi(xi, false, index, l))
			);
#if UseCheckNanInf > 0
			CLCheckNanInfMsg(pyi0[index * span + l], _Xi_bn() : yi is a error number!);
#endif
		}
	}
	else
		_Yi(_Yi_param);
}

void CLBpKernel::_Yi_bn(_Yi_param_def)
{	
	auto& wb = Wb(nn);
	if ((wb.bFlag & FG_WB_BatchNorm) && pxi0) {
		auto pos = index * span + l;
		if (pxi0) {//训练模式
			pxi0[pos] = vm_bnData[wb.index].getYi_h(index, l);
#if UseCheckNanInf > 0
			CLCheckNanInfMsg(pxi0[pos], _Yi_bn() : yi is a error number!);
#endif
			pyi0[pos] = activate_function(nn.transFuncType,	pxi0[pos]);
		}
		else {
			pyi0[pos] = activate_function(nn.transFuncType,
				(vm_bnData[wb.index].getYi_h(index, l))
			);
		}
#if UseCheckNanInf > 0
		CLCheckNanInfMsg(pyi0[pos], _Yi_bn() : yi is a error number!);
#endif
	}
}

Float CLBpKernel::Er(const Float* target, Uint targetSi) const
{
	if (!target)
		return VtFloatMax;
	if (targetSi < outputDimension())
		throw runtime_error("Target is not match output dimension!");
	Float rt = 0;
	auto pyi0 = lastLayYi0Start();
	for (Uint i = 0, si = outputDimension(); i < si; i++)
	{
		rt += loss(pyi0[i], target[i]);
	}
	return rt;
}

Float CLBpKernel::_Er(const Float* pyiData, const Float* target, Uint targetSi) const
{
	if (!target)
		return VtFloatMax;
	if (targetSi < outputDimension())
		throw runtime_error("Target is not match output dimension!");
	Float rt = 0;
	auto pyi0 = &pyiData[lastLayStartNnIndex()];
	for (Uint i = 0, si = outputDimension(); i < si; i++)
	{
		rt += loss(pyi0[i], target[i]);
	}
	return rt;
}

#define makeTransFuncVecUnit(Name) tf_##Name

static const vector<PTransFunc> g_TransFunc = {
	makeTransFuncVecUnit(PRelu),        //传递函数为： y = max(0.7x,x);		
	makeTransFuncVecUnit(Purelin),      //传递函数为： 纯线性函数 y = x;
	makeTransFuncVecUnit(Sigmoid),      //传递函数为： S函数;
	makeTransFuncVecUnit(Tanh),         //传递函数为： tanh函数	;	
	makeTransFuncVecUnit(Relu),         //传递函数为： y = max(0,x);
	makeTransFuncVecUnit(LeakyRelu),    //传递函数为： y = max(0.01x,x);
	makeTransFuncVecUnit(Step),		    //传递函数为： y = abs(x) >= 1 ? 1 : 0;		
	makeTransFuncVecUnit(ELU),
	makeTransFuncVecUnit(SELU),
	makeTransFuncVecUnit(SRelu),
	makeTransFuncVecUnit(HardSigmoid),
	makeTransFuncVecUnit(HardTanh),
	makeTransFuncVecUnit(LeCunTanh),
	makeTransFuncVecUnit(ArcTan),
	makeTransFuncVecUnit(SoftSign),
	makeTransFuncVecUnit(SoftPlus),
	makeTransFuncVecUnit(Signum),
	makeTransFuncVecUnit(BentPurelin),
	makeTransFuncVecUnit(SymmetricalSigmoid),
	makeTransFuncVecUnit(LogLog),
	makeTransFuncVecUnit(Gaussian),
	makeTransFuncVecUnit(Absolute),
	makeTransFuncVecUnit(Sinusoid),
	makeTransFuncVecUnit(Cos),
	makeTransFuncVecUnit(Sinc),
	makeTransFuncVecUnit(Exp), //取e指数
	makeTransFuncVecUnit(Ln), //取ln对数
};

inline PTransFunc getTFunc(Uint tfType) {
	return g_TransFunc[tfType];
}

Float CLBpKernel::activate_function(const Byte trType, const Float x)
{
	return g_TransFunc[trType](x, false);
}

Float CLBpExtend::activate_function_Derv(const Byte trType, const Float y, const Float x)
{
	//有先选用Y模式
	switch (trType) {
	case TF_Sigmoid:return y * (1.0f - y);
	case TF_Tanh:return 1.0f - y * y;
	/*case TF_Purelin:return 1.0f;
	case TF_PRelu:return y >= 0 ? 1.0f : 0.7f;
	case TF_LeakyRelu:return y >= 0 ? 1.0f : 0.01f;
	case TF_Relu:return y >= 0 ? 1.0f : 0.0f;*/
		//case TF_SymmetricalSigmoid:return 0.5 * (1 - y * y);
		//case TF_LeCunTanh:return 1.7519 * 2.0 / 3.0 + 2 / 3 / 1.7519 * y * y;
		//case TF_ELU:return x < 0 ? y + 0.7 : 1;
		//case TF_SELU:return x < 0 ? 1.0507 * (y + 0.7) : 1.0507;
	default:
		return _activate_function_Derv(trType, x);
	}
	throw std::runtime_error("activate_function_Derv tatch bad end error!");
}

Float CLBpExtend::_activate_function_Derv(const Byte trType, const Float x)
{
	return g_TransFunc[trType](x, true);
}

void CLBpExtend::forward_lay_bn_UpdateParam(const Uint is, const Uint ie)
{	
	for (Uint i = is; i < ie; i++)
	{
		vm_bnData[i].forwardUpdate();
	}
}

void CLBpExtend::gradient_lay_bn_createParam(const Uint is, const Uint ie)
{
	for (Uint i = is; i < ie; i++)
	{
		vm_bnData[i].createBackwardParam();
	}
}

void CLBpExtend::gradient_lay_bn_UpdateParam(const Uint is, const Uint ie)
{
	for (Uint i = is; i < ie; i++)
	{
		vm_bnData[i].backwardUpdate();
	}
}

void CLBpExtend::forward(const Uint is, const Uint ie)
{
	for (Uint lay = 0, si = kernel.layerCounts(); lay < si; lay++)//正向执行
	{
		forward_lay(lay, 0, kernel.LayNnCounts(lay), is, ie);
	}
}

void CLBpExtend::forward_lay(const Uint lay, const Uint ns, const Uint ne, const Uint is, const Uint ie)
{
#if UsePFunc > 0
	auto pFunc = 
			(vm_drop[lay] > 0.0 ?
			(lay == 0 ? &CLBpExtend::_forward_i_dp : &CLBpExtend::_forward_h_dp) :
			(lay == 0 ? &CLBpExtend::_forward_i : &CLBpExtend::_forward_h)
			);	
	for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
		(this->*pFunc)(index, lay, pos, Nn(index), is, ie);
#else
	if(vm_drop[lay] > 0.0)
		if(lay == 0)
			for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
				_forward_i_dp(index, lay, pos, Nn(index), is, ie);
		else
			for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
				_forward_h_dp(index, lay, pos, Nn(index), is, ie);
	else 
		if (lay == 0)
			for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
				_forward_i(index, lay, pos, Nn(index), is, ie);
		else
			for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
				_forward_h(index, lay, pos, Nn(index), is, ie);
#endif
}

void CLBpExtend::forward_lay_bn_no(const Uint lay, const Uint ns, const Uint ne, const Uint is, const Uint ie)
{
#if UsePFunc > 0
	auto pFunc = lay == 0 ? &CLBpExtend::_forward_i : &CLBpExtend::_forward_h;
	for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
	{
		(this->*pFunc)(index, lay, pos, Nn(index), is, ie);
	}
#else
	if (lay == 0)
		for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
			_forward_i(index, lay, pos, Nn(index), is, ie);
	else
		for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
			_forward_h(index, lay, pos, Nn(index), is, ie);
#endif
}

void CLBpExtend::forward_lay_bn_xi(const Uint lay, const Uint ns, const Uint ne, const Uint is, const Uint ie)
{
#if UsePFunc > 0
	auto pFunc = lay == 0 ? &CLBpExtend::_forward_i_bn : &CLBpExtend::_forward_h_bn;
	for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
	{
		(this->*pFunc)(index, lay, pos, Nn(index), is, ie);
	}
#else
	if (lay == 0)
		for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
			_forward_i_bn(index, lay, pos, Nn(index), is, ie);
	else
		for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
			_forward_h_bn(index, lay, pos, Nn(index), is, ie);
#endif
}

void CLBpExtend::forward_lay_bn_yi(const Uint lay, const Uint ns, const Uint ne, const Uint is, const Uint ie)
{
	for (Uint pos = ns, index = kernel.LayStartNnIndex(lay) + ns; pos < ne; pos++, index++)
	{
		for (Uint l = is; l < ie; l++)
		{
			kernel._Yi_bn(index, Nn(index), pxi, pyi, nullptr, xy_span, l);
		}
	}
}

void CLBpExtend::_forward_i(_forward_param_def)
{
	for (Uint l = is; l < ie; l++)
	{
		kernel._Yi(index, nn, pxi, pyi, vm_samSets->iv((*vm_samUsage)[l]), xy_span, l,
			pExdata, exdata_span, l);
	}
}

void CLBpExtend::_forward_i_dp(_forward_param_def)
{
	if (nn.bitFlag & FG_NN_Dropout) {
		if (dpRepeatTrainTimesC == 1) {
			//auto xi = bpnn_pXii(index);
			auto yi = bpnn_pYii(index);
			for (Uint l = is; l < ie; l++)
			{
				//if (xi)xi[l] = 0;
				yi[l] = 0;
			}
		}
	}
	else _forward_i(_forward_param);
}

void CLBpExtend::_forward_h(_forward_param_def)
{
	for (Uint l = is; l < ie; l++)
	{
		kernel._Yi(index, nn, pxi, pyi, nullptr, xy_span, l,
			pExdata, exdata_span, l);
	}
}

void CLBpExtend::_forward_i_bn(_forward_param_def)
{
	for (Uint l = is; l < ie; l++)
	{
		kernel._Xi_bn(index, nn, pxi, pyi, 
			vm_samSets->iv((*vm_samUsage)[l]),
			xy_span, l,	pExdata, exdata_span, l);
	}
}

void CLBpExtend::_forward_h_bn(_forward_param_def)
{
	for (Uint l = is; l < ie; l++)
	{
		kernel._Xi_bn(index, nn, pxi, pyi, 
			nullptr, 
			xy_span, l,	pExdata, exdata_span, l);
	}
}

void CLBpExtend::_forward_h_dp(_forward_param_def)
{
	if (nn.bitFlag & FG_NN_Dropout) {
		if (dpRepeatTrainTimesC == 1) {
			//auto xi = bpnn_pXii(index);/drop掉就不再处理xi
			auto yi = bpnn_pYii(index);
			for (Uint l = is; l < ie; l++)
			{
				//if (xi)xi[l] = 0;
				yi[l] = 0;
			}
		}
	}
	else _forward_h(_forward_param);
}

Float CLBpExtend::getMaxMinWij(neuron& nn, Float& vmin, Float& vmax)
{
	Float wq = 0;
	auto pwji = pWji(nn);
	Uint sk = link_size(nn);
	auto link = plink(nn);
	for (Uint k = 0; k < sk; k++)
	{
		if (link[k] < 0)
			continue;
		auto& w = pwji[k];
		if (w > vmax)vmax = w;
		if (w < vmin)vmin = w;
		wq += (w * w);
	}
	wq = sk ? wq / sk : 0;
	wq = sqrt(wq);
	return wq;
}

void CLBpKernel::createWbByWbDef(wbpack& wb, Uint index, const WbDef& def)
{
	wb.index = index;
	wb.bFlag = def.bNotUpdate ? FG_WB_NotUpdate : 0;
	auto pwji = make_wb_Wji_bi(wb, def.wiSize);
	createWbByWbDef(pwji, &wb_bi(wb), def);
}

void CLBpKernel::createWbByWbDef(Float* pwji, Float* pbi, const WbDef& def)
{
	RandInitParam par = def.bi;
	if (par.size() > 0)par.erase(par.begin());
	rand_initData(pbi, 1, (EBP_IT)(Int)(def.bi[0]), par);

	par = def.wi;
	if (par.size() > 0)par.erase(par.begin());
	rand_initData(pwji, def.wiSize, (EBP_IT)(Int)(def.wi[0]), par);
}

DWORD WorkSvc::run(PCLTaskSvcTrdParam var) {
	if (!bpnn)
		throw std::runtime_error("bpnn class point is null!");
	if (!ws_flag)
		throw std::runtime_error("works flag is null!");
	Uint i = Uint(var->info.nIndex) - 1;

	normType:
	while (true)
	{
		switch (ws_flag[i]) {
		case PERS_STANDBY: {
			SwitchToThread();
		}break;
		case PERS_FORD: {
			if (method == 0) {
				GET_TRD_SECTION_TASK(var, ws_samSi, is, ie);
				bpnn->forward(is, ie);
			}
			else {
				GET_TRD_SECTION_TASK(var, pclaySize, js, je);
				bpnn->forward_lay(clayIndex, js, je, 0, ws_samSi);
			}
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_FORD_PREDICT: {
			if (method == 0) {
				GET_TRD_SECTION_TASK(var, ws_samSi, is, ie);
				for (Uint lay = 0, si = bpnn->kernel.layerCounts(); lay < si; lay++)//正向执行
				{
					auto pFunc = lay == 0 ? &CLBpExtend::_forward_i : &CLBpExtend::_forward_h;
					for (Uint pos = 0, index = bpnn->kernel.LayStartNnIndex(lay), posSi = bpnn->kernel.LayNnCounts(lay); pos < posSi; pos++, index++)
						(bpnn->*pFunc)(index, lay, pos, bpnn_Nn(index), is, ie);
				}
			}
			else {
				GET_TRD_SECTION_TASK(var, pclaySize, js, je);
				auto pFunc = clayIndex == 0 ? &CLBpExtend::_forward_i : &CLBpExtend::_forward_h;
				for (Uint pos = js, index = bpnn->kernel.LayStartNnIndex(clayIndex) + js; pos < je; pos++, index++)
					(bpnn->*pFunc)(index, clayIndex, pos, bpnn_Nn(index), 0, ws_samSi);
			}
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_GRAD: {
			if (method == 0) {
				GET_TRD_SECTION_TASK(var, ws_samSi, is, ie);
				bpnn->gradient(is, ie);
			}
			else {
				GET_TRD_SECTION_TASK(var, pclaySize, js, je);
				bpnn->gradient_lay(clayIndex, js, je, 0, ws_samSi);
			}
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_MODIFY: {
			GET_TRD_SECTION_TASK(var, ws_wbSi, is, ie);
			bpnn->modify_wi_and_bi(ws_samSi, is, ie);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_QUIT: {
			goto ret;
		}break;
		case PERS_TURN_NORM: {
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_TURN_BN: {
			goto bnType;
		}break;
		default: {
			if (ws_flag[i] > PERS_TURN_BN)
				CLString("\n错误：Bpnn工作线程组未切换到“Bn”模式！（当前为“正常”模式）\n").printf().messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
			ws_flag[i] = PERS_STANDBY;
		}break;
		}
	}

	bnType:
	while (true) {
		switch (ws_flag[i]) {
		case PERS_STANDBY: {
			SwitchToThread();
		}break;
		case PERS_BN_FORD_XI: {
			GET_TRD_SECTION_TASK(var, pclaySize, js, je);
			//1）：前向输入数据
			bpnn->forward_lay_bn_xi(clayIndex, js, je, 0, ws_samSi);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_BN_FORD_UPDATE: {
			auto& layInfo = bpnn->kernel.vm_layInfo[clayIndex];
			GET_TRD_SECTION_TASK(var, layInfo.iLayWbCounts, js, je);
			//2）：更新权值的u和a2
			bpnn->forward_lay_bn_UpdateParam(layInfo.iLayWbStartIndex + js, layInfo.iLayWbStartIndex + je);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_BN_FORD_YI: {
			GET_TRD_SECTION_TASK(var, pclaySize, js, je);
			//3）：提取真实变换的Yi
			bpnn->forward_lay_bn_yi(clayIndex, js, je, 0, ws_samSi);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_BN_FORD_NO: {
			GET_TRD_SECTION_TASK(var, pclaySize, js, je);
			bpnn->forward_lay_bn_no(clayIndex, js, je, 0, ws_samSi);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_BN_GRAD_PUSH: {
			GET_TRD_SECTION_TASK(var, pclaySize, js, je);
			//1）：压入梯度
			bpnn->gradient_lay_bn_pushGrad(clayIndex, js, je, 0, ws_samSi);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_BN_GRAD_CREATE: {
			auto& layInfo = bpnn->kernel.vm_layInfo[clayIndex];
			GET_TRD_SECTION_TASK(var, layInfo.iLayWbCounts, js, je);
			//2）：构造梯度传递参数
			bpnn->gradient_lay_bn_createParam(layInfo.iLayWbStartIndex + js, layInfo.iLayWbStartIndex + je);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_BN_GRAD_SEND: {
			GET_TRD_SECTION_TASK(var, pclaySize, js, je);
			//3）：传递梯度到上层
			bpnn->gradient_lay_bn_sendGrad(clayIndex, js, je, 0, ws_samSi);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_BN_GRAD_UPDATE: {
			auto& layInfo = bpnn->kernel.vm_layInfo[clayIndex];
			GET_TRD_SECTION_TASK(var, layInfo.iLayWbCounts, js, je);
			//4）：更新bn数据参数
			bpnn->gradient_lay_bn_UpdateParam(layInfo.iLayWbStartIndex + js, layInfo.iLayWbStartIndex + je);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_BN_GRAD_NO: {
			GET_TRD_SECTION_TASK(var, pclaySize, js, je);
			bpnn->gradient_lay_bn_no(clayIndex, js, je, 0, ws_samSi);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_MODIFY: {
			GET_TRD_SECTION_TASK(var, ws_wbSi, is, ie);
			bpnn->modify_wi_and_bi(ws_samSi, is, ie);
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_BN_FORD_PREDICT: {
			if (method == 0) {
				GET_TRD_SECTION_TASK(var, ws_samSi, is, ie);
				for (Uint lay = 0, si = bpnn->kernel.layerCounts(); lay < si; lay++)//正向执行
				{
					auto pFunc = lay == 0 ? &CLBpExtend::_forward_i_bn : &CLBpExtend::_forward_h_bn;
					for (Uint pos = 0, index = bpnn->kernel.LayStartNnIndex(lay), posSi = bpnn->kernel.LayNnCounts(lay);
						pos < posSi; pos++, index++)
						(bpnn->*pFunc)(index, lay, pos, bpnn_Nn(index), is, ie);
				}
			}
			else {
				GET_TRD_SECTION_TASK(var, pclaySize, js, je);
				auto pFunc = clayIndex == 0 ? &CLBpExtend::_forward_i_bn : &CLBpExtend::_forward_h_bn;
				for (Uint pos = js, index = bpnn->kernel.LayStartNnIndex(clayIndex) + js;pos < je; pos++, index++)
					(bpnn->*pFunc)(index, clayIndex, pos, bpnn_Nn(index), 0, ws_samSi);
			}
			ws_flag[i] = PERS_STANDBY;
		}break;
		case PERS_QUIT: {
			goto ret;
		}break;
		case PERS_TURN_NORM: {
			goto normType;
		}break;
		case PERS_TURN_BN: {
			ws_flag[i] = PERS_STANDBY;
		}break;
		default: {
			if (ws_flag[i] < PERS_TURN_NORM)
				CLString("\n错误：Bpnn工作线程组未切换到“正常”模式！（当前为“Bn”模式）\n").printf().messageBoxRef(_lpBpnnMsgBoxTitle, MB_ICONERROR).throw_runtime_error();
			ws_flag[i] = PERS_STANDBY;
		}break;
		}
	}
	ret:
	return 1;
}

inline Uint WorkSvc::getWorkType(Uint samSi) const {
	if (samSi >= 2 && samSi + 1 >= m_core) {
		return 0;//样本量大于线程数 用样本型多线程
	}
	else {
		auto& inf = bpnn->kernel.vm_globleInfo;
		if ((inf.iMaxLayNCounts + inf.iMinLayNCounts) / 2 > 3 * m_core)
			return 1;//否则应该用节点型多线程
		else
			return 0;
	}
}

Uint BpnnLayInfo::getLayerMaxNnCountsInNet() const
{
	Uint max = 0;
	for (const auto& i : *this)
	{
		if (i.iLayNnCounts > max)
			max = i.iLayNnCounts;
	}
	return max;
}

Uint BpnnLayInfo::getLayerMinNnCountsInNet() const
{
	Uint max = UINT_MAX;
	for (const auto& i : *this)
	{
		if (i.iLayNnCounts < max)
			max = i.iLayNnCounts;
	}
	return max == UINT_MAX ? 0 : max;
}

//前向
inline void GRU::forward(in Xt, in S_t_1, in Wr, in Ur, in Br, in Wz, in Uz, in Bz, in Whh, in Uhh, in Bhh, out s_t) {
	auto r_t = (Xt * Wr + S_t_1 * Ur + Br).foreach([](CLMATRIXEX_CALLBACK_PARAM) { v = tf_Sigmoid(v, false); }).move();
	auto z_t = (Xt * Wz + S_t_1 * Uz + Bz).foreach([](CLMATRIXEX_CALLBACK_PARAM) { v = tf_Sigmoid(v, false); }).move();
	auto hh_t = (Xt * Whh + (r_t.mul(S_t_1)) * Uhh + Bhh).foreach([](CLMATRIXEX_CALLBACK_PARAM) { v = tf_Tanh(v, false); }).move();
	s_t = ((1 - z_t).mul(S_t_1) + z_t.mul(hh_t)).move();
}

//前向输出
inline void GRU::forward_o(in S_t, in Wo, in Bo, out o) {
	o = (S_t * Wo + Bo).foreach([](CLMATRIXEX_CALLBACK_PARAM) { v = tf_Sigmoid(v, false); }).move();
}



