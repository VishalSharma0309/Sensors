# Installation script for all the libraries mentioned in the repository

# Made by Vishal Sharma (https://github.com/VishalSharma0309)

#! /bin/bash
# script to install ROS-melodic directly into a linux system
# tested on Ubuntu 18.04.02

#use sudo to run this command
if [ "$(id -u)" -ne 0 ]
        then echo "Must be root"
        exit
fi


