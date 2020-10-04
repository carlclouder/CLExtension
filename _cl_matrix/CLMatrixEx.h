//DESIGNED BY CAILUO @2020-02-10 
//MINI-SUPPORT @ C++14

#pragma once
#ifndef __CL_MATRIX_EX_H__
#define __CL_MATRIX_EX_H__

#include "CLMatrix.h"

// 矩阵类模板
template <class T1>
class CLMatrixExT:public CLMultiDimData<T1>
{
public:
	using base = CLMultiDimData<T1>;
	using obj = CLMatrixExT<T1>;
	using ref = obj&;
	using remove_ref = obj&&;
	typedef std::vector<T1> MatrixLine;
	typedef std::vector<MatrixLine> Matrix;
	typedef std::initializer_list<MatrixLine> MatrixL;
protected:	
	template<class TList>
	void set(size_t r, size_t c, const TList& m) {
		resize(r, c);
#if CLMAT_USE_SSE > 0
		if (matrixUseSSE) {
			for (size_t i = 0; i < r; ++i)
				::memcpy_sse(&(*this)[i][0], &m[i][0], c);
			return;
		}
#endif
		for (size_t i = 0; i < r; ++i)
			for (size_t j = 0; j < c; ++j)
				(*this)[i][j] = m[i][j];
		return;
	}
	void valid() {//extend line to full
		
	}
	void print_(PCStr lpFlag = nullptr) const { 
		size_t r = rows();
		size_t c = cols();
		const size_t precst = 6;   // 小数点后数据最多位数
		const double v6 = ::pow(10.0, precst), v_6 = ::pow(10.0, -((double)precst)), v_13 = ::pow(10.0, -((double)(precst * 2 + 1)));
		size_t n = 0;              // 数据小数点前最大位数
		size_t pre = 0;            // 小数点后数据位数
		size_t wid = 1;            // 控制字符宽度=n+pre+符号位+小数点位
		for (size_t i = 0; i < r; i++)
		{
			for (size_t j = 0; j < c; j++)
			{
				//计算整数位
				size_t nc = 0;
				double maxV = ::abs(double((*this)[i][j]));
				while (maxV >= 1.0) {
					maxV /= 10.0;
					++nc;
				}

				//计算小数位
				auto xs = ((long long)(::abs(double((*this)[i][j] - ((T1)(long long)((*this)[i][j])))) * v6)) * v_6;
				size_t prec = 0;
				while (xs >= v_6) {
					xs *= 10.0;
					xs = xs - (double)(long long)xs + v_13;
					++prec;
					if (prec >= precst)
						break;
				}
				pre = max(pre, prec);
				auto widc = max(1, nc) + (prec > 0 ? prec + 1 : 0) + 1;
				//更新总位数
				wid = max(widc, wid);
			}
		}
		::_tprintf_s(_T("\nMatrix(%d,%d) %s = \n[\n"), (int)r, (int)c, lpFlag == 0 ? _T("") : lpFlag);
		cout << std::setiosflags(std::ios::fixed);
		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				if (j > 0)
					cout << ",";
				if (::abs(double((*this)[i][j] - ((T1)(long long)((*this)[i][j])))) < v_6) //清除末尾全0
					cout << std::setprecision(0) << std::setw(wid) << (*this)[i][j];
				else {
					//计算本元素实际小数位
					auto xs = ((long long)(::abs(double((*this)[i][j] - ((T1)(long long)((*this)[i][j])))) * v6)) * v_6;
					size_t prec = 0;
					while (xs >= v_6)
					{
						xs *= 10.0;
						xs = xs - (double)(long long)xs + v_13;
						++prec;
						if (prec >= precst)
							break;
					}
					cout << std::setprecision(prec) << std::setw(wid) << (*this)[i][j];
				}
			}
			cout << endl;
		}
		printf_s("]\n");
		cout << std::setprecision(6);
		return;
	}
