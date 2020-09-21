//DESIGNED BY CAILUO @2020-02
//MINI-SUPPORT @ C++11
//VERSION:1.0.5

#pragma once

#ifndef __CL_CREATIVELUS_H__
#define __CL_CREATIVELUS_H__

#ifdef BPNNTOOL_EXPORTS		               // dll生成模式下，导出类
#define _dll_ __declspec(dllexport)
#else
#ifdef _LIB                                // VC下标记为lib链接模式下，不做导出标识
#define _dll_
#else
#ifdef _USELIB                             // 由头文件标记为lib链接模式下，不做导出标识
#define _dll_
#else
#define _dll_ __declspec(dllimport)        // 引用dll链接模式下，导入类
#endif
#endif
#endif

#include <vector>
#include <map>
#include <string>

namespace CreativeLus { 

	using namespace std;

	//define Float type-----------------------------------------------------------------------------------------------	
	typedef float Float;
	typedef vector<Float> VLF;
	typedef unsigned char Byte;
	typedef vector<Byte> VLB;
	typedef int Int;
	typedef vector<Int> VLI;
	typedef unsigned int Uint;
	typedef vector<Uint> VLUI;
	typedef bool Bool;
	typedef const char* PCStr;
	typedef void* PVoid;
	
#define ConstE     (2.7182818284590f)  //定义自然常数
#define ConstPi    (3.1415926535898f)  //定义圆周率
#define VtFloatMax (3.402823466e+38f)  //最大浮点值
#define VtEpslon   (1e-8f)             //最小被除数
	   
#define BPNN_DFT_LEARNSTEP         0.1      //默认学习步长
#define BPNN_DFT_MOMENTUM          0.8      //默认动量因子

#define BPNN_DFT_MAXRUNTIMES       100      //默认最大执行次数 
#define BPNN_DFT_ER                0.0001   //默认误差最小精度 
#define BPNN_DFT_ADAM_STEP         0.001    //默认Adam优化的步长，该参数为(1e-5, 0.3)

#define BPNN_DFT_LAYERNUM          1        //默认隐含层层数 
#define BPNN_DFT_PERLAYERROOTNUM   3        //默认隐含层节点数 

	//网络类型
	enum EBP_UT {
		UT_Classify, // 分类，每次拟合采用的样本个数为1个样本
		UT_Approach, // 逼近，每次拟合采用的样本个数为全部训练样本数		
	};

	//样本变换类型
	enum EBP_STT {
		STT_Null,       //数据不做归一化或标准化
		STT_Normalize,  //数据做归一化到(0,1)
		STT_NormalizeEx,//数据做归一化到(-1,1)
		STT_Standart,   //数据做标准化到(0,1)标准分布
	};

	//定义激活函数类型及函数原型
	enum EBP_TF {		
		TF_PRelu,        //传递函数为： y = max(0.7x,x)；		
		TF_Purelin,      //传递函数为： 纯线性函数 y = x；
		TF_Sigmoid,      //传递函数为： S函数
		TF_Tanh,         //传递函数为： tanh函数		
		TF_Relu,         //传递函数为： y = max(0,x)；
		TF_LeakyRelu,    //传递函数为： y = max(0.01x,x)；
		TF_Step,		 //传递函数为： y = abs(x) >= 1 ? 1 : 0；		
		TF_ELU,
		TF_SELU,
		TF_SRelu,
		TF_HardSigmoid,
		TF_HardTanh,
		TF_LeCunTanh,
		TF_ArcTan,
		TF_SoftSign,
		TF_SoftPlus,
		TF_Signum,
		TF_BentPurelin,
		TF_SymmetricalSigmoid,
		TF_LogLog,
		TF_Gaussian,
		TF_Absolute,
		TF_Sinusoid,
		TF_Cos,
		TF_Sinc,
		TF_Exp,          //取e指数
		TF_Ln,           //取ln对数
	};

	//定义权值组合类型及组合函数原型
	enum EBP_WC {
		WC_Add,     //线性相加
		WC_Average, //线性相加后按相加参与个数取均值
		WC_Max,     //范围内按参与所有单个结果取最大
		WC_Min,     //范围内按参与所有单个结果取最小	
		WC_Convolution = WC_Add,//定义卷积方式，等效于WC_Add方式
	};

	//定义损失函数类型及原型 
	enum EBP_LS {
		LS_MeanSquareLoss,  //均方差
		LS_CrossEntropyLoss,//交叉熵
	};

	//正确率的评价方式类型
	enum EBP_CRT {
		CRT_MeanSquareLoss,  //结果向量的均方差损失值小于设定的误差精度
		CRT_MaxValuePosMatch,//结果向量最大值所处位置与目标向量最大值所在的位置相同（一般用于通过最大值进行分类的正确率评价情况）
		CRT_MinValuePosMatch,//结果向量最小值所处位置与目标向量最小值所在的位置相同（一般用于通过最小值进行分类的正确率评价情况）
	};

	//参数初始化类型枚举值，用于权值阈值初始化采用的方法
	enum EBP_IT
	{
		IT_Const,   //常量
		IT_Uniform, //均值分布
		IT_Gaussian,//高斯分布（即正太分布）
		IT_Normal = IT_Gaussian,//正太分布（即高斯分布）
		IT_Xavier,//参数由0均值，标准差为sqrt(2 / (fan_in + fan_out))的正态分布,其中fan_in和fan_out是分别权值张量的输入和输出元素数目；在tanh激活函数上有很好的效果，但不适用于Relu激活函数。
		IT_Msra,//均值为0，方差为（2/输入的个数n）的高斯分布；它特别适合 Relu激活函数。
	};

	//模型参数标准化优化方案，用于某一层或整个网络;
	//若将输入的图像shape记为[N, C, H, W]，则以下：
	//HW表示一个对象空间维度（可以是一个数，可以是一个X列，也可以是一个XY平面像素点数据列），视作一个Instance对象；
	//C表示一个通道维度；
	//N表示一种对象的实际个数维度（即数量维度）；
	enum EBP_NT {
		NT_BatchNorm,// 是在batch上，对NHW做归一化，对小batchsize效果不好；
		NT_LayerNorm,// 在通道方向上，对CHW归一化，主要对RNN作用明显；
		NT_InstanceNorm,// 在图像像素上，对HW做归一化，用在风格化迁移；
		NT_GroupNorm,// 将channel分组，然后再做归一化；
		NT_SwitchableNorm,// 是将BN、LN、IN结合，赋予权重，让网络自己去学习归一化层应该使用什么方法。
	};

	//训练过程是一个延时过程，枚举值标记了训练过程当前设置状态
	enum EBP_TPC {
		TPC_Enable, //可用状态
		TPC_Disable,//训练过程设为不可用，立即返回
		TPC_Stop = TPC_Disable, //训练过程停止并返回
		TPC_Pause,  //暂停训练过程，在控制权线程调用可能锁死，因此在控制权线程设置该标记无效
		TPC_Resume = TPC_Enable,//从当前暂停状态恢复运行
	};

	//define BpnnSampSets -----------------------------------------------------------------------------------------------

	//共有接口
	struct _dll_ IBpnnBase {
		virtual ~IBpnnBase();
	};
	typedef IBpnnBase *PIBpnnBase;

	//输入输出样本对包装类（该类并不复制和产生新的数据，仅仅对已有的BpnnSampSets保存的数据样本对做包装输出）
	class _dll_ BpnnSampPair :IBpnnBase {
	protected:
		Float* piv, * pov;
		Uint ivDim, tvDim;
	public:
		explicit BpnnSampPair();
		explicit BpnnSampPair(Float* iv, Uint ivDim, Float* ov, Uint tvDim);
		virtual ~BpnnSampPair();
		//获取输入数据向量维度
		Uint intputDimension() const;
		//获取目标数据向量维度
		Uint targetDimension() const;
		//样本对包装的数据全部置0
		void clear();
		//由编号i取得iv或者ov中的值对象，i>iv.size()后就开始取ov中的对象了,请注意该方法的使用规则；
		Float& operator[](Uint i);
		//获取包装的输入数据的副本
		VLF inputVec() const;
		//获取包装的目标数据的副本
		VLF targetVec() const;
		//获取包装的输入向量数据指针
		Float* iv() const;
		//获取包装的目标向量数据指针
		Float* tv() const;
	};
	typedef vector<BpnnSampPair> BpnnSampPairList, * PBpnnSampPairList;//样本对集合
	//变换模式描述类
	class _dll_ BpnnTransModelUnit :IBpnnBase {
	public:
		Uint dimIndex;//维度编号
		Uint dimType;//维度的变换类型
		Float vmax;//最小值
		Float vmin;//最小值
		Float vAver;//均值
		Float vStandardDeviation;//标准差
		BpnnTransModelUnit();
		void reset();
		virtual ~BpnnTransModelUnit();
		//将一个值通过映射变换为新值；
		Float forward(Float org) const;
		//将一个变换后的值通过映射逆变换为原值；
		Float backward(Float tag) const;
	};
	//用于保存变换记录
	class _dll_ BpnnSampTransModelRecord:IBpnnBase,public std::map<Uint, BpnnTransModelUnit>{
	public:
		//将一个值通过对应的映射变换为新值；
		Float forward(Float org, Uint index) const;
		//将一个变换后的值通过对应的映射逆变换为原值；
		Float backward(Float tag, Uint index) const;
		//将一个向量通过对应的映射变换为新向量，alignStartIndex表示从转换记录的第几个开始对齐转换；
		void forward(const VLF& org, VLF& tag, Uint alignStartIndex) const;
		//将一个变换后的向量通过对应的映射逆变换为原向量，alignStartIndex表示从转换记录的第几个开始对齐转换；
		void backward(const VLF& tag, VLF& org, Uint alignStartIndex) const;
	};
	typedef vector<Uint> BpnnSampTransTypeVec;//样本（对）变换类型标记序列


