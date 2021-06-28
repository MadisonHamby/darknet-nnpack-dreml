#!/bin/bash

# Run a certain amount of frames
# Saves frames as frame#.txt
#make

for i in {25000..25199}
do
    # Run darknet
    ./darknet detect cfg/yolov3.cfg yolov3.weights data/ZurichMAV/$i.jpg
    #echo "$i\n"

    # Save det-results.txt as 25000.txt in data/ZurichMAV/DetectionResults
    cp det_result.txt /home/madison/mAP/input/detection-results/$i.txt
done
