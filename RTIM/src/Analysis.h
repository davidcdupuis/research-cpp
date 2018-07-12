#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

class Analysis{
public:
  std::string dataset;
  int nodes;
  int edges;

  /* Find number of out-neighbors and in-neighbors and save results */
  void neighbors(int user);
};
#endif
