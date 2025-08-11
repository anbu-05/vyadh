## problems with the current electrical system

![[current topology.png]]

#### architectural problems
- same board handles traversal and LoRa reception
- change in rob arm or science kit module requires change in traversal board for no reason
- IMU and other telemetry data are completely disconnected from the rest of the electrical systems (traction control would need IMU data)
- there's no centralized data hub, the onboard computer has to be manually connected to every board it needs data from

- long UART lines running across the rover over power components, UART is not noise resistant and we've already seen this being a problem when we try to transmit a lot of data over UART (between traversal and rob arm, we had to move a lot of the processing to rob arm esp which was already overloaded with processing. we had to look for other methods like dual core processing on the rob arm board)
- switching between autonomous mode and manual mode requires reprogramming the traversal board -which has caused confusion for software guys because they have to keep asking electrical for advice when programming their microcontroller code
- autonomous and GCS need to learn and write microcontroller code when all they need is data
#### power system issues
- the PDB lacks any sort of BMS -voltage, current, temperature monitoring
- buck converters are hard soldered onto the PDB
	- two bucks are currently shorted and replacing needs time consuming desoldering
- ESPs are powered through USB cables from the onboard computer
	- extra clutter inside the rover
	- difficult to manage with the number of ESPs growing inside the rover
	- micro B is fragile and we've had many cases of micro B ports ripping off of the esp dev boards
	- with a growing number of ESPs (there's already 4-5 ESPs) the laptop's (onboard computer) battery will drain quicker
#### excessive and disorganized wiring 
- power wires:
	- small wire from PDB to motor driver x 6
	- long wire from each motor driver to each motor x 6
	- micro B cable for each ESP connected to a USB hub from the laptop -at least 4-5 ESPs
	- multiple PDB output wires for the robotic arm PDBs
- data wires:
	- 6 x 3 pin JSTs from traversal board to motor drivers
	- 6 encoder JSTs from motors to traversal ESP
	- 2 UART connections: traversal to robotic arm and traversal to science kit
	- more wire if BMS and telemetry are integrates
- more wires = more connectors = more points of failure
- these many connecting wires defeats the entire purpose of custom PCBs


## changes

there are 4 types of changes here
1. modularization
2. standard power format
3. standard data format
4. connection topology

#### modularization
- new PDB consisting of:
	- slotable buck converters and motor drivers
	- battery management system to monitor voltage, current and temperature of the batteries and buck convertors
- separate modules for:
	- LoRa transceiver
	- dedicated board for communication with the onboard computer -this board is analogous to the LoRa transceiver board, it will communicate with the rest of the system the same way the transceiver board will
	- traversal board
	- combined IMU, GPS and sensors board
	- robotic arm and science kit boards
	- a dedicated board for anything else we want to add (a servo control board for cameras maybe, if we are doing it)
#### standard power format
- for power electronics, we will make all the components slotable and modular using direct XT connections (instead of making short male-female XT wires)
- all the compute boards could also be slotted onto a motherboard -which would act as the data interchange hub between the different modules. (if we use STMs here, we could make use of DMA, but we could even just use regular transistors or muxes)
	- every compute board would have a standard connector to connect with each other
	- we could 3D print a mounting with a small adapter PCB for each motor driver and buck convertor that brings all the connectors to one side (even the JSTs)
#### standard data format
- all the control boards would use the same data format. example:
	- Traversal board only needs to receive `{velocity, angle}` and return `{wheel speed}`. it doesn’t care if data comes from LoRa, onboard computer, or any other module — that’s abstracted by the motherboard or bus.
	- now we can develop all three modules seperately. 
		- we can change the transceiver from LoRa to zigbee, in the end we just have to package the data the same way. 
		- we could implement traction control on the traversal board without changing anything on the transceiver or onboard computer comms board.
- there are several such examples. 
	- GCS could decide to integrate BMS data into their GUI without touching the BMS board
	- robotic arm might want to use data from both the controller and the onboard computer at the same time. 
	- the controller could include a small panel for telemetry data. 
	- we might implement traction control which would need IMU data (IMU data is directly connected to the onboard computer now, there's no connection with the traversal board)
	- we could have a second simpler rover that's has the same dimensions as the current one that autonomous could develop using it -they'd just have to unplug their communication module, use it on the dummy rover and plug it right back into the main rover and it would work the same way regardless of what changes we make in the main rover
#### connection topology
all the different modules can be connected in two different ways we've thought of so far
- star topology:
	- ![[star topology.png]]
	- all the different modules would connect to a motherboard with a microcontroller dedicated to handle the data between these boards
	- using Direct Memory Access would mitigate the issue of latency increase due to a middleman
- bus topology:
	- ![[bus topology.png]]
	- there is no dedicated data control microcontroller, all the compute boards are connected to a bus and take turns communicating, have a priority order, etc.
- either of these topologies, we could use a differential signal (CAN or even USB) instead of UART for better signal integrity across the rover
	- both of these have their pros and cons -we could even use a mix of both these topologies
## detailed changes
I described 4 types of changes, now we will define subsystems and apply these 4 types of changes to each subsystem and their interlinks

working control boards
1. main 
2. traversal 
3. robotic arm 
4. science kit
5. on board computer
6. LoRa receiver
7. power distribution
8. telemetry

and the interfaces between them, namely: power and data

###### power and data
- we will use a simple UART to CAN controller like TJA1050 for data transfer connections. 
	- for permanent boards we can made dev boards (for traversal and rob arm example) and have male headers on the main controller boards (this will connect like shields)
	- for distant boards, we can have a simple JST connector (like the telemetry, science kit, PDB)
- for any sort of power slotting we will use XT males and females that are soldered onto the PCBs
- for other power connections, we can use Anderson power pole connectors 

###### main control board
- all the other control boards connect to this board. this acts as a data hub, where each board can publish data and other boards can subscribe to it -inspired from ROS
- this way we can add any other new type of board and it would be very easy to integrate it with the rest of the system
- ![[main control board connections.drawio.png]]
	- the main control board will have 5 communication controllers. traversal, robotic arm and science kit will be always connected to it
	- the lora reciever and laptop reciever will be connected so that only one is used at a time by the main control board
	- the remaining control boards, however many will be connected to a mux and the main control board connected to this mux.
	- this way we can keep the main control board expandable to new modules and prototyping
- the main controller board would put DMA to heavy use. we will use an STM32 for the main control board, where writing DMA code is pretty simple. depending on what the subscribers ask for we can set DMA from one board to the other
	- there would be a constant DMA between the traversal/rob arm and the reciever mux
	- the remaining DMA will be initiated depending on subscription status
###### traversal control board
- shield like module containing uC and CAN controller
- there are 6 wheels: 4 pins -pwm, direction, encoder A, encoder B from each wheel. 2 pins for the TJA1050 convertor and 2 for vcc and gnd; the traversal board will connect with 28 pins to the main controller board
- all the motor drivers connect to the main control board for easier access
- this way we can implement individual wheel control if we plan on it
###### LoRa Receiver
- it's a simple shield like module containing the Lora module, a uC to publish and subscribe to data and a CAN controller
- the 4 pins would be VCC GND CANH CANL
- the antenna will be connected to this module, maybe a 3D printed holder for it.
- communicates with the controller
###### PDB-BMS
- the PDB will have slot-able DC DC buck convertors (that bolt onto the PDB)
	- one 300w 12v buck convertor for rob arm
	- one smaller 5v buck convertor to power all the control boards
- there will be 6 XT60s for the motor drivers to slot onto, and 6 XTs to redirect the motor drivers signals to another set of 6 XTs on the edge of the board to connect the motor to
- one 12v and one 24v XT90s for robotic arm to use
- the motor drivers will have a small custom pcb they will bolt onto so that we can bring all ports to the same side. or we'll use different motor drivers
- ![[Pasted image 20250701195957.png]]
- the PDB will have a BMS that will monitor battery voltage, power use on each port, battery temperature, motor driver temperature etc -and will publish it to the main controller
- short circuit and over current protection on each port
- (need to research more about MD30C)
###### onboard computer control board
- this will look like a thumbstick that you can plug into the laptop, and the other end will connect to the main board using a 4 pin jst for CAN
- the pcb itself will have a uC to talk to the main board and a CAN controller that's it
###### telemetry
- this will be a remote module or a shield depending on where GCS wants to put the sensors and GPS
- it will consist of all the modules GCS wants, and an ESP and CAN convertor
###### robotic arm
- this module depends on how rob arm want to connect with the main data hub
###### science kit
- this module depends on how the science kit will connect to the main data hub
###### rover control commands
- 
