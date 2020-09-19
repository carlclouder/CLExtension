
#ifndef __CL_PARALLEL_H__
#define __CL_PARALLEL_H__

#include <functional>
#include <vector>
#include <thread>
#include <map>
#include <deque>
#include <type_traits>

//��Ƭ����������Ų���ܷ�����currentIndex�Ǵ�0��ʼ��
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

//ȡ��Ӳ��ִ����Դ��
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
	//���캯��,���̲߳����̶߳��У�����ʱ����embrace_back()������������lambda��ʼ��Thread����
	CLThreadPoolBase(size_t threads = parallel_helper_hardware_concurrency()) : stop(false) {
		for (size_t i = 0; i < threads; ++i)
			workers.emplace_back(
				[this]
				{
					for (;;)
					{
						// task��һ���������ͣ���������н�������
						std::function<void()> task;
						{
							//���������������������������ڽ������Զ�����
							std::unique_lock<std::mutex> lock(this->queue_mutex);

							//��1����������������falseʱ�������̣߳�����ʱ�Զ��ͷ�����
							//��2����������������true���ܵ�֪ͨʱ��������Ȼ�������
							this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

							if (this->stop && this->tasks.empty())
								return;

							//���������ȡ��һ������
							task = std::move(this->tasks.front());
							this->tasks.pop();
						}                            // �Զ�����
						task();                     // ִ���������
					}
				}
				);
	}
	//��ģ��,�������
	//����µ������������
	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>
	{
		// ��ȡ��������ֵ����        
		using return_type = typename std::result_of<F(Args...)>::type;

		// ����һ��ָ�����������ָ��
		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);  //����
			if (stop)
				throw std::runtime_error("enqueue on stopped CLThreadPoolBase");

			tasks.emplace([task]() { (*task)(); });         //������������
		}                                                   //�Զ�����
		condition.notify_one();                             //֪ͨ��������������һ���߳�
		return res;
	}
	// ����������ɾ�������߳�
	~CLThreadPoolBase()
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers)
			worker.join();
	}                              //��������

private:
	std::vector< std::thread > workers;            //�̶߳��У�ÿ��Ԫ��Ϊһ��Thread����
	std::queue< std::function<void()> > tasks;     //������У�ÿ��Ԫ��Ϊһ����������    

	std::mutex queue_mutex;                        //������
	std::condition_variable condition;             //��������
	bool stop;                                     //ֹͣ
};

//��ȡȫ�ֲ����̳߳أ����̳߳س�ʼ��Ӳ�������߳������̴߳�����
inline CLThreadPoolBase& parallel_helper_taskPool_static() {
	static CLThreadPoolBase _globle_thread_pool(parallel_helper_hardware_concurrency());
	return _globle_thread_pool;
}

