import tkinter as tk
import tkinter.font as tkFont
from tkinter import *
from tkinter import messagebox, filedialog, simpledialog
import tkinter
from tkinter import ttk
import csv
import datetime
import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import pandas as pd

class App:
    def __init__(self, root):
        # setting title
        root.title("Data Visualizer")
        mainWinBgColor = "#eb6b34"
        root.configure(bg=mainWinBgColor)
        # setting window size
        width=1350
        height=800
        screenwidth = root.winfo_screenwidth()
        screenheight = root.winfo_screenheight()
        alignstr = '%dx%d+%d+%d' % (width, height, (screenwidth - width) / 2, (screenheight - height) / 2)
        root.geometry(alignstr)
        root.resizable(width=False, height=False)

        # Set the font size to 12
        style = Style()
        style.configure('TButton', font=('calibri', 20, 'bold'), borderwidth='4')
        style.map('TButton', foreground=[('active', '!disabled', 'green')], background=[('active', 'black')])

        # Directory path entry
        self.directory_path_var = tk.StringVar()
        self.directory_path_label = tk.Label(root, text="Enter directory path:", bg="#eb6b34")
        self.directory_path_label.place(x=20, y=20)
        self.directory_path_entry = tk.Entry(root, textvariable=self.directory_path_var)
        self.directory_path_entry.place(x=200, y=20)
        self.browse_button = tk.Button(root, text="Browse", command=self.browse_directory, bg="#eb6b34")
        self.browse_button.place(x=400, y=15)

        # Start button
        self.start_button = tk.Button(root, text="Start", command=self.start_process, bg="#eb6b34")
        self.start_button.place(x=1145, y=280, width=195, height=83)

        export_button = tk.Button(root, text="Export", command=self.export_data, bg="#eb6b34")
        export_button.place(x=1145, y=80, width=195, height=83)

        # Additional buttons
        self.add_buttons(root)

    def add_buttons(self, root):
        # Export button
        calibrate_button = tk.Button(root, text="Calibrate", command=self.calibrate, bg="#eb6b34")
        calibrate_button.place(x=1145, y=180, width=195, height=83)

        # Stop button
        stop_button = tk.Button(root, text="Stop", command=self.stop_process, bg="#eb6b34")
        stop_button.place(x=1145, y=480, width=195, height=83)

        # Next button
        next_button = tk.Button(root, text="Next", command=self.next_step, bg="#eb6b34")
        next_button.place(x=1145, y=380, width=195, height=83)

    def browse_directory(self):
        directory_path = filedialog.askdirectory()
        self.directory_path_var.set(directory_path)

    def start_process(self):
        if not self.directory_path_var.get():
            messagebox.showerror("Error", "Please enter a directory path.")
            return

        session_name = simpledialog.askstring("Session Name", "Enter session name:")
        if not session_name:
            messagebox.showerror("Error", "Please enter a session name.")
            return

        session_folder_path = os.path.join(self.directory_path_var.get(), session_name)
        os.makedirs(session_folder_path, exist_ok=True)
        self.session_folder_path = session_folder_path
        self.export_csv()  # Assuming this method exists
        self.export_pdf()  # Call export_pdf method

    def export_data(self):
        x = datetime.datetime.now()
        filename = x.strftime("%Y-%m-%d_%H-%M-%S")
        if not filename.endswith('.csv'):
            filename += '.csv'
        self.data_file_name = filename
        self.export_csv()

    def calibrate(self):
        # Implement calibrate functionality here
        pass

    def stop_process(self):
        # Implement stop process functionality here
        pass

    def next_step(self):
        # Implement next step functionality here
        pass

    def export_csv(self):
        file_path = os.path.join(self.session_folder_path, self.data_file_name)
        with open(file_path, "w", newline="") as data_file:
            csv_writer = csv.writer(data_file)
            csv_writer.writerow(["Timestamp", "293293"])
        messagebox.showinfo("Export", f"CSV file exported successfully as {self.data_file_name}")

    def export_pdf(self):
        features = np.random.rand(100, 6)  # Example features
        time = np.random.uniform(0, 20, 100)  # Example time
        pdf_path = os.path.join(self.session_folder_path, 'plots.pdf')  # Path to save PDF
        with PdfPages(pdf_path) as pdf:
            self.plot_features(features, time, pdf)
        messagebox.showinfo("Export", f"Plots exported successfully as plots.pdf")

    def plot_features(self, features, time, pdf):
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


if __name__ == "__main__":
    root = tk.Tk()
    app = App(root)
    root.mainloop()
