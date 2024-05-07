include "globals.mzn";

int: n = 12;

set of int: N = 1..n;

array[N] of var N: perm;


constraint all_different(perm);

array[int] of var bool: inversions = [perm[i] > perm[j] | i, j in N where i < j];

var int: goal = sum(inversions);

solve minimize goal;
