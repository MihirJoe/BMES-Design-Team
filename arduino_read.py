import time
from serial import Serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from arduino_connect import *
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

class AnimationPlot:

    def __init__(self, ax):
        self.ax = ax

    def animate(self, i, dataList, ser):
        ser.write(b'g')                                     # Transmit the char 'g' to receive the Arduino data point
        arduinoData_string = ser.readline().decode('ascii') # Decode receive Arduino data as a formatted string
        #print(i)                                           # 'i' is a incrementing variable based upon frames = x argument

        try:
            arduinoData_float = float(arduinoData_string)   # Convert to float
            dataList.append(arduinoData_float)              # Add to the list holding the fixed number of points to animate

        except:                                             # Pass if data point is bad                               
            pass

        dataList = dataList[-50:]                           # Fix the list size so that the animation plot 'window' is x number of points
        
        self.ax.clear()                                          # Clear last data frame
        
        self.getPlotFormat()
        self.ax.plot(dataList)                                   # Plot new data frame
        

    def getPlotFormat(self):
        self.ax.set_ylim([-10, 10])                              # Set Y axis limit of plot
        self.ax.set_title("Arduino Data")                        # Set title of figure
        self.ax.set_ylabel("Weight (lbs)")                              # Set title of y axis
                                       

def plotArduino(dataList, ser, root):

    fig = plt.figure()                                      # Create Matplotlib plots fig is the 'higher level' plot window
    ax = fig.add_subplot(111)                               # Add subplot to main fig window

    realTimePlot = AnimationPlot(ax)
                                                       
                                                        # Note that 'fargs' parameter is where we pass in our dataList and Serial object. 
    ani = animation.FuncAnimation(fig, realTimePlot.animate, frames=100, fargs=(dataList, ser), interval=100) 

    plt.show()
    ser.close()

# TODO: add red line marker on plot (ahmad)
# TODO: get time values from arduino (mihir)
# TODO: show plot on GUI (mihir)

# plt.show()                                              # Keep Matplotlib plot persistent on screen until it is closed
# ser.close()  