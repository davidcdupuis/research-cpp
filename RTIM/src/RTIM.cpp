#include "RTIM.h"
#include "Arguments.h"
#include "Graph.h"

void RTIM::pre_process(){
  // does nothing yet
};

void RTIM::live(){
  //does nothing yet
};

int main(int argn, char **argv)
{
    Arguments args = Arguments();
    args.getArguments(argn, argv);
    args.printArguments();
    Graph g = Graph(args.dataset);
    g.print();
}
