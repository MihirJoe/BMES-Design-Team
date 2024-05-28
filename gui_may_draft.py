import tkinter as tk
import tkinter.font as tkFont
from tkinter import *
from tkinter import ttk
from tkinter import messagebox
from tkinter import filedialog
from tkinter import filedialog, messagebox, simpledialog
import csv
import datetime
import os
import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.backends.backend_pdf import PdfPages
import random
import numpy as np
import serial
import time
import threading

class App:
    def __init__(self, root, port, baud):
        self.port = port
        self.baud = baud
        self.angle_data = []
        self.force_data = []
        self.root = root
        self.root.title("Data Visualizer")
        mainWinBgColor = "#eb6b34"
        self.root.configure(bg=mainWinBgColor)

        self.is_reading = False
        self.lock = threading.Lock()

        width = 1350
        height = 800
        screenwidth = self.root.winfo_screenwidth()
        screenheight = self.root.winfo_screenheight()
        alignstr = '%dx%d+%d+%d' % (width, height, (screenwidth - width) / 2, (screenheight - height) / 2)
        self.root.geometry(alignstr)
        self.root.resizable(width=True, height=True)

        try:
            self.ser = serial.Serial(self.port, self.baud)
            print("Serial connection established.")
        except Exception as e:
            print("Failed to establish serial connection:", e)
            return
        
        time.sleep(2)

        style = ttk.Style()
        style.configure('TButton', font=('calibri', 20, 'bold'), borderwidth='4')
        style.map('TButton', foreground=[('active', '!disabled', 'green')], background=[('active', 'black')])

        self.top_frame = tk.Frame(self.root)
        self.top_frame.grid(row=0, column=0, sticky="ew")

        top_frame = self.top_frame
        
        placeholder_text = "Enter directory path"
        self.directory_path_var = tk.StringVar()
        self.directory_path_var.set(placeholder_text)

        self.directory_path_entry = tk.Entry(top_frame, textvariable=self.directory_path_var)
        self.directory_path_entry.config(fg="grey")
        self.directory_path_entry.grid(row=0, column=0, padx=5, pady=5)

        self.browse_button = tk.Button(top_frame, text="Browse", command=self.browse_directory, bg="#eb6b34")
        self.browse_button.grid(row=0, column=1, padx=5, pady=5)

        self.middle_frame = ttk.Frame(self.root)
        self.middle_frame.grid(row=1, column=0, sticky="nsew")
        
        self.x_axis_label = "Angle (deg)"
        self.y_axis_label = "Force (lbs)"
        self.y_scale = 10
        self.fig_width = 5
        self.fig_height = 4

        self.fig, self.ax = plt.subplots(figsize=(self.fig_width, self.fig_height))
        self.line, = self.ax.plot([], [], lw=2)
        self.ax.set_xlim(0, 100)
        self.ax.set_ylim(-2, self.y_scale + 5)
        self.ax.set_xlabel(self.x_axis_label)
        self.ax.set_ylabel(self.y_axis_label)

        self.canvas = FigureCanvasTkAgg(self.fig, master=self.middle_frame)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        self.right_frame = tk.Frame(self.root)
        self.right_frame.grid(row=0, column=1, rowspan=2, sticky="ns")
        
        self.export_button = tk.Button(self.right_frame, text="Export", command=self.export_data, bg="#eb6b34")
        self.export_button.grid(row=0, column=0, padx=5, pady=5, sticky="ew")
        
        self.start_button = tk.Button(self.right_frame, text="Start", command=self.start_process, bg="#eb6b34")
        self.start_button.grid(row=1, column=0, padx=5, pady=5, sticky="ew")
        
        self.stop_button = tk.Button(self.right_frame, text="Stop", command=self.stop_process, bg="#eb6b34")
        self.stop_button.grid(row=2, column=0, padx=5, pady=5, sticky="ew")

        self.root.grid_rowconfigure(1, weight=1)
        self.root.grid_columnconfigure(0, weight=1)

    def read_serial(self):
        while self.is_reading:
            try:
                self.ser.write(b'g')  # write to Arduino
                data = self.ser.readline().decode('utf-8').strip()
                angle, force = map(float, data.split(','))
                print(f"angle: {angle}, force: {force}")
                with self.lock:
                    self.angle_data.append(angle)
                    self.force_data.append(force)
                time.sleep(0.1)
            except Exception as e:
                print("Error reading data:", e)
                self.is_reading = False

    def update_plot(self):
        with self.lock:
            if self.angle_data and self.force_data:
                self.ax.clear()
                self.ax.plot(self.angle_data, self.force_data)
                self.ax.set_ylim(-2, self.y_scale + 5)
                self.ax.set_xlabel(self.x_axis_label)
                self.ax.set_ylabel(self.y_axis_label)
                self.canvas.draw()
        if self.is_reading:
            self.root.after(100, self.update_plot)

    def browse_directory(self):
        directory_path = filedialog.askdirectory()
        self.directory_path_var.set(directory_path)

    def start_process(self):
        if not self.directory_path_var.get() or self.directory_path_var.get() == "Enter directory path":
            messagebox.showerror("Error", "Please enter a directory path.")
            return

        session_name = simpledialog.askstring("Session Name", "Enter session name:")
        if not session_name:
            messagebox.showerror("Error", "Please enter a session name.")
            return

        session_folder_path = os.path.join(self.directory_path_var.get(), session_name)
        os.makedirs(session_folder_path, exist_ok=True)
        self.session_folder_path = session_folder_path

        self.is_reading = True
        self.serial_thread = threading.Thread(target=self.read_serial)
        self.serial_thread.start()
        self.update_plot()

    def export_data(self):
        if not self.angle_data or not self.force_data:
            messagebox.showerror("Error", "No data to export.")
            return

        x = datetime.datetime.now()
        filename = x.strftime("%Y-%m-%d_%H-%M-%S")
        self.csv_filename = filename + '.csv'
        self.pdf_filename = filename + '.pdf'
        
        self.is_reading = False
        self.serial_thread.join()

        self.export_csv()
        self.export_pdf()

        messagebox.showinfo("Export", f"CSV & PDF files exported successfully in {self.session_folder_path}")

    def stop_process(self):
        self.is_reading = False
        self.serial_thread.join()

    def export_csv(self):
        file_path = os.path.join(self.session_folder_path, self.csv_filename)
        with open(file_path, "w", newline="") as data_file:
            csv_writer = csv.writer(data_file)
            csv_writer.writerow([self.x_axis_label, self.y_axis_label])
            for x, y in zip(self.angle_data, self.force_data):
                csv_writer.writerow([x, y])

    def export_pdf(self):
        fig, ax = plt.subplots(figsize=(self.fig_width, self.fig_height))
        ax.plot(self.angle_data, self.force_data, lw=2)
        ax.set_xlim(min(self.angle_data), max(self.angle_data))
        ax.set_ylim(min(self.force_data), max(self.force_data))
        ax.set_xlabel(self.x_axis_label)
        ax.set_ylabel(self.y_axis_label)
        plt.tight_layout()
        file_path = os.path.join(self.session_folder_path, self.pdf_filename)
        with PdfPages(file_path) as pdf:
            pdf.savefig()

def main():
    root = tk.Tk()
    root.title("Real-Time Arduino Data Plot")
    port = "/dev/tty.usbmodem101"  # Update with your port
    baud = 9600  # Update with your baud rate
    arduino_plotter = App(root, port, baud)
    root.mainloop()

if __name__ == "__main__":
    main()
