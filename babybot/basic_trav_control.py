import serial
import time
import sys
import threading
from pynput import keyboard

class ESP32Controller:
    def __init__(self, port='COM3', baudrate=9600):
        """
        Initialize the ESP32 controller
        
        Args:
            port: Serial port (e.g., 'COM3' on Windows, '/dev/ttyUSB0' on Linux)
            baudrate: Communication speed (should match Arduino code)
        """
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.running = False
        
    def connect(self):
        """Connect to the ESP32"""
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
            time.sleep(2)  # Wait for connection to establish
            print(f"Connected to ESP32 on {self.port}")
            return True
        except serial.SerialException as e:
            print(f"Error connecting to {self.port}: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from ESP32"""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("Disconnected from ESP32")
    
    def send_command(self, command):
        """Send a command to ESP32"""
        if self.ser and self.ser.is_open:
            self.ser.write(command.encode())
            print(f"Sent: {command}")
        else:
            print("Not connected to ESP32")
    
    def keyboard_control(self):
        """Control using keyboard input (blocking)"""
        print("\nKeyboard Control Mode:")
        print("W - Forward")
        print("S - Backward") 
        print("A - Turn Left")
        print("D - Turn Right")
        print("Space - Stop")
        print("Q - Quit")
        print("Enter commands:")
        
        while True:
            try:
                key = input().lower().strip()
                
                if key == 'q':
                    break
                elif key in ['w', 's', 'a', 'd', ' ']:
                    self.send_command(key)
                else:
                    print("Invalid key. Use W/A/S/D/Space/Q")
                    
            except KeyboardInterrupt:
                break
    
    def realtime_keyboard_control(self):
        """Real-time keyboard control using pynput"""
        print("\nReal-time Keyboard Control Mode:")
        print("W - Forward")
        print("S - Backward")
        print("A - Turn Left") 
        print("D - Turn Right")
        print("Space - Stop")
        print("ESC - Quit")
        
        def on_press(key):
            try:
                if key.char in ['w', 's', 'a', 'd']:
                    self.send_command(key.char)
            except AttributeError:
                if key == keyboard.Key.space:
                    self.send_command(' ')
                elif key == keyboard.Key.esc:
                    return False
        
        with keyboard.Listener(on_press=on_press) as listener:
            listener.join()

def find_serial_ports():
    """Find available serial ports"""
    import serial.tools.list_ports
    ports = serial.tools.list_ports.comports()
    available_ports = []
    
    print("Available serial ports:")
    for port, desc, hwid in sorted(ports):
        print(f"  {port}: {desc}")
        available_ports.append(port)
    
    return available_ports

def main():
    print("ESP32 Robot Controller")
    print("=" * 30)
    
    # Find available ports
    ports = find_serial_ports()
    
    if not ports:
        print("No serial ports found!")
        return
    
    # Select port
    if len(ports) == 1:
        selected_port = ports[0]
        print(f"Using port: {selected_port}")
    else:
        print("\nSelect a port:")
        for i, port in enumerate(ports):
            print(f"{i + 1}. {port}")
        
        try:
            choice = int(input("Enter port number: ")) - 1
            selected_port = ports[choice]
        except (ValueError, IndexError):
            print("Invalid selection. Using first port.")
            selected_port = ports[0]
    
    # Create controller and connect
    controller = ESP32Controller(port=selected_port)
    
    if not controller.connect():
        return
    
    try:
        # Choose control mode
        print("\nSelect control mode:")
        print("1. Keyboard input (press Enter after each command)")
        print("2. Real-time keyboard control (requires pynput)")
        
        choice = input("Enter choice (1 or 2): ").strip()
        
        if choice == '2':
            try:
                controller.realtime_keyboard_control()
            except ImportError:
                print("pynput not installed. Install with: pip install pynput")
                print("Falling back to keyboard input mode...")
                controller.keyboard_control()
        else:
            controller.keyboard_control()
            
    except KeyboardInterrupt:
        print("\nExiting...")
    finally:
        # Send stop command before disconnecting
        controller.send_command(' ')
        time.sleep(0.1)
        controller.disconnect()

if __name__ == "__main__":
    main()
