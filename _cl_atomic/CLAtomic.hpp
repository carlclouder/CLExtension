
//原子操作类
#ifndef _CL_ATOMIC_H_
#define _CL_ATOMIC_H_

#include "windows.h"
#include "type_traits"

//默认状态下__CLAtomic_Old类对象的原子操作模式是否启动，原子操作在保证线程安全情况下将降低代码效率；
#define CLAtomic_bUseAtomc_def true

template  <class T, bool = false>
struct _atomic_value;

template  <class T>
struct _atomic_value<T, false> {
	mutable T _Value{};
	_atomic_value() = default;
	constexpr _atomic_value(const T Value) noexcept : _Value(Value) {}
	constexpr _atomic_value(T& Value, bool) noexcept : _Value(Value) {}
};

template  <class T>
struct _atomic_value<T, true> {
	T& _Value;
	_atomic_value() :_Value(_getStaticValueRef()) {}
	constexpr _atomic_value(const T Value) noexcept : _Value(_getStaticValueRef()) {}
	constexpr _atomic_value(T& Value, bool) noexcept : _Value(Value) {}
private:
	inline static T& _getStaticValueRef() {
		static T _g_atv = 0;
		return _g_atv;
	}
};
//值类型特化
template <class T, bool isRef = false>
struct _atomic_storage :_atomic_value<T, isRef> {
protected:
	bool _bUseAtomc{ CLAtomic_bUseAtomc_def };

	using _Base = _atomic_value<T, isRef>;
	_atomic_storage() = default;
	constexpr _atomic_storage(const T Value) noexcept : _Base(Value) {}
	constexpr _atomic_storage(T& Value, bool) noexcept : _Base(Value, true) {}
	void store(const T Value) noexcept { this->_Base::_Value = Value; }
	T load() const noexcept { return this->_Base::_Value; }

	template<size_t = sizeof(T)>
	bool compare_exchange_strong(T& _Expected, const T _Desired) noexcept;
	template<>
	bool compare_exchange_strong<2>(T& _Expected, const T _Desired) noexcept {
		static_assert(sizeof(short) == sizeof(T), "Tried to reinterpret memory is not same. ");
		auto old = *(short*)(&_Expected);
		if (InterlockedCompareExchange(&reinterpret_cast<volatile short&>(_Value), *(short*)(&_Desired), old) == old) {
			return true;
		}
		else {
			_Expected = load();
			return false;
		}
	}
	template<>
	bool compare_exchange_strong<4>(T& _Expected, const T _Desired) noexcept {
		static_assert(sizeof(long) == sizeof(T), "Tried to reinterpret memory is not same. ");
		auto old = *(long*)(&_Expected);
		if (InterlockedCompareExchange(&reinterpret_cast<volatile long&>(_Value), *(long*)(&_Desired), old) == old) {
			return true;
		}
		else {
			_Expected = load();
			return false;
		}
	}
	template<>
	bool compare_exchange_strong<8>(T& _Expected, const T _Desired) noexcept {
		static_assert(sizeof(long long) == sizeof(T), "Tried to reinterpret memory is not same. ");
		auto old = *(long long*)(&_Expected);
		if (InterlockedCompareExchange64(&reinterpret_cast<volatile long long&>(_Value), *(long long*)(&_Desired), old) == old) {
			return true;
		}
		else {
			_Expected = load();
			return false;
		}
	}

public:
	//设置该原子对象的状态为：执行原子操作，或执行非原子操作；原子操作更耗时；
	void setUseAtomic(bool bUseAtomc = CLAtomic_bUseAtomc_def) noexcept {
		_bUseAtomc = bUseAtomc;
	}
	//判断该对象状态，是执行原子操作，返回true；
	bool isUseAtomic() const noexcept {
		return _bUseAtomc;
	}
};

template <class T, size_t = sizeof(T), bool = false>
struct _atomic_integral; // not defined

template <class _integral, class T>
volatile _integral* _atomic_address_as(T& _Source) noexcept {
	static_assert(is_integral_v<_integral>, "Tried to reinterpret memory as non-integral");
	return &reinterpret_cast<volatile _integral&>(_Source);
}

