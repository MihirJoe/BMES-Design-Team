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
        #setting title
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
        # self.directory_path_label = tk.Label(top_frame, text="Enter directory path:", bg="#eb6b34")
        # self.directory_path_label.place(x=20, y=20)
        # self.directory_path_label.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        self.directory_path_entry = tk.Entry(top_frame, textvariable=self.directory_path_var)
        # self.directory_path_entry.place(x=200, y=20)
        self.directory_path_entry.config(fg="grey")
        self.directory_path_entry.grid(row=0, column=0, padx=5, pady=5)
        self.browse_button = tk.Button(top_frame, text="Browse", command=self.browse_directory, bg="#eb6b34")
        # self.browse_button.place(x=400, y=15)
        self.browse_button.grid(row=0, column=1, padx=5, pady=5)

        # --- Middle Section --- 
        self.middle_frame = ttk.Frame(self.root)
        self.middle_frame.grid(row=1, column=0, sticky="nsew")
        
        self.fig, self.ax = plt.subplots(figsize=(5, 4))  # Adjust figsize as needed
        self.plot_canvas = FigureCanvasTkAgg(self.fig, master=self.middle_frame)
        self.plot_canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
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


        # --- Middle Section ---

        # self.fig, self.ax = plt.subplots(figsize=(3, 2))
        # self.line, = self.ax.plot([], [], lw=2)
        # self.ax.set_xlim(0, 100)
        # self.ax.set_ylim(0, 1)
        # self.ax.set_xlabel('Time')
        # self.ax.set_ylabel('Value')

        # # Middle Section: Matplotlib Plot
        # self.middle_frame = ttk.Frame(root)
        # self.middle_frame.grid(row=1, column=0, sticky="nsew")
        
        # self.fig, self.ax = plt.subplots(figsize=(5, 4))  # Adjust figsize as needed
        # self.plot_canvas = FigureCanvasTkAgg(self.fig, master=self.middle_frame)
        # self.plot_canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        # # --- Right Section ---
        # self.right_frame = tk.Frame(self.root)
        # self.middle_frame.grid(row=0, column=1)

        # # Create frame for buttons
        # self.button_frame = ttk.Frame(self.right_frame)
        # self.button_frame.grid(row=0, column=2)
        # # self.button_frame.pack(side=tk.RIGHT, padx=10, pady=10)

        # # Start button
        # self.start_button = tk.Button(self.button_frame, text="Start", command=self.start_process, bg="#eb6b34")
        # # self.start_button.place(x=1145, y=280, width=195, height=83)
        # self.start_button.grid(row=1,column=1, pady=(10,5), padx=5, sticky='ew')
        # # Export Button
        # self.export_button = tk.Button(self.button_frame, text="Export", command=self.export_data, bg="#eb6b34")
        # # self.export_button.place(x=1145, y=80, width=195, height=83)
        # self.export_button = tk.Button(self.button_frame, text="Export", bg="#eb6b34")
        # # self.export_button.place(x=25, y=390, width=195, height=83)
        # self.export_button.grid(row=0,column=1, pady=(10,5), padx=5, sticky='ew')

        # Right Section: 4 Buttons Stacked Vertically
        # self.right_frame = ttk.Frame(root)
        # self.right_frame.grid(row=0, column=1, rowspan=2, sticky="ns")
        
        # self.button1 = ttk.Button(self.right_frame, text="Button 1")
        # self.button1.grid(row=0, column=0, padx=5, pady=5, sticky="ew")
        
        # self.button2 = ttk.Button(self.right_frame, text="Button 2")
        # self.button2.grid(row=1, column=0, padx=5, pady=5, sticky="ew")
        
        # self.button3 = ttk.Button(self.right_frame, text="Button 3")
        # self.button3.grid(row=2, column=0, padx=5, pady=5, sticky="ew")
        
        # self.button4 = ttk.Button(self.right_frame, text="Button 4")
        # self.button4.grid(row=3, column=0, padx=5, pady=5, sticky="ew")

        # self.add_buttons(self.button_frame)

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

        # Plotting interface

        # self.fig, self.ax = plt.subplots(figsize=(3, 2))
        # self.line, = self.ax.plot([], [], lw=2)
        # self.ax.set_xlim(0, 100)
        # self.ax.set_ylim(0, 1)
        # self.ax.set_xlabel('Time')
        # self.ax.set_ylabel('Value')

        # self.canvas = FigureCanvasTkAgg(self.fig, master=root)
        # self.canvas_widget = self.canvas.get_tk_widget()
        # # self.canvas_widget.pack(side=tk.LEFT, fill=tk.BOTH, expand=1)
        # # self.canvas_widget.pack(side=tk.LEFT, fit=tk.BOTH, expand=1)
        # self.canvas_widget.grid(row=0, column=0, sticky="nsew")

        # root.grid_rowconfigure(0, weight=1)
        # root.grid_columnconfigure(0, weight=1)

        # self.anim = None
        # self.data = []
        # dataList, ser = getArduinoSerial("/dev/tty.usbmodem2101", 9600) # "/dev/tty.usbmodem2101", 9600
        
        # # Create a Tkinter canvas
        # canvas = FigureCanvasTkAgg(plotArduino(dataList, ser, root), master=root)
        # canvas.draw()
        
        # # Place the canvas on the Tkinter window
        # canvas.get_tk_widget().pack()


        # Additional buttons
        

        # self.create_plot(root)

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
    # def create_plot(self, root):
    #     # Create Matplotlib figure and axes with smaller size
    #     self.fig, self.ax = plt.subplots(figsize=(6, 4))  # Decrease the size here
    #     self.ax.set_title("Animated Plot")
    #     self.ax.set_xlabel("X-axis")
    #     self.ax.set_ylabel("Y-axis")

    #     # Create initial empty plot
    #     self.line, = self.ax.plot([], [])

    #     # Embed the Matplotlib plot into Tkinter GUI
    #     self.canvas = FigureCanvasTkAgg(self.fig, master=root)
    #     self.canvas.draw()
    #     self.canvas.get_tk_widget().place(x=0, y=0)

    #     # Start the animation
    #     self.start_animation()

    # # def update_plot(self, frame):
    # #     # Function to update the plot with random data
    # #     x = range(frame)
    # #     y = [random.randint(0, 100) for _ in x]
    # #     self.line.set_data(x, y)
    # #     realTimePlot = AnimationPlot(self.ax)
    # #     # Set plot limits to ensure all data points are visible
    # #     self.line.axes.set_xlim(0, max(x))
    # #     self.line.axes.set_ylim(0, max(y))

    # #     return self.line,

    

    # def start_animation(self):
    #     # Function to start the animation
    #     def animate(self, dataList, ser):
    #         ser.write(b'g')                                     # Transmit the char 'g' to receive the Arduino data point
    #         arduinoData_string = ser.readline().decode('ascii') # Decode receive Arduino data as a formatted string
    #         #print(i)                                           # 'i' is a incrementing variable based upon frames = x argument

    #         try:
    #             arduinoData_float = float(arduinoData_string)   # Convert to float
    #             dataList.append(arduinoData_float)              # Add to the list holding the fixed number of points to animate

    #         except:                                             # Pass if data point is bad                               
    #             pass

    #         dataList = dataList[-50:]                           # Fix the list size so that the animation plot 'window' is x number of points
            
    #         # self.ax.clear()                                          # Clear last data frame
            
    #         # self.getPlotFormat()
    #         # self.ax.plot(dataList) 
        

    #     # Create FuncAnimation for animated plot
    #     dataList, ser = getArduinoSerial("/dev/tty.usbmodem2101", 9600)
        
    #     self.ani = animation.FuncAnimation(self.fig, animate(dataList, ser), frames=100, interval=100, blit=True) 

    #     # Call the function to periodically update the plot
    #     self.update_plot_gui()

    # def update_plot_gui(self):
    #     # Function to update the plot within Tkinter GUI
    #     self.canvas.draw()

    #     # Schedule the next update after 100 milliseconds
    #     self.canvas._tkcanvas.after(100, self.update_plot_gui)



    # def embed_plot(self, root):
    #     # Create Matplotlib figure and axes
    #     fig, ax = plt.subplots(figsize=(6, 4))
    #     ax.set_title("Animated Plot")
    #     ax.set_xlabel("X-axis")
    #     ax.set_ylabel("Y-axis")

    #     dataList, ser = getArduinoSerial("/dev/tty.usbmodem2101", 9600) # "/dev/tty.usbmodem2101", 9600
    #     realTimePlot = AnimationPlot(ax)
    #     # Create FuncAnimation for animated plot
    #     ani = animation.FuncAnimation(fig, realTimePlot.animate, frames=100, fargs=(dataList, ser), interval=100) 

    #     # Embed the Matplotlib plot into Tkinter GUI
    #     canvas = FigureCanvasTkAgg(fig, master=root)
    #     canvas.draw()
    #     canvas.get_tk_widget().place(x=0, y=0)

    #     self.start_animation(root, ani)


    def start_animation(self):
        self.start_button.config(state=tk.DISABLED)  # Disable the start button
        self.anim = FuncAnimation(self.fig, self.update_plot, interval=100)
        self.canvas.draw()

    def stop_animation(self):
        if self.anim:
            self.anim.event_source.stop()
            self.start_button.config(state=tk.NORMAL)  # Enable the start button when animation stops
            #self.save_data_to_csv()

    def update_plot(self, frame):
        x = np.linspace(0, 100, 100)
        y = np.random.rand(100)
        self.line.set_data(x, y)
        self.data.append((x, y))  # Store data for saving to CSV
        return self.line,
    

    def add_buttons(self, root):
        # Export button

        # Calibrate button
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

    def export_data(self):
        x = datetime.datetime.now()
        filename = x.strftime("%Y-%m-%d_%H-%M-%S")
        if not filename.endswith('.csv'):
            filename += '.csv'
        self.data_file_name = filename
        self.export_csv()
        # Update the label with the exported file name
        self.exported_file_label.config(text=self.data_file_name)

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

 
  
  


if __name__ == "__main__":
    root = tk.Tk()
    app = App(root)
    root.mainloop()
