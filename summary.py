import pandas as pd
from scipy.stats import ttest_ind
import os
import numpy as np

# Define the folder containing the CSV files
input_path = 'input'
output_path = 'output'

# Get the list of data files (e.g., data1.txt, data2.txt, ...)
data_files = [os.path.splitext(f)[0] for f in os.listdir(input_path) if f.endswith('.txt')]

# Define the matchers
matchers = ['kmp', 'brute_force']

# List to store consolidated results
results = []

# Loop through each data file
for data_file in data_files:
    # Construct file paths for kmp and brute_force
    file_kmp = os.path.join(output_path, f"kmp_{data_file}.csv")
    file_brute_force = os.path.join(output_path, f"brute_force_{data_file}.csv")

    # Check if both files exist
    if os.path.exists(file_kmp) and os.path.exists(file_brute_force):
        # Load the data for both versions
        df_kmp = pd.read_csv(file_kmp)
        df_brute_force = pd.read_csv(file_brute_force)

        # Get unique multipliers
        multipliers = sorted(df_kmp['multiplier'].unique())

        # Loop through each multiplier
        for multiplier in multipliers:
            # Filter data for the current multiplier
            times_kmp = df_kmp[df_kmp['multiplier'] == multiplier]['time']
            times_brute_force = df_brute_force[df_brute_force['multiplier'] == multiplier]['time']

            # Calculate means for kmp and brute_force
            mean_kmp = times_kmp.mean()
            mean_brute_force = times_brute_force.mean()

            # Calculate standard errors
            std_error_kmp = times_kmp.std(ddof=1) / np.sqrt(len(times_kmp))
            std_error_brute_force = times_brute_force.std(ddof=1) / np.sqrt(len(times_brute_force))

            # Perform t-test
            t_stat, p_value = ttest_ind(times_kmp, times_brute_force, equal_var=False)  # Welch's t-test

            # Append results (include std_error for both matchers)
            results.append({
                'data_file': data_file,
                'multiplier': multiplier,
                'mean_kmp': mean_kmp,
                'std_error_kmp': std_error_kmp,
                'mean_brute_force': mean_brute_force,
                'std_error_brute_force': std_error_brute_force,
                't_stat': t_stat,
                'p_value': p_value
            })

# Convert results to a DataFrame
consolidated_df = pd.DataFrame(results)

# Sort the consolidated DataFrame by 'data_file' and 'multiplier'
consolidated_df = consolidated_df.sort_values(by=['data_file', 'multiplier'])

# Print the consolidated DataFrame
print(consolidated_df)

# Save the results to file
consolidated_df.to_csv('sorted_consolidated_results.csv', index=False, float_format='%.9f')
