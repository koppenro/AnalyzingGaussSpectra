#!/bin/bash

read -p "Name of the module (e.g. M4550) " modulename
read -p "Temperature (e.g. p16) " temperature

PWD=`pwd`
echo $PWD

for((zaehler=0; zaehler<16; zaehler++))
do
	#FitGaussDistribution is called for every single roc to avoid an error
	./FitGaussDistribution -m -temp $temperature -roc $zaehler -fm
done

./CalibrationLine -m -l $modulename:-:$temperature
./CurrentDependency -m -l $modulename:-:$temperature
