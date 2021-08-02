#pragma once

#include <vector>
#include <ostream>

#include "utils.hpp"

namespace vtree {
struct Instance {
	Instance(string file);
	Instance(uint32_t vars_);

	void AddClause(vector<Lit> clause);
	void UpdClauseInfo();

	vector<vector<Var>> PrimalGraph() const;
	void PrintInfo() const;

	void Print(std::ostream& out) const;

	vector<vector<Lit>> clauses;
	uint32_t vars = 0;
 private:
 	int format = 0;
 	uint32_t unit_clauses = 0;
	uint32_t binary_clauses = 0;
	uint32_t total_lits = 0;
};
} // namespace vtree