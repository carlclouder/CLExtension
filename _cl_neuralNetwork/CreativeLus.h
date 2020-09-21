//DESIGNED BY CAILUO @2020-02
//MINI-SUPPORT @ C++11
//VERSION:1.0.5

#pragma once

#ifndef __CL_CREATIVELUS_H__
#define __CL_CREATIVELUS_H__

#ifdef BPNNTOOL_EXPORTS		               // dll����ģʽ�£�������
#define _dll_ __declspec(dllexport)
#else
#ifdef _LIB                                // VC�±��Ϊlib����ģʽ�£�����������ʶ
#define _dll_
#else
#ifdef _USELIB                             // ��ͷ�ļ����Ϊlib����ģʽ�£�����������ʶ
#define _dll_
#else
#define _dll_ __declspec(dllimport)        // ����dll����ģʽ�£�������
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
	
#define ConstE     (2.7182818284590f)  //������Ȼ����
#define ConstPi    (3.1415926535898f)  //����Բ����
#define VtFloatMax (3.402823466e+38f)  //��󸡵�ֵ
#define VtEpslon   (1e-8f)             //��С������
	   
#define BPNN_DFT_LEARNSTEP         0.1      //Ĭ��ѧϰ����
#define BPNN_DFT_MOMENTUM          0.8      //Ĭ�϶�������

#define BPNN_DFT_MAXRUNTIMES       100      //Ĭ�����ִ�д��� 
#define BPNN_DFT_ER                0.0001   //Ĭ�������С���� 
#define BPNN_DFT_ADAM_STEP         0.001    //Ĭ��Adam�Ż��Ĳ������ò���Ϊ(1e-5, 0.3)

