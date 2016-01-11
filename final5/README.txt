Build the program using the standard cmake method.

The four images in the assignment appear to be rendered, jittered and filtered correctly. I don't know of any bugs at 
this point. However there does seem to be a difference between the diamond the sample code produces and what my code 
produces. I have no idea where this problem could be coming from, and to be perfectly honest, my diamond looks better 
than what the sample code produces, so it doesn't bother me very much.    

Final Project Proposal:
Idea I
I am interested in simulating many falling dominos to spell things out, create images, or do other interesting things visually. 
For a first pass I would focus on just getting a broad system to work which would allow me to script the location of many dominos 
and a camera which would follow/pan out from the process. 
The dominos for a first pass would just be "hinges" attached to the ground, to simplify the development.

Idea II
After thinking more about the dominos, it started to seem to me that there might be a lot of subtle issues which will be hard to get
 right, and could doom the project. So I've come up with a second idea that hopefully will be more suitable (let me know which you 
prefer and with what qualifications...)

A lot of video games include pretty simulations of water. It would be interesting to
use the ray caster we made plus a simple simulation of a 2D scalar wave
equation to create a simulation of water. I was thinking that the project could
progress in the following way:

1) generate a simple animation using sin(kx-wt) etc. to first create a mesh, and then create a rendering. 
It seems that this can be done using the mesh generating code
from some of the original assignments and the ray caster we just made. (Question:
Would it be easier/better to use opengl as the ray caster instead of our code?
Maybe the debugging will take too long with our ray caster).

2) create/use a preexisting solver to solve the 2D scalar wave equation, with
adjustable perturbations/ existing boundary conditions. If it seemed too difficult
to solve the wave equation from scratch, it is very easy to find many c++ codes 
online http://www.uio.no/studier/emner/matnat/ifi/INF2340/v05/foiler/sim04.pdf.

3) If using a canned solver code+our old code seems too easy, maybe it would
be best for me to do the project in opengl and allow active user perturbation of
the water, and maybe even include some objects to move around. For example the 
user could create a perturbation by just clicking on the water. The user could 
also drag a box around which could act as a hard wall boundary condition.

4) Since c++ is sort of a pain sometimes, it would probably be better to debug all 
of the "physics" aspects in matlab. Once the physics is okay, we can translate everything to opengl etc. 
