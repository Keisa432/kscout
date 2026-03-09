# Kscout

This is a similar tool to FMDataLab(https://www.fmdatalab.com/) except it is completely implemented in C. 

Work in progress

## Features

Create .rtf files using the FMDataLab Views. These rtf files can be parsed by the kscout tool. It then generates
role ratings based on the weights in the .krs files for all players and exports it to a file. Currently JSON and
CSV formats are supported.


## TODO

* Missing player roles
* Limit applied player roles
* Make it possible to import current team -> compare players by position? use critera such as age and transfervalue 
to filter players? or by biggest improvement per position?
* GUI
* Further analytics