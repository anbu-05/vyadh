
# Team Vyadh

This repo consists of all my contributions to Team Vyadh.





## controller

### hardware and code

the controller went through many iterations. 

I had started off working with [NRF24L01](https://github.com/boneman420/vyadh/tree/master/controller/controller%20code/failed%20versions/older%20nrf%20versions) modules for wireless communication. but I could never get two nrf modules to work reliably. even when they did work the range was very bad.

![nrf controller](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/pictures/nrf%20controller%202.jpg)
![nrf reciever](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/pictures/nrf%20reciever%201.jpg)

there was a brief moment when i worked with [HC12](https://github.com/boneman420/vyadh/tree/master/controller/controller%20code/failed%20versions/%23hc12%2Besp_now) modules. getting these modules to work was fairly straight forward, but the range, again was not any better.

after both these modules, i decided to try out [LoRa](https://github.com/boneman420/vyadh/tree/master/controller/controller%20code/current%20versions/LoRa%20versions) (specifically the SX1278 module) -and it has been extremely reliable till now. i haven't faced any hiccups with operating the modules nor their range (i get ranges upto 1km with LOS). 

in the LoRa version, ive gone through implementing many features. the electrical subsystem itself consists of 4 main control PCBs as of now (it will be changed out soon) -the [controller](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/current%20versions/LoRa%20versions/controller_v2_duplex/controller_v2_duplex.ino) (transmitter), [reciever and traversal](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/current%20versions/LoRa%20versions/traversal_and_reciever_v3_uart_v3_duplex/traversal_and_reciever_v3_uart_v3_duplex.ino), the [robotic arm](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/current%20versions/LoRa%20versions/robotic_arm_v3_uart_v5/robotic_arm_v3_uart_v5.ino) and [the science kit](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/current%20versions/LoRa%20versions/science_kit_uart_v2/science_kit_uart_v2.ino) 

![(1)](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/pictures/WhatsApp%20Image%202025-04-27%20at%2017.26.51_78b1018d.jpg).

the controller transmits data, the reciever and traversal board recieves this data, controls the traversal and transmits the same data to the robotic arm and science kit PCBs. 

![(1)](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/pictures/WhatsApp%20Image%202025-04-27%20at%2017.22.41_903d0cd3.jpg)
![(2)](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/pictures/WhatsApp%20Image%202025-04-27%20at%2017.22.40_86971ecc.jpg)
![(3)](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/pictures/WhatsApp%20Image%202025-04-27%20at%2017.26.50_374f7ea2.jpg)


(i have also implemented duplex communication where the rover sends back telemetry data)

for the communication between these networks the initial plan was to use ESPNow. i tried it out but it creates too much lag between control and robotic arm. so we decided to just use simple serial (UART) connections for now. the future plan is to use a can BUS

![future topology](https://github.com/boneman420/vyadh/blob/master/controller/controller%20code/pictures/Blank%20diagram.png)

### controller PCB

i also wanted to design a decided PCB for the controller. after practicing making an xl4016 buck converter