public:


	//默认构造
	CLMatrixExT() {}
	~CLMatrixExT() {}
	//带初值设定的矩阵构造
	CLMatrixExT(size_t rows, size_t cols, T1 v = 0)
	{
		make(rows, cols, v);
	}
	//带自定义赋值模式的矩阵构造
	CLMatrixExT(size_t rows, size_t cols, const std::function<void(T1 & item, size_t row, size_t col)> && func) 
	{
		make(rows, cols, func);
	}
	//带自定义赋值模式的方矩阵构造
	CLMatrixExT(size_t siRank, const std::function<void(T1 & item, size_t row, size_t col)> & func) 
	{
		make(siRank, siRank, func);
	}
	CLMatrixExT(const obj& m) 
	{
		*this = m;
	}
	CLMatrixExT(obj&& m) noexcept
	{
		*this = std::move(m);
	}
	template <class T2>	CLMatrixExT(const CLMatrixExT<T2>& m)
	{
		*this = m;
	}
	CLMatrixExT(const Matrix& m)
	{
		*this = m;
	}

	CLMatrixExT(const MatrixL& m) 
	{
		*this = m;
	}

	// 返回矩阵对象右值引用（用于不需要拷贝构造或赋值的情况，提高赋值代码性能）
	remove_ref move() {
		return static_cast<remove_ref>(*this);
	}
	// 打开关闭sse指令加速
	static bool setUseSSE(bool open = true) {
		auto setbk = matrixUseSSE;
		matrixUseSSE = open;
		return setbk;
	}
	// 获得sse指令加速开关值
	static bool isUseSSE() {
		return matrixUseSSE;
	}
	// 设置SSE加速的最小的矩阵宽度
	static size_t setUseSSEMinRank(size_t rank = 10) {
		auto matrixUseSSEMinRankbk = matrixUseSSEMinRank;
		matrixUseSSEMinRank = rank;
		return matrixUseSSEMinRankbk;
	}
	ref operator=(const obj& m) {
		//set(m.rows(), m.cols(), m.matrix);
		base::operator=(m);
		return *this;
	}
	ref operator= (obj&& m) noexcept {
		std::swap(base::_data, m.base::_data);
		base::update();
		m.base::update();
		m.clear();
		return *this;
	}
	template <class T2>	ref operator=(const CLMatrixExT<T2>& m) {
		auto r = m.rows(), c = m.cols();
		resize(r, c);
		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				(*this)[i][j] = T1(m[i][j]);
			}
		}
		return *this;
	}
	ref operator=(const Matrix& m) {
		auto row = m.size();
		auto col = 0;
		for (size_t i = 0; i < row; i++)
		{
			if (m[i].size() > col)
				col = m[i].size();
		}
		resize(row, col);
		for (size_t i = 0; i < rows(); i++)
		{
			for (size_t j = 0; j < m[i].size(); j++)
			{
				(*this)[i][j] = m[i][j];
			}
		}
		valid();
		return *this;
	}
	ref operator=(const MatrixL& m) {
		auto row = m.size();
		auto col = 0;
		for (auto ite = m.begin();  ite !=  m.end(); ++ite)
		{
			if (ite->size() > col)
				col = ite->size();
		}
		resize(row, col);
		size_t i = 0;
		for (auto ite = m.begin(); i < rows(); ++ite)
		{
			for (size_t j = 0; j < ite->size(); j++)
			{
				(*this)[i][j] = ite->at(j);
			}
			++i;
		}
		valid();
		return *this;
	}
	// 输出矩阵元素内容到控制台，参数可传入一个标识字符串
	void print(PCStr lpFlag = nullptr) const {
		return print_(lpFlag);
	}
	// 输出矩阵元素内容到控制台，参数可传入一个标识字符串
	ref print(PCStr lpFlag = nullptr) {		
		return print_(lpFlag), *this;
	}
	// 按指定规则构建矩阵,会按照指定规则修改每一项，方法区别于resize()
	ref make(size_t rows, size_t cols, const std::function<void(T1 & item, size_t row, size_t col)> & func = [](T1& v, size_t r, size_t c) { v = 0; }) {
		resize(rows, cols);
		auto r = this->rows(), c = this->cols();
		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j) {
				func((*this)[i][j], i, j);
			}
		}
		return *this;
	}
	// 按指定规则构建矩阵,会按照指定规则修改每一项，方法区别于resize()
	ref make(size_t rows, size_t cols, T1 v = 0) {
		resize(rows, cols);
		if (v == 0)
			return *this;
		auto r = this->rows(), c = this->cols();
		for (size_t i = 0,si = base::size(); i < si; i++)
		{
			base::element(i) = v;
		}
		/*for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j) {
				(*this)[i][j] = v;
			}
		}*/
		return *this;
	}
	// 按指定规则构建方正矩阵（默认状态填充全0）
	ref makeSquare(size_t newRank, const std::function<void(T1 & item, size_t row, size_t col)> & func = [](T1& v, size_t r, size_t c) { v = 0; }) {
		return make(newRank, newRank, func);
	}
	// 按指定值填充构建方正矩阵并（默认状态填充全0）
	ref makeSquare(size_t newRank, T1 v = 0) {
		return make(newRank, newRank, v);
	}
	// 构建单位矩阵
	ref makeE(size_t newRank) {
		return makeSquare(newRank, [](T1& v, size_t r, size_t c) { if (r == c)v = 1; else v = 0; });
	}
	// 改变当前矩阵大小,并清空所有内容,用0填充；维度含0，就清空
	ref resize(size_t rows, size_t cols)
	{	
		//if (rows == 0 || cols == 0) {
		//	//base::clear();
		//	throw invalid_argument("dimension is 0!");
		//}
		//else 
			base::resize(cols, rows);
		return *this;
	}
	//// 取得一个填充或裁剪行后的新矩阵
	//obj rerows(size_t newRows, T1 v = 0) const {
	//	static_assert(false, "has not completed!");
	//	obj m = *this;
	//	m.resize(newRows, cols(), v);
	//	return std::move(m);
	//}
	//// 取得一个填充或裁剪列后的新矩阵
	//obj recols(size_t newCols, T1 v = 0) const {
	//	static_assert(false, "has not completed!");
	//	obj m = *this;
	//	m.resize(rows(), newCols, v);
	//	return std::move(m);
	//}
	//// 取得一个填充或裁剪行列后的新矩阵
	//obj square(size_t newRank, T1 v = 0)const {
	//	static_assert(false, "has not completed!");
	//	obj m = *this;
	//	m.resize(newRank, newRank, v);
	//	return std::move(m);
	//}
	//// 取得一个按最大行或列数填充或裁剪行列后的新矩阵
	//obj squareMax(T1 v = 0)const {
	//	static_assert(false, "has not completed!");
	//	auto r = max(rows(), cols());
	//	obj m = *this;
	//	m.resize(r, r, v);
	//	return std::move(m);
	//}
	//// 取得一个按最小行或列数填充或裁剪行列后的新矩阵
	//obj squareMin(T1 v = 0)const {
	//	static_assert(false, "has not completed!");
	//	auto r = min(rows(), cols());
	//	obj m = *this;
	//	m.resize(r, r, v);
	//	return std::move(m);
	//}
	
	// 将换两行
	ref swap_row(size_t row1, size_t row2)
	{
		if (row1 != row2 && row1 >= 0 &&
			row1 < rows() && row2 >= 0 && row2 < rows())
		{
			auto col = cols();
			for (size_t i = 0, si = col; i < si; ++i)
			{
				std::swap((*this)[row1][i], (*this)[row2][i]);
			}
		}
		return *this;
	}    
	
	// 将换两列数据
	ref swap_col(size_t col1, size_t col2)
	{
		auto col = cols();
		if (col1 != col2 && col1 >= 0 && col1 < col && col2 >= 0 && col2 < col)
		{
			auto ros = rows();
			for (size_t i = 0, si = ros; i < si; ++i)
			{
				std::swap((*this)[i][col1], (*this)[i][col2]);
			}
		}
		return *this;
	}
	// 用目标向量设置某一行，不会改变维度，多余元素省略
	ref setRow(size_t row, const std::vector<T1>& tag) {
		return setRow(row, tag.data(), tag.size());
	}
	ref setRow(size_t row, const T1* tag,size_t tagSize) {
		if (row < rows()) {
			auto r = (*this)[row];
			auto c = cols(); size_t i = 0;
			for (size_t si = min(c, tagSize); i < si; ++i)
				r[i] = tag[i];
			for (; i < c; ++i)
				r[i] = 0;
		}
		return *this;
	}
	// 用目标向量设置某一列，不会改变维度，多余元素省略
	ref setCol(size_t col, const std::vector<T1>& tag) {
		if (col < cols()) {
			auto r = rows(); size_t i = 0;
			for (size_t si = min(r, tag.size()); i < si; ++i)
				(*this)[i][col] = tag[i];
			for (; i < r; ++i)
				(*this)[i][col] = 0;
		}
		return *this;
	}
	// 从一维向量数据初始化矩阵，数据不够则填充0
	ref getFromVector(const std::vector<T1>& tag) {
		auto r = rows(), c = cols();
		size_t tt = 0, stt = tag.size();
		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				if (tt < stt)
					(*this)[i][j] = tag[tt++];
				else
					(*this)[i][j] = 0;
			}
		}
		return *this;
	}
	// 把矩阵拆解到一个一维向量
	std::vector<T1>& setToVector(std::vector<T1>& tag) const {
		tag.resize(rows() * cols());
		setToVector(tag.data(), tag.size());
		return tag;
	}
	// 把矩阵拆解到一个一维向量
	T1* setToVector(T1* buf,size_t nCounts) const {
		auto r = rows(), c = cols();
		for (size_t i = 0; i < r; ++i){
			for (size_t j = 0; j < c; ++j){
				auto ci = c * i + j;
				if (ci >= nCounts)
					return buf;
				buf[ci] = (*this)[i][j];
			}
		}
		return buf;
	}
	// 清理矩阵
	ref clear()
	{
		base::clear();
		return *this;
	}
	// 通过自定义方式修改矩阵中的每项，也可只修改一个子矩阵区域。若只是需要修改一份矩阵的副本请使用operate()方法。
	ref foreach(const std::function<void(T1 & item, size_t iRow, size_t iCol)> & func,
		size_t startRow = 0, size_t startCol = 0, size_t endRow = 0, size_t endCol = 0)
	{
		return ::foreach(*this, func, startRow, startCol, endRow, endCol);
	}
	// 设置矩阵为特定值，也可只设置一个子矩阵区域
	ref foreach(T1 v, size_t startRow = 0, size_t startCol = 0, size_t endRow = 0, size_t endCol = 0) {
		//lambda捕获参数v赋值给每一项
		return this->foreach([v](T1& i, size_t ro, size_t co) {	i = v; },
			startRow, startCol, endRow, endCol);
	}
	// 矩阵置0
	ref zero() { return this->foreach(0); }
	// 拷贝并逐项操作函数，若是需要修改原矩阵请使用foreach()方法。
	// 作用：取得原矩阵的拷贝并按规则处理每一项后的新矩阵（默认状态下只拷贝什么也不做），也可设定范围让规则只作用于一个子区域
	obj operate(const std::function<void(T1 & item, size_t iRow, size_t iCol)> & func = [](T1&, size_t, size_t) {},
		size_t startRow = 0, size_t startCol = 0, size_t endRow = 0, size_t endCol = 0) const {
		obj m = *this;
		return std::move(::foreach(m, func, startRow, startCol, endRow, endCol));
	}
	ref operator--() {
		return operator-=(1);
	}
	ref operator++() {
		return operator+=(1);
	}
	obj operator--(int) {
		obj m = *this;
		operator-=(1);
		return std::move(m);
	}
	obj operator++(int) {
		obj m = *this;
		operator+=(1);
		return std::move(m);
	}
	// 矩阵的行数
	size_t  rows() const { return base::dimlength(base::dim::row); }
	// 矩阵的列数
	size_t  cols() const { return base::dimlength(base::dim::col); }
	// 是否为空
	bool isEmpty() const { return rows() == 0; }
	// 是否为方阵
	bool isSquare() const { return (!(isEmpty()) && (rows() == cols())); }
	// 是否有无效元素
	bool isInvalid(size_t* row = nullptr, size_t* col = nullptr) const {
		for (size_t i = 0; i < rows(); ++i)
		{
			auto lay = (*this)[i];
			for (size_t j = 0; j < cols(); ++j)
			{
				if (_isnan(lay[j]) || isinf(lay[j])) {
					if (row)*row = i;
					if (col)*col = j;
					return true;
				}
			}
		}
		return false;
	}
	// 出现无效元素，打印并抛出异常
	void invalidPrintAndThrow() const {
		if (isInvalid()) {
			this->print(_T("<Invalid Matrix>"));
			throw std::runtime_error("Invalid Matrix");
		}
	}
	
	template <class T2>	obj& operator+=(const CLMatrixExT<T2>& m)
	{
#if CLMAT_USE_SSE > 0
		if (matrixUseSSE)
			return ::matrixAddSelf(*this, m);
#endif // UseSSE
		size_t r = min(rows(), m.rows());
		size_t c = min(cols(), m.cols());
		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				(*this)[i][j] += m[i][j];
			}
		}
		return *this;
	}
	ref operator+=(T1 v)
	{
		size_t r = rows();
		size_t c = cols();
		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				(*this)[i][j] += v;
			}
		}
		return *this;
	}
	template <class T2> obj& operator-=(const CLMatrixExT<T2>& m)
	{
#if CLMAT_USE_SSE > 0
		if (matrixUseSSE)
			return ::matrixSubSelf(*this, m);
#endif // UseSSE
		size_t r = min(rows(), m.rows());
		size_t c = min(cols(), m.cols());

		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				(*this)[i][j] -= m[i][j];
			}
		}

		return *this;
	}
	ref operator-=(T1 v)
	{
		return *this += (-v);
	}
	template <class T2> obj& operator*=(const CLMatrixExT<T2>& m)
	{
		return *this = ::dotMul(*this, m, (ref)obj());
	}
	ref operator*=(T1 v)
	{
		size_t r = rows();
		size_t c = cols();

		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				(*this)[i][j] *= v;
			}
		}
		return *this;
	}
	template <class T2> obj& operator/=(const CLMatrixExT<T2>& m)
	{
		return operator*=(::inv(m, (ref)obj()));
	}
	ref operator/=(T1 v)
	{
		size_t r = rows();
		size_t c = cols();

		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				(*this)[i][j] /= v;
			}
		}
		return *this;
	}
	ref operator%=(const int v)
	{
		size_t r = rows();
		size_t c = cols();

		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				(*this)[i][j] %= v;
			}
		}
		return *this;
	}
	// 矩阵拷贝后每个元素取指数
	obj pow(T1 v) const
	{
		size_t r = rows();
		size_t c = cols();
		obj m(r, c);

		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				m[i][j] = ::pow((*this)[i][j], v);
			}
		}
		return std::move(m);
	}
	// 矩阵拷贝后每个元素做为底数base的指数计算每一项
	obj powSelf(T1 base) const
	{
		size_t r = rows();
		size_t c = cols();
		obj m(r, c);
		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				m[i][j] = ::pow(base, (*this)[i][j]);
			}
		}
		return std::move(m);
	}
	// 矩阵拷贝后每个元素取e为底数的幂
	obj exp() const
	{
		size_t r = rows();
		size_t c = cols();
		obj m(r, c);

		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				m[i][j] = ::exp((*this)[i][j]);
			}
		}
		return std::move(m);
	}
	// 矩阵拷贝后每个元素开根
	obj sqrt() const
	{
		size_t r = rows();
		size_t c = cols();
		obj m(r, c);

		for (size_t i = 0; i < r; ++i)
		{
			for (size_t j = 0; j < c; ++j)
			{
				m[i][j] = ::sqrt((*this)[i][j]);
			}
		}
		return std::move(m);
	}
	// 对矩阵的所有元素求和
	T1 sum() const {
		auto r = rows(), c = cols();
		T1 ret = 0;
		for (size_t i = 0; i < r; ++i)
			for (size_t j = 0; j < c; ++j)
				ret += (*this)[i][j];
		return ret;
	}
	// 对矩阵的每一行分别求和，得到N行1列矩阵，结果矩阵每行保存原矩阵每行元素相加之和
	obj sumRows() const {
		auto r = rows(), c = cols();
		if (r == 0)return obj();
		obj m(r, 1);
		for (size_t i = 0; i < r; ++i)
		{
			T1 sm = 0;
			for (size_t j = 0; j < c; ++j)
			{
				sm += (*this)[i][j];
			}
			m[i][0] = sm;
		}
		return std::move(m);
	}
	// 对矩阵的每一列分别求和，得到1行N列矩阵，结果矩阵每行保存原矩阵每列元素相加之和
	obj sumCols() const {
		auto r = rows(), c = cols();
		if (c == 0)return obj();
		obj m(1, c);
		for (size_t j = 0; j < c; ++j)
		{
			T1 sm = 0;
			for (size_t i = 0; i < r; ++i)
			{
				sm += (*this)[i][j];
			}
			m[0][j] = sm;
		}
		return std::move(m);
	}
	// 矩阵内积。满足左列=右行条件
	template <class T2> obj dotMul(const CLMatrixExT<T2>& rhs) const {
		return std::move(::dotMul(*this, rhs, (ref)obj()));
	}
	// 矩阵逐点相乘，即元素对应相乘。L行向量 X R横向量，左右操作数的列数必须相同；
	// 右操作数行数 = 1：左操作数的每个行向量分别与右操作数的唯一行向量相乘；
	// 右操作数行数 >= 左操作数的行：左操作数的每个行向量分别对应与右操作数的每个行向量相乘；
	template <class T2> obj mul(const CLMatrixExT<T2>& rhs) const {
		return std::move(::mul(*this, rhs, (ref)obj()));
	}
	// 矩阵逐点相乘，即元素对应相乘。L行向量 X R列向量，左右操作数的列数和行数必须相同；（等价于 右操作数是按其转置来处理逐点相乘的。）
	// 右操作数列数 = 1：左操作数的每个行向量分别与右操作数的唯一行列量相乘；
	// 右操作数列数 >= 左操作数的行：左操作数的每个行向量分别对应与右操作数的每个列向量相乘；
	template <class T2> obj mul_T(const CLMatrixExT<T2>& rhs) const {
		return std::move(::mul_T(*this, rhs, (ref)obj()));
	}
	// 矩阵按列逐点相乘，即元素对应相乘。L列向量 X R列向量，左右操作数的行数必须相同；
	// 右操作数列数 = 1：左操作数的每个列向量分别与右操作数的唯一列向量相乘；
	// 右操作数列数 >= 左操作数的列数：左操作数的每个列向量分别对应与右操作数的每个列向量相乘；
	template <class T2> obj mul_V(const CLMatrixExT<T2>& rhs) const {
		return std::move(::mul_V(*this, rhs, (ref)obj()));
	}
	// 矩阵按列逐点相乘，即元素对应相乘。L列向量 X R行向量，左右操作数的列数和行数必须相同；（等价于 右操作数是按其转置来处理逐点相乘的。）
	// 右操作数行数 = 1：左操作数的每个列向量分别与右操作数的唯一行列量相乘；
	// 右操作数行数 >= 左操作数的列：左操作数的每个列向量分别对应与右操作数的每个行向量相乘；
	template <class T2> obj mul_VT(const CLMatrixExT<T2>& rhs) const {
		return std::move(::mul_VT(*this, rhs, (ref)obj()));
	}
	template<class T2> obj conv(
		const CLMatrixExT<T2>& K, //卷积核
		size_t _stepX = 1, //卷积核X移动步长
		size_t _stepY = 1, //卷积核Y移动步长
		size_t padding = 0,//输入map，即本矩阵的边缘填充宽度
		double paddingValue = 0.0 //输入map，即本矩阵边缘填充内所填充的值（该值不一定是0，根据计算需要自由设置）
	) const {
		return std::move(::conv(*this, K, (ref)obj(), _stepX, _stepY, padding, paddingValue));
	}
	// 是否是奇异矩阵
	bool isSingularMatrix() const
	{
		double detA = det();
		if (detA < 1e-15 && detA > -1e-15)
			return true;
		else return false;
	}
	// 求矩阵行列式
	T1 det() const
	{
		return ::det(*this);
	}
	// 求矩阵的子矩阵行列式
	T1 det(size_t start, size_t end) const
	{
		return ::det(*this, start, end);
	}
	// 求矩阵的绝对值矩阵
	obj abs() const
	{
		return std::move(::abs(*this, (ref)obj()));
	}
	//将一个矩阵变换到对角线矩阵，同一行和同一列的值都加到主对角上
	obj diag() const {
		return std::move(::diag(*this, (ref)obj()));
	}
	// 矩阵的最大元素值
	T1 maxElement() const
	{
		return ::maxElement(*this);
	}
	// 矩阵最大元素值及其所在的行和列
	T1 maxElement(size_t& row, size_t& col) const
	{
		return ::maxElement(*this, row, col);
	}
	// 矩阵的最小元素值
	T1 minElement() const
	{
		return ::minElement(*this);
	}
	// 矩阵最小元素值及其所在的行和列
	T1 minElement(size_t& row, size_t& col) const
	{
		return ::minElement(*this, row, col);
	}
	// 矩阵的转置矩阵
	obj T() const
	{
		return std::move(::T(*this, (ref)obj()));
	}
	// 将对象自身转置
	ref makeT() {
		auto r = rows(), c = cols();
		auto mi = min(r, c);
		obj m(c, r);
		for (size_t i = 0; i < mi; ++i)
			for (size_t j = 0; j < i; ++j)
				m[j][i] = (*this)[i][j];
		if (r > c) {
			for (size_t i = mi; i < r; ++i)
				for (size_t j = 0; j < c; ++j)
					m[j][i] = (*this)[i][j];
		}
		else if (r < c) {
			for (size_t i = 0; i < r; ++i)
				for (size_t j = mi; j < c; ++j)
					m[j][i] = (*this)[i][j];
		}
		*this = std::move(m);
		return *this;
	}
	// 矩阵的子矩阵。rb开始行，re结束行，cb开始列，ce结束列。
	obj subMatrix(size_t rb, size_t cb, size_t re, size_t ce) const
	{
		return std::move(::subMatrix(*this, rb, cb, re, ce, (ref)obj()));
	}
	// 矩阵的逆矩阵，要求原矩阵不为空且为方矩阵
	obj inv() const
	{
		return std::move(::inv(*this, (ref)obj()));
	}
	// 计算方阵 M 的 LU 分解,取得增广和矩阵
	obj LU() const
	{
		return std::move(::LU(*this, (ref)obj()));
	}
	// 计算方阵 M 的 LU 分解,使得 M = LU;其中L为对角线元素全为1的下三角阵，U为对角元素依赖M的上三角阵
	// LU相乘后结果可能存在行或列的位置变换，但不改变矩阵原有性质
	bool LU(ref L, ref U) const {
		return ::LU(*this, L, U);
	}
	// 从输入流读取矩阵
	bool readMatrix(istream& in /*= std::cin */)
	{
		::readMatrix(*this,in);
		if (this->rows() > 0 && this->cols() > 0)
			return true;
		else return false;
	}
	// 从输入流读取矩阵
	bool readMatrix(const tstring& file)
	{
		::readMatrix(*this, file);
		if (this->rows() > 0 && this->cols() > 0)
			return true;
		else return false;
	}
	// 从二进制文件load矩阵
	bool loadMatrix(const tstring& file)
	{
		::loadMatrix(*this, file);
		if (this->rows() > 0 && this->cols() > 0)
			return true;
		else return false;
	}
	// 将矩阵输出到指定输出流
	void printMatrix(ostream& out /*= std::_tprintf_s */) const
	{
		::printMatrix(*this, out);
	}
	// 将矩阵输出到指定输出流
	void printMatrix(const tstring& file) const
	{
		::printMatrix(*this, file);
	}
	// 将矩阵数据存为二进制文件 
	void saveMatrix(const tstring& file) const
	{
		::saveMatrix(*this, file);
	}
	//回调函数标准形式，v项的引用，r为行标，c为列标
	typedef void (*PInitMatrix)(T1& v, size_t r, size_t c);
	static void initE(T1& v, size_t r, size_t c) {
		if (r == c) v = T1(1);
		else v = T1(0);
	};
	static void initRand_F_0_1(T1& v, size_t r, size_t c) {
		v = T1(double(rand()) / RAND_MAX);
	};
	static void initRand_F_0_10(T1& v, size_t r, size_t c) {
		v = T1(double(rand()) / RAND_MAX * 10);
	};
	static void initRand_F_f1_1(T1& v, size_t r, size_t c) {
		v = T1(double(rand()) / RAND_MAX * 2 - 1);
	};
	static void initRand_F_f10_10(T1& v, size_t r, size_t c) {
		v = T1(double(rand()) / RAND_MAX * 20 - 10);
	};
	static void initRand_I_10(T1& v, size_t r, size_t c) {
		v = T1(rand() % 10);
	};
	static void initRand_I_100(T1& v, size_t r, size_t c) {
		v = T1(rand() % 100);
	};
	static void initRand_I_f10_10(T1& v, size_t r, size_t c) {
		v = T1(int(rand() % 20) - 10);
	};
	static void initRand_I_f100_100(T1& v, size_t r, size_t c) {
		v = T1(int(rand() % 200) - 100);
	};
	static obj E(size_t rank) {
		return std::move(obj(rank, initE));
	}
};

