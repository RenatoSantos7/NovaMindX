import os
import subprocess

# Path to your compiled C programs
kmp = "./kmp"
brute_force = "./brute_force"

input_path = "./input"
output_path = "./output"
suffix = "results"

# Remove all files in the output directory
for file in os.listdir(output_path):
    file_path = os.path.join(output_path, file)
    if os.path.isfile(file_path):
        os.remove(file_path)

# List all files in the input directory, sorted alphabetically
input_files = sorted(os.listdir(input_path))

# Execute both programs for each sorted file
for arg in input_files:
    input_file = os.path.join(input_path, arg)
    
    if os.path.isfile(input_file):  # Ensure it's a file (not a directory)
        output_file_kmp = os.path.join(output_path, f"kmp_{os.path.splitext(arg)[0]}_{suffix}.txt")
        output_file_brute_force = os.path.join(output_path, f"brute_force_{os.path.splitext(arg)[0]}_{suffix}.txt")
        
        # Execute the dynamic program first
        subprocess.run([kmp, input_file, output_file_kmp])
        
        # Execute the static program after the dynamic program completes
        subprocess.run([brute_force, input_file, output_file_brute_force])