template <class T, bool isRef>
struct _atomic_integral<T, 1, isRef> : _atomic_storage<T, isRef> { // CLAtomic integral operations using 1-byte intrinsics
protected:
	using _Base = _atomic_storage<T, isRef>;
	_atomic_integral() = default;
	constexpr _atomic_integral(const T Value) noexcept : _Base(Value) {}
	constexpr _atomic_integral(T& Value, bool) noexcept : _Base(Value, true) {}

	T self_add(T _Operand) noexcept {
		char _Result =
			InterlockedExchangeAdd8(_atomic_address_as<char>(this->_Value), static_cast<char>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_and(T _Operand) noexcept {
		char _Result =
			InterlockedAnd8(_atomic_address_as<char>(this->_Value), static_cast<char>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_or(T _Operand) noexcept {
		char _Result =
			InterlockedOr8(_atomic_address_as<char>(this->_Value), static_cast<char>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_xor(T _Operand) noexcept {
		char _Result =
			InterlockedXor8(_atomic_address_as<char>(this->_Value), static_cast<char>(_Operand));
		return static_cast<T>(_Result);
	}
	T operator++(int) noexcept {
		return static_cast<T>(InterlockedExchangeAdd8(_atomic_address_as<char>(this->_Value), 1));
	}
	T operator++() noexcept {
		unsigned char _Before =
			static_cast<unsigned char>(InterlockedExchangeAdd8(_atomic_address_as<char>(this->_Value), 1));
		++_Before;
		return static_cast<T>(_Before);
	}
	T operator--(int) noexcept {
		return static_cast<T>(InterlockedExchangeAdd8(_atomic_address_as<char>(this->_Value), -1));
	}
	T operator--() noexcept {
		unsigned char _Before =
			static_cast<unsigned char>(InterlockedExchangeAdd8(_atomic_address_as<char>(this->_Value), -1));
		--_Before;
		return static_cast<T>(_Before);
	}
};

template <class T, bool isRef>
struct _atomic_integral<T, 2, isRef> : _atomic_storage<T, isRef> { // CLAtomic integral operations using 2-byte intrinsics
protected:
	using _Base = _atomic_storage<T, isRef>;
	_atomic_integral() = default;
	constexpr _atomic_integral(const T _Value) noexcept : _Base(_Value) {}
	constexpr _atomic_integral(T& _Value, bool) noexcept : _Base(_Value, true) {}
	T self_add(T _Operand) noexcept {
		short _Result =
			InterlockedExchangeAdd16(_atomic_address_as<short>(this->_Value),
				static_cast<short>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_and(T _Operand) noexcept {
		short _Result =
			InterlockedAnd16(_atomic_address_as<short>(this->_Value), static_cast<short>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_or(T _Operand) noexcept {
		short _Result =
			InterlockedOr16(_atomic_address_as<short>(this->_Value), static_cast<short>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_xor(T _Operand) noexcept {
		short _Result =
			InterlockedXor16(_atomic_address_as<short>(this->_Value),
				static_cast<short>(_Operand));
		return static_cast<T>(_Result);
	}
	T operator++(int) noexcept {
		unsigned short _After =
			static_cast<unsigned short>(InterlockedIncrement16(_atomic_address_as<short>(this->_Value)));
		--_After;
		return static_cast<T>(_After);
	}
	T operator++() noexcept {
		return static_cast<T>(InterlockedIncrement16(_atomic_address_as<short>(this->_Value)));
	}
	T operator--(int) noexcept {
		unsigned short _After =
			static_cast<unsigned short>(InterlockedDecrement16(_atomic_address_as<short>(this->_Value)));
		++_After;
		return static_cast<T>(_After);
	}
	T operator--() noexcept {
		return static_cast<T>(InterlockedDecrement16(_atomic_address_as<short>(this->_Value)));
	}
};

template <class T, bool isRef>
struct _atomic_integral<T, 4, isRef> : _atomic_storage<T, isRef> { // CLAtomic integral operations using 4-byte intrinsics
protected:
	using _Base = _atomic_storage<T, isRef>;
	_atomic_integral() = default;
	constexpr _atomic_integral(const T _Value) noexcept : _Base(_Value) {}
	constexpr _atomic_integral(T& _Value, bool) noexcept : _Base(_Value, true) {}

	T self_add(T _Operand) noexcept {
		long _Result =
			InterlockedExchangeAdd(_atomic_address_as<long>(this->_Value),
				static_cast<long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_and(T _Operand) noexcept {
		long _Result =
			InterlockedAnd(_atomic_address_as<long>(this->_Value), static_cast<long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_or(T _Operand) noexcept {
		long _Result =
			InterlockedOr(_atomic_address_as<long>(this->_Value), static_cast<long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_xor(T _Operand) noexcept {
		long _Result =
			InterlockedXor(_atomic_address_as<long>(this->_Value), static_cast<long>(_Operand));
		return static_cast<T>(_Result);
	}

	T operator++(int) noexcept {
		unsigned long _After =
			static_cast<unsigned long>(InterlockedIncrement(_atomic_address_as<long>(this->_Value)));
		--_After;
		return static_cast<T>(_After);
	}

	T operator++() noexcept {
		return static_cast<T>(InterlockedIncrement(_atomic_address_as<long>(this->_Value)));
	}

	T operator--(int) noexcept {
		unsigned long _After =
			static_cast<unsigned long>(InterlockedDecrement(_atomic_address_as<long>(this->_Value)));
		++_After;
		return static_cast<T>(_After);
	}

	T operator--() noexcept {
		return static_cast<T>(InterlockedDecrement(_atomic_address_as<long>(this->_Value)));
	}
};

template <class T, bool isRef>
struct _atomic_integral<T, 8, isRef> : _atomic_storage<T, isRef> { // CLAtomic integral operations using 8-byte intrinsics
protected:
	using _Base = _atomic_storage<T, isRef>;
	_atomic_integral() = default;
	constexpr _atomic_integral(const T _Value) noexcept : _Base(_Value) {}
	constexpr _atomic_integral(T& _Value, bool) noexcept : _Base(_Value, true) {}

	T self_add(T _Operand) noexcept {
		long long _Result =
			InterlockedExchangeAdd64(
				_atomic_address_as<long long>(this->_Value), static_cast<long long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_and(T _Operand) noexcept {
		long long _Result =
			InterlockedAnd64(_atomic_address_as<long long>(this->_Value),
				static_cast<long long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_or(T _Operand) noexcept {
		long long _Result =
			InterlockedOr64(_atomic_address_as<long long>(this->_Value),
				static_cast<long long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_xor(T _Operand) noexcept {
		long long _Result =
			InterlockedXor64(_atomic_address_as<long long>(this->_Value),
				static_cast<long long>(_Operand));
		return static_cast<T>(_Result);
	}

	T operator++(int) noexcept {
		unsigned long long _After =
			static_cast<unsigned long long>(InterlockedIncrement64(_atomic_address_as<long long>(this->_Value)));
		--_After;
		return static_cast<T>(_After);
	}

	T operator++() noexcept {
		return static_cast<T>(InterlockedIncrement64(_atomic_address_as<long long>(this->_Value)));
	}

	T operator--(int) noexcept {
		unsigned long long _After =
			static_cast<unsigned long long>(InterlockedDecrement64(_atomic_address_as<long long>(this->_Value)));
		++_After;
		return static_cast<T>(_After);
	}

	T operator--() noexcept {
		return static_cast<T>(InterlockedDecrement64(_atomic_address_as<long long>(this->_Value)));
	}
};

template <class T, bool isRef = false>
struct _atomic_integral_facade : _atomic_integral<T, sizeof(T), isRef> {
protected:
	using _Base = _atomic_integral<T, sizeof(T), isRef>;
	using difference_type = T;
	_atomic_integral_facade() = default;
	constexpr _atomic_integral_facade(const T _Value) noexcept : _Base(_Value) {}
	constexpr _atomic_integral_facade(T& _Value, bool) noexcept : _Base(_Value, true) {}

public:
	T operator++(int)  noexcept {
		return isUseAtomic() ? const_cast<_atomic_integral_facade*>(this)->_Base::operator++(0) : _Value++;
	}

	T operator++()  noexcept {
		return isUseAtomic() ? const_cast<_atomic_integral_facade*>(this)->_Base::operator++() : ++_Value;
	}

	T operator--(int)  noexcept {
		return isUseAtomic() ? const_cast<_atomic_integral_facade*>(this)->_Base::operator--(0) : _Value--;
	}

	T operator--()  noexcept {
		return isUseAtomic() ? const_cast<_atomic_integral_facade*>(this)->_Base::operator--() : --_Value;
	}

	T operator+=(T _Operand) noexcept {
		return isUseAtomic() ? static_cast<T>(this->_Base::self_add(_Operand) + _Operand) : _Value += _Operand;
	}

	T operator-=(T _Operand) noexcept {
		return isUseAtomic() ? static_cast<T>(this->_Base::self_add(T(0) - _Operand) - _Operand) : _Value -= _Operand;
	}

	T operator&=(T _Operand) noexcept {
		return isUseAtomic() ? static_cast<T>(this->_Base::self_and(_Operand) & _Operand) : _Value &= _Operand;
	}

	T operator|=(T _Operand) noexcept {
		return isUseAtomic() ? static_cast<T>(this->_Base::self_or(_Operand) | _Operand) : _Value |= _Operand;
	}

	T operator^=(T _Operand) noexcept {
		return isUseAtomic() ? static_cast<T>(this->_Base::self_xor(_Operand) ^ _Operand) : _Value ^= _Operand;
	}

	template<class T2>
	T operator*=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value *= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp * _Operand));
		return _rt;
	}
	template<class T2>
	T operator/=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value /= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp / _Operand));
		return _rt;
	}

	T operator%=(int _Operand) noexcept {
		if (!isUseAtomic())return _Value %= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp % _Operand));
		return _rt;
	}
};

template <class T, bool isRef = false>
struct _atomic_floating : _atomic_storage<T, isRef> {
protected:
	// provides CLAtomic floating-point operations
	using _Base = _atomic_storage<T, isRef>;
	using difference_type = T;
	_atomic_floating() = default;
	constexpr _atomic_floating(const T _Value) noexcept : _Base(_Value) {}
	constexpr _atomic_floating(T& _Value, bool) noexcept : _Base(_Value, true) {}

public:
	template<class T2>
	T operator+=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value += _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp + _Operand));
		return _rt;
	}
	template<class T2>
	T operator-=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value -= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp - _Operand));
		return _rt;
	}
	template<class T2>
	T operator*=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value *= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp * _Operand));
		return _rt;
	}
	template<class T2>
	T operator/=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value /= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp / _Operand));
		return _rt;
	}
	template<size_t = sizeof(T)>
	T operator%=(int _Operand) noexcept { throw runtime_error; return 0; }
	template<>
	T operator%=<4>(int _Operand) noexcept {
		if (!isUseAtomic())return _Value = long(_Value) % _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = long(_Temp) % _Operand));
		return _rt;
	}
	template<>
	T operator%=<8>(int _Operand) noexcept {
		if (!isUseAtomic())return _Value = long long(_Value) % _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = long long(_Temp) % _Operand));
		return _rt;
	}