template <class T1>
size_t max_idx(const CLMatrixExT<T1>& m, size_t k, size_t n)
{
	size_t p = k;
	for (size_t i = k + 1; i < n; ++i)
		if (LxAbs<T1>(m[p][k]) < LxAbs<T1>(m[i][k]))
			p = i;
	return p;
}

// 通过自定义方式修改矩阵中的每项，也可只修改一个子矩阵区域
template<class T1> CLMatrixExT<T1>& foreach(CLMatrixExT<T1>& m, const std::function<void(T1 & item, size_t iRow, size_t iCol)> & func,
	size_t startRow = 0, size_t startCol = 0, size_t endRow = 0, size_t endCol = 0)
{
	size_t i = min(startRow, endRow), j = min(startCol, endCol);
	size_t i2 = max(startRow, endRow) == 0 ? m.rows() : min(max(startRow, endRow), m.rows());
	size_t j2 = max(startCol, endCol) == 0 ? m.cols() : min(max(startCol, endCol), m.cols());
	for (size_t r = i; r < i2; r++)
		for (size_t c = j; c < j2; c++)
			func(m[r][c], r, c);	
	return m;
}
// 矩阵转置
template<class T1, class T2> CLMatrixExT<T2>& T(const CLMatrixExT<T1>& m, CLMatrixExT<T2>& ret)
{
	if (m.isEmpty()) 
		return ret.clear();
	size_t row = m.cols();
	size_t col = m.rows();
	ret.resize(row, col);
	for (size_t i = 0; i < row; ++i)
	{
		for (size_t j = 0; j < col; ++j)
		{
			ret[i][j] = T2(m[j][i]);
		}
	}
	return ret;
}
#ifdef _WINDOWS_
#define _CLMatrixExT_Runtime_Error_Box(err) ::MessageBoxA(nullptr, (err), "CLMatrixExT Runtime Error", MB_ICONERROR);
#else
#define _CLMatrixExT_Runtime_Error_Box(err)
#endif
#define _CLMatrixEx_Runtime_Error(funcName,ReasonString)\
	{std::string name = #funcName;\
	_CLMatrixExT_Runtime_Error_Box(("Error: CLMatrixExT method \" " + name + " \" \n" + #ReasonString).c_str());\
	throw std::runtime_error(("CLMatrixT Runtime Error: " + name).c_str());}

// 计算方阵行列式
template<class T1> T1 det(const CLMatrixExT<T1>& m)
{
	if (m.isEmpty())
	{
		_CLMatrixEx_Runtime_Error(det, matix obj is empty matrix!);
	}
	else if (!m.isSquare()) {
		_CLMatrixEx_Runtime_Error(det, matix obj is not a square matrix!);
	}
	T1 ret = 0;

	CLMatrixExT<T1> N;
	LU(m, N);

	if (N.isEmpty()) return ret;

	ret = 1.0;
	for (size_t i = 0; i < N.cols(); ++i)
	{
		ret *= N[i][i];
	}

	if (isSignRev(&N[N.rows() - 1],N.dimlength(CLMatrixExT<T1>::dim::col)))
	{
		return -ret;
	}

	return ret;
}
// 计算矩阵指定子方阵的行列式 
template<class T1> T1 det(const CLMatrixExT<T1>& m, size_t start, size_t end)
{
	return det(subMatrix(m, start, end, start, end, CLMatrixExT<T1>()));
}
// 计算绝对值
template<class T1, class T2> CLMatrixExT<T2>& abs(const CLMatrixExT<T1>& m, CLMatrixExT<T2>& ret)
{
	if (m.isEmpty())
	{
		ret.clear();
		_CLMatrixEx_Runtime_Error(abs, matix obj is empty matrix!);
	}
	size_t r = m.rows();
	size_t c = m.cols();
	ret.resize(r, c);
	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			T1 t = m[i][j];
			if (t < 0) ret[i][j] = T2(-t);
			else ret[i][j] = T2(t);
		}
	}
	return ret;
}
// 返回矩阵所有元素的最大值
template<class T1> T1 maxElement(const CLMatrixExT<T1>& m)
{
	if (m.isEmpty()) 
		return 0;

	T1 ret = m[0][0];
	size_t r = m.rows();
	size_t c = m.cols();

	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			if (m[i][j] > ret)
				ret = m[i][j];
		}
	}
	return ret;
}
// 计算矩阵最大值，并返回该元素的引用
template<class T1> T1 maxElement(const CLMatrixExT<T1>& m, size_t& row, size_t& col)
{
	if (m.isEmpty()) 
		return 0.;

	T1 ret = m[0][0];
	row = 0;
	col = 0;

	size_t r = m.rows();
	size_t c = m.cols();

	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			if (m[i][j] > ret)
			{
				ret = m[i][j];
				row = i;
				col = j;
			}
		}
	}
	return ret;
}
// 计算矩阵所有元素最小值
template<class T1> T1 minElement(const CLMatrixExT<T1>& m)
{
	if (m.isEmpty())
		return 0;

	T1 ret = m[0][0];
	size_t r = m.rows();
	size_t c = m.cols();

	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			if (m[i][j] < ret) ret = m[i][j];
		}
	}

	return ret;
}
// 计算矩阵最小值，并返回该元素的引用
template<class T1> T1 minElement(const CLMatrixExT<T1>& m, size_t& row, size_t& col)
{
	if (m.isEmpty()) 
		return 0.;

	T1 ret = m[0][0];
	row = 0;
	col = 0;
	size_t r = m.rows();
	size_t c = m.cols();

	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			if (m[i][j] < ret)
			{
				ret = m[i][j];
				row = i;
				col = j;
			}
		}
	}

	return ret;
}
// 取矩阵中指定位置的子矩阵。rb开始行，re结束行，cb开始列，ce结束列。
template<class T1, class T2> CLMatrixExT<T2>& subMatrix(const CLMatrixExT<T1>& m, size_t _rb, size_t _cb, size_t _re, size_t _ce, CLMatrixExT<T2>& ret)
{
	if (m.isEmpty()) return ret.clear();
	
	auto rb = min(min(_rb, _re), m.rows());
	auto re = min(max(_rb, _re)+1, m.rows());
	auto cb = min(min(_cb, _ce), m.cols());
	auto ce = min(max(_cb, _ce)+1, m.cols());

	if (rb == re || cb == ce) return ret.clear();

	ret.resize(re - rb, ce - cb);

	for (size_t i = rb; i < re; ++i)
	{
		for (size_t j = cb; j < ce; ++j)
		{
			ret[i - rb][j - cb] = T2(m[i][j]);
		}
	}

	return ret;
}
// 计算逆矩阵
template<class T1, class T2> CLMatrixExT<T2>& inv(const CLMatrixExT<T1>& m, CLMatrixExT<T2>& ret)
{
	return LUP_Inverse(m, ret);
}
// 计算方阵 M 的 LU 分解,取得增广和矩阵
template<class T1, class T2> CLMatrixExT<T2>& LU(const CLMatrixExT<T1>& m, CLMatrixExT<T2>& ret)
{
	if (m.isEmpty())
	{
		ret.clear();
		_CLMatrixEx_Runtime_Error(LU, matix obj is empty matrix!);
	}
	else if (!m.isSquare()) {
		ret.clear();
		_CLMatrixEx_Runtime_Error(LU, matix obj is not a square matrix!);
	}

	size_t n = m.rows();
	ret.resize(n + 1, n);

	for (size_t i = 0; i < n; ++i)
	{
		ret[n][i] = -1;
	}

	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = 0; j < n; ++j)
		{
			ret[i][j] = T2(m[i][j]);
		}
	}

	for (size_t k = 0; k < n - 1; ++k)
	{
		size_t p = max_idx(ret, k, n);
		if (p != k)              // 进行行交换
		{
			ret.swap_row(k, p);
			ret[n][k] = T2(p); // 记录将换信息
		}

		if (ret[k][k] == 0)
		{
			cout << endl << "[Runtime error]: Matrix is singular, unable to calculate inverse!" << endl;
			return ret.clear();
		}

		for (size_t i = k + 1; i < n; ++i)
		{
			ret[i][k] /= ret[k][k];
			for (size_t j = k + 1; j < n; ++j)
			{
				ret[i][j] -= ret[i][k] * ret[k][j];
			}
		}
	}

	return ret;
}
// 从输入流读取矩阵
template<class T1> CLMatrixExT<T1>& readMatrix(CLMatrixExT<T1>& M, istream& in = std::cin)
{
	M.clear();
	string str;
	T1 b;
	//CLMatrixExT<T1>::MatrixLine v;
	size_t i = 0, r = 0, c = 0;
	while (getline(in, str))
	{
		for (string::size_type i = 0; i < str.size(); ++i)
		{
			if (str[i] == ',' || str[i] == ';')
			{
				str[i] = ' ';
			}
			else if (str[i] != '.' && (str[i] < '0' || str[i] > '9')
				&& str[i] != ' ' && str[i] != '\t' && str[i] != '-')
			{
				M.clear();
				return M;
			}
		}
		istringstream sstream(str);
		if (++i == 1) {
			sstream >> r >> c;
			M.make(r, c, 0);
			r = M.rows(), c = M.cols();
			continue;
		}
		if (i - 2 < r) {
			for (size_t j = 0; j < c; ++j)
			{
				if (sstream >> b)
					M[i - 2][j] = b;
			}
		}
	}
	return M;
}               // 从指定输入流读入矩阵
// 从文本文件读入矩阵
template<class T1> CLMatrixExT<T1>& readMatrix(CLMatrixExT<T1>& M, const tstring& file)
{
	ifstream fin(file.c_str());
	if (!fin)
	{
		return M.clear();
	}
	readMatrix(M, (istream&)fin);
	fin.close();
	return M;
}                          // 从文本文件读入矩阵
// 从二进制文件load矩阵
template<class T1> CLMatrixExT<T1>& loadMatrix(CLMatrixExT<T1>& m, const tstring& file)
{
	ifstream fin(file.c_str(), std::ios_base::in | std::ios::binary);
	if (!fin) return m.clear();

	char Flag[14];
	fin.read((char*)&Flag, sizeof(Flag));

	string str(Flag);
	if (str != "CLMATRIX_DATA")
	{
		return m.clear();
	}

	int r, c;
	fin.read((char*)&r, sizeof(r));
	fin.read((char*)&c, sizeof(c));

	if (r <= 0 || c <= 0) return m.clear();

	m.resize(r, c);
	double t;

	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			fin.read((char*)&t, sizeof(t));
			m[i][j] = T1(t);
		}
	}

	return m;
}                          // 从二进制文件读取矩阵
// 将矩阵输出到指定输出流
template<class T1> void  printMatrix(const CLMatrixExT<T1>& m, ostream& out = std::cout)
{
	size_t r = m.rows();
	size_t c = m.cols();

	size_t n = 0;              // 数据小数点前最大位数
	double ma = (double)::maxElement(m);
	double mi = (double)::minElement(m);

	double maxV = max(::abs(ma), ::abs(mi));
	while (maxV >= 1.0)
	{
		maxV /= 10;
		++n;
	}
	if (n == 0) n = 1;    // 如果最大数绝对值小于1，这小数点前位数为1，为数字0
	size_t pre = 6;            // 小数点后数据位数
	size_t wid = n + pre + 3;  // 控制字符宽度=n+pre+符号位+小数点位

	out << std::setiosflags(std::ios::fixed);
	out << std::setw(wid) << r << std::setw(wid) << c << endl;
	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			if(::abs(double(m[i][j] - ((T1)(long long)(m[i][j])))) < 1e-6)
				out << std::setprecision(0) << std::setw(wid)  <<  m[i][j];
			else 
				out << std::setprecision(pre) << std::setw(wid) <<  m[i][j];
		}
		out << endl;
	}
	out << std::setprecision(6);
} 
// 将矩阵打印到指定文件 
template<class T1> void  printMatrix(const CLMatrixExT<T1>& m, const tstring& file)
{
	ofstream fout(file.c_str());
	if (!fout) return;

	printMatrix(m, fout);
	fout.close();
}              
// 将矩阵数据存为二进制文件 
template<class T1> void  saveMatrix(const CLMatrixExT<T1>& m, const tstring& file)
{
	if (m.isEmpty()) 
		_CLMatrixEx_Runtime_Error(saveMatrix, matix obj is empty matrix!);

	ofstream fout(file.c_str(), std::ios_base::out | std::ios::binary);
	if (!fout) return;

	int r = m.rows();
	int c = m.cols();
	char Flag[14] = "CLMATRIX_DATA";
	fout.write((char*)&Flag, sizeof(Flag));
	fout.write((char*)&r, sizeof(r));
	fout.write((char*)&c, sizeof(c));

	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			double t = m[i][j];
			fout.write((char*)&t, sizeof(t));
		}
	}

	fout.close();
}                 // 将矩阵保存为二进制文件

