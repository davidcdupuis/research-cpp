#include <boost/graph/adjacency_list.hpp>
#include <rtim.h>
#include <arguments.h>


int main(int argn, char **argv)
{
    Arguments args = new Arguments();
    args.getArguments(argn, argv);
    args.printArguments();

    OutputInfo info(argn, argv);

    delete args;
}
