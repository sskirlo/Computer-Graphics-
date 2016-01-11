#!/bin/sh

#clear out results and make new folder 
outdir="resultfolder"
slash="/"
rm -r ${outdir}
mkdir ${outdir}

basestring="obj_file meshfolder/water_mesh"
obj=".obj"

outfile="water"
png=".png"

range="150"
i="1"

while [ ${i} -le ${range} ]
do
        
	#modify input file to get new precomputed mesh 
	#sed -i '"${basestring}"c\"${basestring}${i}${obj}"' scene_water1.txt
	#sed -i 'obj_file meshfolder/water_meshc\obj_file meshfolder/water_mesh' scene_water1.txt
	#sed -i -e 's/.* obj_file meshfolder/water_mesh .*/ obj_file meshfolder/water_mesh30 ./' scene_water1.txt
	#sed -i 's/^acl verizonfios.*/acl verizonfios src 202.1.2.3/'
	#sed 's/"${basestring}"/"${basestring}${i}${obj}"/' scene_water1.txt > runner.txt
	#sed 's/water_mesh30/water_mesh1/' scene_water1.txt > runner.txt

	sed "s/water_mesh30/water_mesh${i}/" scene_water1.txt > runner.txt

	echo $i
	#./a5 -input runner.txt -size 300 300 -output ${outdir}${slash}${outfile}${i}${png} -normals normal.png -bounces 2 > debug.txt
	./a5 -input runner.txt -jitter -filter -size 300 300 -output ${outdir}${slash}${outfile}${i}${png} -normals normal.png -bounces 2 >debug.txt

	let i=i+1

done


