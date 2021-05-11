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
*    -i --interval	Specify the decimal value for polling in ms. The default is 1000ms.
*    -v --verbose	Print verbose messages
*    -l --logfile	Print output to logfile
*    -s --stop		Stop any running instances of platformstats
*    -h --help		Show this usuage.

 List of stats to print
*    -a --all		Print all supported stats.
*    -c --cpu-util	Print CPU Utilization.
*    -r --ram-util	Print RAM Utilization.
*    -s --swap-util	Print Swap Mem Utilization.
*    -p --power-util	Print Power Utilization.
*    -m --cma-util	Print CMA Mem Utilization.
*    -f --cpu-freq	Print CPU frequency.

## Compile test app
	cd app/
	make clean
	make
### Compile library
	cd src/
	make
### Generate Documentation
	doxygen docs/platformstats_doxygen_config
HTML docs are populated in tmp/html

PDF doc will be available at tmp/latex/refman.pdf
