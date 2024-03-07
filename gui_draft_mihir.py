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
import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
# from arduino_read import *
import random
import numpy as np
import serial
import time

class App:
    def __init__(self, root, port, baud):
        #setting title
        self.port = port
        self.baud = baud
        self.time_list = []
        self.dataList = []
        self.root = root
        self.root.title("Data Visualizer")
        mainWinBgColor = "#eb6b34"
        self.root.configure(bg=mainWinBgColor)
        #setting window size
        # width=956
        # height=644
        width=1350
        height=800
        screenwidth = self.root.winfo_screenwidth()
        screenheight = self.root.winfo_screenheight()
        alignstr = '%dx%d+%d+%d' % (width, height, (screenwidth - width) / 2, (screenheight - height) / 2)
        self.root.geometry(alignstr)
        self.root.resizable(width=True, height=True)

        # Establish Serial Connection

        try:
            self.ser = serial.Serial(self.port, self.baud, timeout=2)
            print("Serial connection established.")
        except Exception as e:
            print("Failed to establish serial connection:", e)
            return

        time.sleep(2)

        # Set the font size to 12
        style = Style()
        style.configure('TButton', font=('calibri', 20, 'bold'), borderwidth='4')
        style.map('TButton', foreground=[('active', '!disabled', 'green')], background=[('active', 'black')])


        # --- Top Section ---
        self.top_frame = tk.Frame(self.root)
        self.top_frame.grid(row=0, column=0, sticky="ew")

        top_frame = self.top_frame
        
        # Directory path entry
        placeholder_text = "Enter directory path"
        self.directory_path_var = tk.StringVar()
        self.directory_path_var.set(placeholder_text)

        self.directory_path_entry = tk.Entry(top_frame, textvariable=self.directory_path_var)
        self.directory_path_entry.config(fg="grey")
        self.directory_path_entry.grid(row=0, column=0, padx=5, pady=5)

        self.browse_button = tk.Button(top_frame, text="Browse", command=self.browse_directory, bg="#eb6b34")
        self.browse_button.grid(row=0, column=1, padx=5, pady=5)

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
        # self.ax.set_title(self.session_folder_path) # TODO: set title to data file name


        # self.fig, self.ax = plt.subplots(figsize=(5, 4))  # Adjust figsize as needed
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.middle_frame)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        # self.fig = Figure(figsize=(5, 4))
        # self.ax = self.fig.add_subplot(111)
        # self.ax.set_xlabel(self.x_axis_label)
        # self.ax.set_ylabel(self.y_axis_label)
        # self.canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        # self.canvas_widget = self.canvas.get_tk_widget()
        # self.canvas_widget.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        # self.anim = None
        # self.data = []
        
        # --- Right Section ---
        self.right_frame = tk.Frame(self.root)
        self.right_frame.grid(row=0, column=1, rowspan=2, sticky="ns")
        
        self.export_button = tk.Button(self.right_frame, text="Export", command=self.export_data, bg="#eb6b34")
        self.export_button.grid(row=0, column=0, padx=5, pady=5, sticky="ew")
        
        self.start_button = tk.Button(self.right_frame, text="Start", command=self.start_process, bg="#eb6b34")
        self.start_button.grid(row=1, column=0, padx=5, pady=5, sticky="ew")
        
        self.stop_button = tk.Button(self.right_frame, text="Stop", command=self.stop_process, bg="#eb6b34")
        self.stop_button.grid(row=2, column=0, padx=5, pady=5, sticky="ew")
        
        self.next_button = tk.Button(self.right_frame, text="Next", command=self.next_step, bg="#eb6b34")
        self.next_button.grid(row=3, column=0, padx=5, pady=5, sticky="ew")


        # Configure Grid Weight to Allow Resizing
        self.root.grid_rowconfigure(1, weight=1)
        self.root.grid_columnconfigure(0, weight=1)

        

        

        # GLabel_237=tk.Label(root)
        # GLabel_237["bg"] = "#eb6b34"
        # ft = tkFont.Font(family='Times',size=14)
        # GLabel_237["font"] = ft
        # GLabel_237["fg"] = "#000000"
        # GLabel_237["justify"] = "center"
        # GLabel_237["text"] = self.start_process
        # GLabel_237.place(relx=0.99, rely=1.0, anchor='se')


        # GLabel_2=tk.Label(root)
        # GLabel_2["bg"] = "#eb6b34"
        # ft = tkFont.Font(family='Times',size=14)
        # GLabel_2["font"] = ft
        # GLabel_2["fg"] = "#000000"
        # GLabel_2["justify"] = "center"
        # x = "hello world."
        # GLabel_2["text"] = x
        # GLabel_2.place(relx=0.88, rely=0.0, anchor='se')

    def update_plot(self):
        try:
            self.ser.write(b'g') # write to Arduino
            data = self.ser.readline().decode('utf-8').strip()
            print("Received data:", data)  # Add this line for debugging
            if data:
                self.time_list.append(time.time())
                self.dataList.append(float(data))
                self.ax.clear()
                self.ax.plot(self.time_list, self.dataList)
                self.canvas.draw()
            self.root.after(100, self.update_plot)  # Schedule the update every 100 milliseconds
        except Exception as e:
            print("Error reading data:", e)
            # Handle serial communication error gracefully
            self.ser.close()  # Close the serial connection
            return


    def on_entry_click(self, event):
        """Function to handle click on the Entry widget."""
        if self.directory_path_var.get() == "Enter directory path...":
            self.directory_path_entry.delete(0, tk.END)  # Clear the Entry widget
            self.directory_path_entry.config(fg='black')  # Change text color to black

    def on_focus_out(self, event):
        """Function to handle focus out from the Entry widget."""
        if not self.directory_path_var.get():
            self.directory_path_entry.insert(0, "Enter directory path...")  # Restore placeholder text
            self.directory_path_entry.config(fg='grey')  # Change text color to grey
    
    # -- Plotting Logic --

    
    
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

        # Start plotting animation
        self.update_plot()

        # self.export_csv()  # Assuming this method exists

    def export_data(self):
        x = datetime.datetime.now()
        filename = x.strftime("%Y-%m-%d_%H-%M-%S")
        if not filename.endswith('.csv'):
            filename += '.csv'
        self.data_file_name = filename
        self.export_csv()
        # Update the label with the exported file name
        # self.exported_file_label.config(text=self.data_file_name)

    def save_data_to_csv(self):
        if self.data:
            with open('live_plot_data.csv', 'w', newline='') as csvfile:
                writer = csv.writer(csvfile)
                writer.writerow(['Time', 'Force'])
                for x, y in self.data:
                    for i in range(len(x)):
                        writer.writerow([x[i], y[i]])

    def calibrate(self):
        
        # TODO: implement calibration logic manually or automatically

        pass

    def stop_process(self):

        # Stop recording
        self.stop_animation()

    def next_step(self):
        # Implement next step functionality here

        # TODO: save time and force columns to same CSV for "n" sessions
        # for this, we should append all the data to a dataframe and then 
        pass

    def export_csv(self):
        file_path = os.path.join(self.session_folder_path, self.data_file_name)
        with open(file_path, "w", newline="") as data_file:
            csv_writer = csv.writer(data_file)
            csv_writer.writerow([self.x_axis_label, self.y_axis_label])
            for x, y in self.data:
                for i in range(len(x)):
                    csv_writer.writerow([x[i], y[i]])

        messagebox.showinfo("Export", f"CSV file exported successfully as {self.data_file_name}")

 
def main():
    root = tk.Tk()
    root.title("Real-Time Arduino Data Plot")
    port = "/dev/tty.usbmodem2101"  # Update with your port
    baud = 9600  # Update with your baud rate
    arduino_plotter = App(root, port, baud)
    root.mainloop()
  

if __name__ == "__main__":
    main()



# TODO:
    
    # 1. play/pause
    # 2. gray out all buttons at start
    # 3. change text field to directory label
    # 4. change plot title to session name
        # 4a. add session label and trial at the top
    # 5. implement next session logic
    # 6. format CSV to have session name as header
    # 7. Get arduino timestamp
    # 8. Arduino in real-time formatted plot & data export
    # 9. Update UI layout
    # 10. implement PDF
    # 11. Format CSV to accomodate multiple sessions
    