#include <iostream>
#include <fstream>

#include "instance.hpp"
#include "treewidth.hpp"
#include "preprocessor.hpp"
#include "utils.hpp"
#include "dtree.hpp"

using namespace vtree;

void InputError() {
	cout<<"Error!"<<endl;
	cout<<"Usage: ./vtree -out_cnf out.cnf -out_dtree out.dtree input.cnf"<<endl;
	cout<<"The default format is c2d dtree. Use -minic2d to use minic2d vtree."<<endl;
	exit(1);
}

int MakeDtree(Var v, Dtree& dt, const TdDec& td, const vector<vector<int>>& td_cls) {
	size_t vars = td_cls.size()-1;
	assert(v >= 1 && v <= vars);
	int root = -1;
	for (Var ch : td.Children(v)) {
		int tr = MakeDtree(ch, dt, td, td_cls);
		if (tr != -1) {
			if (root == -1) {
				root = tr;
			} else {
				root = dt.Merge(root, tr);
			}
		}
	}
	for (int tcl : td_cls[v]) {
		if (root == -1) {
			root = tcl;
		} else {
			root = dt.Merge(root, tcl);
		}
	}
	return root;
}

int main(int argc, char** argv) {
	std::ios_base::sync_with_stdio(0);
	std::cin.tie(0);
	if (argc < 6) {
		InputError();
	}
	// Read input
	string inst_file;
	double tdtime = -1;
	string out_cnf, out_dtree;
	bool minic2d = false;
	for (int i = 1; i < argc; i++) {
		string arg(argv[i]);
		if (arg == "-minic2d") {
			minic2d = true;
		} else if (arg == "-decot") {
			assert(i+1 < argc);
			string tmp(argv[++i]);
			tdtime = stof(tmp);
			assert(tdtime >= 0.1 && tdtime <= 30000);
			cerr<<"-decot "<<tdtime<<": Time limit set for decomposition"<<endl;
		} else if (arg == "-out_cnf") {
			assert(i+1 < argc);
			out_cnf = string(argv[++i]);
			cerr<<"-out_cnf "<<out_cnf<<": Output CNF file"<<endl;
		} else if (arg == "-out_dtree") {
			assert(i+1 < argc);
			out_dtree = string(argv[++i]);
			cerr<<"-out_dtree "<<out_dtree<<": Output dtree file"<<endl;
		} else {
			if (!inst_file.empty()) {
				InputError();
			}
			inst_file = arg;
			cerr<<"Instance: "<<inst_file<<endl;
		}
	}
	if (inst_file.empty() || out_cnf.empty() || out_dtree.empty()) {
		InputError();
	}
	Instance ins(inst_file);
	cerr<<"Instance"<<endl;
	ins.PrintInfo();

	// Preprocessing
	Preprocessor pp;
	ins = pp.Preprocess(ins);
	if (pp.Unsat()) {
		cerr<<"UNSAT by preprocessing"<<endl;
		assert(ins.vars == 2);
		assert(ins.clauses.size() == 2);
		std::ofstream outc(out_cnf);
		ins.Print(outc);
		outc.close();
		if (minic2d) {
			Dtree dt(2);
			dt.Merge(0, 1);
			std::ofstream outd(out_dtree);
			dt.Print(outd, true);
			outd.close();
		} else {
			Dtree dt(2);
			dt.Merge(0, 1);
			std::ofstream outd(out_dtree);
			dt.Print(outd, false);
			outd.close();
		}
		return 0;
	}
	if (ins.vars == 0) {
		cerr<<"Unique solution by preprocessing"<<endl;
		Instance tins(2 + pp.FreeVars());
		tins.AddClause({NegLit(1)});
		tins.AddClause({NegLit(2)});
		assert(tins.clauses.size() == 2);
		std::ofstream outc(out_cnf);
		tins.Print(outc);
		outc.close();
		if (minic2d) {
			Dtree dt(2 + pp.FreeVars());
			int rt = 0;
			for (int i = 1; i < 2 + pp.FreeVars(); i++) {
				rt = dt.Merge(rt, i);
			}
			assert(rt == (int)(2+pp.FreeVars())*2-2);
			std::ofstream outd(out_dtree);
			dt.Print(outd, true);
			outd.close();
		} else {
			Dtree dt(2);
			dt.Merge(0, 1);
			std::ofstream outd(out_dtree);
			dt.Print(outd, false);
			outd.close();
		}
		return 0;
	}
	assert(ins.vars >= 2);
	cerr<<"Preprocessed instance"<<endl;
	cerr<<"Free vars: "<<pp.FreeVars()<<endl;
	ins.PrintInfo();

	// Break into components
	vector<Instance> ins_comps = pp.Components(ins);
	// Instances with most vars first
	std::sort(ins_comps.begin(), ins_comps.end(),
		[](const Instance& a, const Instance& b){
			return a.vars > b.vars;
		});

	double td_time_per_var = 0.01;

	if (tdtime > 0) {
		td_time_per_var = tdtime / (double)ins.vars;
	}

	size_t tot_vars = 0;
	size_t tot_clauses = 0;
	for (Instance& tins : ins_comps) {
		assert(tins.vars >= 2 && tins.clauses.size() >= 1);
		tot_vars += tins.vars;
		tot_clauses += tins.clauses.size();
	}
	size_t cur_vars = 0;
	size_t cur_clauses = 0;
	Instance out_ins(tot_vars + pp.FreeVars());
	Dtree out_dt;
	if (minic2d) {
		out_dt = Dtree(tot_vars + pp.FreeVars());
	} else {
		out_dt = Dtree(tot_clauses);
	}
	vector<int> dt_roots;
	int groot = -1;
	for (Instance& tins : ins_comps) {
		cerr<<"Processing component"<<endl;
		tins.PrintInfo();
		// Compute tree decomposition
		TdDec td_dec(tins, td_time_per_var*(double)tins.vars);
		// Change variable ordering to dfs order
		td_dec.OrderInstance(tins);
		// Add clauses to out_ins
		for (auto clause : tins.clauses) {
			assert(clause.size() >= 2);
			for (Lit& lit : clause) {
				lit += 2*cur_vars;
			}
			out_ins.AddClause(clause);
		}

		vector<vector<int>> td_cls(tins.vars+1);
		if (minic2d) {
			for (int i = 0; i < tins.vars; i++) {
				td_cls[i+1].push_back(i + cur_vars);
			}
		} else {
			for (int i = 0; i < (int)tins.clauses.size(); i++) {
				Var mv = 0;
				for (Lit lit : tins.clauses[i]) {
					mv = max(mv, VarOf(lit));
				}
				assert(mv > 0);
				td_cls[mv].push_back(i + cur_clauses);
			}
			assert(td_cls[0].empty() && td_cls[1].empty());
		}
		cur_vars += tins.vars;
		cur_clauses += tins.clauses.size();

		int root = MakeDtree(1, out_dt, td_dec, td_cls);
		if (groot == -1) {
			groot = root;
		} else {
			groot = out_dt.Merge(groot, root);
		}
	}
	if (minic2d) {
		for (int v = tot_vars; v < tot_vars+pp.FreeVars(); v++) {
			groot = out_dt.Merge(groot, v);
		}
		assert(groot == (int)(tot_vars+pp.FreeVars())*2-2);
	} else {
		assert(groot == (int)tot_clauses*2-2);
	}
	std::ofstream outc(out_cnf);
	out_ins.Print(outc);
	outc.close();
	std::ofstream outd(out_dtree);
	out_dt.Print(outd, minic2d);
	outd.close();
	return 0;
}