	T operator++() noexcept {
		if (!isUseAtomic())return ++_Value;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp + T(1)));
		return _rt;
	}

	T operator--() noexcept {
		if (!isUseAtomic())return --_Value;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp - T(1)));
		return _rt;
	}

	T operator++(int) noexcept {
		if (!isUseAtomic())return _Value++;
		T _Temp{ this->load() };
		while (!this->compare_exchange_strong(_Temp, _Temp + T(1)));
		return _Temp;
	}

	T operator--(int) noexcept {
		if (!isUseAtomic())return _Value--;
		T _Temp{ this->load() };
		while (!this->compare_exchange_strong(_Temp, _Temp - T(1)));
		return _Temp;
	}
};

//泛型化类型选择
template <class T>
using _choose_atomic_base_t =
typename _Select<is_integral_v<T> && !is_same_v<bool, T>>::template _Apply<_atomic_integral_facade<T>,
	typename _Select<is_floating_point_v<T>>::template _Apply<_atomic_floating<T>,
	_atomic_storage<T>>>;

template <class T>
using _choose_atomic_ref_t =
typename _Select<is_integral_v<T> && !is_same_v<bool, T>>::template _Apply<_atomic_integral_facade<T, true>,
	typename _Select<is_floating_point_v<T>>::template _Apply<_atomic_floating<T, true>,
	_atomic_storage<T, true>>>;

