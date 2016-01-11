Build the program using the standard cmake method.

There are one or two bugs that seem to be in my program. In images 
with triangle meshes, the lighting of the surface seems to be off. 
This bug cannot be in the shading method itself because the shading 
of the spheres appears to be correct. 
(For your reference I put the shading method in Renderer because of circular reference issues) 

The 2nd bug appears in rendering image 9, where the feet of the characters 
seem to be cut off my the plane. The characters themselves are in the correct 
location, but the plane appears to be offset up slightly for some reason. 

Final Project Proposal: 
I am interested in simulating many falling dominos to spell things out, create images, or do other interesting things 
visually. For a first pass I would focus on just getting a broad system to work which would allow me to script 
the location of many dominos and a camera which would follow/pan out from the process. The dominos for a first pass
would just be "hinges" attached to the ground, to simplify the development.   