template<class T1, class T2>
bool  operator==(const CLMatrixExT<T1>& lhs, const CLMatrixExT<T2>& rhs)
{
	auto r = lhs.rows(), c = lhs.cols();
	if (r != rhs.rows() || c != rhs.cols())
	{
		return false;
	}

	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			if (lhs[i][j] != T1(rhs[i][j]))
			{
				return false;
			}
		}
	}

	return true;
}
template<class T1>
bool  operator==(const CLMatrixExT<T1>& lhs, T1 v)
{
	auto r = lhs.rows(), c = lhs.cols();
	if (r == 0 || c == 0)
	{
		return false;
	}
	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			if (lhs[i][j] != T1(v))
			{
				return false;
			}
		}
	}
	return true;
}
template<class T1>
bool  operator==(T1 v, const CLMatrixExT<T1>& lhs)
{
	return lhs == v;
}
template<class T1, class T2>
bool  operator!=(const CLMatrixExT<T1>& lhs, const CLMatrixExT<T2>& rhs)
{
	return !(lhs == rhs);
}
template<class T1>
bool  operator!=(const CLMatrixExT<T1>& lhs, T1 v)
{
	return !(lhs == v);
}
template<class T1>
bool  operator!=(T1 v, const CLMatrixExT<T1>& lhs)
{
	return !(lhs == v);
}
template<class T1, class T2>
CLMatrixExT<T1> operator+(const  CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs)
{
	auto m = lhs;
	m += rhs;
	return std::move(m);
}
template<class T1>
CLMatrixExT<T1> operator+(const  CLMatrixExT<T1>& lhs, T1 v)
{
	auto m = lhs;
	m += v;
	return std::move(m);
}
template<class T1>
CLMatrixExT<T1> operator+(T1 v, const  CLMatrixExT<T1>& lhs)
{
	auto m = lhs;
	m += v;
	return std::move(m);
}
template<class T1, class T2>
CLMatrixExT<T1> operator-(const  CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs)
{
	auto m = lhs;
	m -= rhs;
	return std::move(m);
}
template<class T1>
CLMatrixExT<T1> operator-(const  CLMatrixExT<T1>& lhs, T1 v)
{
	auto m = lhs;
	m -= v;
	return std::move(m);
}
template<class T1>
CLMatrixExT<T1> operator-(T1 v, const  CLMatrixExT<T1>& lhs)
{
	return std::move(lhs * (-1) + v);
}


template<class T1, class T2>  CLMatrixExT<T1>& matrixAddSelf(CLMatrixExT<T1>& m, const CLMatrixExT<T2>& rhs)
{
	size_t r = min(m.rows(), rhs.rows());
	size_t c = min(m.cols(), rhs.cols());
	for (size_t i = 0; i < r; ++i)
		for (size_t j = 0; j < c; ++j)
			m[i][j] += rhs[i][j];
	return m;
}
template<>inline  CLMatrixExT<float>& matrixAddSelf(CLMatrixExT<float>& m, const CLMatrixExT<float>& rhs)
{
	size_t r = min(m.rows(), rhs.rows());
	size_t c = min(m.cols(), rhs.cols());
	for (size_t i = 0; i < r; ++i)
		lineAdd_sse(&m[i][0], &rhs[i][0], (int)c, &m[i][0]);
	return m;
}
template<>inline  CLMatrixExT<double>& matrixAddSelf(CLMatrixExT<double>& m, const CLMatrixExT<double>& rhs)
{
	size_t r = min(m.rows(), rhs.rows());
	size_t c = min(m.cols(), rhs.cols());
	for (size_t i = 0; i < r; ++i)
		lineAdd_sse(&m[i][0], &rhs[i][0], (int)c, &m[i][0]);
	return m;
}

template<class T1, class T2>  CLMatrixExT<T1>& matrixSubSelf(CLMatrixExT<T1>& m, const CLMatrixExT<T2>& rhs)
{
	size_t r = min(m.rows(), rhs.rows());
	size_t c = min(m.cols(), rhs.cols());
	for (size_t i = 0; i < r; ++i)
		for (size_t j = 0; j < c; ++j)
			m[i][j] -= rhs[i][j];
	return m;
}
template<>inline  CLMatrixExT<float>& matrixSubSelf(CLMatrixExT<float>& m, const CLMatrixExT<float>& rhs)
{
	size_t r = min(m.rows(), rhs.rows());
	size_t c = min(m.cols(), rhs.cols());
	for (size_t i = 0; i < r; ++i)
		lineSub_sse(&m[i][0], &rhs[i][0], (int)c, &m[i][0]);
	return m;
}
template<>inline  CLMatrixExT<double>& matrixSubSelf(CLMatrixExT<double>& m, const CLMatrixExT<double>& rhs)
{
	size_t r = min(m.rows(), rhs.rows());
	size_t c = min(m.cols(), rhs.cols());
	for (size_t i = 0; i < r; ++i)
		lineSub_sse(&m[i][0], &rhs[i][0], (int)c, &m[i][0]);
	return m;
}

