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

## Data

(example)

nethept/<br>
.&nbsp;&nbsp;imm/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;basic/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_k50_e0,5_ss50.txt<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;live<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_k50_e0,5_urr_v1_s15229_ss25.txt<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;progress/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_k50_e0,5_urr_v1_s15229_prg.txt<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;common<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_k50_e0,5_k50_r5_ap0,8_urr_v1_s15229_ss50.txt<br>
.&nbsp;&nbsp;rtim/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;pre_process/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_d2_m0_infS.txt<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_infS.txt<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;live/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_r1_ap0,8_urr_v1_s15229_ss[seed_set_size].txt<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_r5_ap0,8_urr_v1_s15229_ss50.txt<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;progress/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_r1_ap0,8_urr_v1_s15229_prg.txt<br>
.&nbsp;&nbsp;streams/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;uniform_rand_repeat/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;v1/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_urr_v1_s15229_st.txt<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;v2/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;.&nbsp;&nbsp;NE_urr_v2_s15229_st.txt<br>
.&nbsp;&nbsp;logs/<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;imm.log<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;rtim_pre.log<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;rtim_live.log<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;seed_set_scores.log<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;common.log<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;imm.csv<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;rtim_pre.csv<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;rtim_live.csv<br>
.&nbsp;&nbsp;.&nbsp;&nbsp;seed_set_scores.csv<br>


### Keyword abbreviations

### Datasets

|dataset | abbreviation | nodes | edges
|:-:|:-:|:-:|:-:|
| test | TS | 20 | 38 |
| nethept | NE | 15_229 | 62_752 |
| DBLP | DB | 317_080 | 2_099_731 |
| Youtube | YO | 1_134_962 | 5_975_247 |
| LiveJournal | LJ | 3_997_441 | 69_362_377 |
| Orkut | OR | 3_072_441 | 234_370_165 |
| Twitter | TW | 41_652_230 | 1_468_365_182 |

### Keywords

| keyword | abbreviation | description |
|:-:|:-:|---|
| infScores | infS | Associated file contains user influence scores|
| seedSet | ss | Associated file contains list of user of a given seed set|
| stream | st | Associated file is a list of users simulating a stream of users connecting to the Internet|
| uniformily random repeat | urr | Stream was generated by choosing users uniformily at random with possible repitition |
| uniformily random no repeat | urnr | Stream was generated by choosing users uniformily at random without possible repitition |

### IMM Parameters

| parameter | abbreviation | description |
|:-:|:-:|:-:|
| seed set size | k[value] | size of seed set we want to generate |
| epsilon | e[value] | parameter to determine precision of result |

### RTIM Parameters

| parameter | abbreviation | description |
|:-:|:-:|:-:|
| depth | d[value] | Maximum exploration with which we computed the influence score during the first stage |
| minimum weight | m[value] | minimum edge weight necessary to consider random walk |
|reach | r[value] | the top % of users we would like to target during the live stage (helps determine the influence score threshold)|
| activation probability | ap[value] | the activation probability threshold at which we consider a user to be activated during the live stage |
| max seed set size | k[value] | the maximum seed set size we want to generate during the live stage |
