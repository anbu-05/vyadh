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


