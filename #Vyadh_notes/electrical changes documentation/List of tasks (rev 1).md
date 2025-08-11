test new ideas -> implement them on the rover

we need to test all the new ideas that we're using

1. differential UART
	1. research on and find out differential UART modules -or if there's a simpler way to do it
	2. buy them, connect two ESPs and run a simple communication script between them
	3. check signal quality over a long wire (maybe over power electronics) -compare UART and differential UART
2. DMA access -publishing and subscribing and a datahub using three microcontrollers
	1. communicate between two ESPs through an STM32F0 using DMA
	2. test the limits of the DMA channels. see how many you channels you can have in STM32F0 and F4 -check if you need to buy a higher end uC
	3. check and try to minimize latency and increase throughput
	4. once basic DMA comms is done, work on publishing and subscribing to data using this system -try with the maximum number of members possible
	5. test permanent and temporary DMA channels, see how well these work with mux switches, prioritized communication (one module gets more bandwidth/ faster updates)
	6. test fallback mechanisms if DMA fails or isnt enough
3. PCB testing:
	1. try out making a PCB with an embedded microcontroller once -to familiarize with the workflow
	2. test different kinds of slot connections for connecting two PCBs + mounting mechanisms and their strength
	3. make an adaptor for our motor drivers to bring all the power to one side
	4. find which PCB manufacturer is the cheapest and best
	5. test and research on high current traces
	6. add test points for diagnostics
4. BMS system (test it with a second battery)
	1. research on what's wrong with the way we're using our batteries rn, how it's affecting them, potential risks.
	2. measure current draw at each port when the rover is fully running
	3. find a way and measure temperature when the rover is fully running
	4. look into where to implement voltage and current protection -and how to implement it. look for battery management ICs, fuses
	5. add a logging system, not just single point measurements -and also a way to send this logging system back to ground control
	6. check on and document power sequencing -which modules should turn on first, power paths
5. system wide diagnostics
	1. logging data streams
	2. module failure detection
	3. visualization of data streams
	4. visualization of power paths

---

as we test these ideas, we need to implement them into the rover one by one

prototype -> code development -> defining test cases and verifying the prototype -> PCB design -> verifying the prototype against earlier test cases

now that we've tested all the new tech, we can refer to the `documentation for new electrical changes` pdf and the modules one by one according to spec

##### prototype, code and test:

1. pre-requisite:
	1. define a standard packet structure, ex: {src, dest, payload}
	2. throughout the making of each module, make sure of the standard test cases

2. traversal module:
	1. define a landing payload for this module (like cmd_vel = {vel, dir})
	2. receive the expected payload in a standard packet structure and process it into motor control
	3. simulate a launch payload from this module (encoder value)

3. LoRa receiver module:
	1. connect up a lora module to an esp. receive data from controller.
	2. process this received data into traversal control commands (cmd_vel)
	3. launch this payload to the traversal module and get it working
	4. simulate landing payloads, for feedback to the controller

4. laptop receiver module
	1. exact same as the LoRa receiver module, except for the first step
	2. instead of through LoRa receiver you receive the data from the laptop
	3. the data will be pre processed into traversal control commands by the laptop
	4. receive landing payloads like encoder data and send it back to the laptop

5. main control board
	1. with the three modules done, make a main control board which connects these modules together
	2. set up permanent/high priority channels for these connections.
	3. simulate a low priority module alongside these

6. PDB-BMS
	1. 

7. telemetry module
	...

8. rob arm module
	...

9. science kit module
	...

##### PCB and verify (incomplete)
1. pre requisite
	1. discuss with other domains and set limits on electrical modules of the rover
2. main control board
	1. make a PCB for the finished prototype including the connector design chosen
3. traversal, lora receiver and laptop receiver
	1. make a PCB for the finished prototype and try slotting it into the main control board and get it working
	2. verify with all the previous test cases that these work perfectly
4. PDB-BMS
	1. design a new PDB with all the BMS tech prototyped earlier
	2. test it with all 6 motors being controlled

---

others:
1. traction control
	1. research how inefficient the current rover is due to loss of traction
	2. look into how cars do traction control, and try to implement it on the rover
2. sensor fusion for system wide diagnostics