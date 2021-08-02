#pragma once

#include <vector>
#include <ostream>

namespace vtree {
class Dtree {
 public:
 	Dtree(int clauses_);
 	Dtree();
 	int Merge(int a, int b);
 	void Print(std::ostream& out, bool vtree) const;
 private:
 	int clauses;
 	int cur_node;
 	int tot_nodes;
 	std::vector<int> left;
 	std::vector<int> right;
 	std::vector<int> parent;
};
} // namespace vtree