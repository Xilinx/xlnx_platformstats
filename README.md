# Platformstats

A library that provides APIs to print different platform statistics.

## Supported APIs

The library provides following list of APIs

| API     		| Description         				|
|-----------------	|------------------------------------		|
| CPU Utilization     	| List and print all active CPU Utilization 	|
| RAM Utilization 	| Print RAM memory Utilization 		      	|
| Swap Utilization 	| Print Swap memory Utilization		      	|
| Power Utilization 	| Print SOM Power Utilization 		      	|
| CMA Utilization 	| Print CMA memory Utilization 		      	|
| CPU Frequency 	| List and print all active CPU frequency      	|

## Usage
Usage: platformstats [options] [stats]

 Options
*    -v --verbose	Print verbose messages
*    -l --logfile	Print output to logfile
*    -h --help		Show this usuage.

 List of stats to print
*    -a --all		Print all supported stats.
*    -c --cpu-util	Print CPU Utilization.
*    -p --power-util	Print Power Utilization.
*    -m --mem-util	Print all Mem Utilization.
*    -f --cpu-freq	Print CPU frequency.

## Build Dependencies
- Install swig
	sudo apt install swig

## Compile and install using top-level Makefile
	sudo make install

## Compile each component separately
### Compile test app
	cd app/
	make clean
	make
### Compile library
	cd src/
	make
### Compile python bindings
	./scripts/build.sh
	cd python-bindings
	make
### Generate Documentation
	doxygen docs/platformstats_doxygen_config
HTML docs are populated in tmp/html

PDF doc will be available at tmp/latex/refman.pdf
