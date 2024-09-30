import serial
import time
import platform
import sys

class Actuator:
    def __init__(self):
        self.port = self.get_platform_port()
        self.arduino = None
        try:
            self.arduino = serial.Serial(port=self.port, baudrate=9600, timeout=1)
        except Exception as e:
            print('USB port not found. Arduino connection not established. Please check physical connection.')
            sys.exit(1)  # End program once serial exception is hit.
    
    def __del__(self):
        if self.arduino:
            self.arduino.close()  # Always close the connection when done

    def get_platform_port(self):
        try:
            if platform.system() == "Darwin":
                return '/dev/tty.usbmodem1201'
        except Exception as e:
            print('Arduino connection not established. Please check physical connection. Error: {}'.format(e))
        return ""

    def send_command(self, command):
        self.arduino.write(command.encode())  # Send the command to Arduino
        time.sleep(0.1)  # Wait for Arduino to process the command

    def rotate_motor(self, speed, direction):
        command = f"rotate,{speed},{direction}\n"
        self.send_command(command)

    def stop_motor(self):
        self.send_command("stop\n")
    
    def display_menu(self):
        print("Select Mode:")
        print("1: Run and Auto Retract")
        print("2: Run Only")
        print("3: Retract Only")

    def handle_user_selection(self):
        while True:
            self.display_menu()
            user_input = input("Enter the mode (1-3): ")
            
            if user_input == '1':  # Mode 1: Run and Auto Retract
                run_time = self.get_run_time()  # Get run time from the user
                self.run_and_auto_retract(run_time)
                break
            
            elif user_input == '2':  # Mode 2: Run Only
                run_time = self.get_run_time()  # Get run time from the user
                self.run_only(run_time)
            
            elif user_input == '3':  # Mode 3: Retract Only
                self.retract_only()  # Just retract the actuator
            
            else:
                print("Invalid input. Please enter a valid option (1-3).\n")

    def get_run_time(self):
        while True:
            try:
                run_time = float(input("Enter the run time in seconds (max 10.7 Seconds): "))
                if 0 < run_time <= 10.7:
                    return run_time
                else:
                    print("Please enter a value between 0 and 10.7")
            except ValueError:
                print("Invalid input. Please enter a numeric value.")

    def run_and_auto_retract(self, run_time):
        try:
            self.rotate_motor(100, 1)  # Rotate motor clockwise (CW) at 100% speed
            time.sleep(run_time)  # Run for user-defined time
            self.stop_motor()

            print("Retracting the actuator...")
            self.rotate_motor(100, 0)  # Rotate motor counterclockwise (CCW)
            time.sleep(10.78)  # Ensure full retraction
            self.stop_motor()

        except Exception as e:
            print(f"Error: {e}")

    def run_only(self, run_time):
        try:
            self.rotate_motor(100, 1)  # Rotate motor clockwise (CW)
            time.sleep(run_time)
            self.stop_motor()
        except Exception as e:
            print(f"Error: {e}")

    def retract_only(self):
        try:
            print("Retracting the actuator...")
            self.rotate_motor(100, 0)  # Rotate motor counterclockwise (CCW)
            time.sleep(10.78)  # Ensure full retraction
            self.stop_motor()
        except Exception as e:
            print(f"Error: {e}")


if __name__ == "__main__":
    try:
        actuator = Actuator()
        actuator.handle_user_selection()

    except KeyboardInterrupt:
        print("\nProgram interrupted by user. Closing connection...")
    
    # No need to explicitly close the connection; destructor will handle it.