	//样本集合类
	class _dll_ BpnnSampSets :IBpnnBase {
	protected:
		VLF ivdata;
		VLF tvdata;
		Uint ivDim, tvDim;
		BpnnSampTransModelRecord transModeRec;//数据变换记录
	public:
		//改变数据的维度，并清理内部数据；
		BpnnSampSets& changeDimension(Uint ivDim, Uint tvDim);
		//获取数据集样本对个数；
		Uint size() const;
		//重置和初始化数据集结构、大小、维度、默认值等；只要输入和目标向量维度不变，重置大小不会改变已有数据的值；
		BpnnSampSets& resize(Uint newSize, Float defaultInValue = 0.0, Float defaultTagValue = 0.0, Uint newInDim = 0, Uint newTagDim = 0);
		BpnnSampSets();
		explicit BpnnSampSets(Uint ivDim, Uint tvDim);
		virtual ~BpnnSampSets();
		//获取输入数据向量维度；
		Uint intputDimension() const;
		//获取输出向量数据维度；
		Uint targetDimension() const;
		//获取输入输出向量数据总维度数；
		Uint dimension() const;
		//清除数据，但保留维度信息
		BpnnSampSets& clear();
		//清除数据，且清理维度信息，释放内存；
		BpnnSampSets& reset();
		//保存样本到文件，binMode = true表示2进制文件形式保存（该模式用于大量数据的导出）
		//当采用二进制方式时，文件的扩展名将被修改为固定的".bpnnSampSets"并输出文件，以确保写入固定的二进制数据流文件；
		Bool writeToFile(PCStr file, Bool binMode = true);
		//取得样本文件，保存一份原内部集合对象的副本到bkSet中，bkSet = nullptr 不复制；
		//binMode = true表示读取二进制文件（该模式用于大量数据情况）
		//当采用二进制方式时，指定的文件的扩展名将被自动替换为读取固定扩展名".bpnnSampSets"文件，以确保读取固定的二进制数据流文件；
		Bool readFromFile(PCStr lpFile, Bool binMode = true);
		//对内部样本数据集做归一化或标准化，变换规则由一个BpnnSampTransTypeVec向量标明，每一维度对应一种变换处理方式。
		BpnnSampSets& normalizationOrStandardization(const BpnnSampTransTypeVec& flagVecter);
		//数据变换结构体的记录个数（一般等于输入输出维度总和）
		Uint getTransModRecSize() const; 
		//取得内部变换记录，返回新的实例
		BpnnSampTransModelRecord getTransModRec() const;
		//取得样本文件的变换结构体；
		//binMode = true表示读取二进制文件（该模式用于大量数据情况）
		//当采用二进制方式时，指定的文件的扩展名将被自动替换为读取固定扩展名".bpnnSampSets"文件，以确保读取固定的二进制数据流文件；
		static BpnnSampTransModelRecord getTransModRec(PCStr lpFile, Bool binMode = true);

		//清除内部的变换记录
		BpnnSampSets& clearTransModRec();
		//设置内部变换记录
		BpnnSampSets& setTransModRec(const BpnnSampTransModelRecord& rec);
		//内部数据集增加一个样本对，标将指向最后一个数据
		BpnnSampSets& addSample(const VLF& inputArray, const VLF& targetArray);
		BpnnSampSets& addSample(const Float* inputArray,Uint inputArrayDim, const Float* targetArray,Uint targetArrayDim );
		BpnnSampSets& addSample(const BpnnSampPair& samPair);
		BpnnSampSets& setSample(Uint samIndex, const VLF& inputArray, const VLF& targetArray);
		BpnnSampSets& setSample(Uint samIndex, const Float* inputArray, Uint inputArrayDim, const Float* targetArray, Uint targetArrayDim);
		BpnnSampSets& setSample(Uint samIndex, const BpnnSampPair& samPair);
		//把目标向量的每个维度分量都扩充区分到不同的区间分类中
		void copyAndTargetDimToIntervalClassification(BpnnSampSets& newSampSets, Float classFlagMax, Float classFlagMin, const VLF& intervalTable) const;
		//返回样本对包装对象
		BpnnSampPair operator[](const Uint i);
		//由数字序号获取只读的样本对的输入向量数据指针
		const Float* iv(const Uint i) const;
		//由数字序号获取只读的样本对的目标向量数据指针
		const Float* tv(const Uint i) const;
		//获取样本对的输入向量数据指针头
		const Float* ivData() const;
		//获取样本对的目标向量数据指针头
		const Float* tvData() const;
		//获取样本对的输入向量数据向量
		const VLF& ivDataVec() const;
		//获取样本对的目标向量数据向量
		const VLF& tvDataVec() const;
		//取得整个输入输入向量集中的某一个维度的分量数据,索引重0开始，一次排列，先输入维度，后为输出维度
		void getDimVecData(VLF& reseult, Uint dimVecIndex);
		//取得输入向量集的某一个维度的分量数据,索引重0开始
		void getIntputDimVecData(VLF& reseult, Uint dimVecIndex);
		//取得输出向量集的某一个维度的分量数据,索引重0开始
		void getTargetDimVecData(VLF& reseult, Uint dimVecIndex);
	};

	//define BpnnStructDef -----------------------------------------------------------------------------------------------
	typedef vector<Float> WiInitDef;//权值初始化模式定义结构，如：{IT_Const,1},表示初始化为常量1，{IT_Msar},按Msar方式初始化权值
	typedef vector<Float> BiInitDef;//阈值初始化模式定义结构，如：{IT_Const,1},表示初始化为常量1，{IT_Uniform,-1，1},按均值随机方式初始化值，范围-1到1
	typedef vector<Int> LinkPos;    //隐藏层单元的内部单个组块元中的每个节点 对应的上层链接下标标记，从1开始，当结构为空时候，表示链接上层所有节点，当为-1忽略该节点链接；
	typedef vector<Float> LinkWij;  //隐藏层单元的内部单个组块元中的每个节点 对应的上层链接初始权值，当为空时候，通过随机赋权值初值；
	typedef vector<Float> LinkBi;

	enum EBP_SCP {
		SCP_Fc,    //全连接类型
		SCP_Trans, //传递类型
		SCP_Pool,  //池化类型
		SCP_Conv,  //卷积类型
		SCP_ConvSep,//拆分卷积类型
		SCP_Combin, //相加结合类型
		SCP_Softmax,//softmax分类器类型
	};

	/*
		InputFilterMap 写法：
		SCP_Fc : inputFilterMap = { 上层起始链接神经元编号index，输入个数 };
		SCP_Trans: inputFilterMap = {上层起始链接神经元编号index，输入个数};
		SCP_Pool: inputFilterMap = { 上层起始链接神经元编号index，输入map通道数deep ，输入map宽度wt，输入map高度ht};
		SCP_Conv: inputFilterMap = { 上层起始链接神经元编号index，输入map通道数deep ，输入map宽度wt，输入map高度ht};
		SCP_ConvSep: inputFilterMap = { 上层起始链接神经元编号index，输入map通道数deep ，输入map宽度wt，输入map高度ht};
		SCP_Combin: inputFilterMap = { 上层起始链接神经元编号index，输入个数 };
		SCP_Softmax: inputFilterMap = { 上层起始链接神经元编号index，输入个数 };
	*/
	typedef VLUI InputFilterMap;
	typedef VLUI FilterMapStruct;

