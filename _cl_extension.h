#pragma once
#ifndef __CL_EXTENSION_H__
#define __CL_EXTENSION_H__

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN


//以下是第三方库----------------------------------------------------------------------

//引入json库
#include "_jsoncpp/json.h" //jsoncpp库，效率稍微低一些但是稳定
#include "_rapidjson/CLRapidjsonTX.h" //Tencent（腾讯）开源json库，速度快

// 引入全局链接mysql头
#ifndef _CURRENT_MYSQL_VERSION
#define _CURRENT_MYSQL_VERSION  "5.7.24" //注意使用的版本要正确，否则可能出运行时错误
#endif
#include "_mysql/mysql5.7.24/include/mysql.h"
#ifdef WIN32  //windows 平台
#ifdef _WIN64	//x64
#pragma comment(lib,"D:/Documents/Visual Studio 2019/Projects/Quantitative_investment_system/_cl_extension/_mysql/mysql5.7.24/lib/win64/libmysql.lib")
#else			//x86
#pragma comment(lib,"D:/Documents/Visual Studio 2019/Projects/Quantitative_investment_system/_cl_extension/_mysql/mysql5.7.24/lib/win32/libmysql.lib")
#endif
#endif

#ifdef _mysql_h
#ifdef LIBMYSQL_VERSION
static_assert(_CURRENT_MYSQL_VERSION == MYSQL_SERVER_VERSION, "MySQL server version is not match!");
static_assert(_CURRENT_MYSQL_VERSION == LIBMYSQL_VERSION, "MySQL include lib version is not match!");
#endif
#endif


//以下是自定义库----------------------------------------------------------------------
using namespace std;

//引入公共类库
#include "_cl_common/CLCommon.h"

//引入自定义字符串类库
#include "_cl_string/CLString.h"

//引入矩阵运算类库
#include "_cl_matrix/CLMatrix.h"
#include "_cl_matrix/CLMatrixEx.h"

//引入时间处理工具类库
#include "_cl_time/CLTime.h"

//引入bp神经网络处理类库
#ifndef __CL_CREATIVELUS_H__
	#define _USELIB //定义该标志表示Bpnn是采用lib包方式调用
#endif
#include "_cl_neuralNetwork/CreativeLus.h"
#include "_cl_neuralNetwork/CreativeLusExTools.h"

//引入串行化定义类库
#include "_cl_binSerial/CLBinSerial.h"

//引入智能线图形类库
#include "_cl_smartLine/CLSmartLine.h"

//引超动态数组类库
#include "_cl_arrayTemplate/CLArrayTemplate.h"

//引用网页抓取类库
#include "_cl_httpSpider/CLHttpSpider.h"

//引用匹配类库
#include "_cl_matcher/CLMatcher.h"

//引用行为对象模型类库
#include "_cl_behaviorObjectModel/CLBehaviorObjectModel.h"

//引用windows绘图控件类库
#include "_cl_showTool/CLShowTool.h"

//引用进程帮助类库
#include "_cl_processInfo/CLProcessInfo.h"

//引入IOCP模型工具
#include "_cl_iocpServeBase/CLIocpServeBase.h"

//引入对象内存池工具
#include "_cl_objectMemPool/CLMemPool.h"

//引入jsonTask工具
#include "_cl_jsonTask/CLJsonTask.h"

//引入并行支持
#include "_cl_parallel/CLParallel.h"

//引入原子操作支持工具
#include "_cl_atomic/CLAtomic.hpp"

#endif