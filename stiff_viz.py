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
from matplotlib.backends.backend_pdf import PdfPages
# from arduino_read import *
import random
import numpy as np
import serial
import time
import threading
from Linear_actuator.actuator_basics import Actuator

class App(Actuator):
    def __init__(self, root, port, baud):
        #setting title
        self.port = port
        self.baud = baud
        self.time_list = []
        self.timestamp_label = "Timestamp"
        self.angle_data = []
        self.force_data = []
        self.stiffness_data = []
        self.moment_data = []
        self.root = root
        self.root.title("Data Visualizer")
        mainWinBgColor = "#eb6b34"
        self.root.configure(bg=mainWinBgColor)

        self.is_reading = False
        self.lock = threading.Lock()

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
            self.ser = serial.Serial(self.port, self.baud, timeout=3)
            print("Serial connection established.")
            print("Calibrating sensors...")
            time.sleep(15)
            calibration_message = self.ser.readline().decode('utf-8')
            # print(calibration_message)
            if calibration_message == "a":
                print("Sensors Calibrated!")
            else:
                print("a not received")
        except Exception as e:
            print("Failed to establish serial connection:", e)
            return
        
        # time.sleep(2)
        
        time.sleep(0.1)
        
        # Pull Request Test personal branch
        

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

        self.x_axis_label = "Angle (deg)"
        self.y_axis_label = "Moment (Nm)"
        self.force_label = "Force (lbs)"
        self.stiffness_label = "Stiffness (Nm/deg)"
        self.y_scale = 10
        self.fig_width = 5
        self.fig_height = 4

        self.fig, self.ax = plt.subplots(figsize=(self.fig_width, self.fig_height))
        # self.line, = self.ax.plot([], [], lw=2)
        self.ax.scatter([], [])
        self.ax.set_xlim(0, 100)
        self.ylim = 1
        self.ax.set_ylim(-self.ylim, self.ylim) # TODO: dynamically adjust ylim
        self.ax.set_xlabel(self.x_axis_label)
        self.ax.set_ylabel(self.y_axis_label)
        # self.ax.set_title(self.session_folder_path) # TODO: set title to data file name


        # self.fig, self.ax = plt.subplots(figsize=(5, 4))  # Adjust figsize as needed
        # self.canvas = FigureCanvasTkAgg(self.fig, master=self.middle_frame)
        # self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        self.canvas = FigureCanvasTkAgg(self.fig, master=self.middle_frame)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        # --- Right Section ---
        self.right_frame = tk.Frame(self.root)
        self.right_frame.grid(row=0, column=1, rowspan=2, sticky="ns")
        
        self.export_button = tk.Button(self.right_frame, text="Export", command=self.export_data, bg="#eb6b34")
        self.export_button.grid(row=0, column=0, padx=5, pady=5, sticky="ew")
        
        self.start_button = tk.Button(self.right_frame, text="Start", command=self.start_process, bg="#eb6b34")
        self.start_button.grid(row=1, column=0, padx=5, pady=5, sticky="ew")
        
        self.stop_button = tk.Button(self.right_frame, text="Stop", command=self.stop_process, bg="#eb6b34")
        self.stop_button.grid(row=2, column=0, padx=5, pady=5, sticky="ew")

        # self.force_button = tk.Button(self.right_frame, text="Force", command=self.linear_act_process, bg="#eb6b34")
        self.force_button = tk.Button(self.right_frame, text="Force", command=self.force_button, bg="#eb6b34")
        self.force_button.grid(row=3, column=0, padx=5, pady=5, sticky="ew")
        
        # self.next_button = tk.Button(self.right_frame, text="Next", command=self.next_step, bg="#eb6b34")
        # self.next_button.grid(row=3, column=0, padx=5, pady=5, sticky="ew")


        # Configure Grid Weight to Allow Resizing
        self.root.grid_rowconfigure(1, weight=1)
        self.root.grid_columnconfigure(0, weight=1)

    def read_serial(self):
        while self.is_reading:
            try: 
                self.ser = serial.Serial(self.port, self.baud, timeout=0.1)
                self.ser.write(b'g') # write to Arduino
                data = self.ser.readline().decode('utf-8').strip()

                angle, force = map(float, data.split(','))

                print("Received angle:", angle)  # Add this line for debugging
                print("Received force:", force)  # Add this line for debugging

                with self.lock:
                        self.time_list.append(time.time())

                        # add adjusted angle

                        # calculate moment
                        X_m = 0.3683 # meters (from knee)
                        force_newtons = force * 4.448222 # convert to knewtons
                        moment = X_m * force_newtons 
                        

                        self.angle_data.append(angle)
                        self.moment_data.append(moment)
                        self.force_data.append(force)
                        self.stiffness_data.append(np.abs(moment / (angle + 0.0001)))
                time.sleep(0.1)


            except Exception as e:
                print("Error reading data:", e)
                self.is_reading = False
                self.ser.close()


    def update_plot(self):

        with self.lock:
            if self.angle_data and self.moment_data:
                self.ax.clear()
                self.ax.scatter(self.angle_data, self.moment_data)
                # self.ax.set_ylim(-2, self.y_scale + 5)
                self.ax.set_xlim(min(self.angle_data) - 2, max(self.angle_data) + 2)
                # self.ax.set_ylim(min(self.force_data) - 5, max(self.moment_data) + 5)
                self.ax.set_ylim(min(self.moment_data) - 0.2, max(self.moment_data) + 0.2)
                self.ax.set_xlabel(self.x_axis_label)
                self.ax.set_ylabel(self.y_axis_label)
                self.canvas.draw()
        if self.is_reading:
            self.root.after(100, self.update_plot)

    # def update_plot(self):
    #     with self.lock:
    #         if self.angle_data and self.force_data:
    #             self.scatter.set_offsets(np.c_[self.angle_data[-100:], self.force_data[-100:]])  # Update the last 100 points
    #             self.ax.set_xlim(min(self.angle_data[-100:]), max(self.angle_data[-100:]))
    #             self.ax.set_ylim(min(self.force_data[-100:]), max(self.force_data[-100:]))
    #             self.canvas.draw()
    #     if self.is_reading:
    #         self.root.after(100, self.update_plot)


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
        self.is_reading = True
        self.serial_thread = threading.Thread(target=self.read_serial)
        self.serial_thread.start()
        self.update_plot()

        # self.export_csv()  # Assuming this method exists

    def export_data(self):
        x = datetime.datetime.now()
        filename = x.strftime("%Y-%m-%d_%H-%M-%S")
        self.csv_filename = filename + '.csv'
        self.pdf_filename = filename + '.pdf'
        # if not filename.endswith('.csv'):
        #     filename += '.csv'
        # self.data_file_name = filename
        self.is_reading = False
        # self.serial_thread.join()
        
        # Export data to CSV & PDF
        self.export_csv()
        self.export_pdf()

        messagebox.showinfo("Export", f"CSV & PDF files exported successfully in {self.session_folder_path}")

    def stop_process(self):

        # Stop recording
        self.is_reading = False
        self.serial_thread.join()

    def force_button(self):
        dialog = tk.Toplevel()
        v = tk.IntVar(1)
        option_1 = tk.Radiobutton(dialog,variable=v,value=1,text="Run and Auto Retract")
        option_2 = tk.Radiobutton(dialog,variable=v,value=2,text="Run Only")
        option_3 = tk.Radiobutton(dialog,variable=v,value=2,text="Retract Only")

        self.run_and_auto_retract(10.7)

    # def run_and_auto_retract(self):
    #     try:
    #         # Extend the actuator
    #         print("Extending actuator...")
    #         self.ser.write(b'e')  # Command to extend actuator

    #         # Wait for actuator to extend fully
    #         time.sleep(2)  # Adjust based on actuator speed and required extension time

    #         # Retract the actuator
    #         print("Retracting actuator...")
    #         self.ser.write(b'r')  # Command to retract actuator

    #         # Wait for actuator to retract fully
    #         time.sleep(2)  # Adjust based on actuator speed and required retraction time

    #         # Optional: Automatically start the process again if needed
    #         print("Cycle complete. Restarting process.")
    #         self.start_process()  # Starts the process again if needed

        # except Exception as e:
        #     print("Error controlling actuator:", e)

        

    def next_step(self):
        # Implement next step functionality here

        # TODO: save time and force columns to same CSV for "n" sessions
        # for this, we should append all the data to a dataframe and then 
        pass

    def export_csv(self):
        file_path = os.path.join(self.session_folder_path, self.csv_filename)
        with open(file_path, "w", newline="") as data_file:
            csv_writer = csv.writer(data_file)
            csv_writer.writerow([self.timestamp_label, self.x_axis_label, self.y_axis_label, self.force_label, self.stiffness_label])
            
            for t, x, y, force, stiffness in zip(self.time_list, self.angle_data, self.moment_data, self.force_data, self.stiffness_data):
                csv_writer.writerow([t, x, y, force, stiffness])
                # for i in range(len(self.time_list)):
                    

    def export_pdf(self):

        # Plot figure

        self.fig, self.ax = plt.subplots(figsize=(self.fig_width, self.fig_height))
        # self.line, = self.ax.scatter(self.angle_data, self.force_data, lw=2)
        self.ax.scatter(self.angle_data, self.moment_data)
        self.ax.set_xlim(min(self.angle_data) - 3, max(self.angle_data) + 3)
        self.ax.set_ylim(min(self.moment_data) - 0.2, max(self.moment_data) + 0.2)
        self.ax.set_xlabel(self.x_axis_label)
        self.ax.set_ylabel(self.y_axis_label)

        plt.tight_layout()
        file_path = os.path.join(self.session_folder_path, self.pdf_filename)
        with PdfPages(file_path) as pdf:
            pdf.savefig()

 
def main():
    root = tk.Tk()
    root.title("Real-Time Arduino Data Plot")
    port = "/dev/tty.usbmodem1201" #"/dev/tty.usbmodem2101"  # Update with your port
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
    