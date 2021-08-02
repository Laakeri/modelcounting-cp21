#include <istream>
#include <fstream>
#include <cctype>

#include "instance.hpp"
#include "utils.hpp"

namespace vtree {
namespace {
vector<string> Tokens(string t) {
	vector<string> ret;
	ret.push_back("");
	for (char c : t) {
		if (std::isspace(c)) {
			if (!ret.back().empty()) {
				ret.push_back("");
			}
		} else {
			ret.back() += c;
		}
	}
	if (ret.back().empty()) ret.pop_back();
	return ret;
}
} // namespace

void Instance::AddClause(vector<Lit> clause) {
	assert(!clause.empty());
	for (Lit l : clause) {
		assert(1 <= VarOf(l) && VarOf(l) <= vars);
	}
	SortAndDedup(clause);
	for (size_t i = 1; i < clause.size(); i++) {
		if (VarOf(clause[i-1]) == VarOf(clause[i])) {
			// Tautology
			return;
		}
	}
	clauses.push_back(clause);
}

void Instance::UpdClauseInfo() {
	total_lits = 0;
	unit_clauses = 0;
	binary_clauses = 0;
	for (const auto& clause : clauses) {
		assert(clause.size() > 0);
		if (clause.size() == 1) {
			unit_clauses++;
		} else if (clause.size() == 2) {
			binary_clauses++;
		}
		total_lits += clause.size();
		if (total_lits >= (1<<30)) {
			// Do not support so large formulas
			assert(0);
		}
	}
}

Instance::Instance(uint32_t vars_) : vars(vars_) {}

Instance::Instance(string file) {
	std::ifstream in(file);
	string tmp;
	vector<Lit> cur_clause;
	int pline_clauses = 0;
	int read_clauses = 0;
	while (std::getline(in, tmp)) {
		if (tmp.empty()) continue;
		if (tmp[0] == 'c') continue;
		auto tokens = Tokens(tmp);
		if (tokens.empty()) continue;
		if (tokens[0] == "c") continue;
		if (format == 0 && tokens.size() == 4 && tokens[0] == "p" && tokens[1] == "cnf") {
			format = 1;
			cerr<<"CNF input"<<endl;
			assert(IsInt(tokens[2], 0, 1<<30));
			vars = stoi(tokens[2]);
			assert(IsInt(tokens[3], 0, 1<<30));
			pline_clauses = stoi(tokens[3]);
		} else if (format == 1 && IsInt(tokens[0])) {
			for (string& t : tokens) {
				assert(IsInt(t, -(1<<30), 1<<30));
				int64_t dlit = stoll(t);
				if (dlit == 0) {
					read_clauses++;
					AddClause(cur_clause);
					cur_clause.clear();
				} else {
					dlit = FromDimacs(dlit);
					cur_clause.push_back(dlit);
				}
			}
		}
	}
	assert(cur_clause.empty());
	if (pline_clauses != read_clauses) {
		cerr<<"Warning: p line mismatch. Claimed clauses: "<<pline_clauses<<" actual clauses: "<<read_clauses<<endl;
	}
	UpdClauseInfo();
}

void Instance::PrintInfo() const {
	cerr<<"Vars: "<<vars<<endl;
	cerr<<"Clauses: "<<clauses.size()<<endl;
	cerr<<"Total literals: "<<total_lits<<endl;
	cerr<<"Unit clauses: "<<unit_clauses<<endl;
	cerr<<"Binary clauses: "<<binary_clauses<<endl;
	cerr<<endl;
}

vector<vector<Var>> Instance::PrimalGraph() const {
  vector<vector<Var>> graph(vars+1);
	// Create graph
	for (const auto& clause : clauses) {
		for (size_t i = 0; i < clause.size(); i++) {
			for (size_t ii = i+1; ii < clause.size(); ii++) {
				Var v1 = VarOf(clause[i]);
				Var v2 = VarOf(clause[ii]);
				assert(1 <= v1 && v1 < v2 && v2 <= vars);
				graph[v1].push_back(v2);
				graph[v2].push_back(v1);
			}
		}
	}
	for (Var i = 1; i <= vars; i++) {
		SortAndDedup(graph[i]);
	}
	return graph;
}

void Instance::Print(std::ostream& out) const {
	out<<"p cnf "<<vars<<" "<<clauses.size()<<endl;
	for (const auto& clause : clauses) {
		for (Lit lit : clause) {
			out<<ToDimacs(lit)<<" ";
		}
		out<<0<<'\n';
	}
	out<<std::flush;
}
} // namespace vtree