//泛型化类型选择（选择引用或值类型原子操作基类）
template <class T, bool isRef = false>
using _choose_atomic_base_or_ref_t =
typename _Select<isRef>::template _Apply<_choose_atomic_ref_t<T>, _choose_atomic_base_t<T>>;

//原子操作类（模板）
template <class T>
class CLAtomic :public _choose_atomic_base_or_ref_t<T> { // CLAtomic value
protected:
	using _Base = _choose_atomic_base_or_ref_t<T>;

public:
	// clang-format off
	static_assert(is_trivially_copyable_v<T>&& is_copy_constructible_v<T>&& is_move_constructible_v<T>
		&& is_copy_assignable_v<T>&& is_move_assignable_v<T>,
		"CLAtomic<T> requires T to be trivially copyable, copy constructible, move constructible, copy assignable, "
		"and move assignable.");
	// clang-format on

	using value_type = T;

	//标准构造，初始状态标记对象为执行原子操作的；
	constexpr CLAtomic(const T _Value) noexcept : _Base(_Value) {}

	//默认构造，初始状态标记对象为执行原子操作的；
	constexpr CLAtomic() noexcept(is_nothrow_default_constructible_v<T>) : _Base() {}

	//拷贝构造，初始状态标记对象为执行原子操作的，并忽略原对象的原子操作标记状态，不予复制；
	CLAtomic(const CLAtomic& v)
		: _Base(v.load()) {
	}
	using _Base::load;
	using _Base::store;
	//拷贝构造，初始状态标记对象为执行原子操作的，并忽略原对象的原子操作标记状态，不予复制；
	template<class T2>
	CLAtomic(const CLAtomic<T2>& v)
		: _Base(v.load()) {
	}
	T operator=(const T v2) {
		return _Value = v2;
	}
	template<class T2>
	T operator=(const T2 v2) {
		return _Value = v2;
	}

