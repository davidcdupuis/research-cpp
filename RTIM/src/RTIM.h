#ifndef __RTIM_H__
#define __RTIM_H__

/*RTIM = Real-Time Influence Maximization

  Step1: Pre_process graph by computing influence score of all users
  Step2: Dynamically decide user targeting and save to seed set
*/
class RTIM{
  string dataset;   //dataset name
  int k;            //seed set size limit
  int simulations;  // # simulations for inf score
  int max_depth;    //max exploration depth for inf score

  /*Pre-processing stage
    Compute influence score of every user in graph
  */
  void pre_process();

  /*Live stage
    For stream of user decide to target, if targeted update activation
    probability of neighboring nodes
  */
  void live();

};
#endif
