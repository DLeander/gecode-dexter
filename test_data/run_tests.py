import subprocess
import sys

# Class of colors used for printing, for prettier and more easily read output.
class colours:
    BRIGHT_CYAN = '\033[96m'
    BRIGHT_YELLOW = '\033[93m'
    RESET = '\033[0m'

def clean_output(output_file, model_name):

    # updated_lines includes the processed lines which are then written back.
    updated_lines = f"Results of running {model_name}:\n"

    # True if output of model has been processed or not.
    objective_processed = False
    # Open the output file
    with open(output_file, 'r') as file:
        # Read the entire file
        all_lines = file.readlines()

        for line in all_lines:
            # If line contains %%%mzn-stat: remove it from that line
            if "%%%mzn-stat:" in line:
                line = line.replace("%%%mzn-stat: ", "  ")
            elif objective_processed:
                line = ""
            # If output from model has been processed, then set objective_processed to True.
            if "----------" in line:
                objective_processed = True
            
            # Add the line to updated_lines
            updated_lines += line
        
        # Write back changes to file:
        with open(output_file, 'w') as file:
            file.write(updated_lines)

def run_tests(commands_file):
    print(f"{colours.BRIGHT_CYAN}Running tests...\n")
    # Open the file containing the commands
    with open(commands_file, 'r') as file:
        # Read the entire file
        all_commands = file.read()

        # Split the commands by new line
        commands = all_commands.split('\n')

        # Run each command in commands
        for command in commands:
            print(f"{colours.BRIGHT_YELLOW}Running command: {command}.")

            # Split the command by space
            sep_command = command.split(' ')
            print(f"Solving {sep_command[4]} with data {sep_command[5]} ...")

            # Execute the command (solve model)
            subprocess.run(command, shell=True, check=False, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            print(f"Solving Finished. Saving results in {sep_command[7]}.\n")

            # Clean up the output of minizinc.
            clean_output(sep_command[7], sep_command[4])


    print(f"{colours.BRIGHT_CYAN}All Models Finished, done.{colours.RESET}")


def main():
    if len(sys.argv) != 2:
        print("Usage: python3 run_tests.py <file of minizinc commands>")
        sys.exit(1)
    commands_file = sys.argv[1]
    run_tests(commands_file)

if __name__ == "__main__":
    main()