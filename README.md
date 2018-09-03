# Influence Maximization PhD Research in C++
Private repo for my thesis

## Estimating size of optimal seed set

1. As long as all nodes are not activated randomly select a node and simulate spread.
2. Count number of randomly selected nodes
3. Repeat 1. and 2. 10,000 times
4. Average number of randomly selected nodes is an approximation of k = |S*|, size of optimal seed set

## IM algorithms
### Real-Time Influence Maximization (RTIM)
* Pre-processing
 * Compute inf. score of all nodes using MC simulations
 * Compute inf. threshold as inf. score of top 10% of influencers
* Live
 * update activation probability
 * update influence threshold

### Random Model

* Pure random
 * Randomly select any user in G = (V, E) with repetition until |V| users were randomly selected
* Random decay
 * Randomly select any user in G = (V, E), with repetition but previously selected user cannot be chosen again until x number of other users have been selected. Stop when |V| users were randomly selected

## RTIM program
Date time is displayed in red when program starts and ends.

### Dataset Menu
```terminal
Choose a dataset (nodes, edges):
   [test]        (20, 30])
   [nethept]     (15_229, 62_752)
   [dblp]        (317_080, 2.09M)
   [youtube]     (1.13M, 5.97M)
   [orkut]       (3.07M, 234.37M)
   [livejournal] (3.99M, 69.36M)
   [twitter]     (41.65M, 1.46B)
> dataset name: __
```

### Stage Menu
```terminal
Choose a stage:
   [1] pre-process
   [2] live
   [3] IMM seed set test
   [4] test
   [5] EXIT PROGRAM
> choice: __
```

### Arguments Menu
Depends on selected stage. Input requests are displayed one at a time and only disappear after a short delay once all inputs are in.
#### Pre-processing Menu
```terminal
Input pre_process arguments
> depth (<current default>): __
> minimum weight (<default>): __
```
#### Live Menu
```terminal
Input live arguments
> seed size (<default>): __
> reach (<default>): __
> activation probability threshold (<default>): __
> stream model [rand_repeat, rand_no_repeat](<default>): __
> stream version [1, 2, 3](<default>): __
> stream size (<default>): __
```
#### IMM seed test Menu
Not yet implemented
```terminal

```

### Continue Menu
When a process finishes running this menu is displayed to offer the user a chance to run another operation without having to reload the dataset.
```terminal
Continue:
   [1] Repeat previous stage with same arguments
   [2] Repeat previous stage with new arguments
   [3] Change stage
   [4] Change dataset
   [5] End Program
> choice: __
```
