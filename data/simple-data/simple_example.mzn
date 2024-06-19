include "globals.mzn";        % MiniZinc globals library.

int: a = 9;                   % Fixed integer parameter.
int: b = 3;                   % Fixed integer parameter.
var 1..a: x;                  % Integer decision variable.
array[1..b] of var 1..b: y;   % Array of integer decision variables.

constraint all_different(y) :: domain;  % Constrain array y to have distinct values and use domain propagation.
constraint sum(y) <= x;       % Constrain the sum of array y to be less than or equal to variable x.

solve maximize sum(y) + x;    % Find the solution where the expression is maximised.