from serial import Serial
import time


class CalibrationException(Exception):
    pass


class Arduino:
    """
    The Arduino driver.
    """

    # Implemented class structure
    def __init__(self, port: str):
        self.serial = Serial(port=port, baudrate=9600)
        self.serial.timeout = 30
        if self.serial.readline() != b"a":
            raise CalibrationException()

        self.serial.timeout = 1

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def close(self):
        self.serial.close()  # Always close the connection when done

    def _send_command(self, command: str):
        self.serial.write(command.encode())  # Send the command to Arduino
        self.serial.write(b"\n")
        time.sleep(0.1)  # Wait for Arduino to process the command

    def send_measure_command(self):
        self._send_command("m")

    def recv_measurement(self) -> tuple[float, float]:
        map(float, self.serial.readline().decode("utf-8").split(","))

    def send_extend_command(self, duration_ms: int, speed: int):
        self._send_command(f"e,{duration_ms},f{speed}")

    def send_retract_command(self, duration_ms: int, speed: int):
        self._send_command(f"r,{duration_ms},f{speed}")

    def send_stop_command(self):
        self._send_command("s")
