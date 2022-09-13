# This is an example of .gdbinit, If you want a working one, please do not forget 
# to start the name with a dot (.).
#
# The Black Magic Probe has level shifter buffers on its frontend,
# this gdb script enables Black Magic Probe target power,
# this is needed if you are not providing reference voltage from your target.
# Do not run this script or enable target power if you have VREF connected.
# The BMP side TPWR is connected to the 3.3V regulator of the Black Magic Probe
# and it has no protections, so don't fry your target or Black Magic Probe by
# being hasty and not thinking about what you are doing. :D

# do not ask me if I am sure
set confirm off

# This is for Mac OS X
#tar ext /dev/cu.usbmodemSERIAL
# This is for Linux
target extended-remote /dev/serial/by-id/usb-Black_Sphere_Technologies_Black_Magic_Probe__xxxxxx_xx__vx.x.x-xxx-xxxxxxxx_xxxxxxxx-if00

# Print Black Magic Probe version
monitor version

# Enable sw_dp
monitor sw

# Attach
attach 1
