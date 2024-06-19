import subprocess
import sys

def run_instance(command, num_runs):
    for _ in range(num_runs):
        result = subprocess.run(command, shell=True, check=False, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        result_str = result.stdout.decode()
        if "=====ERROR=====" in result_str:
            print(result_str)
        else:
            # find line that has solveTime in it and print that
            for line in result_str.split("\n"):
                if "solveTime=" in line:
                    print(line)
def main():
    if len(sys.argv) < 3:
        command = "minizinc -t 600000 -s --solver dexter --fzn-flags \"--use-pbs\" models_and_data/pennies-opt/pennies.mzn models_and_data/pennies-opt/n7.dzn"
        num_runs = 10
    else:
        command = sys.argv[1]
        num_runs = sys.argv[2]
    run_instance(command, num_runs)

if __name__ == "__main__":
    main()