template<class T1, class T2, class T3>
CLMatrixExT<T3>& _dotMul_sse(const CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs, CLMatrixExT<T3>& m)
{
	auto I = lhs.rows(), K = lhs.cols(), J = rhs.cols();
	CLMatrixExT<double> _b, a = lhs;
	m.resize(I, J);
	::T(rhs, _b);
#if CLMAT_USE_CXX_PPL > 0
	auto total = I * J;
	if (total >= DOTMUL_RANK_LIMIT * DOTMUL_RANK_LIMIT) {
		auto tsi = total / DOTMUL_BLOCKS + 1;
		parallel_for<size_t>(0, tsi,
			[&J, &K, &m, &a, &_b, &total](size_t idt) {
				size_t idx = idt * DOTMUL_BLOCKS;
				size_t ide = min(idx + DOTMUL_BLOCKS, total);
				for (; idx < ide; ++idx) {
					size_t i = idx / J;
					size_t j = idx % J;
					m[i][j] = T3(lineDotMul_sse(&a[i][0], &_b[j][0], (int)K));
				}
			});
		return m;
	}
#endif
	for (size_t i = 0; i < I; ++i) {
		for (size_t j = 0; j < J; ++j) {
			m[i][j] = T3(lineDotMul_sse(&a[i][0], &_b[j][0], (int)K));
		}
	}
	return m;
}
template<class T2, class T3>
CLMatrixExT<T3>& _dotMul_sse(const CLMatrixExT<double>& lhs, const  CLMatrixExT<T2>& rhs, CLMatrixExT<T3>& m)
{
	auto I = lhs.rows(), K = lhs.cols(), J = rhs.cols();
	m.resize(I, J);
	CLMatrixExT<double> _b; ::T(rhs, _b);
#if CLMAT_USE_CXX_PPL > 0
	auto total = I * J;
	if (total >= DOTMUL_RANK_LIMIT * DOTMUL_RANK_LIMIT) {
		auto tsi = total / DOTMUL_BLOCKS + 1;
		parallel_for<size_t>(0, tsi,
			[&J, &K, &m, &lhs, &_b, &total](size_t idt) {
				size_t idx = idt * DOTMUL_BLOCKS;
				size_t ide = min(idx + DOTMUL_BLOCKS, total);
				for (; idx < ide; ++idx) {
					size_t i = idx / J;
					size_t j = idx % J;
					m[i][j] = T3(lineDotMul_sse(&lhs[i][0], &_b[j][0], (int)K));
				}
			});
		return m;
	}
#endif
	for (size_t i = 0; i < I; ++i) {
		for (size_t j = 0; j < J; ++j) {
			m[i][j] = T3(lineDotMul_sse(&lhs[i][0], &_b[j][0], (int)K));
		}
	}
	return m;
}
template<class T2, class T3>
CLMatrixExT<T3>& _dotMul_sse(const CLMatrixExT<float>& lhs, const  CLMatrixExT<T2>& rhs, CLMatrixExT<T3>& m)
{
	auto I = lhs.rows(), K = lhs.cols(), J = rhs.cols();
	m.resize(I, J);
	CLMatrixExT<float> _b; ::T(rhs, _b);
#if CLMAT_USE_CXX_PPL > 0
	auto total = I * J;	
	if (total >= DOTMUL_RANK_LIMIT * DOTMUL_RANK_LIMIT) {
		auto tsi = total / DOTMUL_BLOCKS + 1;
		parallel_for<size_t>(0, tsi,
			[&J, &K, &m, &lhs, &_b, &total](size_t idt) {
				size_t idx = idt * DOTMUL_BLOCKS;
				size_t ide = min(idx + DOTMUL_BLOCKS, total);
				for (; idx < ide;++idx) {
					size_t i = idx / J;
					size_t j = idx % J;
					m[i][j] = T3(lineDotMul_sse(&lhs[i][0], &_b[j][0], (int)K));
				}
		});
		return m;
	}
#endif
	for (size_t i = 0; i < I; ++i) {
		for (size_t j = 0; j < J; ++j) {
			m[i][j] = T3(lineDotMul_sse(&lhs[i][0], &_b[j][0], (int)K));
		}
	}
	return m;
}
template<class T1, class T2, class T3>
CLMatrixExT<T3>& _mul_sse(const  CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs, CLMatrixExT<T3>& m) {
	auto r1 = lhs.rows(), c1 = lhs.cols();
	m.resize(r1, c1);
	if (rhs.rows() == 1)
		for (size_t i = 0; i < r1; ++i)
			for (size_t j = 0; j < c1; ++j)
				m[i][j] = T3(lhs[i][j] * rhs[0][j]);
	else
		for (size_t i = 0; i < r1; ++i)
			for (size_t j = 0; j < c1; ++j)
				m[i][j] = T3(lhs[i][j] * rhs[i][j]);
	return m;
}
template<>inline CLMatrixExT<float>& _mul_sse(const  CLMatrixExT<float>& lhs, const  CLMatrixExT<float>& rhs, CLMatrixExT<float>& m) {
	auto r1 = lhs.rows(), c1 = lhs.cols();
	m.resize(r1, c1);
	if (rhs.rows() == 1)
		for (size_t i = 0; i < r1; i++)
			lineMul_sse(&lhs[i][0], &rhs[0][0], (int)c1, &m[i][0]);
	else
		for (size_t i = 0; i < r1; i++)
			lineMul_sse(&lhs[i][0], &rhs[i][0], (int)c1, &m[i][0]);
	return m;
}
template<>inline CLMatrixExT<double>& _mul_sse(const  CLMatrixExT<double>& lhs, const  CLMatrixExT<double>& rhs, CLMatrixExT<double>& m) {
	auto r1 = lhs.rows(), c1 = lhs.cols();
	m.resize(r1, c1);
	if (rhs.rows() == 1)
		for (size_t i = 0; i < r1; i++)
			lineMul_sse(&lhs[i][0], &rhs[0][0], (int)c1, &m[i][0]);
	else
		for (size_t i = 0; i < r1; i++)
			lineMul_sse(&lhs[i][0], &rhs[i][0], (int)c1, &m[i][0]);
	return m;
}
template<class T1, class T2, class T3>
CLMatrixExT<T3>& _mul_T_sse(const  CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs, CLMatrixExT<T3>& m) {
	auto r1 = lhs.rows(), c1 = lhs.cols();
	m.resize(r1, c1);
	if (rhs.rows() == 1)
		for (size_t i = 0; i < r1; ++i)
			for (size_t j = 0; j < c1; ++j)
				m[i][j] = T3(lhs[i][j] * rhs[j][0]);
	else
		for (size_t i = 0; i < r1; ++i)
			for (size_t j = 0; j < c1; ++j)
				m[i][j] = T3(lhs[i][j] * rhs[j][i]);
	return m;
}
template<>inline CLMatrixExT<float>& _mul_T_sse(const  CLMatrixExT<float>& lhs, const  CLMatrixExT<float>& rhs, CLMatrixExT<float>& m) {
	auto r1 = lhs.rows(), c1 = lhs.cols();
	m.resize(r1, c1);
	CLMatrixExT<float> b;
	::T(rhs, b);
	if (b.rows() == 1)
		for (size_t i = 0; i < r1; i++)
			lineMul_sse(&lhs[i][0], &b[0][0], (int)c1, &m[i][0]);
	else
		for (size_t i = 0; i < r1; i++)
			lineMul_sse(&lhs[i][0], &b[i][0], (int)c1, &m[i][0]);
	return m;
}
template<>inline CLMatrixExT<double>& _mul_T_sse(const  CLMatrixExT<double>& lhs, const  CLMatrixExT<double>& rhs, CLMatrixExT<double>& m) {
	auto r1 = lhs.rows(), c1 = lhs.cols();
	m.resize(r1, c1);
	CLMatrixExT<double> b;
	::T(rhs, b);
	if (b.rows() == 1)
		for (size_t i = 0; i < r1; i++)
			lineMul_sse(&lhs[i][0], &b[0][0], (int)c1, &m[i][0]);
	else
		for (size_t i = 0; i < r1; i++)
			lineMul_sse(&lhs[i][0], &b[i][0], (int)c1, &m[i][0]);
	return m;
}
inline CLMatrixExT<float>& conv( //float 特化
	const CLMatrixExT<float>& M, //卷积输入map
	const CLMatrixExT<float>& K, //卷积核
	CLMatrixExT<float>& F, //结果集feature map
	size_t _stepX = 1, //卷积核X移动步长
	size_t _stepY = 1, //卷积核Y移动步长
	size_t padding = 0,//输入map边缘填充宽度
	double paddingValue = 0.0//输入map边缘填充内填充的值（该值不一定是0，根据计算需要自由设置）
) {
	if (M.isEmpty() || K.isEmpty()) {
		F.clear();
		_CLMatrixEx_Runtime_Error(conv, param matrix M is empty or matrix K is empty!);
	}
	if (padding >= min(K.rows(), K.cols())) {
		F.clear();
		_CLMatrixEx_Runtime_Error(conv, padding >= K.rows() or K.cols()!);
	}
	int stepX = max(int(_stepX), 1);
	int stepY = max(int(_stepY), 1);
	int cc = ((int)M.cols() + 2 * (int)padding - (int)K.cols()) / stepX + 1;
	int rr = ((int)M.rows() + 2 * (int)padding - (int)K.rows()) / stepY + 1;
	rr = max(rr, 0);
	cc = max(0, cc);
	F.make(rr, cc, 0);
#if CLMAT_USE_CXX_PPL > 0
	auto total = rr * cc;  //PPL处理块
	if (total >= CONV_RANK_LIMIT * CONV_RANK_LIMIT) {
		auto tsi = total / CONV_BLOCKS + 1;
		parallel_for<int>(0, tsi, 
			[&](int idt) {
			int idx = idt * CONV_BLOCKS;
			int ide = min(idx + CONV_BLOCKS, total);
			for (; idx < ide; ++idx) {
				int r = idx / cc;
				int c = idx % cc;
				double t = 0;
				int i = 0 - int(padding) + int(r * stepY), si = i + int(K.rows());
				int kr = 0; int kc = 0;
				for (; i < 0; ++i, kc = 0, ++kr) // up padding
				{
					auto pk = &K[kr][0];
					for (; kc < int(K.cols()); ++kc)
						t += paddingValue * *pk++;
				}
				for (int top = min(si, (int)M.rows()); i < top; ++i, kc = 0, ++kr)
				{
					int j = 0 - int(padding) + int(c * stepX), sj = j + int(K.cols());
					auto pk = &K[kr][0];
					for (; j < 0; ++j) // left padding									
						t += paddingValue * *pk++;
					int top2 = min(sj, (int)M.cols());
					int nCounts = top2 - j;
					if (nCounts > 0) { // 采用SSE
						t += lineDotMul_sse(&M[i][j], pk, nCounts);
						j += nCounts;
						pk += nCounts;
					}
					for (; j < sj; ++j)  // right padding			
						t += paddingValue * *pk++;
				}
				for (; i < si; ++i, kc = 0, ++kr) // down padding
				{
					auto pk = &K[kr][0];
					for (; kc < int(K.cols()); ++kc)
						t += paddingValue * *pk++;
				}
				F[r][c] = float(t);
			}
			});
		return F;
	}
#endif
	for (int r = 0; r < rr; ++r) { //serial处理块	
		for (int c = 0; c < cc; ++c) {
			double t = 0;
			int i = 0 - int(padding) + int(r * stepY), si = i + int(K.rows());
			int kr = 0; int kc = 0;
			for (; i < 0; ++i, kc = 0, ++kr) // up padding
			{
				auto pk = &K[kr][0];
				for (; kc < int(K.cols()); ++kc)
					t += paddingValue * *pk++;
			}
			for (int top = min(si, (int)M.rows()); i < top; ++i, kc = 0, ++kr)
			{
				int j = 0 - int(padding) + int(c * stepX), sj = j + int(K.cols());
				auto pk = &K[kr][0];
				for (; j < 0; ++j) // left padding									
					t += paddingValue * *pk++;
				int top2 = min(sj, (int)M.cols());
				int nCounts = top2 - j;
				if (nCounts > 0) { // 采用SSE
					t += lineDotMul_sse(&M[i][j], pk, nCounts);
					j += nCounts;
					pk += nCounts;
				}
				for (; j < sj; ++j)  // right padding			
					t += paddingValue * *pk++;
			}
			for (; i < si; ++i, kc = 0, ++kr) // down padding
			{
				auto pk = &K[kr][0];
				for (; kc < int(K.cols()); ++kc)
					t += paddingValue * *pk++;
			}
			F[r][c] = float(t);
		}
	};
	return F;
} //end func
inline CLMatrixExT<double>& conv( //double 特化
	const CLMatrixExT<double>& M, //卷积输入map
	const CLMatrixExT<double>& K, //卷积核
	CLMatrixExT<double>& F, //结果集feature map
	size_t _stepX = 1, //卷积核X移动步长
	size_t _stepY = 1, //卷积核Y移动步长
	size_t padding = 0,//输入map边缘填充宽度
	double paddingValue = 0.0//输入map边缘填充内填充的值（该值不一定是0，根据计算需要自由设置）
) {
	if (M.isEmpty() || K.isEmpty()) {
		F.clear();
		_CLMatrixEx_Runtime_Error(conv, param matrix M is empty or matrix K is empty!);
	}
	if (padding >= min(K.rows(), K.cols())) {
		F.clear();
		_CLMatrixEx_Runtime_Error(conv, padding >= K.rows() or K.cols()!);
	}
	int stepX = max(int(_stepX), 1);
	int stepY = max(int(_stepY), 1);
	int cc = ((int)M.cols() + 2 * (int)padding - (int)K.cols()) / stepX + 1;
	int rr = ((int)M.rows() + 2 * (int)padding - (int)K.rows()) / stepY + 1;
	rr = max(rr, 0);
	cc = max(0, cc);
	F.make(rr, cc, 0);
#if CLMAT_USE_CXX_PPL > 0
	auto total = rr * cc;  //PPL处理块
	if (total >= CONV_RANK_LIMIT * CONV_RANK_LIMIT) {
		auto tsi = total / CONV_BLOCKS + 1;
		parallel_for<int>(0, tsi,
			[&](int idt) {
				int idx = idt * CONV_BLOCKS;
				int ide = min(idx + CONV_BLOCKS, total);
				for (; idx < ide; ++idx) {
					int r = idx / cc;
					int c = idx % cc;
					double t = 0;
					int i = 0 - int(padding) + int(r * stepY), si = i + int(K.rows());
					int kr = 0; int kc = 0;
					for (; i < 0; ++i, kc = 0, ++kr) // up padding
					{
						auto pk = &K[kr][0];
						for (; kc < int(K.cols()); ++kc)
							t += paddingValue * *pk++;
					}
					for (int top = min(si, (int)M.rows()); i < top; ++i, kc = 0, ++kr)
					{
						int j = 0 - int(padding) + int(c * stepX), sj = j + int(K.cols());
						auto pk = &K[kr][0];
						for (; j < 0; ++j) // left padding									
							t += paddingValue * *pk++;
						int top2 = min(sj, (int)M.cols());
						int nCounts = top2 - j;
						if (nCounts > 0) { // 采用SSE
							t += lineDotMul_sse(&M[i][j], pk, nCounts);
							j += nCounts;
							pk += nCounts;
						}
						for (; j < sj; ++j)  // right padding			
							t += paddingValue * *pk++;
					}
					for (; i < si; ++i, kc = 0, ++kr) // down padding
					{
						auto pk = &K[kr][0];
						for (; kc < int(K.cols()); ++kc)
							t += paddingValue * *pk++;
					}
					F[r][c] = double(t);
				}
			});
		return F;
	}
#endif
	for (int r = 0; r < rr; ++r) { //serial处理块	
		for (int c = 0; c < cc; ++c) {
			double t = 0;
			int i = 0 - int(padding) + int(r * stepY), si = i + int(K.rows());
			int kr = 0; int kc = 0;
			for (; i < 0; ++i, kc = 0, ++kr) // up padding
			{
				auto pk = &K[kr][0];
				for (; kc < int(K.cols()); ++kc)
					t += paddingValue * *pk++;
			}
			for (int top = min(si, (int)M.rows()); i < top; ++i, kc = 0, ++kr)
			{
				int j = 0 - int(padding) + int(c * stepX), sj = j + int(K.cols());
				auto pk = &K[kr][0];
				for (; j < 0; ++j) // left padding									
					t += paddingValue * *pk++;
				int top2 = min(sj, (int)M.cols());
				int nCounts = top2 - j;
				if (nCounts > 0) { // 采用SSE
					t += lineDotMul_sse(&M[i][j], pk, nCounts);
					j += nCounts;
					pk += nCounts;
				}
				for (; j < sj; ++j)  // right padding			
					t += paddingValue * *pk++;
			}
			for (; i < si; ++i, kc = 0, ++kr) // down padding
			{
				auto pk = &K[kr][0];
				for (; kc < int(K.cols()); ++kc)
					t += paddingValue * *pk++;
			}
			F[r][c] = double(t);
		}
	};
	return F;
} //end func



