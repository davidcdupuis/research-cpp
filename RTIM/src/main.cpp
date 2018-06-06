// File to run desired program
#include "arguments.h"
#include "rtim.h"
#include "graph.h"
#include "tools.h"

using namespace std;

int main(int argn, char **argv)
{
    Arguments args;
    args.getArguments(argn, argv);
    cout<<"\e\[0;32mProgram Start at: " << currentTimestampStr()<<"\e[0m"<<endl;
    args.printArguments();
    cout<<"--------------------------------------------------------------------------------" <<endl;

    // Import graph
    //Graph graph("test");
    //graph.print();
    //cout << "Graph " << graph.dataset << " imported." << endl;

    // Run RTIM stage on graph

    cout<<"\e\[0;31mProgram Terminate at: " << currentTimestampStr()<< "\e[0m"<<endl;
}