	//赋值，并忽略原对象的原子操作标记状态，不予复制；
	T operator=(const CLAtomic& v2) {
		return _Value = v2.load();
	}

	//赋值，并忽略原对象的原子操作标记状态，不予复制；
	template<class T2>
	T operator=(const CLAtomic<T2>& v2) {
		return _Value = v2.load();
	}

	operator T() const noexcept {
		return this->load();
	}
};

//原子操作引用类（模板），该类将在初始化阶段唯一的绑定一个对应类型的数据类型
template <class T>
class CLAtomicRef :public _choose_atomic_base_or_ref_t<T, true> { // CLAtomic value
protected:
	using _Base = _choose_atomic_base_or_ref_t<T, true>;

public:
	// clang-format off
	static_assert(is_trivially_copyable_v<T>&& is_copy_constructible_v<T>&& is_move_constructible_v<T>
		&& is_copy_assignable_v<T>&& is_move_assignable_v<T>,
		"CLAtomic<T> requires T to be trivially copyable, copy constructible, move constructible, copy assignable, "
		"and move assignable.");
	// clang-format on

	using value_type = T;

	//标准构造，初始状态标记对象为执行原子操作的；
	constexpr CLAtomicRef(T& _Value) noexcept : _Base(_Value, true) {}

	//默认构造，初始状态标记对象为执行原子操作的；
	constexpr CLAtomicRef() noexcept(is_nothrow_default_constructible_v<T>) = delete;

	//拷贝构造，初始状态标记对象为执行原子操作的，并忽略原对象的原子操作标记状态，不予复制；
	CLAtomicRef(const CLAtomicRef& v)
		: _Base(v._Value, true) {
	}

	using _Base::load;
	using _Base::store;

	T operator=(const T v2) {
		return _Value = v2;
	}
	template<class T2>
	T operator=(const T2 v2) {
		return _Value = v2;
	}