// 矩阵的标准乘法(内积)
template<class T1, class T2, class T3>
CLMatrixExT<T3>& dotMul(const CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs, CLMatrixExT<T3>& m)
{
	auto c = lhs.cols(), r = rhs.rows();
	if (c != r)
	{
		m.clear();
		char err[256];
		sprintf_s(err, "Error: CLMatrixExT method \" dotMul \", left obj cols(%d) != right obj rows(%d)!", (int)c, (int)r);
		_CLMatrixT_Runtime_Error_Box(err);
		throw std::runtime_error("CLMatrixExT Runtime Error:dotMul");
	}
#if CLMAT_USE_SSE > 0
	if (matrixUseSSE && c >= matrixUseSSEMinRank) //经过测试矩阵横向宽度小于16将没有速度优势，故不再使用SSE
		return ::_dotMul_sse(lhs, rhs, m);
#endif
	r = lhs.rows(), c = rhs.cols();
	m.resize(r, c);
	size_t K = lhs.cols();
#if CLMAT_USE_CXX_PPL > 0  //PPL 代码块
	auto total = r * c;
	if (total >= DOTMUL_RANK_LIMIT * DOTMUL_RANK_LIMIT) {
		auto tsi = total / DOTMUL_BLOCKS + 1;
		parallel_for<size_t>(0, tsi,
			[&c, &K, &m, &lhs, &rhs, &total](size_t idt) {
				size_t idx = idt * DOTMUL_BLOCKS;
				size_t ide = min(idx + DOTMUL_BLOCKS, total);
				for (; idx < ide; ++idx) {
					size_t i = idx / c;
					size_t j = idx % c;
					T1 sum = 0;
					for (size_t k = 0; k < K; ++k) {
						sum += lhs[i][k] * rhs[k][j];
					}
					m[i][j] = T3(sum);
				}
			});
		return m;
	}
#endif
	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			T1 sum = 0;
			for (size_t k = 0; k < K; ++k) {
				sum += lhs[i][k] * rhs[k][j];
			}
			m[i][j] = T3(sum);
		}
	}
	return m;
}
// 矩阵逐点相乘，即元素对应相乘。L行向量 X R横向量，左右操作数的列数必须相同；
// 右操作数行数 = 1：左操作数的每个行向量分别与右操作数的唯一行向量相乘；
// 右操作数行数 >= 左操作数的行：左操作数的每个行向量分别对应与右操作数的每个行向量相乘；
template<class T1, class T2, class T3>
CLMatrixExT<T3>& mul(const  CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs, CLMatrixExT<T3>& m)
{
	auto c1 = lhs.cols(), r1 = lhs.rows();
	auto c2 = rhs.cols(), r2 = rhs.rows();
	if (c2 == c1) {
		if (r2 == 1) {
#if CLMAT_USE_SSE > 0
			if (matrixUseSSE /*&& c2 >= matrixUseSSEMinRank*/)
				return ::_mul_sse(lhs, rhs, m);
#endif
			m.resize(r1, c1);
			for (size_t i = 0; i < r1; ++i)
				for (size_t j = 0; j < c1; ++j)
					m[i][j] = T3(lhs[i][j] * rhs[0][j]);
			return m;
		}
		else if (r2 >= r1) {
#if CLMAT_USE_SSE > 0
			if (matrixUseSSE /*&& c2 >= matrixUseSSEMinRank*/)
				return ::_mul_sse(lhs, rhs, m);
#endif
			m.resize(r1, c1);
			for (size_t i = 0; i < r1; ++i)
				for (size_t j = 0; j < c1; ++j)
					m[i][j] = T3(lhs[i][j] * rhs[i][j]);
			return m;
		}
		m.clear();
		// never arrive here.
		char err[256];
		sprintf_s(err, "Error: CLMatrixExT method \" mul \", cols match, but right obj rows(%d) != 1 and < left obj rows(%d)!", (int)r2, (int)r1);
		_CLMatrixT_Runtime_Error_Box(err);
		throw std::runtime_error("CLMatrixExT Runtime Error:operator*");
	}
	else {
		m.clear();
		char err[256];
		sprintf_s(err, "Error: CLMatrixExT method \" mul \", cols ( %d != %d ) it is not match!", (int)c1, (int)c2);
		_CLMatrixT_Runtime_Error_Box(err);
		throw std::runtime_error("CLMatrixExT Runtime Error:operator*");
	}
}
// 矩阵逐点相乘，即元素对应相乘。L行向量 X R列向量，左右操作数的列数和行数必须相同；（等价于 右操作数是按其转置来处理逐点相乘的。）
// 右操作数列数 = 1：左操作数的每个行向量分别与右操作数的唯一行列量相乘；
// 右操作数列数 >= 左操作数的行：左操作数的每个行向量分别对应与右操作数的每个列向量相乘；
template<class T1, class T2, class T3>
CLMatrixExT<T3>& mul_T(const  CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs, CLMatrixExT<T3>& m)
{
	auto c1 = lhs.cols(), r1 = lhs.rows();
	auto c2 = rhs.rows(), r2 = rhs.cols();
	if (c2 == c1) {
		if (r2 == 1) {
#if CLMAT_USE_SSE > 0
			if (matrixUseSSE && c2 >= matrixUseSSEMinRank)
				return ::_mul_T_sse(lhs, rhs, m);
#endif
			m.resize(r1, c1);
			for (size_t i = 0; i < r1; ++i)
				for (size_t j = 0; j < c1; ++j)
					m[i][j] = T3(lhs[i][j] * rhs[j][0]);
			return m;
		}
		else if (r2 >= r1) {
#if CLMAT_USE_SSE > 0
			if (matrixUseSSE && c2 >= matrixUseSSEMinRank)
				return ::_mul_T_sse(lhs, rhs, m);
#endif
			m.resize(r1, c1);
			for (size_t i = 0; i < r1; ++i)
				for (size_t j = 0; j < c1; ++j)
					m[i][j] = T3(lhs[i][j] * rhs[j][i]);
			return m;
		}
		m.clear();
		// never arrive here.
		char err[256];
		sprintf_s(err, "Error: CLMatrixExT method \" mul_T \", cols match, but right obj cols(%d) != 1 and < left obj rows(%d)!", (int)r2, (int)r1);
		_CLMatrixT_Runtime_Error_Box(err);
		throw std::runtime_error("CLMatrixExT Runtime Error:operator*");
	}
	else {
		m.clear();
		char err[256];
		sprintf_s(err, "Error: CLMatrixExT method \" mul_T \", left cols(%d) !=  right rows(%d) , it is not match!", (int)c1, (int)c2);
		_CLMatrixT_Runtime_Error_Box(err);
		throw std::runtime_error("CLMatrixExT Runtime Error:operator*");
	}
}
//矩阵按列逐点相乘，即元素对应相乘。L列向量 X R列向量，左右操作数的行数必须相同；
//右操作数列数 = 1：左操作数的每个列向量分别与右操作数的唯一列向量相乘；
//右操作数列数 >= 左操作数的列数：左操作数的每个列向量分别对应与右操作数的每个列向量相乘；
template<class T1, class T2, class T3>
CLMatrixExT<T3>& mul_V(const  CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs, CLMatrixExT<T3>& m)
{
	auto c1 = lhs.cols(), r1 = lhs.rows();
	auto c2 = rhs.cols(), r2 = rhs.rows();
	if (r2 == r1) {
		if (c2 == 1) {
#if CLMAT_USE_SSE > 0
			//if (matrixUseSSE && r2 >= matrixUseSSEMinRank)
			//	return ::_mul_T_sse(::T(lhs, CLMatrixExT<T1>()), rhs, m).makeT();
			// 此位置不能采用加速
#endif
			m.resize(r1, c1);
			for (size_t i = 0; i < r1; ++i)
				for (size_t j = 0; j < c1; ++j)
					m[i][j] = T3(lhs[i][j] * rhs[i][0]);
			return m;
		}
		else if (c2 >= c1) {
#if CLMAT_USE_SSE > 0
			if (matrixUseSSE /*&& c2 >= matrixUseSSEMinRank*/)
				return ::_mul_sse(lhs, rhs, m);
#endif
			m.resize(r1, c1);
			for (size_t i = 0; i < r1; ++i)
				for (size_t j = 0; j < c1; ++j)
					m[i][j] = T3(lhs[i][j] * rhs[i][j]);
			return m;
		}
		m.clear();
		// never arrive here.
		char err[256];
		sprintf_s(err, "Error: CLMatrixExT method \" mul_V \", rows match, but right obj cols(%d) != 1 and < left obj cols(%d)!", (int)c2, (int)c1);
		_CLMatrixT_Runtime_Error_Box(err);
		throw std::runtime_error("CLMatrixExT Runtime Error:operator*");
	}
	else {
		m.clear();
		char err[256];
		sprintf_s(err, "Error: CLMatrixExT method \" mul_V \", rows ( %d != %d ) it is not match!", (int)r1, (int)r2);
		_CLMatrixT_Runtime_Error_Box(err);
		throw std::runtime_error("CLMatrixExT Runtime Error:operator*");
	}
}
//矩阵按列逐点相乘，即元素对应相乘。L列向量 X R行向量，左右操作数的列数和行数必须相同；（等价于 右操作数是按其转置来处理逐点相乘的。）
//右操作数行数 = 1：左操作数的每个列向量分别与右操作数的唯一行列量相乘；
//右操作数行数 >= 左操作数的列：左操作数的每个列向量分别对应与右操作数的每个行向量相乘；
template<class T1, class T2, class T3>
CLMatrixExT<T3>& mul_VT(const  CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs, CLMatrixExT<T3>& m)
{
	auto c1 = lhs.cols(), r1 = lhs.rows();
	auto r2 = rhs.cols(), c2 = rhs.rows();
	if (r2 == r1) {
		if (c2 == 1) {
#if CLMAT_USE_SSE > 0
			//if (matrixUseSSE && r1 >= matrixUseSSEMinRank)
			//	return  ::_mul_sse(::T(lhs, CLMatrixExT<T1>()), rhs, m).makeT();
			// 此位置不能采用加速
#endif
			m.resize(r1, c1);
			for (size_t i = 0; i < r1; ++i)
				for (size_t j = 0; j < c1; ++j)
					m[i][j] = T3(lhs[i][j] * rhs[0][i]);
			return m;
		}
		else if (c2 >= c1) {
#if CLMAT_USE_SSE > 0
			//if (matrixUseSSE && r1 >= matrixUseSSEMinRank)
			//	return  ::_mul_sse(::T(lhs, CLMatrixExT<T1>()), rhs, m).makeT();
#endif
			m.resize(r1, c1);
			for (size_t i = 0; i < r1; ++i)
				for (size_t j = 0; j < c1; ++j)
					m[i][j] = T3(lhs[i][j] * rhs[j][i]);
			return m;
		}
		m.clear();
		// never arrive here.
		char err[256];
		sprintf_s(err, "Error: CLMatrixExT method \" mul_VT \", rows and cols is match, but right obj rows(%d) != 1 and < left obj cols(%d)!", (int)c2, (int)c1);
		_CLMatrixT_Runtime_Error_Box(err);
		throw std::runtime_error("CLMatrixExT Runtime Error:operator*");
	}
	else {
		m.clear();
		char err[256];
		sprintf_s(err, "Error: CLMatrixExT method \" mul_VT \", left rows(%d) !=  right cols(%d) , it is not match!", (int)r1, (int)r2);
		_CLMatrixT_Runtime_Error_Box(err);
		throw std::runtime_error("CLMatrixExT Runtime Error:operator*");
	}
}
// 矩阵的标准乘法(内积)
template<class T1, class T2>
CLMatrixExT<T1> operator*(const CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs)
{
	return std::move(::dotMul(lhs, rhs, (CLMatrixExT<T1>&)CLMatrixExT<T1>()));
}
// 矩阵的元素与数分别相乘，即矩阵按比例缩放数值大小
template<class T1, class T2>
CLMatrixExT<T1> operator*(const  CLMatrixExT<T1>& lhs, T2 v)
{
	auto m = lhs;
	m *= T1(v);
	return std::move(m);
}
// 矩阵的元素与数分别相乘，即矩阵按比例缩放数值大小
template<class T1, class T2>
CLMatrixExT<T1> operator*(T2 v, const  CLMatrixExT<T1>& lhs)
{
	auto m = lhs;
	m *= T1(v);
	return std::move(m);
}
// 矩阵标准除法，即左操作数 * 右操作数的逆矩阵
template<class T1, class T2>
CLMatrixExT<T1> operator/(const  CLMatrixExT<T1>& lhs, const  CLMatrixExT<T2>& rhs)
{
	CLMatrixExT<T2> tmp;
	inv(rhs, tmp);
	if (tmp.isEmpty())
		return std::move(tmp);
	return std::move(lhs * tmp);
}
// 矩阵的元素与数分别相除，即矩阵按比例缩放数值大小
template<class T1, class T2>
CLMatrixExT<T1> operator/(const  CLMatrixExT<T1>& lhs, T2 v)
{
	auto m = lhs;
	m /= v;
	return std::move(m);
}
// 数除以矩阵的元素，即矩阵的倒数矩阵与数相乘
template<class T1, class T2>
CLMatrixExT<T1> operator/(T2 v, const  CLMatrixExT<T1>& lhs)
{
	return std::move(lhs.pow(-1.0) * v);
}
// 计算方阵 M 的 LU 分解,使得 M = LU
// 其中L为对角线元素全为1的下三角阵，U为对角元素依赖M的上三角阵
template<class T1, class T2, class T3>bool LU(const CLMatrixExT<T1>& A, CLMatrixExT<T2>& L, CLMatrixExT<T3>& U)
{
	if (A.isEmpty())
	{
		L.clear(); U.clear();
		_CLMatrixEx_Runtime_Error(LUP, matix obj is empty matrix!);
	}
	else if (!A.isSquare()) {
		L.clear(); U.clear();
		_CLMatrixEx_Runtime_Error(LUP, matix obj is not a square matrix!);
	}
	auto M = A;
	if (!LUP_Descomposition(M, L, U, CLMatrixExT<size_t>())) {
		cout << endl << "[Runtime error]: Matrix is singular, unable to calculate inverse!" << endl;
		return false;
	}
	return true;
}
// LUP分解
template<class T1, class T2, class T3>bool LUP_Descomposition(CLMatrixExT<T1>& A, CLMatrixExT<T2>& L, CLMatrixExT<T3>& U, CLMatrixExT<size_t>& PLine)
{
	size_t N = A.rows();
	L.resize(N, N);
	U.resize(N, N);
	PLine.resize(1, N);
	auto& P = PLine[0];
	size_t row = 0;
	for (size_t i = 0; i < N; ++i)
	{
		P[i] = i;
	}
	for (size_t i = 0; i < N - 1; ++i)
	{
		T1 p = 0;
		for (size_t j = i; j < N; ++j)
		{
			if (abs(A[j][i]) > p)
			{
				p = abs(A[j][i]);
				row = j;
			}
		}
		if (0 == p)
		{
			//cout << endl << "矩阵奇异，无法计算逆" << endl;
			return false;
		}

		//交换P[i]和P[row]
		size_t tmp = P[i];
		P[i] = P[row];
		P[row] = tmp;

		T1 tmp2 = 0;
		for (size_t j = 0; j < N; ++j)
		{
			//交换A[i][j]和 A[row][j]
			tmp2 = A[i][j];
			A[i][j] = A[row][j];
			A[row][j] = tmp2;
		}

		//以下同LU分解
		T1 u = A[i][i], l = 0;
		for (size_t j = i + 1; j < N; ++j)
		{
			l = A[j][i] / u;
			A[j][i] = l;
			for (size_t k = i + 1; k < N; ++k)
			{
				A[j][k] = A[j][k] - A[i][k] * l;
			}
		}

	}

	//构造L和U
	for (size_t i = 0; i < N; ++i)
	{
		for (size_t j = 0; j <= i; ++j)
		{
			if (i != j)
			{
				L[i][j] = A[i][j];
			}
			else
			{
				L[i][j] = 1;
			}
		}
		for (size_t k = i; k < N; ++k)
		{
			U[i][k] = A[i][k];
		}
	}
	return true;
}
//LUP求解方程
template<class T1, class T2, class T3>
void LUP_Solve(size_t N, CLMatrixExT<T1>& X, CLMatrixExT<T1>& Y, const CLMatrixExT<T2>& L, const CLMatrixExT<T3>& U, const CLMatrixExT<size_t>& PLine, const CLMatrixExT<T1>& B)
{
	auto& P = PLine[0];
	auto& b = B[0];
	X.resize(1, N);
	Y.resize(1, N);
	auto& x = X[0];
	auto& y = Y[0];
	//正向替换
	for (size_t i = 0; i < N; ++i)
	{
		y[i] = b[P[i]];
		for (size_t j = 0; j < i; ++j)
		{
			y[i] = y[i] - L[i][j] * y[j];
		}
	}
	//反向替换
	for (int i = N - 1; i >= 0; i--)
	{
		x[i] = y[i];
		for (int j = N - 1; j > i; j--)
		{
			x[i] = x[i] - U[i][j] * x[j];
		}
		x[i] /= U[i][i];
	}
}
// LUP分解求逆
template<class T1, class T2> CLMatrixExT<T2>& LUP_Inverse(const CLMatrixExT<T1>& A, CLMatrixExT<T2>& ret)
{
	if (A.isEmpty())
	{
		ret.clear();
		_CLMatrixEx_Runtime_Error(inv, matix obj is empty matrix!);
	}
	else if (!A.isSquare()) {
		ret.clear();
		_CLMatrixEx_Runtime_Error(inv, matix obj is not a square matrix!);
	}

	size_t N = A.cols();

	CLMatrixExT<float> A_mirror(N, N);
	CLMatrixExT<float> inv_A(N, N);//最终的逆矩阵（还需要转置）
	CLMatrixExT<float> inv_A_each(1, N);//矩阵逆的各列
	CLMatrixExT<float> B(1, N);//b阵为B阵的列矩阵分量
	CLMatrixExT<float> Y(1, N);//b阵为B阵的列矩阵分量
	CLMatrixExT<float> L(N, N);
	CLMatrixExT<float> U(N, N);

	CLMatrixExT<size_t> P(1, N);
	for (size_t i = 0; i < N; ++i)
	{
		//构造单位阵的每一列
		for (size_t j = 0; j < N; ++j)B[0][j] = 0;
		B[0][i] = 1;
		A_mirror = A;
		if (!LUP_Descomposition(A_mirror, L, U, P)) {
			//奇异矩阵返回矩阵
			return ret.clear();
		}
		LUP_Solve(N, inv_A_each, Y, L, U, P, B);
		inv_A.setRow(i, &inv_A_each[0], inv_A_each.cols());
	}
	return ::T(inv_A, ret);
}
template<class T2> CLMatrixExT<T2>& LUP_Inverse(const CLMatrixExT<double>& A, CLMatrixExT<T2>& ret) {
	return ::LUP_Inverse<float, T2>(CLMatrixExT<float>(A), ret);
}
// 将一个矩阵变换到对角线矩阵，同一行列的值都加到主对角上
template<class T1, class T2>CLMatrixExT<T2>& diag(const CLMatrixExT<T1>& m, CLMatrixExT<T2>& ret) {
	auto r = m.rows(), c = m.cols();
	if (r == 0 || c == 0)
		return ret.clear();
	auto si = max(r, c);
	ret.makeSquare(si, 0);
	for (size_t i = 0; i < r; ++i)
	{
		for (size_t j = 0; j < c; ++j)
		{
			if (j >= i)
				ret[j][j] += m[i][j];
			else
				ret[i][i] += m[i][j];
		}
	}
	return ret;
}
// 矩阵卷积计算
template<class T1, class T2, class T3> CLMatrixExT<T3>& conv(
	const CLMatrixExT<T1>& M, //卷积输入map
	const CLMatrixExT<T2>& K, //卷积核
	CLMatrixExT<T3>& F, //结果集feature map
	size_t _stepX = 1, //卷积核X移动步长
	size_t _stepY = 1, //卷积核Y移动步长
	size_t padding = 0,//输入map边缘填充宽度
	double paddingValue = 0.0//输入map边缘填充内填充的值（该值不一定是0，根据计算需要自由设置）
) {
	if (M.isEmpty() || K.isEmpty()) {
		F.clear();
		_CLMatrixEx_Runtime_Error(conv, param matrix M is empty or matrix K is empty!);
	}
	if (padding >= min(K.rows(), K.cols())) {
		F.clear();
		_CLMatrixEx_Runtime_Error(conv, padding >= K.rows() or K.cols()!);
	}
	int stepX = max(int(_stepX), 1);
	int stepY = max(int(_stepY), 1);
	int cc = ((int)M.cols() + 2 * (int)padding - (int)K.cols()) / stepX + 1;
	int rr = ((int)M.rows() + 2 * (int)padding - (int)K.rows()) / stepY + 1;
	rr = max(rr, 0);
	cc = max(0, cc);
	F.make(rr, cc, 0);
#if CLMAT_USE_CXX_PPL > 0
	auto total = rr * cc;  //PPL处理块
	if (total >= CONV_RANK_LIMIT * CONV_RANK_LIMIT) {
		auto tsi = total / CONV_BLOCKS + 1;
		parallel_for<int>(0, tsi, [&](int idt) {
			int idx = idt * CONV_BLOCKS;
			int ide = min(idx + CONV_BLOCKS, total);
			for (; idx < ide; ++idx) {
				int r = idx / cc;
				int c = idx % cc;
				double t = 0;
				int i = 0 - int(padding) + int(r * stepY), si = i + int(K.rows());
				int kr = 0; int kc = 0;
				for (; i < 0; ++i, kc = 0, ++kr) // up padding
				{
					auto pk = &K[kr][0];
					for (; kc < int(K.cols()); ++kc)
						t += paddingValue * *pk++;
				}
				for (int top = min(si, (int)M.rows()); i < top; ++i, kc = 0, ++kr)
				{
					int j = 0 - int(padding) + int(c * stepX), sj = j + int(K.cols());
					auto pk = &K[kr][0];
					for (; j < 0; ++j) // left padding									
						t += paddingValue * *pk++;
					for (int top2 = min(sj, (int)M.cols()); j < top2; ++j)
						t += M[i][j] * *pk++;
					for (; j < sj; ++j)  // right padding			
						t += paddingValue * *pk++;
				}
				for (; i < si; ++i, kc = 0, ++kr) // down padding
				{
					auto pk = &K[kr][0];
					for (; kc < int(K.cols()); ++kc)
						t += paddingValue * *pk++;
				}
				F[r][c] = T3(t);
			}
			}); 
		return F;
	}
#endif
	for (int r = 0; r < rr; ++r) { //serial处理块	
		for (int c = 0; c < cc; ++c) {
			double t = 0;
			int i = 0 - int(padding) + int(r * stepY), si = i + int(K.rows());
			int kr = 0; int kc = 0;
			for (; i < 0; ++i, kc = 0, ++kr) // up padding
			{
				auto pk = &K[kr][0];
				for (; kc < int(K.cols()); ++kc)
					t += paddingValue * *pk++;
			}
			for (int top = min(si, (int)M.rows()); i < top; ++i, kc = 0, ++kr)
			{
				int j = 0 - int(padding) + int(c * stepX), sj = j + int(K.cols());
				auto pk = &K[kr][0];
				for (; j < 0; ++j) // left padding									
					t += paddingValue * *pk++;
				for (int top2 = min(sj, (int)M.cols()); j < top2; ++j)
					t += M[i][j] * *pk++;
				for (; j < sj; ++j)  // right padding			
					t += paddingValue * *pk++;
			}
			for (; i < si; ++i, kc = 0, ++kr) // down padding
			{
				auto pk = &K[kr][0];
				for (; kc < int(K.cols()); ++kc)
					t += paddingValue * *pk++;
			}
			F[r][c] = T3(t);
		}
	}; 
	return F;
} //end func

