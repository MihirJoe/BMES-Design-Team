import tkinter as tk
import tkinter.font as tkFont
from tkinter import *
from tkinter import ttk
from tkinter import messagebox
from tkinter import filedialog
from tkinter import filedialog, messagebox, simpledialog
import tkinter
import csv
from tkinter.ttk import *
import datetime
import os
from matplotlib.figure import Figure
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from arduino_read import *
import random
import numpy as np

class App:
    def __init__(self, root):
        # Setting title
        self.root = root
        self.root.title("Data Visualizer")
        mainWinBgColor = "#eb6b34"
        self.root.configure(bg=mainWinBgColor)
        # Setting window size
        width = 1350
        height = 800
        screenwidth = self.root.winfo_screenwidth()
        screenheight = self.root.winfo_screenheight()
        alignstr = '%dx%d+%d+%d' % (width, height, (screenwidth - width) / 2, (screenheight - height) / 2)
        self.root.geometry(alignstr)
        self.root.resizable(width=True, height=True)

        # Set the font size to 12
        style = ttk.Style()
        style.configure('TButton', font=('calibri', 20, 'bold'), borderwidth='4')
        style.map('TButton', foreground=[('active', '!disabled', 'green')], background=[('active', 'black')])

        # --- Top Section ---
        self.top_frame = tk.Frame(self.root)
        self.top_frame.grid(row=0, column=0, sticky="ew")

        top_frame = self.top_frame

        # Directory path label
        self.directory_label = tk.Label(top_frame, text="Directory Path:")
        self.directory_label.grid(row=0, column=0, padx=5, pady=5)

        # Directory path entry
        placeholder_text = "Enter directory path"
        self.directory_path_var = tk.StringVar()
        self.directory_path_var.set(placeholder_text)
        self.directory_path_entry = tk.Entry(top_frame, textvariable=self.directory_path_var, state='disabled')
        self.directory_path_entry.grid(row=0, column=1, padx=5, pady=5)

        self.browse_button = tk.Button(top_frame, text="Browse", command=self.browse_directory, bg="#eb6b34", state='normal')
        self.browse_button.grid(row=0, column=2, padx=5, pady=5)

        # --- Middle Section ---
        self.middle_frame = ttk.Frame(self.root)
        self.middle_frame.grid(row=1, column=0, sticky="nsew")

        # Plotting format
        self.x_axis_label = "Time (s)"
        self.y_axis_label = "Force (lbs)"
        self.fig, self.ax = plt.subplots(figsize=(5, 4))
        self.line, = self.ax.plot([], [], lw=2)
        self.ax.set_xlim(0, 100)
        self.ax.set_ylim(0, 1)
        self.ax.set_xlabel(self.x_axis_label)
        self.ax.set_ylabel(self.y_axis_label)
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.middle_frame)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        self.anim = None
        self.data = []

        # --- Right Section ---
        self.right_frame = tk.Frame(self.root)
        self.right_frame.grid(row=0, column=1, rowspan=2, sticky="ns")

        self.export_button = tk.Button(self.right_frame, text="Export", command=self.export_data, bg="#eb6b34", state='disabled')
        self.export_button.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        self.start_button = tk.Button(self.right_frame, text="Start", command=self.start_process, bg="#eb6b34", state='disabled')
        self.start_button.grid(row=1, column=0, padx=5, pady=5, sticky="ew")

        self.stop_button = tk.Button(self.right_frame, text="Stop", command=self.stop_process, bg="#eb6b34", state='disabled')
        self.stop_button.grid(row=2, column=0, padx=5, pady=5, sticky="ew")

        self.next_button = tk.Button(self.right_frame, text="Next", command=self.next_step, bg="#eb6b34", state='disabled')
        self.next_button.grid(row=3, column=0, padx=5, pady=5, sticky="ew")

        # Configure Grid Weight to Allow Resizing
        self.root.grid_rowconfigure(1, weight=1)
        self.root.grid_columnconfigure(0, weight=1)

    def browse_directory(self):
        directory_path = filedialog.askdirectory()
        if directory_path:
            self.directory_path_var.set(directory_path)
            # Enable buttons after directory selection
            self.enable_buttons()

    def enable_buttons(self):
        # Enable buttons after directory selection
        self.export_button.config(state='normal')
        self.start_button.config(state='normal')
        self.stop_button.config(state='normal')
        self.next_button.config(state='normal')

    def start_animation(self):
        self.start_button.config(state='disabled')  # Disable the start button
        self.anim = FuncAnimation(self.fig, self.update_plot, interval=100)
        self.canvas.draw()

    def stop_animation(self):
        if self.anim:
            self.anim.event_source.stop()
            self.start_button.config(state='normal')  # Enable the start button when animation stops
            self.save_data_to_csv()

    def update_plot(self, frame):
        x = np.linspace(0, 100, 100)
        y = np.random.rand(100)
        self.line.set_data(x, y)
        self.data.append((x, y))  # Store data for saving to CSV
        return self.line,

    def start_process(self):
        if not self.directory_path_var.get():
            messagebox.showerror("Error", "Please enter a directory path.")
            return

        if not hasattr(self, 'session_folder_path'):
            session_name = simpledialog.askstring("Session Name", "Enter session name:")
            if not session_name:
                messagebox.showerror("Error", "Please enter a session name.")
                return

            session_folder_path = os.path.join(self.directory_path_var.get(), session_name)
            os.makedirs(session_folder_path, exist_ok=True)
            self.session_folder_path = session_folder_path

        if not hasattr(self, 'anim') or self.anim is None:
            self.start_animation()
            self.start_button.config(text="Pause")
        else:
            self.pause_animation()
            self.start_button.config(text="Play")

    def start_animation(self):
        self.start_button.config(state='disabled')  # Disable the start button
        self.anim = FuncAnimation(self.fig, self.update_plot, interval=100)
        self.canvas.draw()

    def pause_animation(self):
        if self.anim:
            self.anim.event_source.stop()
            self.anim = None
            

    def export_data(self):
        x = datetime.datetime.now()
        filename = x.strftime("%Y-%m-%d_%H-%M-%S")
        if not filename.endswith('.csv'):
            filename += '.csv'
        self.data_file_name = filename
        self.export_csv()

    def save_data_to_csv(self):
        if self.data:
            with open('live_plot_data.csv', 'w', newline='') as csvfile:
                writer = csv.writer(csvfile)
                writer.writerow(['Time', 'Force'])
                for x, y in self.data:
                    for i in range(len(x)):
                        writer.writerow([x[i], y[i]])

    def stop_process(self):
        # Stop recording
        self.stop_animation()

    def next_step(self):
        # Implement next step functionality here
        pass

    def export_csv(self):
        file_path = os.path.join(self.session_folder_path, self.data_file_name)
        with open(file_path, "w", newline="") as data_file:
            csv_writer = csv.writer(data_file)
            csv_writer.writerow(["Timestamp", "293293"])
        messagebox.showinfo("Export", f"CSV file exported successfully as {self.data_file_name}")


if __name__ == "__main__":
    root = tk.Tk()
    app = App(root)
    root.mainloop()