#define BPNN_DFT_LAYERNUM          1        //Ĭ����������� 
#define BPNN_DFT_PERLAYERROOTNUM   3        //Ĭ��������ڵ��� 

	//��������
	enum EBP_UT {
		UT_Classify, // ���࣬ÿ����ϲ��õ���������Ϊ1������
		UT_Approach, // �ƽ���ÿ����ϲ��õ���������Ϊȫ��ѵ��������		
	};

	//�����任����
	enum EBP_STT {
		STT_Null,       //���ݲ�����һ�����׼��
		STT_Normalize,  //��������һ����(0,1)
		STT_NormalizeEx,//��������һ����(-1,1)
		STT_Standart,   //��������׼����(0,1)��׼�ֲ�
	};

	//���弤������ͼ�����ԭ��
	enum EBP_TF {		
		TF_PRelu,        //���ݺ���Ϊ�� y = max(0.7x,x)��		
		TF_Purelin,      //���ݺ���Ϊ�� �����Ժ��� y = x��
		TF_Sigmoid,      //���ݺ���Ϊ�� S����
		TF_Tanh,         //���ݺ���Ϊ�� tanh����		
		TF_Relu,         //���ݺ���Ϊ�� y = max(0,x)��
		TF_LeakyRelu,    //���ݺ���Ϊ�� y = max(0.01x,x)��
		TF_Step,		 //���ݺ���Ϊ�� y = abs(x) >= 1 ? 1 : 0��		
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
		TF_Exp,          //ȡeָ��
		TF_Ln,           //ȡln����
	};

	//����Ȩֵ������ͼ���Ϻ���ԭ��
	enum EBP_WC {
		WC_Add,     //�������
		WC_Average, //������Ӻ���Ӳ������ȡ��ֵ
		WC_Max,     //��Χ�ڰ��������е������ȡ���
		WC_Min,     //��Χ�ڰ��������е������ȡ��С	
		WC_Convolution = WC_Add,//��������ʽ����Ч��WC_Add��ʽ
	};

	//������ʧ�������ͼ�ԭ�� 
	enum EBP_LS {
		LS_MeanSquareLoss,  //������
		LS_CrossEntropyLoss,//������
	};

	//��ȷ�ʵ����۷�ʽ����
	enum EBP_CRT {
		CRT_MeanSquareLoss,  //��������ľ�������ʧֵС���趨������
		CRT_MaxValuePosMatch,//����������ֵ����λ����Ŀ���������ֵ���ڵ�λ����ͬ��һ������ͨ�����ֵ���з������ȷ�����������
		CRT_MinValuePosMatch,//���������Сֵ����λ����Ŀ��������Сֵ���ڵ�λ����ͬ��һ������ͨ����Сֵ���з������ȷ�����������
	};

	//������ʼ������ö��ֵ������Ȩֵ��ֵ��ʼ�����õķ���
	enum EBP_IT
	{
		IT_Const,   //����
		IT_Uniform, //��ֵ�ֲ�
		IT_Gaussian,//��˹�ֲ�������̫�ֲ���
		IT_Normal = IT_Gaussian,//��̫�ֲ�������˹�ֲ���
		IT_Xavier,//������0��ֵ����׼��Ϊsqrt(2 / (fan_in + fan_out))����̬�ֲ�,����fan_in��fan_out�Ƿֱ�Ȩֵ��������������Ԫ����Ŀ����tanh��������кܺõ�Ч��������������Relu�������
		IT_Msra,//��ֵΪ0������Ϊ��2/����ĸ���n���ĸ�˹�ֲ������ر��ʺ� Relu�������
	};

	//ģ�Ͳ�����׼���Ż�����������ĳһ�����������;
	//���������ͼ��shape��Ϊ[N, C, H, W]�������£�
	//HW��ʾһ������ռ�ά�ȣ�������һ������������һ��X�У�Ҳ������һ��XYƽ�����ص������У�������һ��Instance����
	//C��ʾһ��ͨ��ά�ȣ�
	//N��ʾһ�ֶ����ʵ�ʸ���ά�ȣ�������ά�ȣ���
	enum EBP_NT {
		NT_BatchNorm,// ����batch�ϣ���NHW����һ������СbatchsizeЧ�����ã�
		NT_LayerNorm,// ��ͨ�������ϣ���CHW��һ������Ҫ��RNN�������ԣ�
		NT_InstanceNorm,// ��ͼ�������ϣ���HW����һ�������ڷ��Ǩ�ƣ�
		NT_GroupNorm,// ��channel���飬Ȼ��������һ����
		NT_SwitchableNorm,// �ǽ�BN��LN��IN��ϣ�����Ȩ�أ��������Լ�ȥѧϰ��һ����Ӧ��ʹ��ʲô������
	};

	//ѵ��������һ����ʱ���̣�ö��ֵ�����ѵ�����̵�ǰ����״̬
	enum EBP_TPC {
		TPC_Enable, //����״̬
		TPC_Disable,//ѵ��������Ϊ�����ã���������
		TPC_Stop = TPC_Disable, //ѵ������ֹͣ������
		TPC_Pause,  //��ͣѵ�����̣��ڿ���Ȩ�̵߳��ÿ�������������ڿ���Ȩ�߳����øñ����Ч
		TPC_Resume = TPC_Enable,//�ӵ�ǰ��ͣ״̬�ָ�����
	};

	//define BpnnSampSets -----------------------------------------------------------------------------------------------

	//���нӿ�
	struct _dll_ IBpnnBase {
		virtual ~IBpnnBase();
	};
	typedef IBpnnBase *PIBpnnBase;

	//������������԰�װ�ࣨ���ಢ�����ƺͲ����µ����ݣ����������е�BpnnSampSets�������������������װ�����
	class _dll_ BpnnSampPair :IBpnnBase {
	protected:
		Float* piv, * pov;
		Uint ivDim, tvDim;
	public:
		explicit BpnnSampPair();
		explicit BpnnSampPair(Float* iv, Uint ivDim, Float* ov, Uint tvDim);
		virtual ~BpnnSampPair();
		//��ȡ������������ά��
		Uint intputDimension() const;
		//��ȡĿ����������ά��
		Uint targetDimension() const;
		//�����԰�װ������ȫ����0
		void clear();
		//�ɱ��iȡ��iv����ov�е�ֵ����i>iv.size()��Ϳ�ʼȡov�еĶ�����,��ע��÷�����ʹ�ù���
		Float& operator[](Uint i);
		//��ȡ��װ���������ݵĸ���
		VLF inputVec() const;
		//��ȡ��װ��Ŀ�����ݵĸ���
		VLF targetVec() const;
		//��ȡ��װ��������������ָ��
		Float* iv() const;
		//��ȡ��װ��Ŀ����������ָ��
		Float* tv() const;
	};
	typedef vector<BpnnSampPair> BpnnSampPairList, * PBpnnSampPairList;//�����Լ���
	//�任ģʽ������
	class _dll_ BpnnTransModelUnit :IBpnnBase {
	public:
		Uint dimIndex;//ά�ȱ��
		Uint dimType;//ά�ȵı任����
		Float vmax;//��Сֵ
		Float vmin;//��Сֵ
		Float vAver;//��ֵ
		Float vStandardDeviation;//��׼��
		BpnnTransModelUnit();
		void reset();
		virtual ~BpnnTransModelUnit();
		//��һ��ֵͨ��ӳ��任Ϊ��ֵ��
		Float forward(Float org) const;
		//��һ���任���ֵͨ��ӳ����任Ϊԭֵ��
		Float backward(Float tag) const;
	};
	//���ڱ���任��¼
	class _dll_ BpnnSampTransModelRecord:IBpnnBase,public std::map<Uint, BpnnTransModelUnit>{
	public:
		//��һ��ֵͨ����Ӧ��ӳ��任Ϊ��ֵ��
		Float forward(Float org, Uint index) const;
		//��һ���任���ֵͨ����Ӧ��ӳ����任Ϊԭֵ��
		Float backward(Float tag, Uint index) const;
		//��һ������ͨ����Ӧ��ӳ��任Ϊ��������alignStartIndex��ʾ��ת����¼�ĵڼ�����ʼ����ת����
		void forward(const VLF& org, VLF& tag, Uint alignStartIndex) const;
		//��һ���任�������ͨ����Ӧ��ӳ����任Ϊԭ������alignStartIndex��ʾ��ת����¼�ĵڼ�����ʼ����ת����
		void backward(const VLF& tag, VLF& org, Uint alignStartIndex) const;
	};
	typedef vector<Uint> BpnnSampTransTypeVec;//�������ԣ��任���ͱ������


	//����������
	class _dll_ BpnnSampSets :IBpnnBase {
	protected:
		VLF ivdata;
		VLF tvdata;
		Uint ivDim, tvDim;
		BpnnSampTransModelRecord transModeRec;//���ݱ任��¼
	public:
		//�ı����ݵ�ά�ȣ��������ڲ����ݣ�
		BpnnSampSets& changeDimension(Uint ivDim, Uint tvDim);
		//��ȡ���ݼ������Ը�����
		Uint size() const;
		//���úͳ�ʼ�����ݼ��ṹ����С��ά�ȡ�Ĭ��ֵ�ȣ�ֻҪ�����Ŀ������ά�Ȳ��䣬���ô�С����ı��������ݵ�ֵ��
		BpnnSampSets& resize(Uint newSize, Float defaultInValue = 0.0, Float defaultTagValue = 0.0, Uint newInDim = 0, Uint newTagDim = 0);
		BpnnSampSets();
		explicit BpnnSampSets(Uint ivDim, Uint tvDim);
		virtual ~BpnnSampSets();
		//��ȡ������������ά�ȣ�
		Uint intputDimension() const;
		//��ȡ�����������ά�ȣ�
		Uint targetDimension() const;
		//��ȡ�����������������ά������
		Uint dimension() const;
		//������ݣ�������ά����Ϣ
		BpnnSampSets& clear();
		//������ݣ�������ά����Ϣ���ͷ��ڴ棻
		BpnnSampSets& reset();
		//�����������ļ���binMode = true��ʾ2�����ļ���ʽ���棨��ģʽ���ڴ������ݵĵ�����
		//�����ö����Ʒ�ʽʱ���ļ�����չ�������޸�Ϊ�̶���".bpnnSampSets"������ļ�����ȷ��д��̶��Ķ������������ļ���
		Bool writeToFile(PCStr file, Bool binMode = true);
		//ȡ�������ļ�������һ��ԭ�ڲ����϶���ĸ�����bkSet�У�bkSet = nullptr �����ƣ�
		//binMode = true��ʾ��ȡ�������ļ�����ģʽ���ڴ������������
		//�����ö����Ʒ�ʽʱ��ָ�����ļ�����չ�������Զ��滻Ϊ��ȡ�̶���չ��".bpnnSampSets"�ļ�����ȷ����ȡ�̶��Ķ������������ļ���
		Bool readFromFile(PCStr lpFile, Bool binMode = true);
		//���ڲ��������ݼ�����һ�����׼�����任������һ��BpnnSampTransTypeVec����������ÿһά�ȶ�Ӧһ�ֱ任����ʽ��
		BpnnSampSets& normalizationOrStandardization(const BpnnSampTransTypeVec& flagVecter);
		//���ݱ任�ṹ��ļ�¼������һ������������ά���ܺͣ�
		Uint getTransModRecSize() const; 
		//ȡ���ڲ��任��¼�������µ�ʵ��
		BpnnSampTransModelRecord getTransModRec() const;
		//ȡ�������ļ��ı任�ṹ�壻
		//binMode = true��ʾ��ȡ�������ļ�����ģʽ���ڴ������������
		//�����ö����Ʒ�ʽʱ��ָ�����ļ�����չ�������Զ��滻Ϊ��ȡ�̶���չ��".bpnnSampSets"�ļ�����ȷ����ȡ�̶��Ķ������������ļ���
		static BpnnSampTransModelRecord getTransModRec(PCStr lpFile, Bool binMode = true);

		//����ڲ��ı任��¼
		BpnnSampSets& clearTransModRec();
		//�����ڲ��任��¼
		BpnnSampSets& setTransModRec(const BpnnSampTransModelRecord& rec);
		//�ڲ����ݼ�����һ�������ԣ��꽫ָ�����һ������
		BpnnSampSets& addSample(const VLF& inputArray, const VLF& targetArray);
		BpnnSampSets& addSample(const Float* inputArray,Uint inputArrayDim, const Float* targetArray,Uint targetArrayDim );
		BpnnSampSets& addSample(const BpnnSampPair& samPair);
		BpnnSampSets& setSample(Uint samIndex, const VLF& inputArray, const VLF& targetArray);
		BpnnSampSets& setSample(Uint samIndex, const Float* inputArray, Uint inputArrayDim, const Float* targetArray, Uint targetArrayDim);
		BpnnSampSets& setSample(Uint samIndex, const BpnnSampPair& samPair);
		//��Ŀ��������ÿ��ά�ȷ������������ֵ���ͬ�����������
		void copyAndTargetDimToIntervalClassification(BpnnSampSets& newSampSets, Float classFlagMax, Float classFlagMin, const VLF& intervalTable) const;
		//���������԰�װ����
		BpnnSampPair operator[](const Uint i);
		//��������Ż�ȡֻ���������Ե�������������ָ��
		const Float* iv(const Uint i) const;
		//��������Ż�ȡֻ���������Ե�Ŀ����������ָ��
		const Float* tv(const Uint i) const;
		//��ȡ�����Ե�������������ָ��ͷ
		const Float* ivData() const;
		//��ȡ�����Ե�Ŀ����������ָ��ͷ
		const Float* tvData() const;
		//��ȡ�����Ե�����������������
		const VLF& ivDataVec() const;
		//��ȡ�����Ե�Ŀ��������������
		const VLF& tvDataVec() const;
		//ȡ���������������������е�ĳһ��ά�ȵķ�������,������0��ʼ��һ�����У�������ά�ȣ���Ϊ���ά��
		void getDimVecData(VLF& reseult, Uint dimVecIndex);
		//ȡ��������������ĳһ��ά�ȵķ�������,������0��ʼ
		void getIntputDimVecData(VLF& reseult, Uint dimVecIndex);
		//ȡ�������������ĳһ��ά�ȵķ�������,������0��ʼ
		void getTargetDimVecData(VLF& reseult, Uint dimVecIndex);
	};

	//define BpnnStructDef -----------------------------------------------------------------------------------------------
	typedef vector<Float> WiInitDef;//Ȩֵ��ʼ��ģʽ����ṹ���磺{IT_Const,1},��ʾ��ʼ��Ϊ����1��{IT_Msar},��Msar��ʽ��ʼ��Ȩֵ
	typedef vector<Float> BiInitDef;//��ֵ��ʼ��ģʽ����ṹ���磺{IT_Const,1},��ʾ��ʼ��Ϊ����1��{IT_Uniform,-1��1},����ֵ�����ʽ��ʼ��ֵ����Χ-1��1
	typedef vector<Int> LinkPos;    //���ز㵥Ԫ���ڲ��������Ԫ�е�ÿ���ڵ� ��Ӧ���ϲ������±��ǣ���1��ʼ�����ṹΪ��ʱ�򣬱�ʾ�����ϲ����нڵ㣬��Ϊ-1���Ըýڵ����ӣ�
	typedef vector<Float> LinkWij;  //���ز㵥Ԫ���ڲ��������Ԫ�е�ÿ���ڵ� ��Ӧ���ϲ����ӳ�ʼȨֵ����Ϊ��ʱ��ͨ�������Ȩֵ��ֵ��
	typedef vector<Float> LinkBi;

	enum EBP_SCP {
		SCP_Fc,    //ȫ��������
		SCP_Trans, //��������
		SCP_Pool,  //�ػ�����
		SCP_Conv,  //�������
		SCP_ConvSep,//��־������
		SCP_Combin, //��ӽ������
		SCP_Softmax,//softmax����������
	};

	/*
		InputFilterMap д����
		SCP_Fc : inputFilterMap = { �ϲ���ʼ������Ԫ���index��������� };
		SCP_Trans: inputFilterMap = {�ϲ���ʼ������Ԫ���index���������};
		SCP_Pool: inputFilterMap = { �ϲ���ʼ������Ԫ���index������mapͨ����deep ������map���wt������map�߶�ht};
		SCP_Conv: inputFilterMap = { �ϲ���ʼ������Ԫ���index������mapͨ����deep ������map���wt������map�߶�ht};
		SCP_ConvSep: inputFilterMap = { �ϲ���ʼ������Ԫ���index������mapͨ����deep ������map���wt������map�߶�ht};
		SCP_Combin: inputFilterMap = { �ϲ���ʼ������Ԫ���index��������� };
		SCP_Softmax: inputFilterMap = { �ϲ���ʼ������Ԫ���index��������� };
	*/
	typedef VLUI InputFilterMap;
	typedef VLUI FilterMapStruct;

	struct BpnnStructScriptSet
	{
		Uint upIndex = 0; //��һ��Ҫ���ӵ���ı��
		EBP_SCP type = SCP_Fc; //���������

		/*
		���屾��ű������Ӻ����ӳ�䷽ʽ��
		filterMap д����
			SCP_Fc : filterMap = { �������} ;
			SCP_Trans : filterMap = {} ;
			SCP_Pool : filterMap = { filter��filter�ߣ�X������Y����} ;
			SCP_Conv : filterMap = { filter��filter�ߣ����map���������padding��X������Y����} ;
			SCP_ConvSep : filterMap = { filter��filter�ߣ����padding��X������Y����} ;
			SCP_Combin : filterMap = { ��ϴ��� } ;
			SCP_Softmax : filterMap = { ����������} ;
		*/
		FilterMapStruct filterMap;
		WiInitDef wi; //Ȩֵbi��ʼ������
		BiInitDef bi; //��ֵbi��ʼ������
		Int trFunc = -1;//��������ͣ���ֵΪС��0��ʾʹ�ö�Ӧ�������͵�Ĭ��ֵ
		Int wcFunc = -1;//Ȩֵ��Ϻ������ͣ���ֵΪС��0��ʾʹ�ö�Ӧ�������͵�Ĭ��ֵ
		Int flag = -1;//��ı�ǣ�����������ֵΪС��0��ʾʹ�ö�Ӧ�������͵�Ĭ��ֵ
	};
	typedef vector<BpnnStructScriptSet> BpnnStructScriptLayer;
	/*
	�ű�����ṹ������������ʽ��
	BpnnStructScript scp = {
		{ //��һ�㿪ʼ
			{ 0, SCP_Fc, {10},{ IT_Unifon,-1,1 } }, //һ��1���BpnnStructScriptSet�ṹ������һ��ȫ���Ӳ�������
			{ 0, SCP_Conv,{7,7,256,3,3,3},{IT_Msra},{ IT_Const,0 },TF_Relu,WC_Add,0 }, //һ��2�飬����һ�������������
		},//��һ�����
		{ //�ڶ��㿪ʼ
			{ 0, SCP_Trans}, //����1�飬����һ�����������飬���ӵ�һ��1��
			{ 1, SCP_Pool,{2,2,2,2}}, //����2�飬����һ���ػ��������飬���ӵ�һ��1��
		}//�ڶ������
	}
	*/
	class _dll_ BpnnStructScript :IBpnnBase, public std::vector<BpnnStructScriptLayer> {
	public:
		BpnnStructScript();
		BpnnStructScript(const initializer_list<BpnnStructScriptLayer>& list);
		virtual ~BpnnStructScript();
	};

	//���ز㵥Ԫ���ڲ��������Ԫ����ṹ������齫����һ���ڵ�������ͬ��С�飬
	//ָ��С���нڵ�����������С���ڵĽڵ㴫�ݺ����������±����ͬ��
	//�������Զ�������ṹ����С������Ԫ
	class _dll_ BSB :IBpnnBase {
	public:
		Uint nnCounts;//Ԫ���ڵ�ͬ����Ԫ����
		Byte transFuncType;//���������
		Byte wcFuncType;//Ȩֵ�������
		LinkPos linkPos;//������һ��ڵ������ʾ���ϲ�ڵ�λ�ñ�ǣ���1��ʼ����sizeΪ0ʱ������ȫ���ϲ�ڵ㣻
		LinkWij linkWij;//������һ��ڵ�Ķ�Ӧ���ϲ����ӳ�ʼȨֵ����Ϊ��ʱ����ͨ�������Ȩֵ��ֵ��
		LinkBi bi;//������һ��ڵ�Ķ�Ӧ���ϲ����ӳ�ʼ��ֵ,��Ϊ��ʱ��ȡ���ֵ��
		Int globleWbId;//����wb������������ֵ<0ʱ��ѡ�þֲ�Wb�ṹ
		BSB();
		BSB(Uint _nnCounts, EBP_TF _transFunc);
		BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos);
		BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos, EBP_WC _wcType);
		BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos, EBP_WC _wcType, const LinkWij& _linkForwordNodeWij);
		BSB(Uint _nnCounts, EBP_TF _transFunc, const LinkPos& _linkForwordNodePos, EBP_WC _wcType, const LinkWij& _linkForwordNodeWij, const LinkBi& _bi);
		virtual ~BSB();
		void reset();//����Ԫ������
	};
	typedef vector<BSB> BSLayer;//����ṹ��Ԫ�㶨��ṹ
	typedef vector<BSLayer> BpnnStruct;//����ṹ����ṹ
	typedef initializer_list<BSLayer> BSInitList;//���ڹ�������ز㵥Ԫ����ṹ��
	typedef vector<Uint> ConvolutionKernel;//����˶��������࣬�����磺{ ����ͨ��Indepth,��width,��height,���map����Outdepth}
	typedef vector<Uint> PoolingMap;//�ػ�ӳ�䷽ʽ���壬�����磺{ ����ͨ��depth,��width,��height,X����X_Stride,Y����Y_Stride}

	//Wb��������
	class _dll_ WbDef:IBpnnBase {
	public:
		BiInitDef bi;//��ֵ��ʼ������
		WiInitDef wi;//��ֵ��ʼ������
		Uint wiSize;//Ȩֵ����
		Bool bNotUpdate;//��Ȩֵ�Ƿ���ѵ�������н��и���
		WbDef();
		WbDef(Uint _wiSize, const  WiInitDef& _wi, const BiInitDef& _bi, Bool bNotUpdate = false);
		virtual ~WbDef();
		void reset();
	};
	typedef vector<WbDef> WbLib;//Wb�ⶨ��

	/*
		BpnnStructDef�ṹ������Ҫ�Զ����������ṹ����buildNet()ǰ���壻
		BpnnStructDef �ṹ���巶������3�����ز��1������㣬���£�
			BpnnStructDef mod = {
				{},  //һ���յ����ز㶨�壬��������Ķ��������ɶ������ֵ�����б�ɾ����
				{  //һ�����ز㶨��
					BSB(1,TF_Sigmoid), //һ�����Ԫ���壬���а������ڵ��������ݺ������ͣ��յ������±��ʶ���飻
					BSB(3,TF_Tanh,{}), //һ�����Ԫ���壬���а������ڵ��������ݺ������ͣ��յ������±��ʶ���飻
					BSB(5,TF_Sigmoid)
				},
				{
					BSB(2,TF_Tanh,{0,2,4,6}),  //һ�����Ԫ���壬���а������ڵ��������ݺ������ͣ���ʾ�����������±��ʶ���飻
					BSB(3,TF_Sigmoid,{}),
					BSB(4,TF_Tanh,{1,3,5,7})
				},
				{
					BSB(4,TF_Sigmoid,{}),
					BSB(2,TF_Tanh,{0,1,2,6,7,8}),
					BSB(2,TF_Sigmoid,{})
				},
				//���µ��ж��壬���е��ظ������±꣬�򳬳����±꣬��������ģ���Щ�������ݶ������ڲ��Զ���鲢�����������ֶ���飻
				//ֻ��ע�⣬��������Զ�����������б����ݿ���Ϊ���ˣ���size() == 0����������ڵ�Ҫ�����ϲ�����нڵ㣬����ܺ���Ƶĳ�����Υ���ģ���һ����Ҫע�⣻
				{BSB(2,TF_Tanh,{0,0,0,1,2,3,2,4,4,4,4}),BSB(2,TF_Sigmoid,{0,0,4,8,6,9,9})},
			};
		*/
	class  _dll_ BpnnStructDef :IBpnnBase,public BpnnStruct {
	public:
		//ȫ�ֵ�Ȩֵ��ֵ�����
		WbLib gWb;
		//��һ��ǰ�����Ӽ��
		void preCheck();
		//�ɲ�������WbDef,thisWbIsNotUpdate = false��ʾʹ�ø�Ȩֵ����Ľڵ㽫����ı��Ȩֵ������ʼ����Ȩֵ��ֵ���ֲ��䣬���ڲ���Ҫ����Ȩֵ�Ľڵ㣩
		static WbDef& makeWbDefine(WbDef& def, Uint wiCounts, const WiInitDef& wi, const BiInitDef& bi, const Bool thisWbIsNotUpdate = false);
	public:
		BpnnStructDef();
		BpnnStructDef(const BSInitList& _Ilist);
		virtual ~BpnnStructDef();
		//������ж�������
		void clear();
		BpnnStructDef& operator = (const BSInitList& _Ilist);
		//ȥ���յ���Ч�Ĳ㶨�壬���޸Ķ����еĴ���
		BpnnStructDef& valid();
		//ȡ���Ѷ���Ľṹ��������Ԫ����
		Uint neuronCounts() const;
		//ȡ���Ѷ���Ľṹ��������Ԫ��������һ����鶨����������ɸ���ͬ���Ե���Ԫ��
		Uint getNeuralBlockCounts() const;
		//ȡ�ö�����ܲ���
		Uint layerCounts() const;
		//ȡ�õڼ������Ԫ������layId��0��ʼ��
		Uint getNeuronCountsInLayer(Uint layId);
		//ȡ�õڼ������Ԫ���������layId��0��ʼ��
		Uint getNeuralBlockCountsInLayer(Uint layId);
		//���ļ���ȡģʽ
		Bool readFromFile(PCStr file);
		//���ģʽ���ļ�
		BpnnStructDef& writeToFile(PCStr file);
		//�ɲ���ȡ���µĳߴ糤�� = (baseLen + 2 * padding - kernelLen) / stride + 1;
		static Uint getNewLength(Uint baseLen, Uint kernelLen, Uint padding, Uint stride);

		//��ĳ��������һ��������������
		//�þ�������Ƕ�ͨ���������ˣ�������õ�һ����ͨ������㣻	
		BpnnStructDef& addOneConvolution(
			Uint appendhidelayerIndex, //���ӵ���һ������ı�ű�ʶ(��0��ʼ)
			Uint mapBaseIndex,//���ξ��������Ԫ����Ӧ���ӵ��ϲ�ڵ���ʼ��(��0��ʼ)
			Uint& _in_out_baseLayer_Xsi, //����X��ά��,������ȷ���غ󱣴��µĳߴ�ֵ
			Uint& _in_out_baseLayer_Ysi, //����Y��ά��,������ȷ���غ󱣴��µĳߴ�ֵ
			const ConvolutionKernel& ckMetrix, //�����(���ƽ�������������ͨ������������ȣ����ݣ������磺{ ����ͨ����,filter��,filter��,���map��,padding,,X����,Y����}
			const WiInitDef& wi, //Ȩֵbi��ʼ������
			const BiInitDef& bi, //��ֵbi��ʼ������
			EBP_TF transFunc = EBP_TF::TF_Tanh, //��������ͣ�Ĭ��ΪTF_Tanh��
			EBP_WC wcFunc = EBP_WC::WC_Convolution //Ȩֵ��Ϸ�ʽ��Ĭ��ΪWC_Convolution��
		);
		//��ĳ��������һ���ǽ������������������������ͨ��ȫ���ӣ�����˽��Զ�Ӧ�������λ�õ�map��һ��һ�����
		//��ʹ������ͨ����=���ͨ�����������ckMetrix���������map����һ����,��Ϊ����ͨ��Ӧ��ʼ�յ������map���������������1x1������γ�DepthWise������
		BpnnStructDef& addOneConvolutionSeparable(
			Uint appendhidelayerIndex, //���ӵ���һ������ı�ű�ʶ(��0��ʼ)
			Uint mapBaseIndex,//���ξ��������Ԫ����Ӧ���ӵ��ϲ�ڵ���ʼ��(��0��ʼ)
			Uint& _in_out_baseLayer_Xsi, //����X��ά��,������ȷ���غ󱣴��µĳߴ�ֵ
			Uint& _in_out_baseLayer_Ysi, //����Y��ά��,������ȷ���غ󱣴��µĳߴ�ֵ
			const ConvolutionKernel& ckMetrix, //�����(���ƽ�������������ͨ������������ȣ����ݣ������磺{ ����ͨ����,filter��,filter��,padding,X����,Y����}
			const WiInitDef& wi, //Ȩֵbi��ʼ������
			const BiInitDef& bi, //��ֵbi��ʼ������
			EBP_TF transFunc = EBP_TF::TF_Tanh, //��������ͣ�Ĭ��ΪTF_Tanh��
			EBP_WC wcFunc = EBP_WC::WC_Convolution //Ȩֵ��Ϸ�ʽ��Ĭ��ΪWC_Convolution��
		);
		//��ĳ��������һ���ػ��������(�ػ���������������Ȩֵ����ֵ��Ȩֵȫ1����ֵȫ0)���ػ�ӳ�䶨�壬���磺{ ����ͨ����,filter��,filter��,X����,Y����}
		BpnnStructDef& addOnePooling(
			Uint appendhidelayerIndex, //���ӵ���һ������ı�ű�ʶ(��0��ʼ)
			Uint mapBaseIndex,//���γػ�������Ԫ����Ӧ���ӵ��ϲ�ڵ���ʼ��(��0��ʼ)
			Uint& _in_out_baseLayer_Xsi, //����X��ά��,������ȷ���غ󱣴��µĳߴ�ֵ
			Uint& _in_out_baseLayer_Ysi, //����Y��ά��,������ȷ���غ󱣴��µĳߴ�ֵ
			const PoolingMap& poolMap,//�ػ�ӳ�䶨�壬���磺{ ����ͨ��,filter��,filter��,X����,Y����}
			EBP_WC wcFunc, //Ȩֵ��Ϸ�ʽ����ΪWC_Average��WC_Max�ȣ�
			EBP_TF transFunc = EBP_TF::TF_Purelin //��������ͣ�Ĭ��ΪTF_Purelin��,ע��ػ���Ĵ��ݺ������������������Ҫ���޸ģ�Ӧ��������y=x���ݺ���
		);
		//��ĳһ��������һ��ȫ���Ӳ�����������������
		BpnnStructDef& addOneFullConnect(
			Uint appendhidelayerIndex, //���ӵ���һ������ı�ű�ʶ(��0��ʼ)
			Uint mapBaseIndex,//����ȫ���Ӳ�����Ԫ����Ӧ���ӵ��ϲ�ڵ���ʼ��(��0��ʼ)
			Uint baseLayer_XYZsi, //����X��ά��,��Ϊ0�����ϲ����е�Ԫ,������ȷ���غ󱣴��µ�ѹ����һά�ĳ���ֵ
			Uint thisTimesNodes,//�������ӵ�ȫ������Ԫ������Ϊ0ʱ������ͬ���ϲ�������Ԫ�ۺϣ�����mapBaseIndex��baseLayer_Xsi��baseLayer_Ysi��baseLayer_DepthSi����
			const WiInitDef& wi,//Ȩֵbi��ʼ������
			const BiInitDef& bi,//��ֵbi��ʼ������				
			EBP_TF transFunc, //��������ͣ�����ΪTF_Sigmoid��TF_Tanh��TF_Purelin�ȣ�	
			EBP_WC wcFunc = EBP_WC::WC_Add, //Ȩֵ��Ϸ�ʽ��Ĭ��ΪWC_Add��
			Bool isNoUpdateFlag = false//ָ���ò��Ȩֵ����ֵ�Ƿ�����,isNoUpdateFlag=true��ʾ�ò��ֽڵ㲻����Ȩֵ����ֵ
		);
		//��ĳһ��������һ�������ݲ�������������ݲ������ϲ�ڵ������ֵ���ݵ���һ�㣬һ��һ���ӣ�Ȩֵȫ1����ֵȫ0���������������transFunc���ʹ����ݶȣ�
		BpnnStructDef& addOneTransmit(
			Uint appendhidelayerIndex, //���ӵ���һ������ı�ű�ʶ(��0��ʼ)
			Uint mapBaseIndex,//���δ��ݲ�����Ԫ����Ӧ���ӵ��ϲ�ڵ���ʼ��(��0��ʼ)
			Uint baseLayer_Xsi, //����X��ά��
			Uint baseLayer_Ysi = 1, //����Y��ά�ȣ���Ϊ1�˻�Ϊһά����
			Uint baseLayer_DepthSi = 1, //�������ά�ȣ���Ϊ1�˻�Ϊ��λƽ��
			EBP_TF transFunc = EBP_TF::TF_Purelin //��������ͣ�Ĭ��ΪTF_Purelin: y=f(x)=x),Ҳ���Ը���������װ�������
		);
		//��ĳһ��������һ�����ϲ���Ԫ����ӽ�ϵĲ�����
		BpnnStructDef& addOneConbinate(
			Uint appendhidelayerIndex, //���ӵ���һ������ı�ű�ʶ(��0��ʼ)
			Uint mapBaseIndex, //������ӽ�ϲ�����Ԫ����Ӧ���ӵ��ϲ�ڵ���ʼ��(��0��ʼ)
			Uint baseLayer_SizeSpan, //������ӽ�ϲ�����Ԫ��ȣ�Ҳ�Ǳ��β����������Ԫ��
			Uint baseLayer_SizeSpanTimes //������ӽ�ϲ���������
		);
		//��ĩβ����һ��Softmax������
		BpnnStructDef& addSoftmaxClassifierToEnd(
			Uint mapBaseIndex,//���δ��ݲ�����Ԫ����Ӧ���ӵ��ϲ�ڵ���ʼ��(��0��ʼ)
			Uint baseLayer_XYZsi, //����X��ά��
			Uint classifiers//�������
		);

		/*
			���ýű�����һ���������磻
			script �ű�����ṹ�������˼��뵽������ṹ�嵱ǰ��Ľű�����
			inputFilterMap �ű��ĵ�һ�������ӳ�䷽ʽ��
			inputFilterMap д����
			SCP_Fc : inputFilterMap = { �ϲ���ʼ������Ԫ���index��������� };
			SCP_Trans: inputFilterMap = {�ϲ���ʼ������Ԫ���index���������};
			SCP_Pool: inputFilterMap = { �ϲ���ʼ������Ԫ���index������mapͨ����deep ������map���wt������map�߶�ht};
			SCP_Conv: inputFilterMap = { �ϲ���ʼ������Ԫ���index������mapͨ����deep ������map���wt������map�߶�ht};
			SCP_ConvSep: inputFilterMap = { �ϲ���ʼ������Ԫ���index������mapͨ����deep ������map���wt������map�߶�ht};
			SCP_Combin: inputFilterMap = { �ϲ���ʼ������Ԫ���index��������� };
			SCP_Softmax: inputFilterMap = { �ϲ���ʼ������Ԫ���index��������� };
		*/
		BpnnStructDef& addScript(
			const BpnnStructScript& script,
			const InputFilterMap& inputFilterMap,
			Bool logoutConstructInfo = false  //�ű���������ṹ��ϸ�����Ϣ
		);
	};

	//���dropoutģʽ����
	class _dll_ DRP:IBpnnBase {
	public:
		Uint startlayer;//����dropout����ʼ��������
		Uint endLayer;//����dropout�Ľ�����������
		Float rate;//����dropout�ļ�����
		DRP(Uint layer = 0, Float rate = 0.5);
		DRP(Uint startlayer, Uint endLayer, Float rate);
		virtual ~DRP();
	};
	typedef vector<DRP> DropoutLayerDef;//ִ��dropout��ı��

	typedef vector<Uint> BnLayerIndexList;//ִ��BatchNomlization�Ĳ����������

	typedef map<string, string> BpnnToolInfo;//������Ϣ�б�
	
	//ȡ��CreativeLus���ߵ������Ϣ�б�
	_dll_ BpnnToolInfo getBpnnToolInfo();

	//����executeByJson�Ľṹ������ִ�к����ڲ����涯̬����Ķ���ָ�룬����������ָ�벻��nullptr
	//��ṹ����ʱ�Զ�������ˣ���Ҫ����ָ����Ч�ԣ��뱸��ָ��ֵ�������ýṹ�ڲ���Ӧ����Ϊnullptr��
	//�ṹ�ڲ��ֶ����壺
	//trainSampSets�������õ����ڱ����ڲ��������������϶����ָ�����������ִ�к������ø�ֵ�Ҳ�Ϊnullptr������󱣴�������BpnnSampSets��
	//testSampSets�������õ����ڱ����ڲ���������ȷ�ʲ��Լ��϶����ָ�����������ִ�к������ø�ֵ�Ҳ�Ϊnullptr������󱣴�������BpnnSampSets��
	//bpnnStructDef:�����õ����ڱ����ڲ��������Զ���ṹ���������ָ�����������ִ�к������ø�ֵ�Ҳ�Ϊnullptr������󱣴�������BpnnStructDef��
	//autoFitCallBack:�����õ����ڱ����ڲ��������Զ�����ѧϰ�ʻص������ָ�����������ִ�к������ø�ֵ�Ҳ�Ϊnullptr������󱣴���һ�������õ�δ֪���ͣ�
	class _dll_ BpnnInterfaceStore :IBpnnBase, public std::map<string, PIBpnnBase> {
	public:
		BpnnInterfaceStore();
		void release();
		virtual ~BpnnInterfaceStore();
	};

	//bp��������
	class _dll_ Bpnn :IBpnnBase {
	protected:
		void* kernel;
		void* extend;
	public:
		
		//�Ͽ�ѵ����չ��������ڷ�ѵ��״̬����ʾ�������ͷ�ѵ����չ�����ռ�õ������ڴ棨һ���ڴ�ռ�ñ�Ԥ��ģ���ں˴�ࣩܶ��������һ����С�ļ����ںˡ�
		//�����ںˣ������������ڼ����Ԥ�����Ҫ���֣��ò��ֽ�����ѵ����ɵ�Ԥ��ģ�ͱ�Ҫ���ݣ�һ�����ڷ�����Ʒ����
		Bpnn& detachExtend();

		//���������dropout���ܣ�dropout�޷�������������²�����W��B������Ԫ�����紿���ݹ��ܻ�ػ����ܵ���Ԫ����Ҳ�޷������������Ԫ��
		//DropoutLayerDef�Ƕ�����Ҫdropout�����Ĳ�ı�ǽṹ���ڲ��Ķ��嵥ԪΪDRP�ṹ;
		//���磺{ DRP(1,0.5), DRP(2,3,0.4), DRP(5,0.6) } ����ʾ1�������0.5��2-3�������0.4��5�������0.6��������㲻��dropout��
		//������rate��ʾ��Ԫ��dropout�ļ������ʣ�ֵ��[0.05,0.95]֮�䣬Ĭ��ȡֵ0.5��ֵ�ڷ�Χ֮��ȫ����0���Ըö���;
		//repeatTrainTimes�ظ����ٸ��������ں����һ��dropout�ṹ,��Ϊ0ʱ��ʾ������dropout��
		Bpnn& setDropout(Uint repeatTrainTimes = 0, const DropoutLayerDef& def = DropoutLayerDef());

		//��������Bn������miniBatchSizeΪ0ʱ������Bn��bnLayerListָ��������Bn�Ĳ����ţ�Ϊ�ձ�ʾȫ���㶼����Bn����
		Bpnn& setBatchNormalization(Uint miniBatchSize = 0, const BnLayerIndexList& bnLayerList = BnLayerIndexList());

		//�������������������������ƽ����Ƿ��࣬�������Ǳ���ģ����ڲ�����������setSampleBatchCounts�������������������
		//һ�㣺�ƽ�����ȫ������ϣ�������õ���������ϣ�
		Bpnn& setNetUseTo(EBP_UT type = EBP_UT::UT_Classify);

		//ȡ���������Ԫ����,������������Ч����0
		Uint neuronCounts() const;
		//ȡ��ĳһ��������Ԫ����,������������Ч����0
		Uint neuronCountsInLayer(Uint lay) const;
		//ȡ�����������ά��
		Uint inputDimension() const;
		//ȡ����������wά��
		Uint outputDimension() const;
		//ȡ������Ĳ���,������������Ч����0
		Uint layerCounts() const;
		//ȡ����������ز���������ܲ���-1,������������Ч����0
		Uint hideLayerCounts() const;

		//����������ѵ��ģʽ�²��ö��̼߳���֧�֣�Ĭ��״̬�ǹرգ������߳�������4��cpu����֧�֣����ó��浥�̷߳��������
		//����percentageOfThreadsStartedΪ[0,1]�����ֵ�����������Ķ��߳�����ռ�ܿ����߳����ı�������Ϊ1ʱȫ�����ã���Ϊ0.8ʱֻ����80%���̣߳���Ϊ0ʱ�رգ�
		//����ǰ���߳�ģ���������У�����percentageOfThreadsStarted = 0�������رն��̹߳����飻����Ҳ���������رն��߳�ģ�飨����ʽ�İ�ȫ�˳���
		Bpnn& setMultiThreadSupport(Float percentageOfThreadsStarted = 0);
		//�򿪺͹ر�GPUӲ�����������GPU�����豸�Ļ�
		Bpnn& setGpuAcceleratedSupport(Bool bOpen = false);

		//��ͼ������ʾ��ǣ���״̬��Ǻ󣬺�����train����ѵ�����̻�ѽ�����ص�ͼ�������У������򲻱������й��������ݣ�
		//��flag��bOpen��ֵtrueʱ���ڲ����ݽ�����գ���֮false�������ݹر���ʾ��ǣ����ٽ�����������
		//��������showGraphParam()��ʾ��������
		Bpnn& openGraphFlag(Bool bOpen = true);

		//��ͼ������ʾ����ʾ��������,�÷���Ӧ����openGraphFlag������train������¼���ݺ�ʹ�ã�
		//��openGraphFlagʱfalseͬ������ʾ��������ʾ����֮ǰ�����ݣ�
		//maxDataToShowָ����ʾ����������������������������ᱻ�ضϣ�ֱ��ɾ��������maxDataToShow = 0 ��ʾ���ض����������⽫��ʾ�����Ѽ�¼����������
		//posX,posY��ʾ���ڳ��ֵ����Ͻ����ꣻ
		Bpnn& showGraphParam(Uint maxDataToShow = 0, Int posX = 1, Int posY = 1);

		//��ͼ������ʾ����ʾ����ṹ��,�÷���Ӧ����openGraphFlag������train������¼���ݺ�ʹ�ã�
		//��openGraphFlagʱfalseͬ������ʾ��������ʾ����֮ǰ�����ݣ�
		//isShowDetailָ���Ƿ���ʾ��ǰ��Ȩֵ״̬��ϸͼ�νṹ��
		//posX,posY��ʾ���ڳ��ֵ����Ͻ����ꣻ
		Bpnn& showGraphNetStruct(Bool isShowDetail = false, Int posX = 1, Int posY = 1);

		//����ǰCorrectRateͼ�δ�������ʾ״̬���򽫴��������һ��ָ����bmpͼ���ļ��У�����δ��ʾ��������С�����ļ��޷������ȶ�������FALSE
		Bool exportGraphCorrectRate(PCStr lpfileName);
		//����ǰErͼ�δ�������ʾ״̬���򽫴��������һ��ָ����bmpͼ���ļ��У�����δ��ʾ��������С�����ļ��޷������ȶ�������FALSE
		Bool exportGraphEr(PCStr lpfileName);
		//����ǰLsͼ�δ�������ʾ״̬���򽫴��������һ��ָ����bmpͼ���ļ��У�����δ��ʾ��������С�����ļ��޷������ȶ�������FALSE
		Bool exportGraphLs(PCStr lpfileName);
		//����ǰMcͼ�δ�������ʾ״̬���򽫴��������һ��ָ����bmpͼ���ļ��У�����δ��ʾ��������С�����ļ��޷������ȶ�������FALSE
		Bool exportGraphMc(PCStr lpfileName);

		//ȡ���ڲ������������ݶԸ���
		Uint getSampleCounts() const;
		//ȡ��ѵ����ɵĲ���
		Uint getRunTimes() const;
		//ȡ��ѵ����ɵ��ۼƲ���
		Uint getRunTimesTutal() const;
		//ȡ�õ�ǰѵ�����ֵ
		Float getEr() const;		
		//ȡ�õ�ǰģ��ѧϰ��
		Float getLs() const;
		Float getLsOld() const;
		//ȡ�õ�ǰģ��ѵ��״̬�Ĺ��ԣ���������Ӱ��ϵ��
		Float getMc() const;
		Float getMcOld() const;
		//ȡ�õ�ǰ����������һ�����ֵ�Ĳ�ֵ
		Float getDEr() const;
		//ȡ�õ�ǰ�趨������
		Float getAccuracy() const;
		//ȡ���趨�ĵ���ѵ��������ִ�е��ܲ���
		Uint getMaxTimes() const;
		//����ȷ������ģʽ��ȡ���ڲ�������Լ���õ���ȷ�ʡ�������ȷ��δ���������ֵ����Ԥ�⣩
		Float getSavedCorrectRate() const;
		//ģ���Ƿ���ܣ�˵������ģ�ͱ��������޷���ȡһЩ�ܱ������ڲ��������ݣ�
		//���磺�޷���ȡģ�ͽṹ���޷���ȡģ�ͽڵ������޷���ȡģ�Ͳ����ֲ�����ȣ�
		Bool isEncrypted() const;

		//����������һ�������ַ���
		Bpnn& setName(PCStr lpName);
		//ȡ�����������ַ���
		PCStr getName() const;

		//�����������ݣ����в�������ΪĬ�ϣ������������ڲ����в������ݣ��������ͷ��ѷ���õ��ڴ棻
		Bpnn& reset();
		//Ĭ�Ϲ��캯��
		Bpnn();
		//����ֵ�����ںˣ����ṹ����չ���֣�
		Bpnn(const Bpnn& bpnn);
		//����ֵ�����ںˣ����ḳֵѵ����չ
		Bpnn& operator=(const Bpnn& bpnn);
		//��׼����
		virtual ~Bpnn();

		//���ò㼰���ؽڵ�����������������setStructure()�������û��Ի���setStructure()��Ϊ׼��
		Bpnn& setLayer(Uint hideLayerNumbers = BPNN_DFT_LAYERNUM, Uint hidePerLayerNumbers = BPNN_DFT_PERLAYERROOTNUM);
		//�����ڲ�������lsѧϰ������ѧϰ�ʣ���mc�������ӣ������ݶ��½���ϵ������Momentum�㷨�Ķ���ϵ������er_accuracy���ȣ�
		//ע�⣺�˴�er_accuracy�����������Ŀ��ֵ���������� ��������Ԥ��ʧ���ʵľ��ȣ�
		//ע�⣺�ú���������buildNet()ǰ����,�ڽ�buildNet()����ý������������µ�����ڵ��У���Ҫ��ʾ����updateGlobleParamToNode()��	
		Bpnn& setParam(Float ls = BPNN_DFT_LEARNSTEP, Float er_accuracy = BPNN_DFT_ER, Float mc = BPNN_DFT_MOMENTUM);
		//�������ִ�д�����Ĭ��ΪBPNN_DFT_MAXRUNTIMES��,��Ϊ0ʱ�����ʱ�����õ�����������������������ܸ�����
		Bpnn& setMaxTimes(Uint iMaxTimes = BPNN_DFT_MAXRUNTIMES);
		//����Adaptive Moment Estimation�Ż���������㷨��һ�ּ���ÿ������������Ӧѧϰ�ʵķ�����
		//�൱�� RMSprop + Momentum�����еĳ���belta1 = 0.9��belta2 = 0.999�ǳ���,����steplȡ[1e-5,0.3],Ĭ��0.001��
		Bpnn& setAdam(Bool open = false, Float stepL = BPNN_DFT_ADAM_STEP);
		
#define BPNN_CBF_AUTOFIT_PARAM Float& ls, Float& mc,const Bpnn& bp //�ص�����
		// ����ѧϰ�ʺͶ���ϵ�������У�ѧϰС��0���������޸ģ�����ϵ����[0,1)֮�ⲻ�����޸ġ�
		typedef void (*PCBAutoFitLsAndMc)(BPNN_CBF_AUTOFIT_PARAM);
		typedef void (Bpnn::* PMCBAutoFitLsAndMc)(BPNN_CBF_AUTOFIT_PARAM);
		// ѧϰ�ʼ�����ϵ�������ص��������ã�pFAutoFit == nullptrʱ�򲻽���ѧϰ�ʺͶ���ϵ��������
		// PCBAutoFitLsAndMc pFAutoFit�ǵ���ѧϰ�ʺͶ���ϵ���Ļص�������
		// ���У�ѧϰ��С��0�����޸ģ�����ϵ����[0,1)֮������޸ġ�
		// Ҫʹ�ð�װ�����ʱ��ʹ�� BPNN_PCB_AUTOFIT_MAKE �����ɻص�ָ�룻
		Bpnn& setAutoFitLsAndMc(PCBAutoFitLsAndMc pFAutoFit = nullptr, PVoid pIns = nullptr);

		//���ü�������ͣ����������µ����磬�����Ḳ��BpnnStructDef���������õļ�����������ʹ�á�
		Bpnn& setTransFunc(EBP_TF iBpTypeHide = EBP_TF::TF_PRelu, EBP_TF iBpTypeOut = EBP_TF::TF_PRelu);
		//������ʧ�������͡�
		Bpnn& setLossFunc(EBP_LS lossId = EBP_LS::LS_MeanSquareLoss);

		//����Ȩֵw����ֵbi��ʼ��ȡֵ�������[A,B]���ʼ��Ϊ��ֵW��
		//Ĭ��Ϊ(-1~1),������A��B�Ĵ�С����
		//��A=Bʱ����wҲ���ʼ��Ϊһ����ֵ��
		//ע�⣬�ú���������buildNet()ǰ���á�
		//��BpnnStructDefģʽ�����ø�ֵֵ��δ��ģʽ������ʼ��˵���Ĳ�����Ч��
		Bpnn& setWbiDefault(Float W);
		Bpnn& setWbiDefault(Float A, Float B);

		//�����Ƿ�����������ѡȡִ�У���һ��ѵ�����ڵ��������������������������ʱ��������ģʱ���ø�ֵû�����壻
		Bpnn& setUseRandSample(Bool _isUse = false);

#define BPNN_CBF_MONITOR_PARAM Int c, Int max, PCStr inf
		typedef void (Bpnn::* PMCBMonitor)(BPNN_CBF_MONITOR_PARAM);//�����Ա����ָ��
		typedef void (*PCBMonitor)(BPNN_CBF_MONITOR_PARAM);//���庯��ָ��

		//һ�����������������״̬�ļ�����࣬����������ʵ�ֻ������й�����ʾ���ܣ�
		class _dll_ MonitorExample :IBpnnBase {
		protected:
			Int m_printCounts;
			Int m_bkChars;
			void* pTick;
		public:
			MonitorExample();
			virtual ~MonitorExample();
			void reset();
			//��ʵ�ֻ������ܵ�����buildNet�ȴ����Խӿڵĺ��������ڴ�ӡ����ִ�й����е��ڲ�״̬��
			//Ĭ��ʵ���ˣ�������ʾ����ʾ��ʾ������ʱ��������ʾ�ȡ�
			// c��ʾ��ǰ��Ϣ���id��max��ʾ����Ϣ����lpInfoΪ��Ϣ�ַ�����
			// ��c < 0 ʱ�򣬴�����һ����ʾ��Ϣ����c��max��>=0ʱ����ʾ������һ��������Ϣ��
			virtual void print(BPNN_CBF_MONITOR_PARAM);
		};

		//�������硣����ǰ����ԭ���磬�ú���������train()ǰ���ã�
		//_pIns��ʾ���Իص��������ڵ������ָ�룬Ϊnullptr��ʾ���������
		//_pCbFun��ʾ���Իص������ĺ���ָ�룬Ҳ������������Ա������ָ�루����ˣ�������� BPNN_PCB_MONITOR_MAKE ������̬ת������
		Bpnn& buildNet(PCBMonitor _pCbFun = nullptr, PVoid _pIns = nullptr);

		//���������ѵ�����̵ĵ�ǰ״̬���ڷ�����Ȩ�̵߳��ã���ʹѵ�������˳�/��ͣ/�����Ȳ�����
		Bpnn& setTrainState(EBP_TPC currentState = TPC_Enable);
		EBP_TPC getTrainState() const;

		//����ѧϰ���Ե�ǰ�ڲ���һ���������������Ϊ���ݣ������ظ����㣩,�κ������˳�����true�����������
		//����������д���δ�������˳�����false��
		//������������ۼƴ������˳�����true��
		//����δ׼���ã��˳�����false��
		//pOutEa,pOutLs,pOutMc��ʾÿ����һ�ε����������Ĳ�������ѹ�뵽vector�У���һ�ν���train�ᱻ��գ���
		//_pIns��ʾ���Իص��������ڵ������ָ�룻
		//_pCbFun��ʾ���Իص������ĺ���ָ�룬Ҳ����ʱ������Ա������ָ�루����ˣ�������� BPNN_PCB_MONITOR_MAKE ������̬ת������
		Bool train(VLF* pOutEa = nullptr, VLF* pOutLs = nullptr, VLF* pOutMc = nullptr, PCBMonitor _pCbFun = nullptr, PVoid _pIns = nullptr);

		//�����ı��ļ���������絽�ļ�����������״̬��
		//binMode = true��ʾ������������ļ������ڴ�������������ı��ļ����ɽ����������
		//�����ö����Ʒ�ʽʱ���ļ�����չ�������޸�Ϊ�̶���".bpnn"������ļ�����ȷ��д��̶��Ķ������������ļ���
		//encrypteMod = trueģ�ͼ���
		Bool writeBpnnToFile(PCStr lpFileFullPathName = nullptr, Bool binMode = true,Bool encrypteMod = false);

		//���ļ��������ݣ��������������絽�����У���������ļ��������򲻸ı����е�����ṹ������
		//ע�⣺���ļ�Ŀ���ļ����ڣ��������������ṹ���ڲ�������Զ���BS���綨�������ȶ��󣻲��Ӱ�Ŀ���ļ����ݼ��غ������µ����磻
		//binMode = true��ʾ��ȡ�������ļ���һ�����ڴ������磬�ı��ļ������������
		//�����ö����Ʒ�ʽʱ��ָ�����ļ�����չ�������Զ��滻Ϊ��ȡ�̶���չ��".bpnn"�ļ�����ȷ����ȡ�̶��Ķ������������ļ���
		Bool readBpnnFormFile(PCStr lpFile, Bool binMode = true);

		//���ⲿ�������������õ��ڲ������ѵ����������
		//ע�⣺Ϊ��Լ�ڴ棬���粢���ᱣ��BpnnSampSets�ṹ����ĸ�����ֻ����һ�ݶ�������ã��ڲ��������ָ�룩����ȷ�����������������ж������Ч�ԣ�
		Bpnn& setSampSets(const BpnnSampSets& tag);

		//����ȡ��һ��Ԥ����������ȷ���ɹ����ʣ�
		//predict Ԥ������ָ�루��Ӧ����ѵ�������Ľṹһֱ��������ͬ������Ϊnullptrʱ��Ԥ������ѡ���ڲ�ѵ������������,
		//nCounst  ȡ�����ж��ٸ�����ɹ��ʼ����Ԥ��������Ϊ0ʱ��ʹ��ȫ����,
		//useRandom ��ѡ������������ʱ���Ƿ����������������ۣ�
		//crtype ��ȷ�����������õķ�������
		Float getCorrectRate(const BpnnSampSets* predict = 0, Uint nCounst = 0, Bool useRandom = false, EBP_CRT crtype = CRT_MeanSquareLoss);

		//��ģ������Ϊ��Ԥ����ȷ����Ϊģ���������۱�׼��ģʽ������ģ��ΪĿ����ʧ����ģʽ��
		//��ȷ�� = ��ȷ�������� / ͳ�Ƶ��������������У�����������ν�ġ���ȷ����ʾ��ʧ����ErС���������ȣ��ƽ���������������Сֵ���ֵ�λ����Ŀ����������ֵλ����ͬ�����ࣩ
		//correctRate Ԥ��������Ŀ����ȷ��[0��1]֮���ֵ,��correctRate=0ʱ�ر���ȷ������ģʽ��
		//predict Ԥ������ָ�루��Ӧ����ѵ�������Ľṹһ����������ͬ������Ϊnullptrʱ��Ԥ������ѡ���ڲ�ѵ������������,
		//nCounst  ȡ�����ж��ٸ�����ɹ��ʼ����Ԥ��������Ϊ0ʱ��ʹ��ȫ����,
		//useRandom ��ѡ������������ʱ���Ƿ����������������ۣ���ȡ��ȫ���������Ը�ֵ��
		//crtype ��ȷ�����������õķ�������
		Bpnn& setCorrectRateEvaluationModel(Float correctRate = 0, const BpnnSampSets* predict = 0, Uint nCounst = 0, Bool useRandom = false, EBP_CRT crtype = CRT_MeanSquareLoss);
		//���ģ���Ƿ�������ȷ��������ѵ���Ƿ������ģ�
		Bool isCorrectRateEvaluationModel() const;

		//����SGD�Ż�������
		//���õ���ѵ��������ͬʱ������ϻع��һ�����������ݸ�����������ѵ��ָ����һ����ǰ�����һ�����򴫵ݣ�
		//nCounts: �� N = ��ѵ��������ʱ
		//nCounts = 0 �� >= N������ѵ���������������õ��������ϵ�ȫ�����ݣ��ƽ����⣩��Ϊһ�����������ݸ�������ʱ��ӦSGD�㷨�е�Batch Gradient Descent��ȫ���������ݶ��½��㷨����
		//nCounts = 1 ��ʾͬһѵ������ʹ��1����������ѵ����ϣ���UseRandomSamp = trueʱ����ӦSGD�㷨�е�Stochastic Gradient Descent�������������ݶ��½��㷨����Ĭ�������һ��Ϊ�����������½�����
		//nCounts > 1 ��ʾͬһѵ������ʹ�ö����������Ϊһ�����������ݸ���>1��С��ȫ��������������������ѵ����ϣ�
		//��ʱ��ӦSGD�㷨�е�Mini-batch Gradient Descent��С�������������ݶ��½��㷨����
		//UseRandomSamp:
		//UseRandomSamp = true��ʾ�� nCounts > 0�� nCounts < N ʱȡ�õ��������Ƿ����������������������ȡ�ģ������ȡ��������ѵ����ϣ�����Ч����ģ�ͷ�����������
		//��nCounts >= �������� ʱUseRandomSamp = trueָ�������ȡ����������Ͻ�û�����壻
		Bpnn& setSampleBatchCounts(Uint nCounts = 1, Bool UseRandomSamp = false);


		//������������������ı�����ˡ����øú�������������������������,����ѧϰ��ɺ�ִ��Ԥ��ʱ�����룬֮�����predict()���ɺ�getOutput(outVt)ȡ�ý��
		//ע�⣺Ϊ��Լ�ڴ棬���粢���ᱣ��inputArray�ṹ����ĸ�����ֻ����һ�ݶ�������ã��ڲ��������ָ�룩����ȷ���ں������������ж������Ч�ԣ�
		Bpnn& setInput(const Float* inputArray, Uint dataDim);
		Bpnn& setInput(const VLF& inputArray);

		//ȡ��ʵ�����,�Զ���ʼ��out_yi,ʧ�ܷ���false��
		Bool getOutput(VLF& out_yi) const;

		//ѵ���õ�ģ�ʹ�����ԣ�����Er��tagVec = nullptrʱ�򣬷���VtFloatMax����
		//inputVec ����Ĳ������ݣ�ά�ȱ����ģ������ά����ȣ����򱨴�
		//tagVec = nullptr Ŀ����ʵ�����ֵΪnullptrʱ�򲻼���Er��
		//_out_resultVec = nullptr ģ����������ָ�룬ֵΪnullptrʱ�����ģ�ͽ����
		//��tagVec��_resultVec��Ϊnullʱ���÷���û�����壨ʲôҲ��������
		//��Ԥ�⺯��ʹ�õ����������������ڶ��߳��п��ܴ������ݻ�����ռ���޷�Ԥ֪����������ڶ��߳���ִ��Ԥ�⣬
		//Ӧ��ʹ��makeIndependentDataBuf()����ר������������ʹ��predictWithIndependentData()���ж���Ԥ�⣻
		Float predict(const VLF& inputVec, VLF* _out_resultVec = nullptr, VLF* tagVec = nullptr);
		Float predict(const Float* inputArray, Uint inputArraySize, VLF* _out_resultVec = nullptr, VLF* tagVec = nullptr);
		//��Ԥ�⺯��ǰ����setInput�����ú�getOutputȡ�ý����
		Bpnn& predict();

		//�����߳��У�����һ�����������������������Ϊ��ǰ�߳��еĶ������㹹��һ��ר���������������������̶߳�ռ����ֹ���߳�ͬʱԤ��ʱ����������ռ���ң�
		//����ʹ��predictWithIndependentData����Ԥ�⣻
		void makeIndependentDataBuf(VLF& yiData) const;
		//ѵ���õ�ģ�ʹ�����ԣ�����Er��tagVec = nullptrʱ�򣬷���VtFloatMax����
		//inputVec ����Ĳ������ݣ�ά�ȱ����ģ������ά����ȣ����򱨴�
		//tagVec = nullptr Ŀ����ʵ�����ֵΪnullptrʱ�򲻼���Er��
		//_out_resultVec = nullptr ģ����������ָ�룬ֵΪnullptrʱ�����ģ�ͽ����
		//��tagVec��_resultVec��Ϊnullʱ���÷���û�����壨ʲôҲ��������
		//��Ԥ�⺯��ʹ��ר��������������Ԥ�⣨���ڶ��̶߳���Ԥ�⣩,����ǰӦ����makeIndependentDataBuf()���������ר����������
		Float predictWithIndependentData(Float* pyiData, const VLF& inputVec, VLF* _out_resultVec = nullptr, VLF* tagVec = nullptr);
		Float predictWithIndependentData(VLF& yiData, const VLF& inputVec, VLF* _out_resultVec = nullptr, VLF* tagVec = nullptr);

		/*
		�����Զ���ĸ���������ṹģʽ����modΪ��ʱ������Զ���ģʽ,���ָ�ʹ��Ĭ�ϵ�ͨ������������ڵ�����ʽ���������磻
		BpnnStructDef�ṹ���壬����Ҫ��������ṹ��
		ע�⣺���粢���ᱣ��BpnnStructDef�ṹ����ĸ�����ֻ����һ�ݶ�������ã���ȷ����buildNet()����ǰ�������Ч�ԣ�
		��buildNet()ǰ���ã�
		BpnnStructDef �ṹ���巶������3�����ز��1������㣬���£�
			BpnnStructDef mod = {
				{},  //һ���յ����ز㶨�壬��������Ķ��������ɶ������ֵ�����лᱻɾ�����˴�����һ��ʾ����ʵ�����壻
				{  //һ�����ز㶨��
					BSB(1,TF_Sigmoid), //һ�����Ԫ���壬���а������ڵ��������ݺ������ͣ��յ������±��ʶ���飻
					BSB(3,TF_Tanh,{}), //һ�����Ԫ���壬���а������ڵ��������ݺ������ͣ��յ������±��ʶ���飻
					BSB(5,TF_Sigmoid)
				},
				{
					BSB(2,TF_Tanh,{0,2,4,6}),  //һ�����Ԫ���壬���а������ڵ��������ݺ������ͣ���ʾ�����������±��ʶ���飻
					BSB(3,TF_Sigmoid,{}),
					BSB(4,TF_Tanh,{1,3,5,7})
				},
				{
					BSB(4,TF_Sigmoid,{}),
					BSB(2,TF_Tanh,{0,1,2,6,7,8}),
					BSB(2,TF_Sigmoid,{})
				},
				//���µ��ж��壬���е��ظ������±꣬�򳬳����±꣬��������ģ���Щ�������ݶ������ڲ��Զ���鲢�����������ֶ���飻
				//ֻ��ע�⣬��������Զ�����������б����ݿ���Ϊ���ˣ���size() == 0����������ڵ�Ҫ�����ϲ�����нڵ㣬����ܺ���Ƶĳ�����Υ���ģ���һ����Ҫע�⣻
				{
					BSB(2,TF_Tanh,{0,0,0,1,2,3,2,4,4,4,4}),
					BSB(2,TF_Sigmoid,{0,0,4,8,6,9,9})
				},
			};
		*/
		/*
		BpnnStructDef mod = {
			// �����һ�㣨�ò�ֱ�ӽ����������ݵĸ���ά�ȣ���
			// �Ե�һ��BSBΪ����������ˣ���������6����ͬ��Ԫ�����ݺ���ΪPRelu������û�ж�����ϲ���������������Ա�����ö��ϲ�ȫ���ӣ�������ҪҲ������ָ�����ӣ���
			// ����������4���飬ÿ�������Ԫ����������ͬ�����Ա��������Ԫ����Ϊ������Ԫ������ӣ�����һ����6+4+6+4=20����Ԫ�����˱��㶨����ϣ�			
			{BSB(6,TF_PRelu),BSB(4,TF_LeakyRelu,{}),BSB(6,TF_PRelu),BSB(4,TF_LeakyRelu,{})},

			// ����ڶ��㣺�ܼ�4�飨4��BSB�ṹ�����Ե�1�飨BSB��Ϊ�������鹲4����Ԫ�����ݺ���ΪTanh��ָ������һ�����ӵı��Ϊ{1,3,5,7,9}����Ԫ
			// ע���ϲ����ӱ���Ǵ�1��ʼ�����ģ�������0���ر�ע�⣻��û��ָ�����ϲ����ӱ�ţ����Ƶ�һ���������������ö��ϲ�ȫ���ӷ�ʽ��
			{BSB(4,TF_Tanh,{1,3,5,7,9}),BSB(6,TF_Sigmoid,{2,4,6,8,10}),BSB(4,TF_Tanh,{11,13,15,17,19}),BSB(6,TF_Sigmoid,{12,14,16,18,20})},

			// �����㣨���������Զ��壬����ͬ�ϣ��˴�ֻ����ʾ��
			{BSB(6,TF_PRelu,{1,2,3,4,5,6}),BSB(8,TF_LeakyRelu,{7,8,9,10,11,12,13,14}),BSB(6,TF_PRelu,{15,16,17,18,19,20})},

			// ���Ĳ㣨���������Զ��壬����ͬ�ϣ��˴�ֻ����ʾ��
			{BSB(4,TF_Tanh,{1,3,5,7,9}),BSB(6,TF_Sigmoid,{2,4,6,8,10}),BSB(4,TF_Tanh,{11,13,15,17,19}),BSB(6,TF_Sigmoid,{12,14,16,18,20})},

			// ���һ�㣨����㣬�ò���뱻���壩���������ǵ����⣬���ά�ȱ���Ϊ4����������㹹�������Ԫ������Ҳ����Ϊ4����BSB�����޹أ�ֻ�ͱ�����Ԫ�����йأ������ϲ�Ҳ����ȫ����
			{BSB(4,TF_PRelu)},

		};
		*/
		Bpnn& setStructure(const BpnnStructDef& mod);

		//���ڲ�����������ṹ��ͼƬ��ʽ������bitmap�ļ�,bUseDetailMode = true�򿪻�ͼϸ�ڣ����ȨֵȨ�ص�����
		Bool exportGraphNetStruct(PCStr outFileName, Bool bUseDetailMode = false);

		//��ʾ�ڲ�Ȩֵ����ֵ�����ݷֲ�������������õĵ���Ԥ�������
		//����������ʾ����һ������id������ƽ̨��ͬ���岻ͬ��windows��Ϊ���ھ����;���������޷���ʾ��
		//ע�⣺�ú���ֻ�Ƕ��ں˲��ֲ���״̬����ʾ�����Ὣ��ʾ�ؼ����󱣴��ڽṹ�ڲ������Ӧ�ñ��淵��ֵ�Ӷ�ʵ�ֶԿؼ��ĺ������ơ�
		//sectionCountsΪ�ֲ�������(��Ϊ0����ʾ��������Ӧƽ�����)
		void* showWbDataDistribution(Int x = 1,Int y = 1,Uint cx = 750,Uint cy = 300, Uint sectionCounts = 0) const;

		//����ڲ�Ȩֵ����ֵ�����ݷֲ���������������޷���ʾ��
		void exportWbDataDistribution(VLF& retRange, VLF& retFreq, Uint sectionCounts = 50, Float* vmin = nullptr, Float* vmax = nullptr) const;
				
		/*
		ͨ��Json�ı���Ϊ���̽ű���ִ�����¼��๤������������ȡ�����������ɽṹ���壬�������磬ִ��ѵ���ȣ���ʾ�����;
		//uesdSamSets�������õ����ڱ����ڲ��������������϶���ָ��ı������������ø�ֵ����Ϊnullptr�������粻��ִ�м����������Ĺ���;
		//�����������ɹ���ض����ط�0��Чָ�룬����һ���ڶ��Ϸ���Ķ��������������ڽ�������Ҫ�ֶ�delete�ͷţ�
		//uesdCheckSamSets�������õ����ڱ����ڲ���������ȷ�ʲ��Լ��϶���ָ��ı������������ø�ֵ����Ϊnullptr�������粻����
		//�������������ڲ�����ȷ���ж�ģʽ�����ʹ��ѵ���������ݼ���Ϊ�������ݼ�;
		//���ò����������ɹ���ض����ط�0��Чָ�룬����һ���ڶ��Ϸ���Ķ��������������ڽ�������Ҫ�ֶ�delete�ͷţ�
		// json��ʽ���£�
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

		//ͨ��Json�ļ���Ϊ���̽ű�ִ�������¼��๤������������ȡ�����������ɽṹ���壬�������磬ִ��ѵ���ȣ���ʾ�����;
		//trainSampSets�������õ����ڱ����ڲ��������������϶���ָ��ı������������ø�ֵ����Ϊnullptr�������粻��ִ�м����������Ĺ���;
		//�����������ɹ���ض����ط�0��Чָ�룬����һ���ڶ��Ϸ���Ķ��������������ڽ�������Ҫ�ֶ�delete�ͷţ�
		//testSampSets�������õ����ڱ����ڲ���������ȷ�ʲ��Լ��϶���ָ��ı������������ø�ֵ����Ϊnullptr�������粻����
		//�������������ڲ�����ȷ���ж�ģʽ�����ʹ��ѵ���������ݼ���Ϊ�������ݼ�;
		//���ò����������ɹ���ض����ط�0��Чָ�룬����һ���ڶ��Ϸ���Ķ��������������ڽ�������Ҫ�ֶ�delete�ͷţ�		
		Bool executeByJsonFile(BpnnInterfaceStore* iStore, PCStr jsonFile);
		static Bool executeByJsonFile(Bpnn& bp, BpnnInterfaceStore* iStore, PCStr jsonFile);
	};

	typedef Bpnn* PBpnn; //BP��������


	//���ɣ���������µĺ�����Ϊ�ص������ģ�����ָ�룬���ڸ��ֿ����Իص��ķ�����
	//����˵����( Ŀ������󣬱���ص������ָ��ı������ƣ�����ص����лص�����ָ��ı������ƣ�Ŀ������ ��Ŀ�����еĻص�������)
#define  BPNN_PCB_MONITOR_MAKE(CallBack_PtParam_Name, Full_Name_Of_CallBack_Func_In_Tag_Class) \
	auto Cb_##tempMonitorCbPointVarName = &(Full_Name_Of_CallBack_Func_In_Tag_Class);\
	Bpnn::PCBMonitor CallBack_PtParam_Name = (*(Bpnn::PCBMonitor*)&Cb_##tempMonitorCbPointVarName); 

	//���ɣ���������µĺ�����Ϊ�ص������ģ�����ָ�룬����setAutoFitLsAndMc()�������θ��ֿ����Իص��ķ�����
	//����˵����( Ŀ������󣬱���ص������ָ��ı������ƣ�����ص����лص�����ָ��ı������ƣ�Ŀ������ ��Ŀ�����еĻص�������)
#define  BPNN_PCB_AUTOFIT_MAKE(CallBack_PtParam_Name, Full_Name_Of_CallBack_Func_In_Tag_Class) \
	auto Cb_##tempAutoFitCbPointVarName = &(Full_Name_Of_CallBack_Func_In_Tag_Class);\
	Bpnn::PCBAutoFitLsAndMc CallBack_PtParam_Name = (*(Bpnn::PCBAutoFitLsAndMc*)&Cb_##tempAutoFitCbPointVarName); 
	
//ȫ�־�̬�ӿ�-------------------------------------------------------------

	//�鿴�����ļ����豸,type = 0�ֶ�ѡ��= 1 ����򻯣�= 2 �����ϸ
	_dll_ void showAcceleratorDeviceAmp(Uint type = 0);
	//ȡ�� (0,x) ����������������rand()%x��
	_dll_ Int  rand_i_0_x(const Int x);
	//	ȡ�� (a,b) ����������������rand()%(b-a)��
	_dll_ Int  rand_i_a_b(const Int a, const Int  b);
	//	ȡ�� [a,b) �ҿ���������������rand()%(b-a)+a��
	_dll_ Int rand_i_ai_b(const Int  a, const Int  b);
	//	ȡ�� [a,b] ����������������rand()%(b-a+1)+a��
	_dll_ Int rand_i_ai_bi(const Int  a, const Int  b);
	//	ȡ�� (a,b] ����������������rand()%(b-a)+a+1��
	_dll_ Int rand_i_a_bi(const Int  a, const Int  b);
	//	ȡ�� (0,1) ֮��ĸ�������rand()/double(RAND_MAX)��
	_dll_ Float rand_f_0_1();
	//	ȡ�� (a,b) ֮��ĸ�������rand()/double(RAND_MAX)*((b) - (a))+(a)��
	_dll_ Float rand_f_a_b(const Float  a, const Float  b);
	typedef vector<Float> RandInitParam;//rand_initData()�����Ͷ�Ӧ�����б�
	//��itTypeָ���ķ�ʽ��ʼ�����ݻ����������б��������ݿ��԰�������������ĳ����Χ�����ض��ֲ���������ȣ�������ο�EBP_IT�оٵ����ͣ���
	//paramLst�ǲ����б����ݲ�ͬ�ĳ�ʼ�������в�ͬ�����壻
	_dll_ void rand_initData(Float* pData, Uint dataSize, EBP_IT itType, const RandInitParam& paramLst = RandInitParam());
	
}
namespace CL = CreativeLus;//���ƿռ䣺�������߼���Ԫ
namespace Cl = CreativeLus;//���ƿռ䣺�������߼���Ԫ
namespace cl = CreativeLus;//���ƿռ䣺�������߼���Ԫ

#endif