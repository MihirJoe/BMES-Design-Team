import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import pandas as pd

def plot_features(features, time, pdf):
    num_features = features.shape[1]
    num_pages = -(-num_features // 4)  # Ceiling division to calculate number of pages needed

    for page_num in range(num_pages):
        fig, axs = plt.subplots(2, 2, figsize=(16, 12))

        for i in range(4):
            feature_num = page_num * 4 + i
            if feature_num >= num_features:
                break  # No more features to plot

            ax = axs[i // 2, i % 2]
            ax.scatter(time, features[:, feature_num])
            ax.set_title(f'Feature {feature_num + 1} vs Time')
            ax.set_xlabel('Time (seconds)')
            ax.set_ylabel(f'Feature {feature_num + 1}')
            ax.grid(True)

        plt.tight_layout()
        pdf.savefig()
        plt.close()

# Generate random values for time (column 0)
time = np.random.uniform(0, 20, 100)  # 100 samples, values between 0 and 20 seconds

# Generate random values for features (columns 1, 2, 3, ..., N)
num_features = 6  # Number of features/columns
features = np.random.rand(100, num_features)

# Create column names
column_names = ['time'] + [f'feature{i}' for i in range(1, num_features + 1)]

# Create a pandas DataFrame
df = pd.DataFrame(np.column_stack((time, features)), columns=column_names)

# Save the DataFrame to a CSV file
fpath = "data.csv"
df.to_csv(fpath, index=False)

# Create PDF file for plots
pdf_path = "plots.pdf"
with PdfPages(pdf_path) as pdf:
    plot_features(features, time, pdf)

print("Data saved to:", fpath)
print("Plots saved to:", pdf_path)
