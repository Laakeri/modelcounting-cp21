#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <limits>
#include <algorithm>
#include <chrono>

namespace vtree {
#define F first
#define S second
typedef uint32_t Lit;
typedef uint32_t Var;

using std::vector;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::max;
using std::min;
using std::pair;
using std::to_string;
using std::swap;

inline Lit Neg(Lit x) {
	return x^1;
}

inline Var VarOf(Lit x) {
	return x/2;
}

inline Lit PosLit(Var x) {
	return x*2;
}

inline Lit NegLit(Var x) {
	return x*2+1;
}

inline Lit MkLit(Var var, bool phase) {
	if (phase) {
		return PosLit(var);
	} else {
		return NegLit(var);
	}
}

inline bool IsPos(Lit x) {
	return !(x&1);
}

inline bool IsNeg(Lit x) {
	return x&1;
}

inline Lit FromDimacs(int64_t x) {
	assert(x != 0);
	assert(std::llabs(x) < (1ll << 30ll));
	if (x > 0) {
		return PosLit(x);
	} else {
		return NegLit(-x);
	}
}

inline int64_t ToDimacs(Lit x) {
	assert(x >= 2);
	if (x&1) {
		return -(int64_t)VarOf(x);
	} else {
		return VarOf(x);
	}
}

inline bool IsInt(const string& s, int64_t lb=std::numeric_limits<int64_t>::min(), int64_t ub=std::numeric_limits<int64_t>::max()) {
  try {
    int x = std::stoll(s);
    return lb <= x && x <= ub;
  } catch (...) {
    return false;
  }
}

template<typename T>
T Power2(int p) {
	assert(p >= 0);
	if (p == 0) return 1;
	if (p%2 == 0) {
		T x = Power2<T>(p/2);
		return x*x;
	} else {
		return Power2<T>(p-1)*2;
	}
}

template<typename T>
void SortAndDedup(vector<T>& vec) {
  std::sort(vec.begin(), vec.end());
  vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

template<typename T>
void SwapDel(vector<T>& vec, size_t i) {
	assert(i < vec.size());
	std::swap(vec[i], vec.back());
	vec.pop_back();
}

template<typename T>
void ShiftDel(vector<T>& vec, size_t i) {
	assert(i < vec.size());
	for (; i+1 < vec.size(); i++) {
		vec[i] = std::move(vec[i+1]);
	}
	vec.pop_back();
}

template<typename T>
int Ind(const std::vector<T>& a, const T& x) {
	int ind = std::lower_bound(a.begin(), a.end(), x) - a.begin();
	assert(a[ind] == x);
	return ind;
}

template<typename T>
bool BS(const std::vector<T>& a, const T x) {
  return std::binary_search(a.begin(), a.end(), x);
}

class Timer {
 private:
  bool timing;
  std::chrono::duration<double> elapsedTime;
  std::chrono::time_point<std::chrono::steady_clock> startTime;
 public:
  Timer();
  void start();
  void stop();
  void clear();
  double get();
};

inline Timer::Timer() {
  timing = false;
  elapsedTime = std::chrono::duration<double>(std::chrono::duration_values<double>::zero());
}

inline void Timer::start() {
  if (timing) return;
  timing = true;
  startTime = std::chrono::steady_clock::now();
}

inline void Timer::stop() {
  if (!timing) return;
  timing = false;
  std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::steady_clock::now();
  elapsedTime += (endTime - startTime);
}

inline double Timer::get() {
  if (timing) {
    stop();
    std::chrono::duration<double> ret = elapsedTime;
    start();
    return ret.count();
  }
  else {
    return elapsedTime.count();
  }
}

inline void Timer::clear() {
  elapsedTime = std::chrono::duration<double>(std::chrono::duration_values<double>::zero());
}
} // namespace vtree