# Influence Maximization PhD Research in C++
Private repo for my thesis

## Estimating size of optimal seed set

1. As long as all nodes are not activated randomly select a node and simulate spread.
2. Count number of randomly selected nodes
3. Repeat 1. and 2. 10,000 times
4. Average number of randomly selected nodes is an approximation of k = |S*|, size of optimal seed set

## Sample Influence Maximization (SIM)

Method based on optimal seed set size to find the optimal seed set.

## Real-Time Influence Maximization (RTIM)
* Pre-processing
 * Compute inf. score of all nodes using MC simulations
 * Compute inf. threshold as inf. score of top 10% of influencers
* Live
 * update activation probability
 * update influence threshold

### RTB Simulator

#### Random Model

* Pure random
 * Randomly select any user in G = (V, E) with repetition until |V| users were randomly selected
* Random decay
 * Randomly select any user in G = (V, E), with repetition but previously selected user cannot be chosen again until x number of other users have been selected. Stop when |V| users were randomly selected

#### Log-based

* Use log-files to determine which users are available to target

| File              | Purpose                                          |
|-------------------|--------------------------------------------------|
| research_data     | functions to import research data                |
| rtim_queue        | Multiprocessing Real-Time Influence Maximization |
| rtim              | Real-Time Influence Maximization                 |
| optimal_size      | Compute size of optimal seed set                 |
| optimal_size_mp   | Multiprocess size of optimal seed set            |
| weighted_cascade  | Module to compute weighted cascade edge weights  |
| network_info      | Compute graph topology information               |
