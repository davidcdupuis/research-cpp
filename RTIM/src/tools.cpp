#include "tools.h"

OutputInfo::OutputInfo(int argn, char ** argv){
    cout<<"\e\[0;32mProgram Start at: " << currentTimestampStr()<<"\e[0m"<<endl;
    cout<<"\e\[0;32mProgram version: " << __head_version << "\e[0m"<<endl;
    cout<<"Arguments: ";
    for(int i=0; i<argn; i++){
        cout<<argv[i]<<" ";
    }
    cout<<endl;
    cout<<"--------------------------------------------------------------------------------" <<endl;
}

OutputInfo::~OutputInfo(){
    cout<<"\e\[0;31mProgram Terminate at: " << currentTimestampStr()<< "\e[0m"<<endl;
    cout<<"\e\[0;31mProgram version: " << __head_version << "\e[0m"<<endl;
}
