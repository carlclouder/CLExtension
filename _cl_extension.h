#pragma once
#ifndef __CL_EXTENSION_H__
#define __CL_EXTENSION_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN


//�����ǵ�������----------------------------------------------------------------------

//����json��
#include "_jsoncpp/json.h" //jsoncpp�⣬Ч����΢��һЩ�����ȶ�
#include "_rapidjson/CLRapidjsonTX.h" //Tencent����Ѷ����Դjson�⣬�ٶȿ�

//�������Զ����----------------------------------------------------------------------
using namespace std;

//���빫�����
#include "_cl_common/CLCommon.h"

//�����Զ����ַ������
#include "_cl_string/CLString.h"

//��������������
#include "_cl_matrix/CLMatrix.h"

//����ʱ�䴦���������
#include "_cl_time/CLTime.h"

//����bp�����紦�����
#ifndef __CL_CREATIVELUS_H__
	#define _USELIB //����ñ�־��ʾBpnn�ǲ���lib����ʽ����
#endif
#include "_cl_neuralNetwork/CreativeLus.h"
#include "_cl_neuralNetwork/CreativeLusExTools.h"

//���봮�л��������
#include "_cl_binSerial/CLBinSerial.h"

//����������ͼ�����
#include "_cl_smartLine/CLSmartLine.h"

//������̬�������
#include "_cl_arrayTemplate/CLArrayTemplate.h"

//������ҳץȡ���
#include "_cl_httpSpider/CLHttpSpider.h"

//����ƥ�����
#include "_cl_matcher/CLMatcher.h"

//������Ϊ����ģ�����
#include "_cl_behaviorObjectModel/CLBehaviorObjectModel.h"

//����windows��ͼ�ؼ����
#include "_cl_showTool/CLShowTool.h"

//���ý��̰������
#include "_cl_processInfo/CLProcessInfo.h"

//����IOCPģ�͹���
#include"_cl_iocpServeBase/CLIocpServeBase.h"

//��������ڴ�ع���
#include "_cl_objectMemPool/CLMemPool.h"

//����jsonTask����
#include "_cl_jsonTask/CLJsonTask.h"

//���벢��֧��
#include "_cl_parallel/CLParallel.h"

#endif