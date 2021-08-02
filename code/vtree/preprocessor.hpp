#pragma once

#include "utils.hpp"
#include "instance.hpp"

#include <map>

namespace vtree {
// Very simple preprocessor. 
// Currently just unit propagation, failed literal probing, and compress variables
class Preprocessor {
 public:
 	Instance Preprocess(Instance ins);
 	bool Unsat() const;
 	vector<Instance> Components(const Instance& ins) const;
 	uint32_t FreeVars() const;
 private:
 	bool FailedLiterals(const Instance& ins);
 	bool Propagate(const Instance& ins);
 	bool UnitProp(const Instance& ins);
 	vector<Lit> prop_q;

 	vector<vector<int>> occ_list;
 	vector<Lit> var_map;
 	vector<char> assign;
 	uint32_t free_vars = 0;
 	uint32_t assigned = 0;
 	bool unsat = false;
 	int propagations = 0;
};
} // namespace vtree