	struct BpnnStructScriptSet
	{
		Uint upIndex = 0; //上一层要链接的组的编号
		EBP_SCP type = SCP_Fc; //本组的类型

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
		FilterMapStruct filterMap;
		WiInitDef wi; //权值bi初始化参数
		BiInitDef bi; //阈值bi初始化参数
		Int trFunc = -1;//激活函数类型，当值为小于0表示使用对应生成类型的默认值
		Int wcFunc = -1;//权值组合函数类型，当值为小于0表示使用对应生成类型的默认值
		Int flag = -1;//组的标记（保留），当值为小于0表示使用对应生成类型的默认值
	};
	typedef vector<BpnnStructScriptSet> BpnnStructScriptLayer;
	/*
	脚本定义结构，类似如下形式：
	BpnnStructScript scp = {
		{ //第一层开始
			{ 0, SCP_Fc, {10},{ IT_Unifon,-1,1 } }, //一层1组的BpnnStructScriptSet结构，定义一个全连接层描述组
			{ 0, SCP_Conv,{7,7,256,3,3,3},{IT_Msra},{ IT_Const,0 },TF_Relu,WC_Add,0 }, //一层2组，定义一个卷积层描述组
		},//第一层结束
		{ //第二层开始
			{ 0, SCP_Trans}, //二层1组，定义一个传递描述组，链接到一层1组
			{ 1, SCP_Pool,{2,2,2,2}}, //二层2组，定义一个池化层描述组，链接到一层1组
		}//第二层结束
	}
	*/
	class _dll_ BpnnStructScript :IBpnnBase, public std::vector<BpnnStructScriptLayer> {
	public:
		BpnnStructScript();
		BpnnStructScript(const initializer_list<BpnnStructScriptLayer>& list);
		virtual ~BpnnStructScript();
	};

