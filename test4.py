import serial
import time
import tkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

class ArduinoPlotter:
    def __init__(self, master, port, baud):
        self.master = master
        self.port = port
        self.baud = baud
        self.time_list = []
        self.dataList = []

        try:
            self.ser = serial.Serial(self.port, self.baud, timeout=2)
            print("Serial connection established.")
        except Exception as e:
            print("Failed to establish serial connection:", e)
            return

        time.sleep(2)

        self.fig = Figure(figsize=(8, 6))
        self.ax = self.fig.add_subplot(111)
        self.ax.set_xlabel('Time')
        self.ax.set_ylabel('Data')
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.master)
        self.canvas_widget = self.canvas.get_tk_widget()
        self.canvas_widget.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        self.start_button = tk.Button(master, text="Start", command=self.start)
        self.start_button.pack(side=tk.BOTTOM)

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
            self.master.after(100, self.update_plot)  # Schedule the update every 100 milliseconds
        except Exception as e:
            print("Error reading data:", e)
            # Handle serial communication error gracefully
            self.ser.close()  # Close the serial connection
            return

    def start(self):
        self.update_plot()

def main():
    root = tk.Tk()
    root.title("Real-Time Arduino Data Plot")
    port = "/dev/tty.usbmodem2101"  # Update with your port
    baud = 9600  # Update with your baud rate
    arduino_plotter = ArduinoPlotter(root, port, baud)
    root.mainloop()

if __name__ == "__main__":
    main()
