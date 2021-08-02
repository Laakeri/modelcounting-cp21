#pragma once

#include "instance.hpp"
#include "utils.hpp"

namespace vtree {
class TdDec {
 public:
 	TdDec(const Instance& ins, double time);
 	void OrderInstance(Instance& ins);
 	const vector<Var>& Children(Var var) const;
 private:
 	// Distance from root to vertex. Roots are 1
 	vector<int> rank;
 	// Distance from vertex to farthest leaf. Leaves are 1
 	vector<int> depth;
 	// Parent of vertex in the tree.
 	vector<Var> parent;
 	// Children
 	vector<vector<Var>> chs;
};
} // namespace vtree