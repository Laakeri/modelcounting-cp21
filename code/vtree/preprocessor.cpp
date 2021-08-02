#include "preprocessor.hpp"


namespace vtree {
namespace {
Instance UnsatInst() {
	Instance ins(2);
	ins.AddClause({NegLit(1)});
	ins.AddClause({PosLit(1)});
	return ins;
}

bool IsSat(const vector<Lit>& clause, const vector<char>& assign) {
	for (int i = 0; i < (int)clause.size(); i++) {
		if (assign[VarOf(clause[i])] == 1 && IsPos(clause[i])) {
			return true;
		}
		if (assign[VarOf(clause[i])] == 2 && IsNeg(clause[i])) {
			return true;
		}
	}
	return false;
}
} // namespace

bool Preprocessor::Propagate(const Instance& ins) {
	for (int i = 0; i < (int)prop_q.size(); i++) {
		Lit ff = prop_q[i];
		Var v = VarOf(ff);
		if (IsPos(ff)) {
			assert(assign[v] == 2);
		} else {
			assert(assign[v] == 1);
		}
		for (int cl_id : occ_list[ff]) {
			assert(cl_id >= 0 && cl_id < (int)ins.clauses.size());
			const auto& clause = ins.clauses[cl_id];
			if (IsSat(clause, assign)) {
				continue;
			}
			int ass = 0;
			bool fo = false;
			for (Lit lit : clause) {
				if (assign[VarOf(lit)]) {
					ass++;
				}
				if (lit == ff) {
					fo = true;
				}
			}
			assert(fo);
			assert(ass <= (int)clause.size());
			if (ass == (int)clause.size()) {
				return true;
			}
			if (ass+1 == (int)clause.size()) {
				for (Lit lit : clause) {
					if (!assign[VarOf(lit)]) {
						ass++;
						prop_q.push_back(Neg(lit));
						if (IsPos(lit)) {
							assign[VarOf(lit)] = 1;
						} else {
							assign[VarOf(lit)] = 2;
						}
					}
				}
				assert(ass == (int)clause.size());
			}
		}
	}
	return false;
}

bool Preprocessor::FailedLiterals(const Instance& ins) {
	cerr<<"pp: FL"<<endl;
	assert(!unsat);
	int fo = 0;
	for (Var i = 1; i <= ins.vars; i++) {
		if (assign[i]) continue;
		for (int test=1;test<=2;test++){
			assert(!assign[i]);
			assert(prop_q.empty());
			assign[i] = test;
			if (test == 1) {
				prop_q.push_back(NegLit(i));
			} else {
				prop_q.push_back(PosLit(i));
			}
			bool fail = Propagate(ins);
			while (!prop_q.empty()) {
				Var v = VarOf(prop_q.back());
				prop_q.pop_back();
				assert(assign[v]);
				assign[v] = 0;
			}
			if (fail) {
				fo = i;
				assign[i] = 3-test;
				if (test == 1) {
					prop_q.push_back(PosLit(i));
				} else {
					prop_q.push_back(NegLit(i));
				}
				unsat = Propagate(ins);
				prop_q.clear();
				if (unsat) {
					return true;
				}
				assert(assign[i]);
				assert(prop_q.empty());
				break;
			}
		}
	}
	assert(!unsat);
	assert(fo == 0 || assign[fo]);
	return fo;
}

bool Preprocessor::UnitProp(const Instance& ins) {
	cerr<<"pp: UP"<<endl;
	assert(prop_q.empty());
	assert(!unsat);
	for (int i = 0; i < (int)ins.clauses.size(); i++) {
		const auto& clause = ins.clauses[i];
		if (IsSat(clause, assign)) {
			continue;
		}
		int ass = 0;
		for (Lit lit : clause) {
			if (assign[VarOf(lit)]) {
				ass++;
			}
		}
		assert(ass <= (int)clause.size());
		if (ass == (int)clause.size()) {
			// UNSAT
			unsat = true;
			return true;
		}
		if (ass+1 == (int)clause.size()) {
			for (Lit lit : clause) {
				if (!assign[VarOf(lit)]) {
					ass++;
					prop_q.push_back(Neg(lit));
					if (IsPos(lit)) {
						assign[VarOf(lit)] = 1;
					} else {
						assign[VarOf(lit)] = 2;
					}
				}
			}
			assert(ass == (int)clause.size());
		}
	}
	bool fo = prop_q.size() > 0;
	assert(!unsat);
	unsat = Propagate(ins);
	prop_q.clear();
	return fo;
}

Instance Preprocessor::Preprocess(Instance ins) {
	assert(assign.empty());
	assign.resize(ins.vars + 1);
	occ_list.resize(ins.vars*2 + 2);
	for (int i = 0; i < (int)ins.clauses.size(); i++) {
		auto& clause = ins.clauses[i];
		for (Lit lit : clause) {
			occ_list[lit].push_back(i);
		}
	}

	// Preprocessing techniques start
	bool fo = true;
	UnitProp(ins);
	if (unsat) {
		return UnsatInst();
	}
	while (fo) {
		fo = FailedLiterals(ins);
		if (unsat) {
			return UnsatInst();
		}
	}
	// Preprocessing techniques end

	// Remove assigned lits and satisfied clauses
	for (Var i = 1; i <= ins.vars; i++) {
		if (assign[i]) {
			assigned++;
		}
	}
	for (int i = 0; i < (int)ins.clauses.size(); i++) {
		auto& clause = ins.clauses[i];
		if (IsSat(clause, assign)) {
			SwapDel(ins.clauses, i);
			i--;
			continue;
		}
		for (int j = 0; j < (int)clause.size(); j++) {
			Var v = VarOf(clause[j]);
			if (assign[v]) {
				if (IsPos(clause[j])) {
					assert(assign[v] == 2);
				} else {
					assert(assign[v] == 1);
				}
				SwapDel(clause, j);
				j--;
			}
		}
		assert(clause.size() >= 2);
	}

	// Compress variables
	vector<Lit> real_vars;
	for (const auto& clause : ins.clauses) {
		for (Lit lit : clause) {
			assert(assign[VarOf(lit)] == 0);
			real_vars.push_back(VarOf(lit));
		}
	}
	SortAndDedup(real_vars);
	assert(assigned + real_vars.size() <= ins.vars);
	free_vars = ins.vars - assigned - real_vars.size();
	ins.vars = real_vars.size();
	for (auto& clause : ins.clauses) {
		for (Lit& lit : clause) {
			Var nvar = Ind(real_vars, VarOf(lit))+1;
			if (IsPos(lit)) {
				lit = PosLit(nvar);
			} else {
				lit = NegLit(nvar);
			}
			assert(1 <= VarOf(lit) && VarOf(lit) <= ins.vars);
		}
		sort(clause.begin(), clause.end());
		for (int i = 1; i < (int)clause.size(); i++) {
			assert(VarOf(clause[i]) != VarOf(clause[i-1]));
		}
	}
	ins.UpdClauseInfo();
	return ins;
}

bool Preprocessor::Unsat() const {
	return unsat;
}

uint32_t Preprocessor::FreeVars() const {
	return free_vars;
}

namespace {
void Dfs(int x, const vector<vector<Var>>& g, vector<int>& u, int cc) {
  if (u[x]) return;
  u[x] = cc;
  for (int nx : g[x]) {
    Dfs(nx, g, u, cc);
  }
}
} // namespace

vector<Instance> Preprocessor::Components(const Instance& ins) const {
	auto graph = ins.PrimalGraph();
	vector<int> u(ins.vars+1);
	int cc = 0;
	for (Var i = 1; i <= ins.vars; i++) {
		if (!u[i]) {
			cc++;
			Dfs(i, graph, u, cc);
		}
	}
	assert(cc >= 1);
	cerr<<"Components: "<<cc<<endl;
	if (cc == 1) {
		return {ins};
	}
	vector<uint32_t> cvars(cc+1);
	vector<Var> vmap(ins.vars+1);
	for (Var i = 1; i <= ins.vars; i++) {
		assert(1 <= u[i] && u[i] <= cc);
		cvars[u[i]]++;
		vmap[i] = cvars[u[i]];
	}
	vector<Instance> instances;
	for (int i = 1; i <= cc; i++) {
		assert(cvars[i] >= 2);
		instances.push_back(Instance(cvars[i]));
	}
	for (const auto& clause : ins.clauses) {
		int tcc = 0;
		vector<Lit> mclause;
		for (Lit lit : clause) {
			if (tcc == 0) {
				tcc = u[VarOf(lit)];
			} else {
				assert(u[VarOf(lit)] == tcc);
			}
			if (IsPos(lit)) {
				mclause.push_back(PosLit(vmap[VarOf(lit)]));
			} else {
				mclause.push_back(NegLit(vmap[VarOf(lit)]));
			}
		}
		instances[tcc-1].AddClause(mclause);
	}
	for (auto& inst : instances) {
		assert(inst.vars >= 2);
		assert(inst.clauses.size() >= 1);
		inst.UpdClauseInfo();
	}
	return instances;
}
} // namespace vtree