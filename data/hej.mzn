include "globals.mzn";
int: n = 7;
array[1..n] of var 1..n+1: x;
array[1..n] of var 1..n+1: y;
array[1..n] of var 1..n+1: z;

constraint all_different(x);
constraint sum(y) = n;
constraint count(z, 5) == 3;

var int: a = sum(x);
var int: b = sum(y);
var int: c = sum(z);


var int: obj;
constraint obj = a - b + c;
solve :: int_search(x++y++z, input_order, indomain_min) minimize obj;