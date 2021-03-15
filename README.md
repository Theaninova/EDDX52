# EDDX52
Direct Output Extension for EDDiscovery

For a quick introduction, please refer to the wiki secion.

IF you are interested in video updates, also consider subscribing to my YouTube channel. A playlist of video demos of the plugin can be found here:
https://www.youtube.com/watch?v=X7Aa_dmgoHg&list=PLx_w0I1Dt0F18xxf1sGLsxxo1jC_-v9dm

This is a plugin for EDDiscovery. It is written in C++ and relies on the DirectOutput.dll by Saitek. Purpose of the plugin is giving you the ability to extend the functionality of EDD with your X52 Pro Joystick through the Action language.

Current features include:

Setting the color of each LED (RED/GREEN/YELLOW/OFF)
Setting the color of each LED component (excluding FIRE and THROTTLE) to ON/OFF
Setting a blinking pattern individually on each LED component consisting of repeats, repeat Offset, flash duration, flash Offset
Syncing all flash patterns
Turning all LEDs off at once

Adding entries to the MFD
Adding callbacks to the MFD entries
Getting total amount of lines on the MFD
Jumping to a line on the MFD
Deleting all entries on the MFD

MFD is scrollable with right scrolling wheel
MFD entries are selectable with right scrolling wheel click

Functionality for Saitek FiP (Flight Instrument Panel) is *possible*, including recieving callback from buttons, setting button illumination and possibly setting image on FiP
