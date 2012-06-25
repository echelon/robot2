robot2
======

After two years of nothing, I've decided to continue my work with elementary 
robotics. This time I utilize a vastly different, decoupled architecture that
relies on ZeroMQ for messaging the robot control daemon. This will make for a
much more robust robot platform that can benefit from quick prototyping. 

A central control daemon (written in C++) listens on port `TBD` for
instructions and relays them to the robot. 

Robot Hardware Support
----------------------

Currently I am working with the Robotics Connection Serializer, but I plan to
do work with Arduino in the near future. In the end I aim to support both
platforms. 


Components Included
-------------------

**Hardware Control**

* **daemon/** -- _(TODO)_ Contains the main driver daemon; written in C++.

**Basic Clients**

* **keycontrol/** -- _(TODO)_ Control the robot with the keyboard;
					 written in Python.
* **joycontrol/** -- _(TODO)_ Control the robot with a joypad;
					 written in Python.

**Advanced Clients**

I will be writing some basic AI to control the robot through sensor perception
and simple heuristics. These will likely be contained in different
repositories, but I will make note of them here. 

API Documentation
-----------------

_TODO_. Considering JSON protocol. 

Gamepad Support in Unix
-----------------------

**PS3**

For PS3 Sixaxis controller support, compile the 
[QtSixA](http://qtsixa.sourceforge.net/) library.
From there, Sixaxis controllers work out of the box over bluetooth.

* `sixad` starts the service
* `qtsixa` can configure individual device profiles, functions, features, etc.

Note: Remove xorg joystick input packages to prevent it from aliasing the
mouse.

**Xbox 360**

_TODO_

**Wii**

_TODO_

License
=======

Copyright (c) 2008-2010, 2012 Brandon Thomas

* http://possibilistic.org
* echelon@gmail.com 

This code is available under the MIT and GPL 2 licenses. 

