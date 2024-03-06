import time
from serial import Serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation

class AnimationPlot:
    def __init__(self, x_limit):
        self.window_size = 50  # Number of samples to show in the window
        self.x_limit = x_limit  # X limit where the red slider will stay fixed

    def animate(self, i, dataList, ser):
        ser.write(b'g')                                     
        arduinoData_string = ser.readline().decode('ascii') 
        
        try:
            arduinoData_float = float(arduinoData_string)
            dataList.append(arduinoData_float)             
        except:                                            
            pass

        num_samples = len(dataList)
        start_index = max(0, num_samples - self.window_size)  # Start index for plotting
        end_index = min(self.x_limit, num_samples - 1)  # End index for plotting

        ax.clear()                                          
        self.getPlotFormat()
        ax.plot(range(start_index, num_samples), dataList[start_index:])  # Plot from start_index to end
        
        # Add red line marker for the most recent value
        if num_samples > 0:
            ax.axvline(x=end_index, color='red', linestyle='-')
            ax.plot(end_index, dataList[end_index], 'k*')  # Black star on the actual value of y

        # Update x-axis limits to show the window
        ax.set_xlim([start_index, self.x_limit])
        ax.set_ylim([min(dataList), max(dataList)])  # Y-axis fits based on data range

    def getPlotFormat(self):
        ax.set_title("Arduino Data")  # Set title of figure
        ax.set_ylabel("Weight (lbs)")  # Set title of y axis

x_limit = 100  # Specify the x limit
realTimePlot = AnimationPlot(x_limit)

dataList = []                                           # Create empty list variable for later use
                                                        
fig = plt.figure()                                      # Create Matplotlib plots fig is the 'higher level' plot window
ax = fig.add_subplot(111)                               # Add subplot to main fig window

realTimePlot = AnimationPlot()

arduino_port = "COM4"
ser = Serial(arduino_port, 9600)                         # Establish Serial object with COM port and BAUD rate to match Arduino Port/rate
time.sleep(2)                                           # Time delay for Arduino Serial initialization 

                                                        # Matplotlib Animation Fuction that takes takes care of real time plot.
                                                        # Note that 'fargs' parameter is where we pass in our dataList and Serial object. 
ani = animation.FuncAnimation(fig, realTimePlot.animate, fargs=(dataList, ser), interval=100) 

# TODO: add red line marker on plot 
# TODO: get time values from arduino

plt.show()                                              # Keep Matplotlib plot persistent on screen until it is closed
ser.close()  