	//赋值，并忽略原对象的原子操作标记状态，不予复制；
	T operator=(const CLAtomicRef& v2) {
		return _Value = v2.load();
	}
	template<class T2>
	T operator=(const CLAtomicRef<T2>& v2) {
		return _Value = v2.load();
	}
	T operator=(const CLAtomic<T>& v2) {
		return _Value = v2.load();
	}
	//赋值，并忽略原对象的原子操作标记状态，不予复制；
	template<class T2>
	T operator=(const CLAtomic<T2>& v2) {
		return _Value = v2.load();
	}
	operator T() const noexcept {
		return this->load();
	}
};

//旧的原子操作类（模板,非浮点类型执行速度慢）
template<class T>
class __CLAtomic_Old {
	static_assert(sizeof(T) % 2 == 0, "__CLAtomic_Old: target type is not support!");
protected:
	T Target;
	bool bUseAtomc;


	inline static bool atomicCAS(volatile SHORT* dest, SHORT newvalue, SHORT oldvalue)
	{
		return InterlockedCompareExchange16(dest, newvalue, oldvalue) == oldvalue ? true : false;
	}
	inline static bool atomicCAS(volatile USHORT* dest, USHORT newvalue, USHORT oldvalue)
	{
		return atomicCAS(reinterpret_cast<SHORT volatile*>(dest), SHORT(newvalue), SHORT(oldvalue));
	}
	inline static bool atomicCAS(volatile INT* dest, INT newvalue, INT oldvalue)
	{
		return InterlockedCompareExchange((LONG volatile*)dest, (LONG)newvalue, (LONG)oldvalue) == (LONG)oldvalue ? true : false;
	}
	inline static bool atomicCAS(volatile UINT* dest, UINT newvalue, UINT oldvalue)
	{
		return atomicCAS(reinterpret_cast<INT volatile*>(dest), INT(newvalue), INT(oldvalue));
	}
	inline static bool atomicCAS(volatile LONG* dest, LONG newvalue, LONG oldvalue)
	{
		return InterlockedCompareExchange(dest, newvalue, oldvalue) == oldvalue ? true : false;
	}
	inline static bool atomicCAS(volatile ULONG* dest, ULONG newvalue, ULONG oldvalue)
	{
		return atomicCAS(reinterpret_cast<LONG volatile*>(dest), LONG(newvalue), LONG(oldvalue));
	}
	inline static bool atomicCAS(volatile LONG64* dest, LONG64 newvalue, LONG64 oldvalue)
	{
		return InterlockedCompareExchange64(dest, newvalue, oldvalue) == oldvalue ? true : false;
	}
	inline static bool atomicCAS(volatile ULONG64* dest, ULONG64 newvalue, ULONG64 oldvalue)
	{
		return atomicCAS(reinterpret_cast<LONG64 volatile*>(dest), LONG64(newvalue), LONG64(oldvalue));
	}
	inline static bool atomicCAS(volatile FLOAT* dest, FLOAT newvalue, FLOAT oldvalue)
	{
		auto old1 = *(LONG*)(void*)&oldvalue;
		return InterlockedCompareExchange(reinterpret_cast<LONG volatile*>(dest), *(LONG*)(void*)&newvalue, old1) == old1 ? true : false;
	}
	inline static bool atomicCAS(volatile DOUBLE* dest, DOUBLE newvalue, DOUBLE oldvalue)
	{
		auto old1 = *(LONG64*)(void*)&oldvalue;
		return InterlockedCompareExchange64(reinterpret_cast<LONG64 volatile*>(dest), *(LONG64*)(void*)&newvalue, old1) == old1 ? true : false;
	}
	inline void set(T dest) {
		throw logic_error("__CLAtomic_Old::set is not impliment!");
		Target = dest;
	}
	inline T increment()
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old + T(1), old));
		return old;
	}
	inline T decrement()
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old - T(1), old));
		return old;
	}
	inline T add(volatile T v)
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old + v, old));
		return old;
	}
	inline T mul(volatile T v)
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old * v, old));
		return old;
	}
	inline T div(volatile T v)
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old / v, old));
		return old;
	}
	inline T surplus(volatile int v)
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old % int(v), old));
		return old;
	}
