#include "solver.h"

#include <iostream>

#include <vector>

//#include <malloc.h>
#include <string>

#include <sys/time.h>
#include <sys/resource.h>

using namespace std;


int main(int argc, char *argv[]) {

  string input_file;
  Solver theSolver;


  if (argc <= 1) {
    cout << "Usage: sharpSAT [options] [CNF_File]" << endl;
    cout << "Options: " << endl;
    cout << "\t -noPP  \t turn off preprocessing" << endl;
    cout << "\t -q     \t quiet mode" << endl;
    cout << "\t -t [s] \t set time bound to s seconds" << endl;
    cout << "\t -noCC  \t turn off component caching" << endl;
    cout << "\t -cs [n]\t set max cache size to n MB" << endl;
    cout << "\t -noIBCP\t turn off implicit BCP" << endl;
    cout << endl;
    cout << "The following are new:" << endl;
    cout << "\t -tw    \t enable treewidth" << endl;
    cout << "\t -decot \t time limit for anytime tree decomposition" << endl;
    cout << "\t -decow \t how much treewidth is weighted in branching. If not specified, 10^7." << endl;
    cout << "\t -nofreq\t disable frequency scores in VSADS" << endl;
    cout << "\t -noact \t disable activity scores in VSADS" << endl;
    cout << "\t -noactinit\t init activity scores to 0 instead of freq" << endl;
    cout << "\t -nocdcl\t no CDCL" << endl;
    cout << "\t -wemod \t weight mode" << endl;
    cout << "\t" << endl;
    return -1;
  }

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-noCC") == 0)
      theSolver.config().perform_component_caching = false;
    if (strcmp(argv[i], "-noIBCP") == 0)
      theSolver.config().perform_failed_lit_test = false;
    if (strcmp(argv[i], "-noPP") == 0)
      theSolver.config().perform_pre_processing = false;
    else if (strcmp(argv[i], "-q") == 0)
      theSolver.config().quiet = true;
    else if (strcmp(argv[i], "-v") == 0)
      theSolver.config().verbose = true;
    else if (strcmp(argv[i], "-t") == 0) {
      if (argc <= i + 1) {
        cout << " wrong parameters" << endl;
        return -1;
      }
      theSolver.config().time_bound_seconds = atol(argv[i + 1]);
      if (theSolver.config().verbose)
        cout << "time bound set to" << theSolver.config().time_bound_seconds << "s\n";
     } else if (strcmp(argv[i], "-cs") == 0) {
      if (argc <= i + 1) {
        cout << " wrong parameters" << endl;
        return -1;
      }
      theSolver.statistics().maximum_cache_size_bytes_ = atol(argv[i + 1]) * (uint64_t) 1000000;
    } else if (strcmp(argv[i], "-nofreq") == 0) {
      assert(theSolver.config().vsads_freq == true);
      theSolver.config().vsads_freq = false;
    } else if (strcmp(argv[i], "-noact") == 0) {
      assert(theSolver.config().vsads_act == true);
      theSolver.config().vsads_act = false;
    } else if (strcmp(argv[i], "-noactinit") == 0) {
      assert(theSolver.config().vsads_act_init == true);
      theSolver.config().vsads_act_init = false;
    } else if (strcmp(argv[i], "-tw") == 0) {
      theSolver.config().treewidth = true;
    } else if (strcmp(argv[i], "-decot") == 0) {
      if (argc <= i + 1) {
        cout << " wrong parameters" << endl;
        return -1;
      }
      double decot = atof(argv[++i]);
      assert(decot >= 1 && decot <= 7200);
      theSolver.config().decomp_time = decot;
    } else if (strcmp(argv[i], "-decow") == 0) {
      if (argc <= i + 1) {
        cout << " wrong parameters" << endl;
        return -1;
      }
      double decow = atof(argv[++i]);
      assert(decow > 0.09);
      theSolver.config().decomp_weight = decow;
    } else if (strcmp(argv[i], "-nocdcl") == 0) {
      assert(theSolver.config().cdcl == true);
      theSolver.config().cdcl = false;
    } else if (strcmp(argv[i], "-wemod") == 0) {
      if (argc <= i + 1) {
        cout << " wrong parameters" << endl;
        return -1;
      }
      int wemod = atoi(argv[++i]);
      assert(wemod >= 1 && wemod <= 3);
      theSolver.config().weight_mode = wemod;
    } else
      input_file = argv[i];
  }

  theSolver.solve(input_file);

//  cout << sizeof(LiteralID)<<"MALLOC_STATS:" << endl;
//  malloc_stats();

//  rusage ru;
//  getrusage(RUSAGE_SELF,&ru);
//
//   cout << "\nRus: " <<  ru.ru_maxrss*1024 << endl;
//  cout << "\nMALLINFO:" << endl;
//
//  cout << "total " << mallinfo().arena + mallinfo().hblkhd << endl;
//  cout <<  mallinfo().arena << "non-mmapped space allocated from system " << endl;
//  cout <<  mallinfo().ordblks << "number of free chunks " << endl;
//  cout <<  mallinfo().smblks<< "number of fastbin blocks " << endl;
//  cout <<  mallinfo().hblks<< " number of mmapped regions " << endl;
//  cout <<  mallinfo().hblkhd<< "space in mmapped regions " << endl;
//  cout <<  mallinfo().usmblks<< " maximum total allocated space " << endl;
//  cout <<  mallinfo().fsmblks<< "space available in freed fastbin blocks " << endl;
//  cout <<  mallinfo().uordblks<< " total allocated space " << endl;
//  cout <<  mallinfo().fordblks<< "total free space " << endl;
//  cout <<  mallinfo().keepcost<< " top-most, releasable (via malloc_trim) space " << endl;
  return 0;
}
