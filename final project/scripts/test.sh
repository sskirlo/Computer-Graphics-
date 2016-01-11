#!/bin/sh

#test water
#./a5 -input scene_water1.txt -size 300 300 -output water.png -normals normal.png -bounces 3
#./a5 -input scene_water1.txt -jitter -filter -size 300 300 -output water.png -normals normal.png -bounces 4
#./a5 -input scene_shore1.txt -size 300 300 -output water.png -normals normal.png -bounces 3
./a5 -input scene_shore2.txt -size 300 300 -output water.png -normals normal.png -bounces 3
#./sample_solution/a5soln_mac -input data/scene06_bunny_1k.txt -jitter -filter -size 300 300 -output s06.png -normals snormal.png -bounces 4

min=5
max=10

#compare spheres
#./inst/a4 -input data/scene10_sphere.txt -jitter -filter  -size 300 300 -output 10.png -shadows -bounces 4 -normals normal.png -depth ${min} ${max} depth.png
#./sample_solution/a5soln_mac -input data/scene10_sphere.txt -jitter -filter  -size 300 300 -output s10.png -shadows -bounces 4 -normals snormal.png -depth ${min} ${max} sdepth.png



#jitter
#./inst/a4 -input data/scene10_sphere.txt -jitter  -size 300 300 -output 10.png -shadows -bounces 4 -normals normal.png -depth ${min} ${max} depth.png
#./sample_solution/a5soln_mac -input data/scene10_sphere.txt -jitter  -size 300 300 -output s10.png -shadows -bounces 4 -normals snormal.png -depth ${min} ${max} sdepth.png

#compare boxes
#./inst/a4 -input data/scene10_sphere_mod2.txt   -size 300 300 -output 10.png -shadows -bounces 1 -normals normal.png -depth ${min} ${max} depth.png
#./sample_solution/a5soln_mac -input data/scene10_sphere_mod2.txt   -size 300 300 -output s10.png -shadows -bounces 1 -normals snormal.png -depth ${min} ${max} sdepth.png

#compare cube 
#./inst/a4 -input data/scene11_cube.txt     -size 300 300 -output 11.png -shadows -bounces 4
#./sample_solution/a5soln_mac -input data/scene11_cube.txt     -size 300 300 -output s11.png -shadows -bounces 4
