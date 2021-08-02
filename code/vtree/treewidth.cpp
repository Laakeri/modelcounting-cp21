#include "treewidth.hpp"

#include <chrono>
#include <fstream>
#include <ostream>
#include <set>
#include <sstream>

namespace vtree {
using std::set;

namespace treewidth {
std::string TmpInstance(int a, int b, int c) {
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  uint64_t micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
  string dir = "td_tmp";
  return dir+"/instance"+std::to_string(micros)+"_"+std::to_string(a)+"_"+std::to_string(b)+"_"+std::to_string(c)+".tmp";
}

int TWCen(int x, int p, int n, const vector<vector<int>>& bags, const vector<vector<int>>& tree, int& cen) {
  assert(x >= 1 && x < (int)bags.size());
  assert(bags.size() == tree.size());
  assert(cen == 0);
  int intro = 0;
  for (int nx : tree[x]) {
    if (nx == p) continue;
    int cintro = TWCen(nx, x, n, bags, tree, cen);
    intro += cintro;
    if (cintro >= n/2) {
      assert(cen);
      return intro;
    }
  }
  for (int v : bags[x]) {
    if (!binary_search(bags[p].begin(), bags[p].end(), v)) {
      intro++;
    }
  }
  if (intro >= n/2) {
    cen = x;
  }
  return intro;
}

void TWDes(int x, int p, Var pv, const vector<vector<int>>& bags, const vector<vector<int>>& tree, vector<Var>& ret) {
  assert(x >= 1 && x < (int)bags.size());
  assert(bags.size() == tree.size());
  for (int v : bags[x]) {
    if (ret[v] == 0) {
      ret[v] = pv;
      pv = v;
    }
  }
  for (int nx : tree[x]) {
    if (nx != p) {
      TWDes(nx, x, pv, bags, tree, ret);
    }
  }
}

vector<Var> Treewidth(uint32_t n, const vector<vector<Var>>& graph, double time) {
  assert(graph.size() == n+1);
  if (n==0){
    return {};
  }
  if (n==1){
    return {1};
  }
  if (time < 0.1) {
    vector<Var> parent(n+1);
    for (uint32_t i = 1; i <= n; i++) {
      parent[i] = i-1;
    }
    return parent;
  }
  assert(n>=2);
  int m = 0;
  for (Var i = 1; i <= n; i++) {
    m += (int)graph[i].size();
  }
  assert(m%2 == 0);
  m/=2;
  string tmp1 = TmpInstance(n, 1, m);
  string tmp2 = TmpInstance(n, 2, m);
  std::ofstream out(tmp1);
  out<<"p tw "<<n<<" "<<m<<endl;
  for (Var i = 1; i <= n; i++) {
    for (Var ni : graph[i]) {
      assert(ni != i);
      if (ni > i) {
        out<<i<<" "<<ni<<endl;
      }
    }
  }
  out.close();
  string tw_binary = "../flow-cutter-pace17/flow_cutter_pace17";
  string cmd = "timeout " + to_string(time) + "s " + tw_binary + " <" + tmp1 + " >" + tmp2 + " 2>/dev/null";
  cerr << "CMD:" << endl;
  cerr << cmd << endl;
  int status = system(cmd.c_str());
  assert(status >= 0);
  assert(WIFEXITED(status));
  assert(WEXITSTATUS(status) == 124); // TIMEOUT timed out
  cerr << "tw finish ok" << endl;
  int width = 0;
  vector<vector<int>> bags;
  vector<vector<int>> tree;
  std::ifstream in(tmp2);
  string tmp;
  int real_width = 0;
  while (getline(in, tmp)) {
    std::stringstream ss(tmp);
    ss>>tmp;
    if (tmp == "c") continue;
    if (tmp == "s") {
      ss>>tmp;
      assert(tmp == "td");
      assert(bags.empty());
      int bs,nn;
      ss>>bs>>width>>nn;
      assert(nn == (int)n);
      bags.resize(bs+1);
      tree.resize(bs+1);
      cout << "claim width " << width << endl;
    } else if (tmp == "b") {
      int bid;
      ss>>bid;
      assert(bid >= 1 && bid < (int)bags.size());
      assert(bags[bid].empty());
      int v;
      while (ss>>v) {
        assert(v >= 1 && v <= (int)n);
        bags[bid].push_back(v);
      }
      assert((int)bags[bid].size() <= width);
      sort(bags[bid].begin(), bags[bid].end());
      bags[bid].erase(unique(bags[bid].begin(), bags[bid].end()), bags[bid].end());
      real_width = max(real_width, (int)bags[bid].size());
    } else {
      int a = stoi(tmp);
      assert(1 <= a && a < (int)bags.size());
      int b;
      ss>>b;
      assert(a != b);
      tree[a].push_back(b);
      tree[b].push_back(a);
    }
  }
  in.close();
  system(("rm -f " + tmp1).c_str());
  system(("rm -f " + tmp2).c_str());
  assert(bags[0].empty());
  assert(tree[0].empty());
  int centroid = 0;
  TWCen(1, 0, n, bags, tree, centroid);
  assert(centroid >= 1 && centroid < (int)bags.size());
  // Sort so that highest degree variables are first in bags
  for (auto& bag : bags) {
    sort(bag.begin(), bag.end(), [&](int a, int b){
      return graph[a].size() > graph[b].size();
    });
  }
  vector<Var> ret(n+1);
  TWDes(centroid, 0, n+1, bags, tree, ret);
  assert(ret[0] == 0);
  int roots = 0;
  for (int i = 1; i <= (int)n; i++) {
    if ((int)ret[i] == n+1) {
      roots++;
      ret[i] = 0;
    } else {
      assert(ret[i] >= 1 && ret[i] <= n);
    }
  }
  assert(roots == 1);
  return ret;
}
void Des(Var i, int de, const vector<vector<Var>>& chs, vector<int>& rank, vector<int>& depth) {
  size_t n = chs.size();
  assert(rank.size() == n);
  assert(1 <= i && i < n);
  assert(de>=1);
  assert(rank[i]==0);
  rank[i]=de;
  depth[i]=1;
  for (int nx : chs[i]){
    Des(nx, de+1, chs, rank, depth);
    depth[i] = max(depth[i], depth[nx]+1);
  }
}
} // namespace treewidth

TdDec::TdDec(const Instance& ins, double time) {
  auto graph = ins.PrimalGraph();
  parent = treewidth::Treewidth(ins.vars, graph, time);
  assert(parent.size() == ins.vars+1);
  rank.resize(ins.vars+1);
  chs.resize(ins.vars+1);
  depth.resize(ins.vars+1);
  for (Var i = 1; i <= ins.vars; i++) {
  	if (parent[i] != 0) {
  		chs[parent[i]].push_back(i);
  	}
  }
  for (Var i = 1; i <= ins.vars; i++) {
  	if (parent[i] == 0) {
  		treewidth::Des(i, 1, chs, rank, depth);
  	}
  }
  int de = 0;
  int de2 = 0;
  for (Var i = 1; i <= ins.vars; i++) {
  	assert(rank[i] > 0);
  	assert(depth[i] > 0);
  	de = max(de, rank[i]);
  	de2 = max(de2, depth[i]);
  }
  assert(de == de2);
  cerr<<"Depth "<<de<<endl;
  cerr<<endl;
}

const vector<Var>& TdDec::Children(Var var) const {
	assert(var < chs.size());
	return chs[var];
}

namespace {
void odfs1(Var v, const vector<vector<Var>>& chs, Var& t, vector<Var>& dfs_map, vector<int>& rank, vector<Var>& parent, int de, vector<int>& depth) {
	assert(dfs_map[v] == 0);
	t++;
	dfs_map[v] = t;
	rank[dfs_map[v]] = de;
	depth[dfs_map[v]] = 1;
	for (Var nv : chs[v]) {
		odfs1(nv, chs, t, dfs_map, rank, parent, de+1, depth);
		parent[dfs_map[nv]] = dfs_map[v];
		depth[dfs_map[v]] = max(depth[dfs_map[v]], depth[dfs_map[nv]]+1);
	}
}
} // namespace

void TdDec::OrderInstance(Instance& ins) {
	assert(ins.vars+1 == rank.size());
	vector<Var> dfs_map(ins.vars+1);
	for (Var i = 1; i <= ins.vars; i++) {
		if (!parent[i]) {
			Var x = 0;
			parent[1] = 0;
			odfs1(i, chs, x, dfs_map, rank, parent, 1, depth);
			assert(x == ins.vars);
		}
	}
	int roots = 0;
	for (Var i = 1; i <= ins.vars; i++) {
		chs[i].clear();
		if (!parent[i]) {
			roots++;
		}
	}
	assert(roots == 1);
	int de = 0;
	for (Var i = 1; i <= ins.vars; i++) {
		if (parent[i]) {
			assert(rank[i] > 1);
			assert(rank[i] == rank[parent[i]]+1);
			chs[parent[i]].push_back(i);
			de = max(de, rank[i]);
		} else {
			assert(rank[i] == 1);
		}
	}
	assert(de == depth[1]);
	for (auto& clause : ins.clauses) {
		for (Lit& lit : clause) {
			if (IsPos(lit)) {
				lit = PosLit(dfs_map[VarOf(lit)]);
			} else {
				lit = NegLit(dfs_map[VarOf(lit)]);
			}
		}
		sort(clause.begin(), clause.end());
		for (size_t i = 1; i < clause.size(); i++) {
			Var v0 = VarOf(clause[i-1]);
			Var v1 = VarOf(clause[i]);
			assert(v0 != v1);
			assert(rank[v0] < rank[v1]);
			while (v1 != v0) {
				assert(rank[v0] < rank[v1]);
				v1 = parent[v1];
			}
		}
	}
	sort(ins.clauses.begin(), ins.clauses.end());
}
} // namespace vtree