import serial
import time
import platform
import sys

def get_platform_port():
    port = ""
    try:
        if platform.system() == "Darwin":
            port = '/dev/tty.usbmodem1201'
        return port
    except Exception as e:
        print('Arduino connection not established. Please check physical connection. Error: {}'.format(e))





    # Set up serial communication with the Arduino
try:
    arduino = serial.Serial(port=get_platform_port(), baudrate=9600, timeout=1)
except Exception as e:
    # print('Arduino connection not established. Please check physical connection. Error: {}'.format(e))
    print('USB port not found. Arduino connection not established. Please check physical connection.')
    sys.exit(1) # End program once exception serial exception is hit. Priority: High!



def send_command(command):
    arduino.write(command.encode())  # Send the command to Arduino
    time.sleep(0.1)  # Wait for Arduino to process the command

def rotate_motor(speed, direction):
    command = f"rotate,{speed},{direction}\n"
    send_command(command)

def stop_motor():
    send_command("stop\n")
 
# Display menu to user
def display_menu():
    print("Select Mode:")
    print("1: Run and Auto Retract")
    print("2: Run Only")
    print("3: Retract Only")

# Function to handle user interaction
def handle_user_selection():
    while True:
        display_menu()
        user_input = input("Enter the mode (1-3): ")
        
        if user_input == '1':  # Mode 1: Run and Auto Retract
            run_time = get_run_time()  # Get run time from the user
            run_and_auto_retract(run_time)
            break
        
        elif user_input == '2':  # Mode 2: Run Only
            run_time = get_run_time()  # Get run time from the user
            run_only(run_time)
        
        elif user_input == '3':  # Mode 3: Retract Only
            retract_only()  # Just retract the actuator
        
        else:
            print("Invalid input. Please enter a valid option (1-3).\n")

def get_run_time():
    # Ask user for how many seconds to run the motor
    while True:
        try:
            run_time = float(input("Enter the run time in seconds (max 10.6 Seconds): "))
            if 0 < run_time <= 10.6:
                return run_time
            else:
                print("Please enter a value between 0 and 10.6.")
        except ValueError:
            print("Invalid input. Please enter a numeric value.")

# Function for Mode 1: Run and Auto Retract
def run_and_auto_retract(run_time):
    try:
        # Rotate motor clockwise (CW) at 100% speed for user-specified time
        rotate_motor(100, 1)  # 1 for CW
        time.sleep(run_time)  # Run for user-defined time
        stop_motor()

        # Rotate motor counterclockwise (CCW) to fully retract the actuator
        print("Retracting the actuator...")
        rotate_motor(100, 0)  # 0 for CCW
        time.sleep(10.78)  # Run for 10.78 seconds to ensure full retraction (150mm/14mm per second)
        stop_motor()

    except Exception as e:
        print(f"Error: {e}")

# Function for Mode 2: Run Only
def run_only(run_time):
    try:
        # Rotate motor clockwise (CW) at 100% speed for user-specified time
        rotate_motor(100, 1)  # 1 for CW
        time.sleep(run_time)  # Run for user-defined time
        stop_motor()

    except Exception as e:
        print(f"Error: {e}")

# Function for Mode 3: Retract Only
def retract_only():
    try:
        # Rotate motor counterclockwise (CCW) to fully retract the actuator
        print("Retracting the actuator...")
        rotate_motor(100, 0)  # 0 for CCW
        time.sleep(10.78)  # Run for 10.78 seconds to ensure full retraction
        stop_motor()

    except Exception as e:
        print(f"Error: {e}")

# Main execution loop
try:
    handle_user_selection()

except KeyboardInterrupt:
    print("\nProgram interrupted by user. Closing connection...")

finally:
    arduino.close()  # Always close the connection when done
