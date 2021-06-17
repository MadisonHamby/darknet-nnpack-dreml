#!/bin/bash

# Run a certain amount of frames
# run python script to get min and max tensors and write them to csv

i=1
until [ $i -gt 81101 ]
do
  # Run darknet
  ./darknet detect cfg/yolov3.cfg yolov3.weights data/ZurichMAV/downsample_frames/AGZ_$i.jpg

  # Run python script
  python3 tensor_range_csv.py

  ((i=i+100))
done
