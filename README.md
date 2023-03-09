Description :
============
I, K.M. Arun Kumar alias Arunkumar Murugeswaran, just shared my works, which I worked as learning path and practiced Embedded C programming using LPC2138 (ARM7TDMI-S 32 bit RISC microcontroller) for Application for scalable 5 floor single elevator controller.

Features of 5 floor single elevator controller are:
====================================================
01: easy scalable to max floors, with little and easy code modification.
02: provision for emergency stop and overload protection.
03: proper sequence of floor detection.
04: timeout if proper event does not occur.
05: handle many different floor call request.
06:  Elevator control is based on two basic principles.
 A: Continue to travel in the current cabin movement direction (up or down) while there are still remaining requests in that same cabin movement direction.
 B: If there are no further requests in that direction, then stop and become idle, or change direction if there are requests in the opposite direction.
Elevator’s cabin floor call for each floor and only one hall floor call for each floor are considered. 
07: cabin door and floor door alignment, car door open and close sequence are considered.
08: cabin door opens while closing the door, when manual door open is triggered or user blocks the closing door.
09: cabin door which is open closes the door, if timeout on cabin door to remain open or manual door close is triggered.
10:   improper event detection and secure the elevator from improper event.

Elevator Control Algorithms
============================

Step 1: Initial when the elevator is powered on, make sure that car door is closed, and when start button is pressed, and find the current floor that elevator is at present and move to predefined default floor, then go to Step 2. 

Step 2: After elevator is at floor, make sure that elevator is not moving, if so then the car door is opened, then go step 3. 

Step 3: Scan each not already active floor hall calls switches and in car call switches. If in car call is in current floor, then no action is taken.  If in car call is not in current floor, then take decision, whether to move the elevator up or down. We use selective up algorithm for elevator car up movement and selective down algorithm for car down movement, to conserve power, then go to step 4. Eg, car is positioned at floor 2 and car is moving down, then hall call switch at floor 4, hall car switch at floor 0 and in car call switch for floor 2 are set, then due to current car movement is down, then move down and stop at floor 0, then move up and stops at 2,  and then floor 4. 

Step 4: After elevator is requested to move to up or down the floor, and after predefined max timeout to close the car door or car door close switch is pressed, then activate car door to close, make sure that no person entry or exit the car, then go to step 5. If a person makes an exit or entry, then car door is opened, go to step 3.

Step 5: Move the elevator to next floor that in car call or hall call is made, then go to step 2.  


Elevator FSM States
====================

Elevator controller is done by LPC2138.   Elevator car (or lift cabin) can be FSM states are as follows:


Idle:
------
At first, when elevator is powered on, it is in idle FSM state. If elevator is just powered on then go to Prepare to Start FSM state. If the previous FSM is Abnormal FSM state, then take no action.


Prepare to Start:
-----------------
Allow access to Limit switch car door close. Make sure that car door is closed by sensing limit switch for car door close and floor door and car door are aligned correctly. If it is car door not closed by sensing limit switch for car door close, and then go to triggerdoor close FSM state. If the car door is closed,  then Allow access to Car Start button  and go to Wait for Start FSM state.

Wait for Start:
---------------
Wait for Car Start button to be pressed. When Car Start button is pressed, then don’t allow access to start button, allow access to limit switches in all floors, allow access to emergency stop button and next stop floor to predefined default floor elevator is go to Decide car movement FSM state. 



Decide car movement:
--------------------
By sensing limit switch for each floor, find the floor does elevator car is at present. Get Compare the current floor that elevator is positioned with next stop floor, if current floor == next stop floor, and then go to trigger stop FSM state. If current floor < next stop floor, then go to TriggerMove up FSM state, else go to triggerMove down FSM state.


Trigger Move Up:
----------------
Car Pulley Up is signaled is set and at the same time Car pulley down signal is reset, moves the car up, and go to Prepare to move FSM state. 


Trigger Move Down:
------------------
Car Pulley Down is signaled is set and at the same time Car pulley up signal is reset, to moves the car down, and go to Prepare to move FSM state.


