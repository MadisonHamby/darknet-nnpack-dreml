#!/bin/bash

# Run a certain amount of frames
# Saves frames as frame#.txt
#make

for i in {25000..25050}
do
    # Run darknet
    ./darknet detect cfg/yolov3.cfg yolov3.weights data/ZurichMAV/Frames/$i.jpg
    #echo "$i\n"

    # Save det-results.txt as 25000.txt in data/ZurichMAV/DetectionResults
    cp det_result.txt /home/madison/mAP/input/detection-results/$i.txt
done
