// File to run desired program
#include "Arguments.h"
#include "RTIM.h"
#include "Graph.h"

using namespace std;

int main(int argn, char **argv)
{
    Arguments args;
    args.getArguments(argn, argv);
    args.printArguments();
    cout<<"--------------------------------------------------------------------------------" <<endl;

    // Import graph
    //Graph graph("test");
    //graph.print();
    //cout << "Graph " << graph.dataset << " imported." << endl;

    // Run RTIM stage on graph
}
