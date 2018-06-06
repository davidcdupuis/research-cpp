#include "RTIM.h"
#include "Arguments.h"
#include "Tools.h"

void RTIM::pre_process(){
  // does nothing yet
};

void RTIM::live(){
  //does nothing yet
};

int main(int argn, char **argv)
{
    Arguments args = new Arguments();
    args.getArguments(argn, argv);
    args.printArguments();

    OutputInfo info(argn, argv);

    delete args;
}