//��ȡ������ɶ���װ������ȫ���̳߳صȴ����У�
template<class F, class... Args>
auto parallel_helper_task(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>
{
	return parallel_helper_taskPool_static().enqueue(std::forward<F>(f), std::forward<Args>(args)...);
}
//�Զ���ֲ��̳߳ذ�װ�ࣨ��ջ�Ϸ�����̳߳ؿռ䣩
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
//ȫ���̳߳��ػ���װ��
template<>struct _CLTaskPool<true> {
	CLThreadPoolBase& pool = parallel_helper_taskPool_static();
	template<class F, class... Args>
	auto operator()(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>
	{
		return pool.enqueue(std::forward<F>(f), std::forward<Args>(args)...);
	}
};

//ȫ�ֹ����ڴ�أ�ͨ��operator()����ɴ���������
struct CLTaskPoolStatic :_CLTaskPool<true> {};
//�ֲ��ڴ�أ�ͨ��operator()����ɴ��������񷽷�
struct CLTaskPool :_CLTaskPool<> {
	//�ֲ��ڴ�أ�trdCounts��ʾ�����ĵȴ��߳���
	CLTaskPool(size_t trdCounts = parallel_helper_hardware_concurrency())
		:_CLTaskPool<>(trdCounts) {}
};
#endif

//�����쳣����
enum class parallel_exception_control_type
{
	pll_ec_null,//δ����
	pll_ec_continue, //���Ƽ������쳣������
	pll_ec_break,    //���Ʊ��߳��˳����쳣������
	pll_ec_break_all,//����ȫ���߳����˳����쳣������
};
//���Ƽ������쳣������
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

//�߳̿��ƺ�����������һѭ�����ú�����Ҫ��parallel�߳�����ִ�У�
inline void parallel_control_continue() {
	throw parallel_exception(parallel_exception_control_type::pll_ec_continue);
};
#define parallel_continue (parallel_control_continue()) //�߳̿��ƣ�������һѭ��
//�߳̿��ƺ��������߳��˳����ú�����Ҫ��parallel�߳�����ִ�У�
inline void parallel_control_break() {
	throw parallel_exception(parallel_exception_control_type::pll_ec_break);
};
#define parallel_break (parallel_control_break()) //�߳̿��ƣ����߳��˳�
//�߳̿��ƺ����������������߳����˳����ú�����Ҫ��parallel�߳�����ִ�У�
inline void parallel_control_break_all() {
	throw parallel_exception(parallel_exception_control_type::pll_ec_break_all);
};
#define parallel_break_all (parallel_control_break_all()) //�߳̿��ƣ����������Ĳ����߳����˳�

//do not use it!!!
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void _parallel_iter(size_t nThreadsCounts, _iter startedIndex, _iter endIndex, _function&& func, _task&& task
	, std::random_access_iterator_tag) {
	auto pfunc = [](bool* bKeep,size_t i, size_t nThreadsCounts, _iter _iis, _iter _iie, _function&& func) {
		size_t is, ie;
		parallel_helper_getThreadSection(_iie - _iis, nThreadsCounts, i, is, ie); //��Ƭ
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
	//��װ��ɶ�������
	std::vector<std::future<void>> works(nThreadsCounts);
	for (size_t i = 0; i < nThreadsCounts; i++)
		works[i] = task(pfunc,&bKeep, i, nThreadsCounts, startedIndex, endIndex, func);//run	
	//���߳�ͬ��
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
	//��װ��ɶ�������
	std::vector<std::future<void>> works(nThreadsCounts);
	bool bKeep = true;
	for (size_t i = 0; i < nThreadsCounts; i++) {
		parallel_helper_getThreadSection(si, nThreadsCounts, i, is, ie);//��Ƭ
		pAdv(endIndex, ie - is);
		works[i] = task(pfunc,&bKeep, startedIndex, endIndex, func);//run
		startedIndex = endIndex;
	}
	//���߳�ͬ��
	for (auto& i : works)i.wait();
}

//ִ��������߳�ͬ����nThreadsCounts���̣߳�����STL�������������ĵ�����startedIndex��endIndex��������endIndex����Ԫ�أ�
//�ص���������Ϊ������Ԫ�����͵����ã�
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void parallel_iter(size_t nThreadsCounts, _iter&& startedIndex, _iter&& endIndex, _function&& func, _task&& task = _task()) {
	_parallel_iter(nThreadsCounts, startedIndex, endIndex, func, task
		, typename std::iterator_traits<typename std::decay<_iter>::type>::iterator_category()
		//, typename _get_fun_args_counts<std::result_of_t<_function>,std::param_type<_function>>::type()
	);
}
//ִ��������߳�ͬ����nThreadsCounts���̣߳�����STL�������������ĵ�����startedIndex��endIndex��������endIndex����Ԫ�أ�
//�ص���������Ϊ������Ԫ�����͵����ã�
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void parallel(size_t nThreadsCounts, _iter&& startedIndex, _iter&& endIndex, _function&& func, _task&& task = _task()) {
	_parallel_iter(nThreadsCounts, startedIndex, endIndex, func, task
		, typename std::iterator_traits<typename std::decay<_iter>::type>::iterator_category()
		//, typename _get_fun_args_counts<typename std::function<_function>>::type()
	);
}

//ִ��������߳�ͬ����cpu���������̣߳�����STL�������������ĵ�����startedIndex��endIndex��������endIndex����Ԫ�أ�
//�ص���������Ϊ������Ԫ�����͵����ã�
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void parallel_iter(_iter&& startedIndex, _iter&& endIndex, _function&& func, _task&& task = _task()) {
	parallel_iter(parallel_helper_hardware_concurrency(), startedIndex, endIndex, func, task);
}
//ִ��������߳�ͬ����cpu���������̣߳�����STL�������������ĵ�����startedIndex��endIndex��������endIndex����Ԫ�أ�
//�ص���������Ϊ������Ԫ�����͵����ã�
template<class _iter, class _function, class _task = CLTaskPoolStatic>
void parallel(_iter&& startedIndex, _iter&& endIndex, _function&& func, _task&& task = _task()) {
	parallel_iter(parallel_helper_hardware_concurrency(), startedIndex, endIndex, func, task);
}

//ִ��������߳�ͬ����nThreadsCounts���̣߳���������con�Ĵ��±�startedIndex��endIndex��������endIndex����Ԫ�أ�
//�ص���������Ϊ�����������Ԫ�����͵����ã�
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
	//��װ��ɶ�������
	std::vector<std::future<void>> works(nThreadsCounts);
	for (size_t i = 0; i < nThreadsCounts; i++)
		works[i] = task(pfunc, &bKeep, i, nThreadsCounts, &con, startedIndex, endIndex, func);//run
	//���߳�ͬ��
	for (auto& i : works)i.wait();
}

//ִ��������߳�ͬ����nThreadsCounts���̣߳���������con�Ĵ��±�startedIndex��endIndex��������endIndex����Ԫ�أ�
//�ص���������Ϊ�����������Ԫ�����͵����ã�
template<class _contain, class _function, class _task = CLTaskPoolStatic>
void parallel(size_t nThreadsCounts, _contain&& con, size_t startedIndex, size_t endIndex, _function&& func, _task&& task = _task()) {
	parallel(nThreadsCounts, con, startedIndex, endIndex, func, task);
}

//ִ��������߳�ͬ����cpu���������̣߳���������con�Ĵ��±�startedIndex��endIndex��������endIndex����Ԫ�أ�
//�ص���������Ϊ�����������Ԫ�����͵����ã�
template<class _contain, class _function, class _task = CLTaskPoolStatic>
void parallel(_contain&& con, size_t startedIndex, size_t endIndex, _function&& func, _task&& task = _task()) {
	parallel(parallel_helper_hardware_concurrency(), con, startedIndex, endIndex, func, task);
}

//ִ��������߳�ͬ����nThreadsCounts���̣߳��̺߳�������Ϊ��ǰ�߳��±�i(һ��0��ʼ������)���ܵ������߳���nThreadsCounts
//�����̵߳�future���У�
template<class _function,class _task = CLTaskPoolStatic>
auto parallel_proc(size_t nThreadsCounts, _function&& func, _task&& task = _task()) {
	typedef typename result_of<_function(size_t, size_t)>::type key_type;
	//��װ��ɶ�������
	std::vector<std::future<key_type>> works(nThreadsCounts);
	for (size_t i = 0; i < nThreadsCounts; i++)
		works[i] = task(func, i, nThreadsCounts);//run
	//���߳�ͬ��
	for (auto& i : works)i.wait();
	return works;
}

//ִ��������߳�ͬ����cpu���������̣߳�
//�̺߳�������ʽΪ��R(*)(T1 i,T2 nThreadsCounts);
//����Ϊ��iΪ��ǰ�߳��±�(һ��0��ʼ������)����ΪT1��nThreadsCountsΪ�ܵ������߳�������ΪT2���߳��巵��ֵ����ΪR��
//���������̵߳�future���У�
template<class _function, class _task = CLTaskPoolStatic>
auto parallel_proc(_function&& func, _task&& task = _task()) {
	return parallel_proc(parallel_helper_hardware_concurrency(), func, task);
}

#endif