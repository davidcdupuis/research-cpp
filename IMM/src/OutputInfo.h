#ifndef __OUTPUTINFO_H__
#define __OUTPUTINFO_H__

string __head_version = "";

class OutputInfo{
public:
  OutputInfo(int argn, char ** argv);
  ~OutputInfo();
};

#endif
