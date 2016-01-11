#!/bin/sh

#clear out results and make new folder 
outdir="resultfolder2"
slash="/"
rm -r ${outdir}
mkdir ${outdir}

outfile="water"
png=".png"

range="234"
i="1"

while [ ${i} -le ${range} ]
do
        
	#modify input file to get new precomputed mesh 

	sed "s/water_mesh1/water_mesh${i}/" scene_shore2.txt > runner.txt

	echo $i
	#./a5 -input runner.txt -size 300 300 -output ${outdir}${slash}${outfile}${i}${png} -normals normal.png -bounces 2 > debug.txt
	./a5 -input runner.txt -jitter -filter -size 300 300 -output ${outdir}${slash}${outfile}${i}${png} -normals normal.png -bounces 3 >debug.txt

	let i=i+1

done


