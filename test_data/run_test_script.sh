# Usage: ./run_test_script.sh <mzn file> <dzn file> <number of times to run>
# Example: ./run_test_script.sh pennies-opt.mzn pennies-opt.dzn 5

#!/bin/bash

# Check that the number of arguments is 4
if [ $# -ne 4 ]; then
    echo "Usage: ./run_test_script.sh <mzn file> <dzn file> <number of times to run> <output file>"
    exit 1
fi

# Check that the first argument is a .mzn file
if [[ $1 != *.mzn ]]; then
    echo "First argument must be a .mzn file"
    exit 1
fi

# Check that the second argument is a .dzn file
if [[ $2 != *.dzn ]]; then
    echo "Second argument must be a .dzn file"
    exit 1
fi

# Check that the third argument is a postive number
if ! [[ $3 =~ ^[1-9]+$ ]]; then
    echo "Third argument must be a positive number"
    exit 1
fi

# Run mzn file 5 times and store result in output file
for i in $(seq 1 $3); do
    # Run mzn file and store result in output file
    minizinc --solver gecode -o $4 --output-time $1 $2
    echo $1 >> $4
done