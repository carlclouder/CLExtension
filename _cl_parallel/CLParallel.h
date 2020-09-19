
#ifndef __CL_PARALLEL_H__
#define __CL_PARALLEL_H__

#include <functional>
#include <vector>
#include <thread>
#include <map>
#include <deque>
#include <type_traits>

//分片函数，按标号拆分总份数，currentIndex是从0开始的
inline void parallel_helper_getThreadSection(size_t ssi, size_t sectionTotals, size_t currentIndex, size_t& iStart, size_t& iEnd) {
	auto n = ssi / sectionTotals;
	auto yu = ssi % sectionTotals;
	if (currentIndex < yu)
	{
		iStart = (currentIndex)*n + (currentIndex);
		iEnd = iStart + n + 1;
	}
	else {
		iStart = (currentIndex)*n + yu;
		iEnd = iStart + n;
	}
}

//取得硬件执行资源数
inline size_t parallel_helper_hardware_concurrency() {
	static auto _core = size_t(std::thread::hardware_concurrency());
	return _core;
}

#ifndef _Parallel_Helper_ThreadPool_
#define _Parallel_Helper_ThreadPool_
#include <future>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
class CLThreadPoolBase {
public:
	//构造函数,把线程插入线程队列，插入时调用embrace_back()，用匿名函数lambda初始化Thread对象
	CLThreadPoolBase(size_t threads = parallel_helper_hardware_concurrency()) : stop(false) {
		for (size_t i = 0; i < threads; ++i)
			workers.emplace_back(
				[this]
				{
					for (;;)
					{
						// task是一个函数类型，从任务队列接收任务
						std::function<void()> task;
						{
							//给互斥量加锁，锁对象生命周期结束后自动解锁
							std::unique_lock<std::mutex> lock(this->queue_mutex);

							//（1）当匿名函数返回false时才阻塞线程，阻塞时自动释放锁。
							//（2）当匿名函数返回true且受到通知时解阻塞，然后加锁。
							this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

							if (this->stop && this->tasks.empty())
								return;

							//从任务队列取出一个任务
							task = std::move(this->tasks.front());
							this->tasks.pop();
						}                            // 自动解锁
						task();                     // 执行这个任务
					}
				}
				);
	}
	//类模板,任务入队
	//添加新的任务到任务队列
	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>
	{
		// 获取函数返回值类型        
		using return_type = typename std::result_of<F(Args...)>::type;

		// 创建一个指向任务的智能指针
		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);  //加锁
			if (stop)
				throw std::runtime_error("enqueue on stopped CLThreadPoolBase");

			tasks.emplace([task]() { (*task)(); });         //把任务加入队列
		}                                                   //自动解锁
		condition.notify_one();                             //通知条件变量，唤醒一个线程
		return res;
	}
	// 析构函数，删除所有线程
	~CLThreadPoolBase()
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers)
			worker.join();
	}                              //析构函数

private:
	std::vector< std::thread > workers;            //线程队列，每个元素为一个Thread对象
	std::queue< std::function<void()> > tasks;     //任务队列，每个元素为一个函数对象    

	std::mutex queue_mutex;                        //互斥量
	std::condition_variable condition;             //条件变量
	bool stop;                                     //停止
};

//获取全局并行线程池（该线程池初始化硬件可用线程数个线程待命）
inline CLThreadPoolBase& parallel_helper_taskPool_static() {
	static CLThreadPoolBase _globle_thread_pool(parallel_helper_hardware_concurrency());
	return _globle_thread_pool;
}

