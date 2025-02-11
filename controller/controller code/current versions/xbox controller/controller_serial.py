import pygame
import sys
import serial

# Initialize Pygame
pygame.init()

# Set up display
screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption('Gamepad Interface')

# Colors
WHITE = (255, 255, 255)
GRAY = (18, 18, 18)
RED = (255, 0, 0)
BLUE = (0, 0, 255)

# Initialize the joysticks
pygame.joystick.init()

# Check if there is at least one joystick
if pygame.joystick.get_count() < 1:
    print("Please connect at least one joystick!")
    pygame.quit()
    sys.exit()

# Select the first joystick
joystick = pygame.joystick.Joystick(0)
joystick.init()

print(f"Joystick name: {joystick.get_name()}")
print(f"Number of axes: {joystick.get_numaxes()}")
print(f"Number of buttons: {joystick.get_numbuttons()}")

# Initialize the serial port
try:
    ser = serial.Serial(port='COM12', baudrate=115200, timeout=1)  # Adjust port and baudrate
    print("Serial port connected")
except serial.SerialException:
    print("Failed to connect to the serial port!")
    pygame.quit()
    sys.exit()

# Main loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Clear the screen
    screen.fill(GRAY)

    # Get joystick axes values
    left_stick_x = joystick.get_axis(0)
    left_stick_y = joystick.get_axis(1)
    right_stick_x = joystick.get_axis(2)
    right_stick_y = joystick.get_axis(3)

    # Get trigger values
    raw_left_trigger = joystick.get_axis(4)
    raw_right_trigger = joystick.get_axis(5)

    # Normalize trigger values to [0, 1]
    normalized_left_trigger = max(0, (raw_left_trigger + 1) / 2)
    normalized_right_trigger = max(0, (raw_right_trigger + 1) / 2)


    # Get button states
    num_buttons = joystick.get_numbuttons()
    buttons = [joystick.get_button(i) for i in range(num_buttons)]

    # Get D-pad state
    dpad_state = joystick.get_hat(0)  # Get the D-pad state as (x, y)

    # Format data for serial transmission
    # data_to_send = f"LX:{left_stick_x:.2f},LY:{left_stick_y:.2f}," \
    #             f"RX:{right_stick_x:.2f},RY:{right_stick_y:.2f}," \
    #             f"LT:{normalized_left_trigger:.2f},RT:{normalized_right_trigger:.2f}," \
    #             f"DPAD:{dpad_state[0]},{dpad_state[1]}," \
    #             f"BTN:{','.join(map(str, buttons))}\n"

    data_to_send = {left_stick_x, left_stick_y,
                    right_stick_x, right_stick_y,
                    normalized_left_trigger, normalized_right_trigger,
                    dpad_state[0], dpad_state[1],
                    num_buttons[0], num_buttons[1], num_buttons[2], num_buttons[3], num_buttons[4], num_buttons[5]}

    # Send the data over the serial port
    try:
        ser.write(data_to_send)
    except serial.SerialException:
        print("Error writing to the serial port!")

    # Draw D-pad on screen
    dpad_center_x, dpad_center_y = 400, 300  # Center of the D-pad
    dpad_size = 20  # Size of the circles representing the D-pad directions

    # Draw D-pad background
    pygame.draw.circle(screen, BLUE, (dpad_center_x, dpad_center_y), 60, 2)

    # Draw D-pad buttons
    if dpad_state[0] == -1:  # Left
        pygame.draw.circle(screen, RED, (dpad_center_x - 40, dpad_center_y), dpad_size)
    else:
        pygame.draw.circle(screen, WHITE, (dpad_center_x - 40, dpad_center_y), dpad_size)
        pygame.draw.circle(screen, BLUE, (dpad_center_x - 40, dpad_center_y), dpad_size, 2)

    if dpad_state[0] == 1:  # Right
        pygame.draw.circle(screen, RED, (dpad_center_x + 40, dpad_center_y), dpad_size)
    else:
        pygame.draw.circle(screen, WHITE, (dpad_center_x + 40, dpad_center_y), dpad_size)
        pygame.draw.circle(screen, BLUE, (dpad_center_x + 40, dpad_center_y), dpad_size, 2)

    if dpad_state[1] == 1:  # Up
        pygame.draw.circle(screen, RED, (dpad_center_x, dpad_center_y - 40), dpad_size)
    else:
        pygame.draw.circle(screen, WHITE, (dpad_center_x, dpad_center_y - 40), dpad_size)
        pygame.draw.circle(screen, BLUE, (dpad_center_x, dpad_center_y - 40), dpad_size, 2)

    if dpad_state[1] == -1:  # Down
        pygame.draw.circle(screen, RED, (dpad_center_x, dpad_center_y + 40), dpad_size)
    else:
        pygame.draw.circle(screen, WHITE, (dpad_center_x, dpad_center_y + 40), dpad_size)
        pygame.draw.circle(screen, BLUE, (dpad_center_x, dpad_center_y + 40), dpad_size, 2)



    scaled_left_stick_x = int(((left_stick_x + 1) / 2) * 1023)
    scaled_left_stick_y = int(((left_stick_y + 1) / 2) * 1023)
    scaled_right_stick_x = int(((right_stick_x + 1) / 2) * 1023)
    scaled_right_stick_y = int(((right_stick_y + 1) / 2) * 1023)

    scaled_left_trigger = int(((raw_left_trigger + 1) / 2) * 1023)
    scaled_right_trigger = int(((raw_right_trigger + 1) / 2) * 1023)


    # print(f"L X: {scaled_left_stick_x}, L Y: {scaled_left_stick_y}")
    # print(f"R X: {scaled_right_stick_x}, R Y: {scaled_right_stick_y}")
    # print(f"L T: {scaled_left_trigger}, R T: {scaled_right_trigger}")

    # Get button states

    # Draw left joystick
    pygame.draw.rect(screen, WHITE, (150, 250, 100, 100))
    pygame.draw.rect(screen, BLUE, (150, 250, 100, 100), 2)
    pygame.draw.circle(screen, RED, (150 + int(left_stick_x * 50) + 50, 250 + int(left_stick_y * 50) + 50), 10)

    # Draw right joystick
    pygame.draw.rect(screen, WHITE, (550, 250, 100, 100))
    pygame.draw.rect(screen, BLUE, (550, 250, 100, 100), 2)
    pygame.draw.circle(screen, RED, (550 + int(right_stick_x * 50) + 50, 250 + int(right_stick_y * 50) + 50), 10)

    # Draw triggers
    pygame.draw.rect(screen, WHITE, (700, 100, 50, 200))
    pygame.draw.rect(screen, BLUE, (700, 100, 50, 200), 2)
    pygame.draw.rect(screen, RED, (700, 300 - int(normalized_right_trigger * 200), 50, int(normalized_right_trigger * 200)))

    pygame.draw.rect(screen, WHITE, (50, 100, 50, 200))
    pygame.draw.rect(screen, BLUE, (50, 100, 50, 200), 2)
    pygame.draw.rect(screen, RED, (50, 300 - int(normalized_left_trigger * 200), 50, int(normalized_left_trigger * 200)))

    # Draw buttons
    if num_buttons > 0 and buttons[0]:  # A button
        pygame.draw.circle(screen, RED, (400, 500), 20)
    else:
        pygame.draw.circle(screen, WHITE, (400, 500), 20)
        pygame.draw.circle(screen, BLUE, (400, 500), 20, 2)
        
    if num_buttons > 1 and buttons[1]:  # B button
        pygame.draw.circle(screen, RED, (450, 450), 20)
    else:
        pygame.draw.circle(screen, WHITE, (450, 450), 20)
        pygame.draw.circle(screen, BLUE, (450, 450), 20, 2)

    if num_buttons > 2 and buttons[2]:  # X button
        pygame.draw.circle(screen, RED, (350, 450), 20)
    else:
        pygame.draw.circle(screen, WHITE, (350, 450), 20)
        pygame.draw.circle(screen, BLUE, (350, 450), 20, 2)
        
    if num_buttons > 3 and buttons[3]:  # Y button
        pygame.draw.circle(screen, RED, (400, 400), 20)
    else:
        pygame.draw.circle(screen, WHITE, (400, 400), 20)
        pygame.draw.circle(screen, BLUE, (400, 400), 20, 2)

    # Draw shoulder buttons
    if num_buttons > 4 and buttons[4]:  # LB button
        pygame.draw.rect(screen, RED, (100, 100, 50, 30))
    else:
        pygame.draw.rect(screen, WHITE, (100, 100, 50, 30))
        pygame.draw.rect(screen, BLUE, (100, 100, 50, 30), 2)
        
    if num_buttons > 5 and buttons[5]:  # RB button
        pygame.draw.rect(screen, RED, (650, 100, 50, 30))
    else:
        pygame.draw.rect(screen, WHITE, (650, 100, 50, 30))
        pygame.draw.rect(screen, BLUE, (650, 100, 50, 30), 2)

    #joystick buttons
    if num_buttons > 8 and buttons[8]:  # left joystick click
        pygame.draw.rect(screen, RED, (100, 130, 50, 30))
    else:
        pygame.draw.rect(screen, WHITE, (100, 130, 50, 30))
        pygame.draw.rect(screen, BLUE, (100, 130, 50, 30), 2)

    if num_buttons > 9 and buttons[9]:  #right joystick click
        pygame.draw.rect(screen, RED, (650, 130, 50, 30))
    else:
        pygame.draw.rect(screen, WHITE, (650, 130, 50, 30))
        pygame.draw.rect(screen, BLUE, (650, 130, 50, 30), 2)

    # Update display
    pygame.display.flip()

# Quit Pygame
ser.cose()
pygame.quit()