	//隐藏层单元的内部单个组块元定义结构，该组块将定义一个节点属性相同的小组，
	//指定小组中节点数量，所有小组内的节点传递函数和链接下标均相同；
	//该类是自定义网络结构的最小描述单元
	class _dll_ BSB :IBpnnBase {
	public:
		Uint nnCounts;//元组内的同类神经元个数
		Byte transFuncType;//激活函数类型
		Byte wcFuncType;//权值组合类型
		LinkPos linkPos;//链接上一层节点的所表示的上层节点位置标记，从1开始，当size为0时候链接全部上层节点；
		LinkWij linkWij;//链接上一层节点的对应的上层链接初始权值，当为空时候，则通过随机赋权值初值；
		LinkBi bi;//链接上一层节点的对应的上层链接初始阈值,当为空时候取随机值；
		Int globleWbId;//采用wb数据索引，当值<0时，选用局部Wb结构
		BSB();
		BSB(Uint _nnCounts, EBP_TF _transFunc);
		BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos);
		BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos, EBP_WC _wcType);
		BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos, EBP_WC _wcType, const LinkWij& _linkForwordNodeWij);
		BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos, EBP_WC _wcType, const LinkWij& _linkForwordNodeWij, const LinkBi& _bi);
		virtual ~BSB();
		void reset();//重置元组描述
	};
	typedef vector<BSB> BSLayer;//网络结构单元层定义结构
	typedef vector<BSLayer> BpnnStruct;//网络结构定义结构
	typedef initializer_list<BSLayer> BSInitList;//用于构造的隐藏层单元定义结构表
	typedef vector<Uint> ConvolutionKernel;//卷积核定义描述类，定义如：{ 输入通道Indepth,宽width,高height,输出map个数Outdepth}
	typedef vector<Uint> PoolingMap;//池化映射方式定义，定义如：{ 输入通道depth,宽width,高height,X步幅X_Stride,Y步幅Y_Stride}

	//Wb表述定义
	class _dll_ WbDef:IBpnnBase {
	public:
		BiInitDef bi;//阈值初始化条件
		WiInitDef wi;//阈值初始化条件
		Uint wiSize;//权值个数
		Bool bNotUpdate;//该权值是否在训练过程中进行更新
		WbDef();
		WbDef(Uint _wiSize, const  WiInitDef& _wi, const BiInitDef& _bi, Bool bNotUpdate = false);
		virtual ~WbDef();
		void reset();
	};
	typedef vector<WbDef> WbLib;//Wb库定义

	/*
		BpnnStructDef结构，你需要自定义的神经网络结构；在buildNet()前定义；
		BpnnStructDef 结构定义范例，有3个隐藏层和1个输出层，如下：
			BpnnStructDef mod = {
				{},  //一个空的隐藏层定义，该无意义的定义在生成对象构造或赋值过程中被删除；
				{  //一个隐藏层定义
					BSB(1,TF_Sigmoid), //一个组块元定义，其中包含：节点数，传递函数类型，空的链接下标标识数组；
					BSB(3,TF_Tanh,{}), //一个组块元定义，其中包含：节点数，传递函数类型，空的链接下标标识数组；
					BSB(5,TF_Sigmoid)
				},
				{
					BSB(2,TF_Tanh,{0,2,4,6}),  //一个组块元定义，其中包含：节点数，传递函数类型，显示标明的链接下标标识数组；
					BSB(3,TF_Sigmoid,{}),
					BSB(4,TF_Tanh,{1,3,5,7})
				},
				{
					BSB(4,TF_Sigmoid,{}),
					BSB(2,TF_Tanh,{0,1,2,6,7,8}),
					BSB(2,TF_Sigmoid,{})
				},
				//以下的行定义，其中的重复链接下标，或超出的下标，都是允许的，这些错误数据都会在内部自动检查并被清理，无需手动检查；
				//只是注意，当因错误被自动清理后，链接列表内容可能为空了（即size() == 0），则代表本节点要链接上层的所有节点，这可能和设计的初衷是违背的，这一点需要注意；
				{BSB(2,TF_Tanh,{0,0,0,1,2,3,2,4,4,4,4}),BSB(2,TF_Sigmoid,{0,0,4,8,6,9,9})},
			};
		*/
	class  _dll_ BpnnStructDef :IBpnnBase,public BpnnStruct {
	public:
		//全局的权值阈值定义库
		WbLib gWb;
		//做一次前向链接检查
		void preCheck();
		//由参数构造WbDef,thisWbIsNotUpdate = false表示使用该权值定义的节点将不会改变该权值（即初始化后权值阈值保持不变，用于不需要跟新权值的节点）
		static WbDef& makeWbDefine(WbDef& def, Uint wiCounts, const WiInitDef& wi, const BiInitDef& bi, const Bool thisWbIsNotUpdate = false);
	public:
		BpnnStructDef();
		BpnnStructDef(const BSInitList& _Ilist);
		virtual ~BpnnStructDef();
		//清空所有定义内容
		void clear();
		BpnnStructDef& operator = (const BSInitList& _Ilist);
		//去掉空的无效的层定义，并修改定义中的错误
		BpnnStructDef& valid();
		//取得已定义的结构的所有神经元个数
		Uint neuronCounts() const;
		//取得已定义的结构的所有神经元组块个数（一个组块定义包含了若干个相同属性的神经元）
		Uint getNeuralBlockCounts() const;
		//取得定义的总层数
		Uint layerCounts() const;
		//取得第几层的神经元数量，layId从0开始；
		Uint getNeuronCountsInLayer(Uint layId);
		//取得第几层的神经元组块数量，layId从0开始；
		Uint getNeuralBlockCountsInLayer(Uint layId);
		//从文件读取模式
		Bool readFromFile(PCStr file);
		//输出模式到文件
		BpnnStructDef& writeToFile(PCStr file);
		//由参数取得新的尺寸长度 = (baseLen + 2 * padding - kernelLen) / stride + 1;
		static Uint getNewLength(Uint baseLen, Uint kernelLen, Uint padding, Uint stride);

		//向某隐层增加一个卷积操作输出；
		//该卷积可以是多通道输入卷积核，操作后得到一个多通道输出层；	
		BpnnStructDef& addOneConvolution(
			Uint appendhidelayerIndex, //增加到哪一个隐层的编号标识(从0开始)
			Uint mapBaseIndex,//本次卷积操作单元起点对应链接的上层节点起始号(从0开始)
			Uint& _in_out_baseLayer_Xsi, //基层X向维度,函数正确返回后保存新的尺寸值
			Uint& _in_out_baseLayer_Ysi, //基层Y向维度,函数正确返回后保存新的尺寸值
			const ConvolutionKernel& ckMetrix, //卷积核(卷积平面个数代表输入通道数（输入深度）数据），形如：{ 输入通道数,filter宽,filter高,输出map数,padding,,X步幅,Y步幅}
			const WiInitDef& wi, //权值bi初始化参数
			const BiInitDef& bi, //阈值bi初始化参数
			EBP_TF transFunc = EBP_TF::TF_Tanh, //激活函数类型（默认为TF_Tanh）
			EBP_WC wcFunc = EBP_WC::WC_Convolution //权值组合方式（默认为WC_Convolution）
		);
		//向某隐层增加一个非交叉卷积操作输出，即：非输入通道全叠加，卷积核仅对对应输入深度位置的map做一对一卷积；
		//这使得输入通道数=输出通道数，卷积核ckMetrix不存在输出map数这一参数,因为输入通道应该始终等于输出map个数，后层若链接1x1卷积可形成DepthWise操作；
		BpnnStructDef& addOneConvolutionSeparable(
			Uint appendhidelayerIndex, //增加到哪一个隐层的编号标识(从0开始)
			Uint mapBaseIndex,//本次卷积操作单元起点对应链接的上层节点起始号(从0开始)
			Uint& _in_out_baseLayer_Xsi, //基层X向维度,函数正确返回后保存新的尺寸值
			Uint& _in_out_baseLayer_Ysi, //基层Y向维度,函数正确返回后保存新的尺寸值
			const ConvolutionKernel& ckMetrix, //卷积核(卷积平面个数代表输入通道数（输入深度）数据），形如：{ 输入通道数,filter宽,filter高,padding,X步幅,Y步幅}
			const WiInitDef& wi, //权值bi初始化参数
			const BiInitDef& bi, //阈值bi初始化参数
			EBP_TF transFunc = EBP_TF::TF_Tanh, //激活函数类型（默认为TF_Tanh）
			EBP_WC wcFunc = EBP_WC::WC_Convolution //权值组合方式（默认为WC_Convolution）
		);
		//向某隐层增加一个池化操作输出(池化操作不调整本层权值和阈值，权值全1，阈值全0)；池化映射定义，形如：{ 输入通道数,filter宽,filter高,X步幅,Y步幅}
		BpnnStructDef& addOnePooling(
			Uint appendhidelayerIndex, //增加到哪一个隐层的编号标识(从0开始)
			Uint mapBaseIndex,//本次池化操作单元起点对应链接的上层节点起始号(从0开始)
			Uint& _in_out_baseLayer_Xsi, //基层X向维度,函数正确返回后保存新的尺寸值
			Uint& _in_out_baseLayer_Ysi, //基层Y向维度,函数正确返回后保存新的尺寸值
			const PoolingMap& poolMap,//池化映射定义，形如：{ 输入通道,filter宽,filter高,X步幅,Y步幅}
			EBP_WC wcFunc, //权值组合方式（可为WC_Average，WC_Max等）
			EBP_TF transFunc = EBP_TF::TF_Purelin //激活函数类型（默认为TF_Purelin）,注意池化层的传递函数参数非特殊情况不要做修改，应采用线性y=x传递函数
		);
		//向某一隐层增加一个全连接操作输出；激活函数类型
		BpnnStructDef& addOneFullConnect(
			Uint appendhidelayerIndex, //增加到哪一个隐层的编号标识(从0开始)
			Uint mapBaseIndex,//本次全连接操作单元起点对应链接的上层节点起始号(从0开始)
			Uint baseLayer_XYZsi, //基层X向维度,当为0链接上层所有单元,函数正确返回后保存新的压缩到一维的长度值
			Uint thisTimesNodes,//本次增加的全连接神经元数，当为0时数量等同于上层所有神经元综合，忽略mapBaseIndex，baseLayer_Xsi，baseLayer_Ysi及baseLayer_DepthSi参数
			const WiInitDef& wi,//权值bi初始化参数
			const BiInitDef& bi,//阈值bi初始化参数				
			EBP_TF transFunc, //激活函数类型（可以为TF_Sigmoid、TF_Tanh、TF_Purelin等）	
			EBP_WC wcFunc = EBP_WC::WC_Add, //权值组合方式（默认为WC_Add）
			Bool isNoUpdateFlag = false//指定该层的权值及阈值是否会跟新,isNoUpdateFlag=true表示该部分节点不跟新权值和阈值
		);
		//向某一隐层增加一个纯传递操作输出；（传递操作将上层节点输出等值传递到下一层，一对一链接，权值全1，阈值全0，不调整，会根据transFunc类型传递梯度）
		BpnnStructDef& addOneTransmit(
			Uint appendhidelayerIndex, //增加到哪一个隐层的编号标识(从0开始)
			Uint mapBaseIndex,//本次传递操作单元起点对应链接的上层节点起始号(从0开始)
			Uint baseLayer_Xsi, //基层X向维度
			Uint baseLayer_Ysi = 1, //基层Y向维度，当为1退化为一维线性
			Uint baseLayer_DepthSi = 1, //基层深度维度，当为1退化为二位平面
			EBP_TF transFunc = EBP_TF::TF_Purelin //激活函数类型（默认为TF_Purelin: y=f(x)=x),也可以根据网络组装需求调整
		);
		//向某一隐层增加一个对上层神经元做相加结合的操作；
		BpnnStructDef& addOneConbinate(
			Uint appendhidelayerIndex, //增加到哪一个隐层的编号标识(从0开始)
			Uint mapBaseIndex, //本次相加结合操作单元起点对应链接的上层节点起始号(从0开始)
			Uint baseLayer_SizeSpan, //本次相加结合操作单元跨度，也是本次操作的输出神经元数
			Uint baseLayer_SizeSpanTimes //本次相加结合操作次数，
		);
		//向末尾增加一个Softmax分类器
		BpnnStructDef& addSoftmaxClassifierToEnd(
			Uint mapBaseIndex,//本次传递操作单元起点对应链接的上层节点起始号(从0开始)
			Uint baseLayer_XYZsi, //基层X向维度
			Uint classifiers//分类个数
		);

		/*
			采用脚本创建一个复杂网络；
			script 脚本定义结构，描述了加入到本定义结构体当前层的脚本描述
			inputFilterMap 脚本的第一层的输入映射方式：
			inputFilterMap 写法：
			SCP_Fc : inputFilterMap = { 上层起始链接神经元编号index，输入个数 };
			SCP_Trans: inputFilterMap = {上层起始链接神经元编号index，输入个数};
			SCP_Pool: inputFilterMap = { 上层起始链接神经元编号index，输入map通道数deep ，输入map宽度wt，输入map高度ht};
			SCP_Conv: inputFilterMap = { 上层起始链接神经元编号index，输入map通道数deep ，输入map宽度wt，输入map高度ht};
			SCP_ConvSep: inputFilterMap = { 上层起始链接神经元编号index，输入map通道数deep ，输入map宽度wt，输入map高度ht};
			SCP_Combin: inputFilterMap = { 上层起始链接神经元编号index，输入个数 };
			SCP_Softmax: inputFilterMap = { 上层起始链接神经元编号index，输入个数 };
		*/
		BpnnStructDef& addScript(
			const BpnnStructScript& script,
			const InputFilterMap& inputFilterMap,
			Bool logoutConstructInfo = false  //脚本生成网络结构明细输出信息
		);
	};

	//层的dropout模式定义
	class _dll_ DRP:IBpnnBase {
	public:
		Uint startlayer;//启动dropout的起始层索引号
		Uint endLayer;//启动dropout的结束层索引号
		Float rate;//启动dropout的剪除率
		DRP(Uint layer = 0, Float rate = 0.5);
		DRP(Uint startlayer, Uint endLayer, Float rate);
		virtual ~DRP();
	};
	typedef vector<DRP> DropoutLayerDef;//执行dropout层的标记

	typedef vector<Uint> BnLayerIndexList;//执行BatchNomlization的层的索引序列

	typedef map<string, string> BpnnToolInfo;//工具信息列表
	
	//取得CreativeLus工具的相关信息列表；
	_dll_ BpnnToolInfo getBpnnToolInfo();

	//传入executeByJson的结构，用于执行函数内部保存动态分配的对象指针，若保存对象的指针不是nullptr
	//则结构析构时自动对象；因此，若要保持指针有效性，请备份指针值，并设置结构内部对应的项为nullptr；
	//结构内部字段意义：
	//trainSampSets：是设置的用于保存内部传出的样本集合对象的指针变量，若由执行函数设置该值且不为nullptr，则对象保存了类型BpnnSampSets；
	//testSampSets：是设置的用于保存内部传出的正确率测试集合对象的指针变量，若由执行函数设置该值且不为nullptr，则对象保存了类型BpnnSampSets；
	//bpnnStructDef:是设置的用于保存内部传出的自定义结构描述对象的指针变量，若由执行函数设置该值且不为nullptr，则对象保存了类型BpnnStructDef；
	//autoFitCallBack:是设置的用于保存内部传出的自动调整学习率回调对象的指针变量，若由执行函数设置该值且不为nullptr，则对象保存了一个不可用的未知类型；
	class _dll_ BpnnInterfaceStore :IBpnnBase, public std::map<string, PIBpnnBase> {
	public:
		BpnnInterfaceStore();
		void release();
		virtual ~BpnnInterfaceStore();
	};

	//bp神经网络类
	class _dll_ Bpnn :IBpnnBase {
	protected:
		void* kernel;
		void* extend;
	public:
		
		//断开训练扩展组件。即在非训练状态下显示调用以释放训练扩展组件所占用的所有内存（一般内存占用比预测模型内核大很多），仅保留一个较小的计算内核。
		//计算内核：是神经网络用于计算和预测的主要部分，该部分仅保留训练完成的预测模型必要数据（一般用于发布产品）；
		Bpnn& detachExtend();

		//启动网络的dropout功能；dropout无法剪除不参与更新参数（W或B）的神经元（例如纯传递功能或池化功能的神经元），也无法剪除输出层神经元；
		//DropoutLayerDef是定义了要dropout操作的层的标记结构，内部的定义单元为DRP结构;
		//例如：{ DRP(1,0.5), DRP(2,3,0.4), DRP(5,0.6) } ，表示1层剪除率0.5，2-3层剪除率0.4，5层剪除率0.6，其余各层不做dropout；
		//剪除率rate表示神经元的dropout的剪除概率，值在[0.05,0.95]之间，默认取值0.5，值在范围之外全部置0忽略该定义;
		//repeatTrainTimes重复多少个计算周期后更新一次dropout结构,当为0时表示不启动dropout；
		Bpnn& setDropout(Uint repeatTrainTimes = 0, const DropoutLayerDef& def = DropoutLayerDef());

		//设置网络Bn操作：miniBatchSize为0时候不启动Bn，bnLayerList指定了启动Bn的层的序号，为空表示全部层都启动Bn操作
		Bpnn& setBatchNormalization(Uint miniBatchSize = 0, const BnLayerIndexList& bnLayerList = BnLayerIndexList());

		//函数用于设置网络适用于做逼近或是分类，但并不是必须的；其内部仅仅调用了setSampleBatchCounts对样本拟合数量做设置
		//一般：逼近采用全样本拟合，分类采用单个样本拟合；
		Bpnn& setNetUseTo(EBP_UT type = EBP_UT::UT_Classify);

		//取得网络的神经元总数,加密网络则无效返回0
		Uint neuronCounts() const;
		//取得某一网络层的神经元总数,加密网络则无效返回0
		Uint neuronCountsInLayer(Uint lay) const;
		//取得网络的输入维度
		Uint inputDimension() const;
		//取得网络的输出w维度
		Uint outputDimension() const;
		//取得网络的层数,加密网络则无效返回0
		Uint layerCounts() const;
		//取得网络的隐藏层层数，即总层数-1,加密网络则无效返回0
		Uint hideLayerCounts() const;

		//设置神经网络训练模式下采用多线程技术支持（默认状态是关闭）核心线程数少于4的cpu将不支持，并用常规单线程方案替代；
		//参数percentageOfThreadsStarted为[0,1]间的数值，代表启动的多线程数量占总可用线程数的比例，当为1时全部启用，当为0.8时只启动80%的线程，当为0时关闭；
		//若当前多线程模块正在运行，设置percentageOfThreadsStarted = 0将立即关闭多线程工作组；函数也用于立即关闭多线程模块（阻塞式的安全退出）
		Bpnn& setMultiThreadSupport(Float percentageOfThreadsStarted = 0);
		//打开和关闭GPU硬件加速如果有GPU加速设备的话
		Bpnn& setGpuAcceleratedSupport(Bool bOpen = false);

		//打开图像结果显示标记，打开状态标记后，后续的train方法训练过程会把结果返回到图像数据中，不打开则不保存运行过程流数据；
		//当flag从bOpen赋值true时候，内部数据将被清空，反之false保留数据关闭显示标记，不再接受数据流；
		//后续可用showGraphParam()显示数据流；
		Bpnn& openGraphFlag(Bool bOpen = true);

		//打开图像结果显示（显示参数流）,该方法应该在openGraphFlag后运行train方法记录数据后使用，
		//若openGraphFlag时false同样会显示，不过显示的是之前的数据；
		//maxDataToShow指定显示的最大数据量，超过该量数据流会被截断（直接删除），当maxDataToShow = 0 表示不截断数据流，这将显示所有已记录的数据流；
		//posX,posY表示窗口出现的左上角坐标；
		Bpnn& showGraphParam(Uint maxDataToShow = 0, Int posX = 1, Int posY = 1);

		//打开图像结果显示（显示网络结构）,该方法应该在openGraphFlag后运行train方法记录数据后使用，
		//若openGraphFlag时false同样会显示，不过显示的是之前的数据；
		//isShowDetail指定是否显示当前的权值状态详细图形结构；
		//posX,posY表示窗口出现的左上角坐标；
		Bpnn& showGraphNetStruct(Bool isShowDetail = false, Int posX = 1, Int posY = 1);

		//若当前CorrectRate图形窗口是显示状态，则将窗口输出到一个指定的bmp图形文件中，窗口未显示、窗口最小化、文件无法创建等都将返回FALSE
		Bool exportGraphCorrectRate(PCStr lpfileName);
		//若当前Er图形窗口是显示状态，则将窗口输出到一个指定的bmp图形文件中，窗口未显示、窗口最小化、文件无法创建等都将返回FALSE
		Bool exportGraphEr(PCStr lpfileName);
		//若当前Ls图形窗口是显示状态，则将窗口输出到一个指定的bmp图形文件中，窗口未显示、窗口最小化、文件无法创建等都将返回FALSE
		Bool exportGraphLs(PCStr lpfileName);
		//若当前Mc图形窗口是显示状态，则将窗口输出到一个指定的bmp图形文件中，窗口未显示、窗口最小化、文件无法创建等都将返回FALSE
		Bool exportGraphMc(PCStr lpfileName);

		//取得内部样本集的数据对个数
		Uint getSampleCounts() const;
		//取得训练完成的步数
		Uint getRunTimes() const;
		//取得训练完成的累计步数
		Uint getRunTimesTutal() const;
		//取得当前训练误差值
		Float getEr() const;		
		//取得当前模型学习率
		Float getLs() const;
		Float getLsOld() const;
		//取得当前模型训练状态的惯性（动量）项影响系数
		Float getMc() const;
		Float getMcOld() const;
		//取得当前的误差相较上一步误差值的差值
		Float getDEr() const;
		//取得当前设定的误差精度
		Float getAccuracy() const;
		//取得设定的单词训练周期所执行的总步数
		Uint getMaxTimes() const;
		//在正确率评价模式下取得内部保存的以计算好的正确率。（若正确率未计算过，则值不可预测）
		Float getSavedCorrectRate() const;
		//模型是否加密；说明：若模型被加密则无法获取一些受保护的内部参数数据，
		//例如：无法获取模型结构、无法获取模型节点数、无法获取模型参数分布情况等；
		Bool isEncrypted() const;

		//给网络设置一个名称字符串
		Bpnn& setName(PCStr lpName);
		//取得网络名称字符串
		PCStr getName() const;

		//重置网络数据，所有参数均变为默认，并清理所有内部已有参数数据，但不会释放已分配好的内存；
		Bpnn& reset();
		//默认构造函数
		Bpnn();
		//仅赋值计算内核，不会构造扩展部分；
		Bpnn(const Bpnn& bpnn);
		//仅赋值计算内核，不会赋值训练扩展
		Bpnn& operator=(const Bpnn& bpnn);
		//标准析构
		virtual ~Bpnn();

		//设置层及隐藏节点数，后续若调用了setStructure()，则设置会以会以setStructure()的为准；
		Bpnn& setLayer(Uint hideLayerNumbers = BPNN_DFT_LAYERNUM, Uint hidePerLayerNumbers = BPNN_DFT_PERLAYERROOTNUM);
		//设置内部参数，ls学习步长（学习率），mc动量因子（动量梯度下降法系数，即Momentum算法的动力系数），er_accuracy误差精度；
		//注意：此处er_accuracy是样本输出与目标值的整体误差精度 并非样本预测失败率的精度；
		//注意：该函数请在在buildNet()前调用,在建buildNet()后调用将不会立即更新到网络节点中，需要显示调用updateGlobleParamToNode()。	
		Bpnn& setParam(Float ls = BPNN_DFT_LEARNSTEP, Float er_accuracy = BPNN_DFT_ER, Float mc = BPNN_DFT_MOMENTUM);
		//设置最大执行次数，默认为BPNN_DFT_MAXRUNTIMES次,当为0时如果此时已设置的样本集数据则采用样本集总个数。
		Bpnn& setMaxTimes(Uint iMaxTimes = BPNN_DFT_MAXRUNTIMES);
		//设置Adaptive Moment Estimation优化器，这个算法是一种计算每个参数的自适应学习率的方法；
		//相当于 RMSprop + Momentum；其中的超参belta1 = 0.9，belta2 = 0.999是常量,步长stepl取[1e-5,0.3],默认0.001。
		Bpnn& setAdam(Bool open = false, Float stepL = BPNN_DFT_ADAM_STEP);
		
#define BPNN_CBF_AUTOFIT_PARAM Float& ls, Float& mc,const Bpnn& bp //回调函数
		// 调整学习率和动量系数。其中，学习小于0将不接受修改，动量系数在[0,1)之外不接受修改。
		typedef void (*PCBAutoFitLsAndMc)(BPNN_CBF_AUTOFIT_PARAM);
		typedef void (Bpnn::* PMCBAutoFitLsAndMc)(BPNN_CBF_AUTOFIT_PARAM);
		// 学习率即动量系数调整回调函数设置，pFAutoFit == nullptr时候不进行学习率和动量系数调整；
		// PCBAutoFitLsAndMc pFAutoFit是调整学习率和动量系数的回调函数，
		// 其中，学习率小于0忽略修改，动量系数在[0,1)之外忽略修改。
		// 要使用包装类对象时，使用 BPNN_PCB_AUTOFIT_MAKE 宏生成回调指针；
		Bpnn& setAutoFitLsAndMc(PCBAutoFitLsAndMc pFAutoFit = nullptr, PVoid pIns = nullptr);

		//设置激活函数类型，并立即更新到网络，函数会覆盖BpnnStructDef定义下设置的激活函数，请谨慎使用。
		Bpnn& setTransFunc(EBP_TF iBpTypeHide = EBP_TF::TF_PRelu, EBP_TF iBpTypeOut = EBP_TF::TF_PRelu);
		//设置损失函数类型。
		Bpnn& setLossFunc(EBP_LS lossId = EBP_LS::LS_MeanSquareLoss);

		//设置权值w和阈值bi初始化取值随机区间[A,B]或初始化为定值W；
		//默认为(-1~1),不区分A和B的大小区别。
		//档A=B时，则w也会初始化为一个定值。
		//注意，该函数请在在buildNet()前调用。
		//在BpnnStructDef模式下设置该值值对未对模式做出初始化说明的部分有效。
		Bpnn& setWbiDefault(Float W);
		Bpnn& setWbiDefault(Float A, Float B);

		//设置是否采用样本随机选取执行，当一个训练周期的拟合样本数量等于样本集总数时候采用随机模时设置该值没有意义；
		Bpnn& setUseRandSample(Bool _isUse = false);

#define BPNN_CBF_MONITOR_PARAM Int c, Int max, PCStr inf
		typedef void (Bpnn::* PMCBMonitor)(BPNN_CBF_MONITOR_PARAM);//定义成员函数指针
		typedef void (*PCBMonitor)(BPNN_CBF_MONITOR_PARAM);//定义函数指针

		//一个用于神经网络的运行状态的监控器类，它的作用是实现基础单行滚动显示功能；
		class _dll_ MonitorExample :IBpnnBase {
		protected:
			Int m_printCounts;
			Int m_bkChars;
			void* pTick;
		public:
			MonitorExample();
			virtual ~MonitorExample();
			void reset();
			//已实现基础功能的用于buildNet等带外显接口的函数，用于打印函数执行过程中的内部状态；
			//默认实现了，步数显示，提示显示，运行时间数据显示等。
			// c表示当前信息编号id，max表示总信息数，lpInfo为信息字符串；
			// 当c < 0 时候，代表传入一个提示信息；当c和max都>=0时，表示传入了一个过程信息；
			virtual void print(BPNN_CBF_MONITOR_PARAM);
		};

		//构建网络。构造前清理原网络，该函数必须在train()前调用；
		//_pIns表示外显回掉函数所在的类对象指针，为nullptr表示不导入对象；
		//_pCbFun表示外显回掉函数的函数指针，也可以是类对象成员函数的指针（若如此，必须调用 BPNN_PCB_MONITOR_MAKE 宏做静态转换）；
		Bpnn& buildNet(PCBMonitor _pCbFun = nullptr, PVoid _pIns = nullptr);

		//设置网络的训练过程的当前状态（在非所有权线程调用，可使训练过程退出/暂停/重启等操作）
		Bpnn& setTrainState(EBP_TPC currentState = TPC_Enable);
		EBP_TPC getTrainState() const;

		//运行学习（以当前内部的一对输入输出数据作为依据，不断重复计算）,任何收敛退出返回true；其他情况：
		//超过最大运行次数未收敛，退出返回false。
		//超过收敛达标累计次数，退出返回true。
		//网络未准备好，退出返回false。
		//pOutEa,pOutLs,pOutMc表示每计算一次迭代，产生的参数数据压入到vector中（第一次进入train会被清空）；
		//_pIns表示外显回掉函数所在的类对象指针；
		//_pCbFun表示外显回掉函数的函数指针，也可以时类对象成员函数的指针（若如此，必须调用 BPNN_PCB_MONITOR_MAKE 宏做静态转换）；
		Bool train(VLF* pOutEa = nullptr, VLF* pOutLs = nullptr, VLF* pOutMc = nullptr, PCBMonitor _pCbFun = nullptr, PVoid _pIns = nullptr);

		//创建文本文件并输出网络到文件，保存网络状态；
		//binMode = true表示输出到二进制文件，用于大型网络输出，文本文件生成较慢的情况。
		//当采用二进制方式时，文件的扩展名将被修改为固定的".bpnn"并输出文件，以确保写入固定的二进制数据流文件；
		//encrypteMod = true模型加密
		Bool writeBpnnToFile(PCStr lpFileFullPathName = nullptr, Bool binMode = true,Bool encrypteMod = false);

		//从文件加载数据，并生成整个网络到对象中；若定义的文件不存在则不改变现有的网络结构参数；
		//注意：若文件目标文件存在，则会先清理网络结构及内部保存的自定义BS网络定义描述等对象；并从按目标文件内容加载和生成新的网络；
		//binMode = true表示读取二进制文件，一般用于大型网络，文本文件较慢的情况。
		//当采用二进制方式时，指定的文件的扩展名将被自动替换为读取固定扩展名".bpnn"文件，以确保读取固定的二进制数据流文件；
		Bool readBpnnFormFile(PCStr lpFile, Bool binMode = true);

		//用外部样本集对象设置到内部保存的训练样本集。
		//注意：为节约内存，网络并不会保存BpnnSampSets结构对象的副本，只保留一份对象的引用（内部保存对象指针），请确保在网络生命周期中对象的有效性；
		Bpnn& setSampSets(const BpnnSampSets& tag);

		//计算取得一次预测样本的正确（成功）率；
		//predict 预测样本指针（它应该与训练样本的结构一直且意义相同），当为nullptr时候预测样本选用内部训练样本的数据,
		//nCounst  取用其中多少个参与成功率计算的预测样本（为0时候使用全部）,
		//useRandom 当选用数少于总数时候，是否采用随机样本做评价；
		//crtype 正确率评价所采用的方法类型
		Float getCorrectRate(const BpnnSampSets* predict = 0, Uint nCounst = 0, Bool useRandom = false, EBP_CRT crtype = CRT_MeanSquareLoss);

		//把模型设置为以预测正确率作为模型收敛评价标准的模式，否则模型为目标损失评价模式。
		//正确率 = 正确样本个数 / 统计的样本总数，其中，单个样本所谓的“正确”表示损失评价Er小于收敛精度（逼近），或者最大或最小值出现的位置与目标向量的最值位置相同（分类）
		//correctRate 预测样本的目标正确率[0，1]之间的值,当correctRate=0时关闭正确率评价模式；
		//predict 预测样本指针（它应该与训练样本的结构一致且意义相同），当为nullptr时候预测样本选用内部训练样本的数据,
		//nCounst  取用其中多少个参与成功率计算的预测样本（为0时候使用全部）,
		//useRandom 当选用数少于总数时候，是否采用随机样本做评价，当取用全部样本忽略该值；
		//crtype 正确率评价所采用的方法类型
		Bpnn& setCorrectRateEvaluationModel(Float correctRate = 0, const BpnnSampSets* predict = 0, Uint nCounst = 0, Bool useRandom = false, EBP_CRT crtype = CRT_MeanSquareLoss);
		//检查模型是否是以正确率来评价训练是否收敛的；
		Bool isCorrectRateEvaluationModel() const;

		//设置SGD优化参数：
		//设置单次训练周期中同时用于拟合回归的一批次样本数据个数；（单次训练指的是一次向前运算和一个反向传递）
		//nCounts: 当 N = 总训练样本数时
		//nCounts = 0 或 >= N，单次训练采用整个已设置的样本集合的全部数据（逼近问题）作为一批次样本数据个数，此时对应SGD算法中的Batch Gradient Descent（全数据批次梯度下降算法）；
		//nCounts = 1 表示同一训练步内使用1个样本参与训练拟合；当UseRandomSamp = true时，对应SGD算法中的Stochastic Gradient Descent（单数据批次梯度下降算法），默认情况下一般为单数具批次下降法；
		//nCounts > 1 表示同一训练步内使用多个样本（作为一批次样本数据个数>1但小于全部数据样本个数）参与训练拟合；
		//此时对应SGD算法中的Mini-batch Gradient Descent（小批量数据批次梯度下降算法）；
		//UseRandomSamp:
		//UseRandomSamp = true表示在 nCounts > 0且 nCounts < N 时取用的样本对是否是在整个样本集中随机抽取的（随机抽取样本参与训练拟合，可有效增加模型泛化能力）。
		//当nCounts >= 样本总量 时UseRandomSamp = true指定随机抽取样本参与拟合将没有意义；
		Bpnn& setSampleBatchCounts(Uint nCounts = 1, Bool UseRandomSamp = false);


		//设置输入端向量，不改变输出端。调用该函数不会增加样本对数量计数,用于学习完成后执行预测时的输入，之后调用predict()生成和getOutput(outVt)取得结果
		//注意：为节约内存，网络并不会保存inputArray结构对象的副本，只保留一份对象的引用（内部保存对象指针），请确保在后续调用周期中对象的有效性；
		Bpnn& setInput(const Float* inputArray, Uint dataDim);
		Bpnn& setInput(const VLF& inputArray);

		//取得实际输出,自动初始化out_yi,失败返回false。
		Bool getOutput(VLF& out_yi) const;

		//训练好的模型带入测试，返回Er（tagVec = nullptr时候，返回VtFloatMax），
		//inputVec 输入的测试数据，维度必须和模型输入维度相等，否则报错；
		//tagVec = nullptr 目标真实输出，值为nullptr时则不计算Er；
		//_out_resultVec = nullptr 模型输出结果集指针，值为nullptr时则不输出模型结果；
		//若tagVec和_resultVec都为null时，该方法没有意义（什么也不做）。
		//该预测函数使用的是内置数据区，在多线程中可能存在数据互相抢占而无法预知结果，所以在多线程中执行预测，
		//应该使用makeIndependentDataBuf()创建专用数据区，并使用predictWithIndependentData()进行独立预测；
		Float predict(const VLF& inputVec, VLF* _out_resultVec = nullptr, VLF* tagVec = nullptr);
		Float predict(const Float* inputArray, Uint inputArraySize, VLF* _out_resultVec = nullptr, VLF* tagVec = nullptr);
		//该预测函数前调用setInput，调用后getOutput取得结果；
		Bpnn& predict();

		//并行线程中，构造一个独立计算的数据区（函数为当前线程中的独立运算构造一个专用数据区，该数据区由线程独占，防止多线程同时预测时的数据区抢占混乱）
		//而后使用predictWithIndependentData进行预测；
		void makeIndependentDataBuf(VLF& yiData) const;
		//训练好的模型带入测试，返回Er（tagVec = nullptr时候，返回VtFloatMax），
		//inputVec 输入的测试数据，维度必须和模型输入维度相等，否则报错；
		//tagVec = nullptr 目标真实输出，值为nullptr时则不计算Er；
		//_out_resultVec = nullptr 模型输出结果集指针，值为nullptr时则不输出模型结果；
		//若tagVec和_resultVec都为null时，该方法没有意义（什么也不做）。
		//该预测函数使用专用数据区的数据预测（用于多线程独立预测）,调用前应该用makeIndependentDataBuf()构造独立的专用数据区；
		Float predictWithIndependentData(Float* pyiData, const VLF& inputVec, VLF* _out_resultVec = nullptr, VLF* tagVec = nullptr);
		Float predictWithIndependentData(VLF& yiData, const VLF& inputVec, VLF* _out_resultVec = nullptr, VLF* tagVec = nullptr);

		/*
		采用自定义的复杂神经网络结构模式，当mod为空时候清除自定义模式,并恢复使用默认的通过层数及各层节点数方式来定义网络；
		BpnnStructDef结构定义，你需要的神经网络结构；
		注意：网络并不会保存BpnnStructDef结构对象的副本，只保留一份对象的引用，请确保在buildNet()调用前对象的有效性；
		在buildNet()前调用；
		BpnnStructDef 结构定义范例，有3个隐藏层和1个输出层，如下：
			BpnnStructDef mod = {
				{},  //一个空的隐藏层定义，该无意义的定义在生成对象构造或赋值过程中会被删除；此处仅作一个示范无实际意义；
				{  //一个隐藏层定义
					BSB(1,TF_Sigmoid), //一个组块元定义，其中包含：节点数，传递函数类型，空的链接下标标识数组；
					BSB(3,TF_Tanh,{}), //一个组块元定义，其中包含：节点数，传递函数类型，空的链接下标标识数组；
					BSB(5,TF_Sigmoid)
				},
				{
					BSB(2,TF_Tanh,{0,2,4,6}),  //一个组块元定义，其中包含：节点数，传递函数类型，显示标明的链接下标标识数组；
					BSB(3,TF_Sigmoid,{}),
					BSB(4,TF_Tanh,{1,3,5,7})
				},
				{
					BSB(4,TF_Sigmoid,{}),
					BSB(2,TF_Tanh,{0,1,2,6,7,8}),
					BSB(2,TF_Sigmoid,{})
				},
				//以下的行定义，其中的重复链接下标，或超出的下标，都是允许的，这些错误数据都会在内部自动检查并被清理，无需手动检查；
				//只是注意，当因错误被自动清理后，链接列表内容可能为空了（即size() == 0），则代表本节点要链接上层的所有节点，这可能和设计的初衷是违背的，这一点需要注意；
				{
					BSB(2,TF_Tanh,{0,0,0,1,2,3,2,4,4,4,4}),
					BSB(2,TF_Sigmoid,{0,0,4,8,6,9,9})
				},
			};
		*/
		/*
		BpnnStructDef mod = {
			// 构造第一层（该层直接接纳输入数据的各个维度）：
			// 以第一个BSB为例，它表达了，该组内有6个相同神经元，传递函数为PRelu，由于没有定义对上层的链接描述，所以本组采用对上层全连接（若有需要也可自行指定链接）；
			// 而本层内有4个组，每个组的神经元数量各不相同，所以本层的总神经元数量为各组神经元数量相加，即第一层有6+4+6+4=20个神经元，到此本层定义完毕；			
			{BSB(6,TF_PRelu),BSB(4,TF_LeakyRelu,{}),BSB(6,TF_PRelu),BSB(4,TF_LeakyRelu,{})},

			// 构造第二层：总计4组（4个BSB结构），以第1组（BSB）为例：该组共4个神经元，传递函数为Tanh，指定对上一层链接的编号为{1,3,5,7,9}的神经元
			// 注意上层链接编号是从1开始计数的，而不是0，特别注意；若没有指定了上层链接编号（类似第一层的情况），将采用对上层全连接方式。
			{BSB(4,TF_Tanh,{1,3,5,7,9}),BSB(6,TF_Sigmoid,{2,4,6,8,10}),BSB(4,TF_Tanh,{11,13,15,17,19}),BSB(6,TF_Sigmoid,{12,14,16,18,20})},

			// 第三层（根据需求自定义，方法同上，此处只做演示）
			{BSB(6,TF_PRelu,{1,2,3,4,5,6}),BSB(8,TF_LeakyRelu,{7,8,9,10,11,12,13,14}),BSB(6,TF_PRelu,{15,16,17,18,19,20})},

			// 第四层（根据需求自定义，方法同上，此处只做演示）
			{BSB(4,TF_Tanh,{1,3,5,7,9}),BSB(6,TF_Sigmoid,{2,4,6,8,10}),BSB(4,TF_Tanh,{11,13,15,17,19}),BSB(6,TF_Sigmoid,{12,14,16,18,20})},

			// 最后一层（输出层，该层必须被定义）：由于我们的问题，输出维度必须为4，所以输出层构造的总神经元数量，也必须为4（和BSB个数无关，只和本层神经元总数有关），对上层也采用全连接
			{BSB(4,TF_PRelu)},

		};
		*/
		Bpnn& setStructure(const BpnnStructDef& mod);

		//把内部构件的网络结构以图片方式导出到bitmap文件,bUseDetailMode = true打开绘图细节，表达权值权重等数据
		Bool exportGraphNetStruct(PCStr outFileName, Bool bUseDetailMode = false);

		//显示内部权值及阈值的数据分布情况，帮助更好的调整预设参数；
		//函数返回显示器的一个控制id（根据平台不同意义不同，windows下为窗口句柄）;加密网络无法显示；
		//注意：该函数只是对内核部分参数状态的显示，不会将显示控件对象保存在结构内部，因此应该保存返回值从而实现对控件的后续控制。
		//sectionCounts为分布区段数(当为0，表示采用自适应平滑拟合)
		void* showWbDataDistribution(Int x = 1,Int y = 1,Uint cx = 750,Uint cy = 300, Uint sectionCounts = 0) const;

		//输出内部权值及阈值的数据分布情况；加密网络无法显示；
		void exportWbDataDistribution(VLF& retRange, VLF& retFreq, Uint sectionCounts = 50, Float* vmin = nullptr, Float* vmax = nullptr) const;
				
		/*
		通过Json文本作为流程脚本来执行如下几类工作，包括：读取样本集，生成结构定义，构造网络，执行训练等，显示结果等;
		//uesdSamSets：是设置的用于保存内部传出的样本集合对象指针的变量，若不设置该值（设为nullptr）则网络不会执行加载样本集的过程;
		//设置样本集成功则必定返回非0有效指针，这是一个在堆上分配的对象，网络生命周期结束后需要手动delete释放；
		//uesdCheckSamSets：是设置的用于保存内部传出的正确率测试集合对象指针的变量，若不设置该值（设为nullptr）则网络不设置
		//测试样本集并在采用正确率判断模式情况下使用训练样本数据集作为测试数据集;
		//设置测试样本集成功则必定返回非0有效指针，这是一个在堆上分配的对象，网络生命周期结束后需要手动delete释放；
		// json格式如下：
{
 "enable": 1,
  "BpnnSampSets": [
	""
  ],
  "BpnnStructDef": [
	{
	  "writeToFile": "",
	  "readFromFile": "",
	  "mod": {
		"enable": 0,
		"Struct": [
		  [
			{
			  "nCounts": 1,
			  "trFunc": 3,
			  "linkPos": [1,2,3,6,7,8],
			  "wcFunc": 0,
			  "wi": [0],
			  "bi": [0.25,0]
			}
		  ]
		]
	  },
	  "addScript": {
		"enable": 0,
		"script": [
		  [
			{
			  "upIndex": 0,
			  "type": 3,
			  "filterMap": [7,7,256,3,3,3],
			  "wi": [0],
			  "bi": [0.25,0],
			  "trFunc": 0,
			  "wcFunc": 0,
			  "flag": 0
			}
		  ]
		],
		"inputFilterMap": [3,5,5,3],
		"logout": 1
	  }
	}
  ],
  "reset": 1,
  "setName": "bp-test1",
  "setAutoFitLsAndMc": [    1,    0.95,    0.05  ],
  "setBatchNormalization": [    500,    1  ],
  "setCorrectRateEvaluationModel": [    500,    1  ],
  "setDropout": [ 0,    2,    2,    0.1  ],
  "setLayer": [    0,    3  ],
  "setStructure": "def1",
  "setLossFunc": 0,
  "setMaxTimes": 0.5,
  "setMultiThreadSupport": 1,
  "setNetUseTo": 0,
  "setParam": [    0.001,    0.025,    0.8  ],
  "setSampSets": "def1",
  "setSampleBatchCounts": [    500,    1  ],
  "setTransFunc": [    0,    1  ],
  "setUseRandSample": 1,
  "setWbiDefault": [    -1,    1  ],
  "openGraphFlag": 1,
  "buildNet": 1,
  "readBpnnFormFile":  {
	"enable": 1,
	"filePath": "",
	"binMode": 1
  },
  "train": 1,
  "writeBpnnToFile": {
	"enable": 1,
	"filePath": "",
	"binMode": 1,
	"encrypteMod": 0
  },
  "showGraphNetStruct": [    0,    1,    1  ],
  "showGraphParam": [    0,    1,    1  ],
  "showWbDataDistribution": [    0,    1,    1  ],
  "exportGraphCorrectRate": "",
  "exportGraphEr": "",
  "exportGraphNetStruct": ""
}
		*/
		
		Bool executeByJson(BpnnInterfaceStore* iStore, PCStr jsonString);
		static Bool executeByJson(Bpnn& bp, BpnnInterfaceStore* iStore, PCStr jsonString);

		//通过Json文件作为流程脚本执行来如下几类工作，包括：读取样本集，生成结构定义，构造网络，执行训练等，显示结果等;
		//trainSampSets：是设置的用于保存内部传出的样本集合对象指针的变量，若不设置该值（设为nullptr）则网络不会执行加载样本集的过程;
		//设置样本集成功则必定返回非0有效指针，这是一个在堆上分配的对象，网络生命周期结束后需要手动delete释放；
		//testSampSets：是设置的用于保存内部传出的正确率测试集合对象指针的变量，若不设置该值（设为nullptr）则网络不设置
		//测试样本集并在采用正确率判断模式情况下使用训练样本数据集作为测试数据集;
		//设置测试样本集成功则必定返回非0有效指针，这是一个在堆上分配的对象，网络生命周期结束后需要手动delete释放；		
		Bool executeByJsonFile(BpnnInterfaceStore* iStore, PCStr jsonFile);
		static Bool executeByJsonFile(Bpnn& bp, BpnnInterfaceStore* iStore, PCStr jsonFile);
	};

	typedef Bpnn* PBpnn; //BP神经网络类


	//生成（以类对象下的函数作为回调函数的）函数指针，用于各种可外显回掉的方法中
	//参数说明：( 目标类对象，保存回调类对象指针的变量名称，保存回调类中回调函数指针的变量名称，目标类名 ，目标类中的回调函数名)
