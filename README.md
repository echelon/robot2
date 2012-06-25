robot2
======

After two years of nothing, I've decided to continue my work with elementary 
robotics. This time I utilize a vastly different, decoupled architecture that
relies on ZeroMQ for messaging a central robot control daemon. This will make 
for a much more robust robot platform that can benefit from quick prototyping. 

The central control daemon (written in C++) listens on port `TBD` for
instructions and relays them to the robot. 

Robot Hardware Support
----------------------

Currently I am working with the Robotics Connection Serializer, but I plan to
do work with Arduino in the near future. In the end I aim to support both
platforms. 


Components Included
-------------------

**Hardware Control**

* **daemon/** -- Contains the main driver daemon; written in C++.
				 There remains much work to be done to make the drive control
				 more sophisticated, but it can handle basic movement and
				 control timeouts. 

**Basic Clients**

* **keycontrol.py** -- Control the robot with the keyboard;
					 written in Python/PyGame. 
* **joycontrol.py** -- Control the robot with a joypad;
					 written in Python/PyGame.

**Advanced Clients**

I will be writing some basic AI to control the robot through sensor perception
and simple heuristics. These will likely be contained in different
repositories, but I will make note of them here. 

API Documentation
-----------------

_TODO_. Considering JSON protocol. This will follow when a more robust
mechanism for controlling the robot speed differentials is implemented.
It will probably also allow specification of timeouts in the event commands
are being relayed over the network. 

Gamepad Support in Unix
-----------------------

**PS3 controller**

For PS3 Sixaxis controller support, compile the 
[QtSixA](http://qtsixa.sourceforge.net/) library.
From there, Sixaxis controllers work out of the box over bluetooth.

* `sixad` starts the service
* `qtsixa` can configure individual device profiles, functions, features, etc.

Note: Remove xorg joystick input packages to prevent it from aliasing the
mouse.

**Xbox 360 controller**

_TODO_

**Wiimote**

_TODO_

License
=======

Copyright (c) 2008-2010, 2012 Brandon Thomas

* http://possibilistic.org
* echelon@gmail.com 

This code is available under the MIT and GPL 2 licenses. 

