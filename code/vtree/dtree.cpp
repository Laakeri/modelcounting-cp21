#include "dtree.hpp"

#include "utils.hpp"

namespace vtree {

Dtree::Dtree(int clauses_) : clauses(clauses_), cur_node(clauses_), tot_nodes(clauses_*2-1) {
	assert(clauses >= 1);
	left.resize(tot_nodes-clauses);
	right.resize(tot_nodes-clauses);
	parent.resize(tot_nodes);
}

Dtree::Dtree() : Dtree(1) {}

int Dtree::Merge(int a, int b) {
	assert(a >= 0 && b >= 0 && a < tot_nodes && b < tot_nodes && a != b);
	assert(parent[a] == 0 && parent[b] == 0);
	int tt = cur_node++;
	assert(cur_node <= tot_nodes);
	parent[a] = tt;
	parent[b] = tt;
	left[tt-clauses] = a;
	right[tt-clauses] = b;
	return tt;
}

void Dtree::Print(std::ostream& out, bool vtree) const {
	assert(cur_node == tot_nodes);
	int root = tot_nodes-1;
	assert(parent[root] == 0);
	if (vtree) {
		out<<"vtree "<<tot_nodes<<'\n';
	} else {
		out<<"dtree "<<tot_nodes<<'\n';
	}
	for (int i = 0; i < tot_nodes; i++) {
		if (i < clauses) {
			assert(parent[i] >= clauses);
			if (vtree) {
				out<<"L "<<i<<" "<<i+1<<'\n';
			} else {
				out<<"L "<<i<<'\n';
			}
		} else {
			if (i != root) assert(parent[i] > i);
			assert(parent[left[i-clauses]] == i && parent[right[i-clauses]] == i);
			if (vtree) {
				out<<"I "<<i<<" "<<left[i-clauses]<<" "<<right[i-clauses]<<'\n';
			} else {
				out<<"I "<<left[i-clauses]<<" "<<right[i-clauses]<<'\n';
			}
		}
	}
	out<<std::flush;
}
} // namespace vtree