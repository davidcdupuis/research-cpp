# Influence Maximization Program

A C++ program to run any desired influence maximization algorithm

## Console Interface

```terminal
-----------Main Menu--------------
  [1] run experiments.txt
  [2] run seperate tests
  [3] run test
> choice : ___
```

### Experiments.txt

A file containing a list of commands to execute automatically.

> -a rtim -d nethept -pre -d 2 ... <br>
> -a rtim -d nethept -live -k <br>
> . <br>
> .

Displays the Main Menu when finished to allow the user to repeat experiments with changes or run seperate tests.

### Seperate Tests

```terminal
----------Datasets (nodes, edges)----------
  [test]        (20, 30])
  [nethept]     (15_229, 62_752)
  [dblp]        (317_080, 2.09M)
  [youtube]     (1.13M, 5.97M)
  [orkut]       (3.07M, 234.37M)
  [livejournal] (3.99M, 69.36M)
  [twitter]     (41.65M, 1.46B)
> dataset name: ___
```

```terminal
--------Choose Algorithm-----------
  [1] RTIM
  [2] IMM
  [3] TIM+
  [4] InfScore
> choice : ___
```

```terminal
--------InfScore Menu------------
  [1] Use Monte Carlo simulations
  [2] Use estimation
  [3] Other
> choice : ___
```

#### RTIM
```terminal
----------RTIM Menu--------------
  [1] pre_process
  [2] live
  [3] test
```

```terminal
---------RTIM Pre-Process Params----------
> depth (<current default>): __
> minimum weight (<default>): __
```

#### IMM

```terminal
---------IMM Menu-----------------

```

##### TIM+

```terminal
---------TIM+ Menu---------------

```

### Repeat

```terminal
---------Repeat Menu-------------

```
