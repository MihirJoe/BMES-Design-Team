import platform
import serial.tools.list_ports

def list_all_ports():
    ports = serial.tools.list_ports.comports()
    return [port.device for port in ports]

def find_active_port():
    os_name = platform.system()
    ports = list_all_ports()

    if ports:
        print("Available USports:")
        for index, port in enumerate(ports):
            print(f"{index + 1}: {port}")
        
        # Prompt the user to select a port
        try:
            choice = int(input("Select a port by number: ")) - 1
            if 0 <= choice < len(ports):
                print(f"Using active port: {ports[choice]}")
                return ports[choice]
            else:
                print("Invalid selection.")
        except ValueError:
            print("Please enter a valid number.")
    else:
        print("No ports available.")

    return None

# Example usage
active_port = find_active_port()

if active_port:
    print(f"Using active port: {active_port}")
else:
    print("No suitable port found. Please check your connection.")
