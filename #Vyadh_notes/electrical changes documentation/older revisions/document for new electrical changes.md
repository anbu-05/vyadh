ill divide this document into two parts: the idea itself and how we're gonna execute it

## Idea
### the existing mess
the current rover's electrical system is a mess
![[current topology.png]]
- the board contains both receiving and traversal codes -which makes LoRa duplex unnecessarily complicated
- if you wanna run the rover in autonomous mode, new code has to be uploaded onto the "main" board
- all the microcontrollers in this system are powered by USB cables connected to laptops
	- extra wires making electrical management and organization harder
	- the whole power bank issue (although it could've been easily mitigated)
- all of the different boards are in different parts of the rover -and you need to connect them over a long distance and over power electronics
	- all the microcontrollers communicate over UART -which isnt really meant for more than a few centimeters and we saw this very well in our rover, we had to offload a lot of the processing to the robotic arm ESP and keep data transfer as little as possible
	- now with new designs and new routing, we need a differential protocol for signal integrity close to power electronics and high power antenna 
	- and hence CAN -it's already very widely used in cars, so we can rely on it easily
- the current topology also makes it harder to develop parallelly
	- a change in how the robotic arm is controlled using the controller needs a code change in the traversal board
	- both electrical and autonomous have to work on the same control board for moving the rover
	- the telemetry is completely disconnected from the rest of the rover (IMU could be used for working on traction control if we decide to develop that)

###### the ideal version of this mess
![[star topology.png]]
most of these problems can be cleared immediately by:
- separating traversal and reception and having a main control board both connect to
- powering these individual control boards using the PDB instead of micro B ports
- using CAN instead of UART between boards
- making all the data available in the main control board so that the onboard computer can grab it easily

### bus topology
![[bus topology.png]]
the ideal version, the immediate solution would simply add an extra uC that manages every other uC.

it adds unnecessary steps for everyone's development
- why would GCS go through a middleman uC for no reason, when they can connect their laptop directly to the telemetry uC
- same goes for autonomous -why go through a middleman when they can control the traversal board directly
the reason is it creates a mess electrically, it gets harder to organize when all electrical systems aren't connected to each other

a better solution that would reduce the electrical mess and also provide direct communication between *any* two modules is using a bus topology

a bus would provide a way for all the modules to communicate with each other while reducing the number of wires
 - lets say GCS decide to add battery management metrics to their GUI
	 - with the existing mess, GCS would have to connect the BMS uC to their laptop -another extra USB write
 - robotic arm could decide to use both onboard computer control and manual controller control at the same time (hypothetical scenario)
	 - with the existing system they'd have to connect to the traversal board using UART, they'd also have to connect to the onboard computer using
- the onboard computer can receive data from all the other modules from a single point.
- this would reduce the number of uC software needs to deal with to 1, and they wont need to write uC code for any other parts of the rover
