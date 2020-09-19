#include "CLParallel.h"

#include "../_cl_common/CLCommon.h"
#include "ppl.h"
using namespace std;
//ʵ����
volatile const CLThreadPoolBase* const _pTrdPool = &parallel_helper_taskPool_static();

//һ��������õĲ��Ժ���,�������뱸��
void testParallel() {
	std::mutex cs;
	const int sii = 50000;
	const int cycle = 5000;
	int con[sii] = { 2 };
	vector<int> con2(sii, 2);
	unordered_map<int, int> con3;
	map<int, int> con4;
	for (int i = 0; i < sii; ++i)con3[i] = 2, con4[i] = 2;
	auto incre = [&](int& i) {i += 1; };

	auto ttt0 = clock();
	for (size_t i = 0; i < cycle; i++)
		for (size_t j = 0; j < sii; j++)
			incre(con[j]);
	auto ttt1 = (clock() - ttt0) / double(CLOCKS_PER_SEC); ttt0 = clock();
	for (size_t i = 0; i < cycle; i++)
		parallel(&con[0],0, sii, incre); //����ָ��
	auto ttt2 = (clock() - ttt0) / double(CLOCKS_PER_SEC);
	//for (size_t i = 0; i < cycle; i++)
	//	parallel(con2,0, con2.size(), incre); //����ָ��
	//parallel(con2,0, sii,incre); //����ָ��
	//parallel(&con[0],0, sii,incre); //����ָ��
	//parallel(con,0, sii,incre); //����ָ��
	//parallel(con,0,sii,incre); //����ָ��
	ttt0 = clock();
	for (size_t i = 0; i < cycle; i++)
		parallel_iter(con2.begin(), con2.end(), incre);
	auto ttt3 = (clock() - ttt0) / double(CLOCKS_PER_SEC); ttt0 = clock();
	for (size_t i = 0; i < cycle; i++)
		concurrency::parallel_for_each(con2.begin(), con2.end(), incre);
	auto ttt6 = (clock() - ttt0) / double(CLOCKS_PER_SEC); ttt0 = clock();
	for (size_t i = 0; i < cycle; i++)
		parallel_iter(con3.begin(), con3.end(), [](pair<const int, int>& i) {i.second += 1; });
	auto ttt4 = (clock() - ttt0) / double(CLOCKS_PER_SEC); ttt0 = clock();
	for (size_t i = 0; i < cycle; i++)
		concurrency::parallel_for_each(con3.begin(), con3.end(), [](pair<const int, int>& i) {i.second += 1; });
	auto ttt5 = (clock() - ttt0) / double(CLOCKS_PER_SEC); ttt0 = clock();
	for (size_t i = 0; i < cycle; i++)
		parallel_iter(con4.begin(), con4.end(), [](pair<const int, int>& i) {i.second += 1; });
	auto ttt7 = (clock() - ttt0) / double(CLOCKS_PER_SEC); ttt0 = clock();
	for (size_t i = 0; i < cycle; i++)
		concurrency::parallel_for_each(con4.begin(), con4.end(), [](pair<const int, int>& i) {i.second += 1; });
	auto ttt8 = (clock() - ttt0) / double(CLOCKS_PER_SEC); ttt0 = clock();
	cout << "\n[ ������ " << sii << " ][ ѭ������ " << cycle << " ] -> ִ��ʱ��ͳ�� ---------------"
		<< endl << "����                   �� " << ttt1 << " ��"
		<< endl << "����                   �� " << ttt2 << " ��"
		<< endl << "vector                 �� " << ttt3 << " ��"
		<< endl << "vector       (Win PPL) �� " << ttt6 << " ��"
		<< endl << "unordered_map          �� " << ttt4 << " ��"
		<< endl << "unordered_map(Win PPL) �� " << ttt5 << " ��"
		<< endl << "map                    �� " << ttt7 << " ��"
		<< endl << "map          (Win PPL) �� " << ttt8 << " ��";

	size_t tCounts2 = 30;
	auto pfun2 = [&cs, tCounts2](int i, int ni) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		cs.lock();
		cout << endl << "Running thread " << i + 1 << " / " << tCounts2;
		cs.unlock();
	};
	auto pfun3 = [&cs](int i, int ni) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		cs.lock();
		cout << endl << "Running thread " << i + 1 << " / " << parallel_helper_hardware_concurrency();
		cs.unlock();
	};
	cout << "\n\n���������֤��ָ���������������--------------";
	parallel_proc(tCounts2, pfun2);
	cout << "\n\n���������֤����������ȷ���������������--------------";
	parallel_proc(pfun3);
	int ci = 0;
	auto ft = parallel_proc([&ci](int trdIndex, int trdIndex2)->std::string {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		ci++;
		char buf[40];
		sprintf_s(buf, "\n%d ��� ci �޸�Ȩ ci = %d", trdIndex, ci); 
		return buf; 
		});
	for (size_t i = 0; i < ft.size(); i++)
		cout << ft[i].get();
	return;
}