Prepare to move:
----------------
Hall call switch and in cabin call switch of the current car floor position are allowed to access and go to moving FSM state. 


Moving:
------
By sensing limit switch for each floor, find the floor does elevator car is at present. Get Compare the current floor that elevator is positioned with next stop floor, if current floor == next stop floor, and then go to trigger stop FSM state, else go to Moving FSM state. For each floor visited, then display floor and if previous FSM state is trigger move up, then in elevator movement status, display move up, else move down status. 


Trigger stop:
-------------
Reset the car pulley up and car pulley down signals, to stop the car, and due inertia elevator car does not stop immediately, sense motion a timer is set which counts 5 seconds then sets a signal after that. The need for this counter is first to ensure the car has come to stand still position since there will be car movement even after the motor stops for few seconds. This movement is taken into account and therefore the motor is signaled to stop once it passes by the limit switch which is fixed at a location few millimeters below the actual floor level. The time that takes the car to stop from the moment the motor is signaled to stop is enough to make the car reach the actual floor level, and then go to wait till car stop FSM state.

Wait till car stop:
------------------
After timeout, make sure that elevator car is stationary and floor door and elevator car door are correctly aligned, and then go to trigger door open FSM state.

Trigger door open:
------------------
Trigger door open signal to open elevator car as well as floor door, and at that same the door closing signal is reset to ensure that the two signals are not sent simultaneously causing serious design and operation errors. Timer for max time car door open signal is run and then go to Wait till Door opened FSM state.


Wait till Door opened:
----------------------
When timer is expired, and then send alarm that limit switch is not detected, go to Abnormal event FSM state. When limit switch for car door open is detected, and then go to prepare to user entry and exit FSM state.


Prepare to user entry and exit:
-------------------------------
Current car floor position’s corresponding is deleted from the list of yet to service floor calls and deactivate corresponding hall floor call switch and in cabin call switch and led to that floor’s hall call and in car call is reset and disabled, and then go to userentry or exit FSM.

User entry and exit:
--------------------
Person enters and exits the car. When car is overload, stop the timer and set an alarm with an overload message and then some person exits the car, so that car is not loaded. When noncurrent car position floor’s hall call switch or in car call switch is pressed and corresponding floor call is not present in list of yet to service floor calls, then add that floor call in the list of  yet to service floor calls and glow corresponding pressed hall call switch’s led or pressed in car call switch’s led. When there is at least on pending floor calls, then Timer is set (that timer defines how long the car and floor door will remain open before a signal to close the car and floor door is made), then go to prepare to close FSM state. If there is no pending floor calls, then go to user entry or exit FSM.

Prepare to door close:  
----------------------
When timer is expired for which the door should remain open start signaling (10 seconds after the door open signal was set high) OR if the door close button was pushed even before the 10 seconds specified are over, then go to Trigger Door Close FSM state. 


Trigger Door Close:
-------------------
Set the timer for car door close and Activate car door close signal for car door motor to close the door, at that same the door opening signal is reset to ensure that the two signals are not sent simultaneously causing serious design and operation errors and go to wait till door closed FSM state. 


Wait till door closed:
----------------------
If limit switch is not detected, even after timer had expired, then set a alarm with message that limit switch for door close is not detected and then go to abnormal event FSM state. If previous FSM state is user entry and exit FSM state and when the car door was closing and when a person motion is detected such as entry or exit or car positioned floor’s corresponding hall call switch is pressed at the time of car door closing, then reset the timer and then go to Trigger door open FSM state. When limit switch for car door close is sensed, then deactivate car door close signal to stop the car door motor, if previous FSM state is user entry and exit FSMstate, then go to compute next stop floor FSM state and if previous FSM state is Idle, then allow access to Car Start button, and then go to wait for startFSM state. 


Compute next stop floor:
-----------------------
Keep track for last visited floor, current car movement, yet to service floor calls and determine the car that is currently positioned. If the car is positioned at the third floor, and the fourth floor called for the car, then the car has to answer the request of the fourth and higher floors and continue moving up to reduce power consumption. If the car is positioned at the third floor, currently car is moving up and the fourth floor call is not made for the car and second floor call is made, then the car has to answer the request of the floor second or lower floors. Then go to decide car movement FSM state.