typedef CLMatrixExT<float> CLMatrixEx;//float型矩阵类
typedef CLMatrixExT<double> CLMatrixExD;//double型矩阵类
typedef CLMatrixExT<float> CLMatrixExF;//float型矩阵类
typedef CLMatrixExT<short> CLMatrixExS;//short型矩阵类
typedef CLMatrixExT<int> CLMatrixExI;//int型矩阵类
typedef CLMatrixExT<long long> CLMatrixExLL;//long long型矩阵类
typedef CLMatrixExT<long> CLMatrixExL;//long型矩阵类

template class CLMatrixExT<float>;

// 测试检查本机SSE参数的最佳值，返回值用于CLMatrixEx::setUseSSEMinRank()的参数
inline size_t matrixExSSEParamFitValue() {
	auto MakeXF = [](CLMATRIXF_CALLBACK_PARAM) { v = 1; };
	auto MakeXD = [](CLMATRIXD_CALLBACK_PARAM) { v = 1; };
	auto MakeXI = [](CLMATRIXI_CALLBACK_PARAM) { v = 1; };
	size_t base = 1, times = 1000;
	auto bkset = CLMatrixEx::setUseSSE(true);
	auto bksi = CLMatrixEx::setUseSSEMinRank(0);
	size_t mk1 = 0, mk2 = 0, mk3 = 0;
	for (size_t i = 0; i < 20; i++)
	{
		base += 1;
		CLMatrixExF a(base, MakeXF), b(base, MakeXF);
		CLMatrixExD c(base, MakeXD), d(base, MakeXD);
		CLMatrixExI e(base, MakeXI), f(base, MakeXI);
		CLMatrixEx::setUseSSE(true);
		auto t0 = clock();
		for (size_t j = 0; j < times; j++)a* b;
		auto t1 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)c* d;
		auto t2 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)e* f;
		auto t5 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		CLMatrixEx::setUseSSE(false);
		for (size_t j = 0; j < times; j++)a* b;
		auto t3 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)c* d;
		auto t4 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)e* f;
		auto t6 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		if (mk1 == 0 && t1 < t3)mk1 = i + 2;
		if (mk2 == 0 && t2 < t4)mk2 = i + 2;
		if (mk3 == 0 && t5 < t6)mk3 = i + 2;
		if (mk1 && mk2 && mk3)
			break;
	}
	CLMatrixEx::setUseSSE(bkset);
	CLMatrixEx::setUseSSEMinRank(bksi);
	size_t rt = (2 * mk2 + 7 * mk1 + 1 * mk3) / 10;
	return rt;
}
// 做本地效率测试,并输出结果
inline void matrixExLocalTest() {
#undef AXB
#undef CXD
#undef EXF
#define AXB a.conv(b)
#define CXD c.conv(d)
#define EXF e.conv(f)
//#define AXB a* b
//#define CXD c* d
//#define EXF e* f
//#define AXB a/ b
//#define CXD c/ d
//#define EXF e/ f
//#define AXB a.mul_V(b)
//#define CXD c.mul_V(d)
//#define EXF e.mul_V(f)
//#define AXB a.mul_T(b)
//#define CXD c.mul_T(d)
//#define EXF e.mul_T(f)
//#define AXB a.mul(b)
//#define CXD c.mul(d)
//#define EXF e.mul(f)
//#define AXB (a+b)
//#define CXD (c-d)
//#define EXF (e+f)
	auto bku = CLMatrixEx::isUseSSE();
	cout << "\n\n测试本机不同大小矩阵的类型运行效率----------------------------------";
	srand((unsigned int)time(0));
	auto MakeXF = [](CLMATRIXF_CALLBACK_PARAM) { v = rand() % 100 * 0.01f; };
	auto MakeXD = [](CLMATRIXD_CALLBACK_PARAM) { v = rand() % 100 * 0.01; };
	auto MakeXI = [](CLMATRIXI_CALLBACK_PARAM) { v = rand() % 10; };
	size_t base = 1;
	size_t base2 = 1;
	size_t times = 10000000;
	cout << "\n\n指数级递增测试";
	for (size_t i = 0; base <= 512; i++)
	{
		base *= 2; times /= 4;
		times = max(1, times);
		//base2 = base;//卷积测试时候注释掉这一行定义
		CLMatrixExF a(base, MakeXF), b(base2, MakeXF);
		CLMatrixExD c(base, MakeXD), d(base2, MakeXD);
		CLMatrixExI e(base, MakeXI), f(base2, MakeXI);
		base2 = max(min(base/4, 64),1);
		//base2 = min(base/4, 64);
		CLMatrixEx::setUseSSE(true);
		auto t0 = clock();
		for (size_t j = 0; j < times; j++)AXB;
		auto t1 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)CXD;
		auto t2 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)EXF;
		auto t5 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		CLMatrixEx::setUseSSE(false);
		for (size_t j = 0; j < times; j++)AXB;
		auto t3 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)CXD;
		auto t4 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)EXF;
		auto t6 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		printf_s("\n%zd:%zd rank=%zd, f=%.3fs %s uf=%.3fs (%+.2f%%), d=%.3fs %s ud=%.3fs (%+.2f%%), i=%.3fs %s ui=%.3fs (%+.2f%%)",
			i + 1, times, base, t1, (t1 < t3 ? "快<" : ">"), t3, (t1 - t3) / t3 * 100,
			t2, (t2 < t4 ? "快<" : ">"), t4, (t2 - t4) / t4 * 100,
			t5, (t5 < t6 ? "快<" : ">"), t6, (t5 - t6) / t6 * 100);
	}
	cout << endl;
	CLMatrixExF M1, M3;
	CLMatrixExD M2, M4;
	CLMatrixExI M5, M6;
	int open = 1;
	base = 1, base2 = 1; times = 1000000;
	cout << "\n线性递增测试";
	for (size_t i = 0; i < 31; i++)
	{
		base += 1; times = times * 4 / 5;
		//base2 = base;//卷积测试时候注释掉这一行定义
		CLMatrixExF a(base, MakeXF), b(base2, MakeXF);
		CLMatrixExD c(base, MakeXD), d(base2, MakeXD);
		CLMatrixExI e(base, MakeXI), f(base2, MakeXI);
		base2 = min(base / 2, 7);
		CLMatrixEx::setUseSSE(true);
		auto t0 = clock();
		for (size_t j = 0; j < times; j++)AXB;
		auto t1 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)CXD;
		auto t2 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)EXF;
		auto t5 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		CLMatrixEx::setUseSSE(false);
		for (size_t j = 0; j < times; j++)AXB;
		auto t3 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)CXD;
		auto t4 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		for (size_t j = 0; j < times; j++)EXF;
		auto t6 = double(clock() - t0) / CLOCKS_PER_SEC; t0 = clock();
		printf_s("\n%zd:%zd rank=%zd, f=%.3fs %s uf=%.3fs (%+.2f%%), d=%.3fs %s ud=%.3fs (%+.2f%%), i=%.3fs %s ui=%.3fs (%+.2f%%)",
			i + 1, times, base, t1, (t1 < t3 ? "快<" : ">"), t3, (t1 - t3) / t3 * 100,
			t2, (t2 < t4 ? "快<" : ">"), t4, (t2 - t4) / t4 * 100,
			t5, (t5 < t6 ? "快<" : ">"), t6, (t5 - t6) / t6 * 100);
		if (base > 8 && open) {
			open = 0;
			CLMatrixEx::setUseSSE(false);
			M1 = AXB;
			M2 = CXD;
			M5 = EXF;
			CLMatrixEx::setUseSSE(true);
			M3 = AXB;
			M4 = CXD;
			M6 = EXF;
		}
	}
	cout << endl;
	if (M1 != M3)
		M1.print(_T(" a * b no use")), M3.print(_T(" a * b use")), (M1 - M3).print(_T(" delta a b use"));
	if (M2 != M4)
		M2.print(_T(" c * d no use")), M4.print(_T(" c * d use")), (M2 - M4).print(_T(" delta c d use"));
	if (M5 != M6)
		M5.print(_T(" e * f no use")), M6.print(_T(" e * f use")), (M5 - M6).print(_T(" delta e f use"));
	cout << "\n当前矩阵的加速优化宽度最小宽度" << matrixUseSSEMinRank << endl << endl;
	CLMatrixEx::setUseSSE(bku);//还原
}

#endif