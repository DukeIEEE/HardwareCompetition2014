#!/bin/bash
counter=0
echo Image Capture Running
cd 'image_capture'
while true
do
	read
	filename=$(printf 'image_%s.jpg' $counter)
	echo $filename
	raspistill -o $filename -t 50
	counter=$((counter+1))
done