//获取任务完成对象（装载任务到全局线程池等待队列）
template<class F, class... Args>
auto parallel_helper_task(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>
{
	return parallel_helper_taskPool_static().enqueue(std::forward<F>(f), std::forward<Args>(args)...);
}
//自定义局部线程池包装类（在栈上分配的线程池空间）
template<bool useStaticPool = false>struct _CLTaskPool {
	CLThreadPoolBase& pool;
	template<class F, class... Args>
	auto operator()(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type> {
		return pool.enqueue(std::forward<F>(f), std::forward<Args>(args)...);
	}
	_CLTaskPool(size_t trdCounts = parallel_helper_hardware_concurrency())
		:pool(*(new CLThreadPoolBase(trdCounts))) {}
	~_CLTaskPool() {
		delete& pool;
	}
};
//全局线程池特化包装类
template<>struct _CLTaskPool<true> {
	CLThreadPoolBase& pool = parallel_helper_taskPool_static();
	template<class F, class... Args>
	auto operator()(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>
	{
		return pool.enqueue(std::forward<F>(f), std::forward<Args>(args)...);
	}
};

//全局共享内存池，通过operator()插入可带参数任务
struct CLTaskPoolStatic :_CLTaskPool<true> {};
//局部内存池，通过operator()插入可带参数任务方法
struct CLTaskPool :_CLTaskPool<> {
	//局部内存池，trdCounts表示创建的等待线程数
	CLTaskPool(size_t trdCounts = parallel_helper_hardware_concurrency())
		:_CLTaskPool<>(trdCounts) {}
};
#endif

//控制异常类型
enum class parallel_exception_control_type
{
	pll_ec_null,//未定义
	pll_ec_continue, //控制继续的异常控制类
	pll_ec_break,    //控制本线程退出的异常控制类
	pll_ec_break_all,//控制全部线程组退出的异常控制类
};
//控制继续的异常控制类
class parallel_exception :public std::runtime_error {
	parallel_exception_control_type type;
public:	
	parallel_exception(parallel_exception_control_type _type = parallel_exception_control_type::pll_ec_null) 
		:std::runtime_error("Parallel control exception.") {
		switch (_type)
		{
		case parallel_exception_control_type::pll_ec_continue:
		case parallel_exception_control_type::pll_ec_break:
		case parallel_exception_control_type::pll_ec_break_all:
			type = _type;
			break;
		default:
			type = parallel_exception_control_type::pll_ec_null;
			break;
		}
	};
	parallel_exception_control_type getType() const { return type; }
};

//线程控制函数：继续下一循环（该函数需要在parallel线程体中执行）
inline void parallel_control_continue() {
	throw parallel_exception(parallel_exception_control_type::pll_ec_continue);
};
#define parallel_continue (parallel_control_continue()) //线程控制：继续下一循环
//线程控制函数：本线程退出（该函数需要在parallel线程体中执行）
inline void parallel_control_break() {
	throw parallel_exception(parallel_exception_control_type::pll_ec_break);
};
#define parallel_break (parallel_control_break()) //线程控制：本线程退出
//线程控制函数：本次启动的线程组退出（该函数需要在parallel线程体中执行）
inline void parallel_control_break_all() {
	throw parallel_exception(parallel_exception_control_type::pll_ec_break_all);
};
#define parallel_break_all (parallel_control_break_all()) //线程控制：本次启动的并行线程组退出

//do not use it!!!
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void _parallel_iter(size_t nThreadsCounts, _iter startedIndex, _iter endIndex, _function&& func, _task&& task
	, std::random_access_iterator_tag) {
	auto pfunc = [](bool* bKeep,size_t i, size_t nThreadsCounts, _iter _iis, _iter _iie, _function&& func) {
		size_t is, ie;
		parallel_helper_getThreadSection(_iie - _iis, nThreadsCounts, i, is, ie); //分片
		auto iis = _iis + is, iie = _iis + ie;
	ag:
		try {
			for (; iis != iie; ++iis) {
				if (*bKeep)func(*iis);
				else break;
			}
		}
		catch (const parallel_exception& e) {
			switch (e.getType())
			{
			case parallel_exception_control_type::pll_ec_continue:++iis; goto ag; break;
			case parallel_exception_control_type::pll_ec_break_all:	*bKeep = false;	break;
			default:break;
			}
		}
	};
	bool bKeep = true;
	//组装完成对象序列
	std::vector<std::future<void>> works(nThreadsCounts);
	for (size_t i = 0; i < nThreadsCounts; i++)
		works[i] = task(pfunc,&bKeep, i, nThreadsCounts, startedIndex, endIndex, func);//run	
	//主线程同步
	for (auto& i : works)i.wait();
}
//do not use it!!!
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void _parallel_iter(size_t nThreadsCounts, _iter startedIndex, _iter endIndex, _function&& func, _task&& task
	, std::bidirectional_iterator_tag) {
	auto pfunc = [](bool* bKeep,_iter iis, _iter iie, _function&& func) {
	ag:
		try {
			for (; iis != iie; ++iis) {
				if (*bKeep)func(*iis);
				else break;
			}
		}
		catch (const parallel_exception & e) {
			switch (e.getType())
			{
			case parallel_exception_control_type::pll_ec_continue:++iis; goto ag; break;
			case parallel_exception_control_type::pll_ec_break_all:	*bKeep = false;	break;
			default:break;
			}
		}
	};
	auto pAdv = [](_iter& iter, size_t d) {	while (d--) ++iter; };
	auto pDiff = [](_iter startedIndex, _iter endIndex)-> size_t {size_t si = 0; for (; startedIndex != endIndex; ++startedIndex)++si; return si; };
	size_t is, ie;
	size_t si = pDiff(startedIndex, endIndex);
	endIndex = startedIndex;
	//组装完成对象序列
	std::vector<std::future<void>> works(nThreadsCounts);
	bool bKeep = true;
	for (size_t i = 0; i < nThreadsCounts; i++) {
		parallel_helper_getThreadSection(si, nThreadsCounts, i, is, ie);//分片
		pAdv(endIndex, ie - is);
		works[i] = task(pfunc,&bKeep, startedIndex, endIndex, func);//run
		startedIndex = endIndex;
	}
	//主线程同步
	for (auto& i : works)i.wait();
}

//执行与调度线程同步的nThreadsCounts个线程，并行STL容器，从容器的迭代器startedIndex至endIndex（不包含endIndex）个元素，
//回调函数传参为容器的元素类型的引用；
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void parallel_iter(size_t nThreadsCounts, _iter&& startedIndex, _iter&& endIndex, _function&& func, _task&& task = _task()) {
	_parallel_iter(nThreadsCounts, startedIndex, endIndex, func, task
		, typename std::iterator_traits<typename std::decay<_iter>::type>::iterator_category()
		//, typename _get_fun_args_counts<std::result_of_t<_function>,std::param_type<_function>>::type()
	);
}
//执行与调度线程同步的nThreadsCounts个线程，并行STL容器，从容器的迭代器startedIndex至endIndex（不包含endIndex）个元素，
//回调函数传参为容器的元素类型的引用；
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void parallel(size_t nThreadsCounts, _iter&& startedIndex, _iter&& endIndex, _function&& func, _task&& task = _task()) {
	_parallel_iter(nThreadsCounts, startedIndex, endIndex, func, task
		, typename std::iterator_traits<typename std::decay<_iter>::type>::iterator_category()
		//, typename _get_fun_args_counts<typename std::function<_function>>::type()
	);
}

//执行与调度线程同步的cpu核心数个线程，并行STL容器，从容器的迭代器startedIndex至endIndex（不包含endIndex）个元素，
//回调函数传参为容器的元素类型的引用；
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void parallel_iter(_iter&& startedIndex, _iter&& endIndex, _function&& func, _task&& task = _task()) {
	parallel_iter(parallel_helper_hardware_concurrency(), startedIndex, endIndex, func, task);
}
//执行与调度线程同步的cpu核心数个线程，并行STL容器，从容器的迭代器startedIndex至endIndex（不包含endIndex）个元素，
//回调函数传参为容器的元素类型的引用；
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void parallel(_iter&& startedIndex, _iter&& endIndex, _function&& func, _task&& task = _task()) {
	parallel_iter(parallel_helper_hardware_concurrency(), startedIndex, endIndex, func, task);
}

//执行与调度线程同步的nThreadsCounts个线程，并行容器con的从下标startedIndex至endIndex（不包含endIndex）个元素，
//回调函数传参为容器或数组的元素类型的引用；
template<class _contain, class _function, class _task = CLTaskPoolStatic>
void parallel(size_t nThreadsCounts, _contain& con, size_t startedIndex, size_t endIndex, _function&& func, _task&& task = _task()) {
	auto pfunc = [](bool* bKeep,size_t ni, size_t nThreadsCounts, _contain* con, size_t startedIndex, size_t endIndex, _function&& func) {
		size_t is, ie;
		parallel_helper_getThreadSection(endIndex - startedIndex, nThreadsCounts, ni, is, ie);
		is += startedIndex;
		ie += startedIndex;
		ag:
		try {
			for (; is < ie; ++is) {
				if (*bKeep)func((*con)[is]);
				else break;
			}
		}
		catch (const parallel_exception & e) {
			switch (e.getType())
			{
			case parallel_exception_control_type::pll_ec_continue:++is; goto ag; break;
			case parallel_exception_control_type::pll_ec_break_all:	*bKeep = false;	break;
			default:break;
			}
		}
	};
	bool bKeep = true;
	//组装完成对象序列
	std::vector<std::future<void>> works(nThreadsCounts);
	for (size_t i = 0; i < nThreadsCounts; i++)
		works[i] = task(pfunc, &bKeep, i, nThreadsCounts, &con, startedIndex, endIndex, func);//run
	//主线程同步
	for (auto& i : works)i.wait();
}

//执行与调度线程同步的nThreadsCounts个线程，并行容器con的从下标startedIndex至endIndex（不包含endIndex）个元素，
//回调函数传参为容器或数组的元素类型的引用；
template<class _contain, class _function, class _task = CLTaskPoolStatic>
void parallel(size_t nThreadsCounts, _contain&& con, size_t startedIndex, size_t endIndex, _function&& func, _task&& task = _task()) {
	parallel(nThreadsCounts, con, startedIndex, endIndex, func, task);
}

//执行与调度线程同步的cpu核心数个线程，并行容器con的从下标startedIndex至endIndex（不包含endIndex）个元素，
//回调函数传参为容器或数组的元素类型的引用；
template<class _contain, class _function, class _task = CLTaskPoolStatic>
void parallel(_contain&& con, size_t startedIndex, size_t endIndex, _function&& func, _task&& task = _task()) {
	parallel(parallel_helper_hardware_concurrency(), con, startedIndex, endIndex, func, task);
}

//执行与调度线程同步的nThreadsCounts个线程，线程函数传参为当前线程下标i(一个0开始的索引)和总的任务线程数nThreadsCounts
//返回线程的future队列；
template<class _function,class _task = CLTaskPoolStatic>
auto parallel_proc(size_t nThreadsCounts, _function&& func, _task&& task = _task()) {
	typedef typename result_of<_function(size_t, size_t)>::type key_type;
	//组装完成对象序列
	std::vector<std::future<key_type>> works(nThreadsCounts);
	for (size_t i = 0; i < nThreadsCounts; i++)
		works[i] = task(func, i, nThreadsCounts);//run
	//主线程同步
	for (auto& i : works)i.wait();
	return works;
}

//执行与调度线程同步的cpu核心数个线程；
//线程函数体形式为：R(*)(T1 i,T2 nThreadsCounts);
//传参为：i为当前线程下标(一个0开始的索引)类型为T1，nThreadsCounts为总的任务线程数类型为T2，线程体返回值类型为R；
//函数返回线程的future队列；
template<class _function, class _task = CLTaskPoolStatic>
auto parallel_proc(_function&& func, _task&& task = _task()) {
	return parallel_proc(parallel_helper_hardware_concurrency(), func, task);
}

#endif