Emergency stop:
---------------
When emergency stop switch is pressed, for emergency case, a message is set as emergency stop and then goes to Abnormalevent FSM. 

Abnormal event:
---------------
Set the alarm and buzzer is set and if previous FSM state is Emergency Stop, then move to next floor and stop the car movement, and if previous FSM state is Emergency Stop or trigger door close, then trigger the car door to open and doors is opened. If previous FSM state is Emergency Stop or trigger door close or Trigger Door open, then car door open and car door close are reset and then go to idle FSM state.  Make sure that car is stationary, floor door and elevator car door are correctly aligned and door is opened. 


Project using LPC2138, included with design, development, simulated and tested, using a simulator, are as follows :
====================================================================================================================
1:
2: Implementation of function operation of scanf, sscanf, printf, sprintf, gets, puts, putchar and getchar using Embedded C in LPC2138 microcontroller.
3: Single user account authentication with authentication timeout.
 
CAUTION:
========
Schematics and simulation is done by Proteus CAD. NOT EXPERIMENTED IN REAL TIME ENVIRONMENT.

Purpose :
=========
In all my respective repositories, I just shared my works that I worked as the learning path and practiced, with designed, developed, implemented, simulated and tested, including some projects, assignments, documentations and all other related files and some programming that might not being implement, not being completed, lacks some features or have some bugs. Purpose of all my repositories, if used, can be used for LEARNING AND EDUCATIONAL PURPOSE ONLY. It can be used as the open source and freeware. Kindly read the LICENSE.txt for license, terms and conditions about the use of source codes, binaries, documentation and all other files, located in all my repositories. 
       
My Thanks and Tribute :
========================

I thank to my family, Friends, Teachers, People behind the toolchains and references that I used, all those who directly or indirectly supported me and/or helped me and/or my family, Nature and God. My tribute to my family, Friends, Teachers, People behind the toolchains and references that I used, Nature, Jimmy Dog, God and all those, who directly or indirectly help and/or support me and/or my family.

Toolchains that I used for LPC2138 Application design and development are as follows :
======================================================================================
1: IDE for Application development of LPC2138                               - MDK-ARM Professional Keil uvision 5. 
2: PC tool for Programming Flash of LPC2138                                 - Flash Magic.
3: LCD                                                                      - JHD162A.
4: Desktop Computer Architecture and OS for development                     - Intel X64 & Windows 7 (32 bit).
5: CADD and Simulations                                                     - Proteus 8.0 Pro and/or Proteus 8.3 Pro SP2. 
6: Code editor                                                              - Notepad++.
7: Documentation                                                            - Microsoft Office 2007 (alternative LibreOffice) and Text Editor. 

Some reference that I refered for LPC2138 Application design and development, are as follows :
==============================================================================================
1: Schaum's Outline of Programming with C, 2nd Edition - Book authored by Byron Gottfried.
2: Understanding and Using C Pointers: Core Techniques for Memory Management - Book authored by Richard M. Reese. 
3: Embedded C - Book authored by Michael J. Pont.
4: Reusable Firmware Development: A Practical Approach to APIs, HALs and Drivers - Book authored by Jacob Beningo. 
5: https://www.embedded.com/building-reusable-device-drivers-for-microcontrollers/ - Building reusable device drivers for microcontrollers.
6: https://www.nxp.com/docs/en/data-sheet/LPC2131_32_34_36_38.pdf - LPC2131/32/34/36/38 - Single-chip 16/32-bit microcontrollers; 32/64/128/256/512 kB ISP/IAP flash with 10-bit ADC and DAC,Rev. 5.1 — 29 July 2011 Product data sheet. 
7: Hitachi HD44780U - LCD product data sheet.
8: http://www.electronicwings.com/arm7  - ARM 7 Inside - LPC2148.


Note :
======
Kindly read in the source codes, if mentioned, about the Program Description or Purpose, Known Bugs, Caution & Notes and Documentations. Some applications are also tested in a LPC2138 development board.

Repository Web Link :
=====================
https://github.com/k-m-arun-kumar-5