#define  BPNN_PCB_MONITOR_MAKE(CallBack_PtParam_Name, Full_Name_Of_CallBack_Func_In_Tag_Class) \
	auto Cb_##tempMonitorCbPointVarName = &(Full_Name_Of_CallBack_Func_In_Tag_Class);\
	Bpnn::PCBMonitor CallBack_PtParam_Name = (*(Bpnn::PCBMonitor*)&Cb_##tempMonitorCbPointVarName); 

	//生成（以类对象下的函数作为回调函数的）函数指针，用于setAutoFitLsAndMc()函数传参各种可外显回掉的方法中
	//参数说明：( 目标类对象，保存回调类对象指针的变量名称，保存回调类中回调函数指针的变量名称，目标类名 ，目标类中的回调函数名)
#define  BPNN_PCB_AUTOFIT_MAKE(CallBack_PtParam_Name, Full_Name_Of_CallBack_Func_In_Tag_Class) \
	auto Cb_##tempAutoFitCbPointVarName = &(Full_Name_Of_CallBack_Func_In_Tag_Class);\
	Bpnn::PCBAutoFitLsAndMc CallBack_PtParam_Name = (*(Bpnn::PCBAutoFitLsAndMc*)&Cb_##tempAutoFitCbPointVarName); 
	
//全局静态接口-------------------------------------------------------------

	//查看主机的加速设备,type = 0手动选择，= 1 输出简化，= 2 输出详细
	_dll_ void showAcceleratorDeviceAmp(Uint type = 0);
	//取得 (0,x) 开区间的随机整数：rand()%x；
	_dll_ Int  rand_i_0_x(const Int x);
	//	取得 (a,b) 开区间的随机整数：rand()%(b-a)；
	_dll_ Int  rand_i_a_b(const Int a, const Int  b);
	//	取得 [a,b) 右开区间的随机整数：rand()%(b-a)+a；
	_dll_ Int rand_i_ai_b(const Int  a, const Int  b);
	//	取得 [a,b] 闭区间的随机整数：rand()%(b-a+1)+a；
	_dll_ Int rand_i_ai_bi(const Int  a, const Int  b);
	//	取得 (a,b] 左开区间的随机整数：rand()%(b-a)+a+1；
	_dll_ Int rand_i_a_bi(const Int  a, const Int  b);
	//	取得 (0,1) 之间的浮点数：rand()/double(RAND_MAX)；
	_dll_ Float rand_f_0_1();
	//	取得 (a,b) 之间的浮点数：rand()/double(RAND_MAX)*((b) - (a))+(a)；
	_dll_ Float rand_f_a_b(const Float  a, const Float  b);
	typedef vector<Float> RandInitParam;//rand_initData()的类型对应参数列表；
	//由itType指定的方式初始化数据或连续数据列表（生成数据可以包括常数或满足某个范围或者特定分布的随机数等，类型请参考EBP_IT列举的类型）；
	//paramLst是参数列表，根据不同的初始化类型有不同的意义；
	_dll_ void rand_initData(Float* pData, Uint dataSize, EBP_IT itType, const RandInitParam& paramLst = RandInitParam());
	
}
namespace CL = CreativeLus;//名称空间：创造性逻辑单元
namespace Cl = CreativeLus;//名称空间：创造性逻辑单元
namespace cl = CreativeLus;//名称空间：创造性逻辑单元

#endif