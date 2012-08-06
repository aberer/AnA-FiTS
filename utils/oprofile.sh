#! /bin/bash

SESSION_FOLDER=/lhome/labererae/ffits/profile 


# sudo -s 

# opcontrol --deinit
# echo 0 > /proc/sys/kernel/nmi_watchdog


sudo killall oprofiled

progName=$1
shift
sudo opcontrol --reset
sudo opcontrol --no-vmlinux --session-dir=$SESSION_FOLDER --event=CPU_CLK_UNHALTED:6000 
sudo opcontrol --separate=thread
sudo opcontrol --start
$progName $* 
# sudo opcontrol --dump
sudo opcontrol --stop


sudo opreport --session-dir=$SESSION_FOLDER -l image:$progName 
# sudo opannotate  --session-dir=$SESSION_FOLDER  --source $progName 