public:
	using value_type = T;

	inline __CLAtomic_Old() :bUseAtomc(CLAtomic_bUseAtomc_def) {
	}
	inline __CLAtomic_Old(T v, bool _bUseAtomc = CLAtomic_bUseAtomc_def)
		: Target(v), bUseAtomc(_bUseAtomc) {
	}
	inline __CLAtomic_Old(const __CLAtomic_Old& v)
		: Target(v.Target), bUseAtomc(v.bUseAtomc) {
	}
	template<class T2>
	inline __CLAtomic_Old(const __CLAtomic_Old<T2>& v)
		: Target(v()), bUseAtomc(v.isUseAtomic()) {
	}
	template<class T2>
	inline __CLAtomic_Old(T2 v)
		: Target(T(v)), bUseAtomc(CLAtomic_bUseAtomc_def) {
	}
	__CLAtomic_Old& setUseAtomic(bool _bUseAtomc = CLAtomic_bUseAtomc_def) {
		return bUseAtomc = _bUseAtomc, *this;
	}
	bool isUseAtomic() const {
		return bUseAtomc;
	}
	inline __CLAtomic_Old& operator=(T v2) {
		return Target = v2, *this;
	}
	inline __CLAtomic_Old& operator=(const __CLAtomic_Old& v2) {
		return Target = v2.Target, *this;
	}
	template<class T2>
	inline __CLAtomic_Old& operator=(T2 v2) {
		return Target = v2, *this;
	}
	template<class T2>
	inline __CLAtomic_Old& operator=(const __CLAtomic_Old<T2>& v2) {
		return Target = v2(), *this;
	}
	//默认为原子操作++()，当setUseAtomic(false)时采用非原子操作方式；
	inline __CLAtomic_Old& operator++() {
		return bUseAtomc ? increment() : ++Target, * this;
	}
	//默认为原子操作--()，当setUseAtomic(false)时采用非原子操作方式；
	inline __CLAtomic_Old& operator--() {
		return bUseAtomc ? decrement() : --Target, * this;
	}
	//默认为原子操作()++，当setUseAtomic(false)时采用非原子操作方式；
	inline T operator++(int) {
		if (bUseAtomc) {
			return increment();
		}
		else {
			T old = Target;
			++Target;
			return old;
		}
	}
	//默认为原子操作()--，当setUseAtomic(false)时采用非原子操作方式；
	inline T operator--(int) {
		if (bUseAtomc) {
			return decrement();
		}
		else {
			T old = Target;
			--Target;
			return old;
		}
	}
	//默认为原子操作+=，当setUseAtomic(false)时采用非原子操作方式；
	inline __CLAtomic_Old& operator+=(T v) {
		return  bUseAtomc ? add(v) : Target += v, *this;
	}
	//默认为原子操作-=，当setUseAtomic(false)时采用非原子操作方式；
	inline __CLAtomic_Old& operator-=(T v) {
		return  bUseAtomc ? add(0 - v) : Target -= v, *this;
	}
	//默认为原子操作*=，当setUseAtomic(false)时采用非原子操作方式；
	inline __CLAtomic_Old& operator*=(T v) {
		return  bUseAtomc ? mul(v) : Target *= v, *this;
	}
	//默认为原子操作/=，当setUseAtomic(false)时采用非原子操作方式；
	inline __CLAtomic_Old& operator/=(T v) {
		return  bUseAtomc ? div(v) : Target /= v, *this;
	}
	//默认为原子操作%=，当setUseAtomic(false)时采用非原子操作方式；
	inline  __CLAtomic_Old& operator%=(int v) {
		return  bUseAtomc ? surplus(v) : Target %= v, *this;
	}
	inline operator T() const noexcept { return Target; }
	inline operator T& () noexcept { return Target; }
	inline T operator()() const noexcept { return Target; }
	template<class T2> inline T operator+(const T2 v) const { return Target + v; }
	template<class T2> inline T operator-(const T2 v) const { return Target - v; }
	template<class T2> inline T operator*(const T2 v) const { return Target * v; }
	template<class T2> inline T operator/(const T2 v) const { return Target / v; }
	template<class T2> inline T operator%(const T2 v) const { return Target % (int)v; }
};

#endif