NAME:Tife Akinola
STUDENT #: 7773942
COURSE: Graphics 2
ASSIGNMENT: 1

The platform used to test my program was a Windows operating system with a Graphics card of Intel(R) UHD Grpahics 620
The IDE used to write the program was Microsoft visual studio 2017. 
The four main files are the vshader7, fshader7, main.cpp, and a1.cpp. 

For the Boing! program, I created a shpere that bounces up and down on a grid. When the sphere touces the ground of 
the grid, it releases a particle system of traingles that disperse in different directions. 

For my additional feature, I made a particle system for everytime the ball touches the ground. The code for the particle 
system could be found in the particle.cpp and particle.h files. 

There are 3 views for the program. Pressing the spacebar 3 times will move you through all three views. 

The quad and the Grid make up the ground and the wall for the sphere. There are imaginary walls on both directions of the 
x-axis for the sphere to bounce off. 

The vertex shader required to know the position of the geometry and how to get to them.
To help it keep track of what was to be drawn on the screen, the vao index was passed to it. Also, to help it 
know what colour belonged to what geometry, I passed to it the wall index. this was used to know if it was drawning a gid or a quad.

The fragment shader required the colour of the geometry. 

To open up the code, click on the boing.sln file. Then the program can be open and ran.