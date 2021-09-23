#!/usr/bin/python3
#
# Copyright (c) 2021, NVIDIA CORPORATION. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#

import argparse
import random
import time
from pythonosc import udp_client
import jetson.inference
import jetson.utils

import argparse
import sys

# parse the command line
parser = argparse.ArgumentParser(description="Run pose estimation DNN on a video/image stream.", 
                                 formatter_class=argparse.RawTextHelpFormatter, epilog=jetson.inference.poseNet.Usage() +
                                 jetson.utils.videoSource.Usage() + jetson.utils.videoOutput.Usage() + jetson.utils.logUsage())

parser.add_argument("input_URI", type=str, default="", nargs='?', help="URI of the input stream")
parser.add_argument("output_URI", type=str, default="", nargs='?', help="URI of the output stream")
parser.add_argument("--network", type=str, default="resnet18-body", help="pre-trained model to load (see below for options)")
parser.add_argument("--overlay", type=str, default="links,keypoints", help="pose overlay flags (e.g. --overlay=links,keypoints)\nvalid combinations are:  'links', 'keypoints', 'boxes', 'none'")
parser.add_argument("--threshold", type=float, default=0.15, help="minimum detection threshold to use") 

try:
	opt = parser.parse_known_args()[0]
except:
	print("")
	parser.print_help()
	sys.exit(0)

# load the pose estimation model
net = jetson.inference.poseNet(opt.network, sys.argv, opt.threshold)

# create video sources & outputs
input = jetson.utils.videoSource(opt.input_URI, argv=sys.argv)
output = jetson.utils.videoOutput(opt.output_URI, argv=sys.argv)

client = udp_client.SimpleUDPClient("192.168.1.151", 7700) # setup OSC with target IP and port

# variables for working out which person is biggest
global biggest
biggest = 0
global diffPrev
diffPrev = 0

# variables for head motion
global look
look = 0
global lookPrev
lookPrev = 0

# variables for left arm
global armL
armL = 0
global armLPrev
armLPrev = 0

# variables for right arm
global armR
armR = 0
global armRPrev
armRPrev = 0

# process frames until the user exits
while True:
    # capture the next image
    img = input.Capture()

    # perform pose estimation (with overlay)
    poses = net.Process(img, overlay=opt.overlay)

    # print the pose results
    #print("detected {:d} objects in image".format(len(poses)))

    # list of keypoints
    # nose	 	0
    # eye L	 	1
    # eye R		2
    # ear L		3
    # ear R		4
    # shoulder L	5
    # shoulder R	6
    # elbow L		7
    # elbow R		8

    for pose in poses:				# go through all of the people detected and find the biggest
        #print(pose.ID)
        eye1_idx = pose.FindKeypoint(1)
        eye1 = pose.Keypoints[eye1_idx]
        eye2_idx = pose.FindKeypoint(2)
        eye2 = pose.Keypoints[eye2_idx]
        diff = eye1.x - eye2.x			# compare eye distance to find the biggest

        if diff > diffPrev:
            biggest
            biggest = (pose.ID)
            diffPrev = diff			# bookmark the biggest 
        
    for pose in poses:				# go through the poses again
        if (pose.ID) == biggest:		# only look at the biggest person

            nose_idx = pose.FindKeypoint(0)		# nose
            neck_idx = pose.FindKeypoint(17)		# neck
            nose = pose.Keypoints[nose_idx]
            neck = pose.Keypoints[neck_idx]

            shoulderL_idx = pose.FindKeypoint(5)	# left arm
            elbowL_idx = pose.FindKeypoint(7)
            shoulderL = pose.Keypoints[shoulderL_idx]
            elbowL = pose.Keypoints[elbowL_idx]

            shoulderR_idx = pose.FindKeypoint(6)	# right arm
            elbowR_idx = pose.FindKeypoint(8)
            shoulderR = pose.Keypoints[shoulderR_idx]
            elbowR = pose.Keypoints[elbowR_idx]


            ######## work out which way you're looking ########

            if nose.x < neck.x - 40: 		
                look = 0
            elif nose.x > neck.x + 40:
                look = 1    
            else:
                look = 2                             

            if look != lookPrev:
                if look == 0:
                    print("look right")
                    client.send_message("/look/1", 1.0)
                elif look == 1:
                    print("look left")
                    client.send_message("/look/0", 1.0)
                elif look == 2:
                    print("look middle")
                    client.send_message("/look/2", 1.0)

            lookPrev = look   			# bookmark previous value

            ######## left arm pose #########

            if elbowL.y < shoulderL.y - 60:
                armL = 0
            elif elbowL.y > shoulderL.y + 60:
                armL = 1
            else:
                armL = 2

            if armLPrev != armL:
                if armL == 0:
                    print("left arm up")
                    client.send_message("/armR/0", 1.0)
                elif armL == 1:
                    print("left arm down")
                    client.send_message("/armR/1", 1.0)
                elif armL == 2:
                    print("left arm middle")
                    client.send_message("/armR/2", 1.0)

            armLPrev = armL

            ######## right arm pose #########

            if elbowR.y < shoulderR.y - 60:
                armR = 0
            elif elbowR.y > shoulderR.y + 60:
                armR = 1
            else:
                armR = 2

            if armRPrev != armR:
                if armR == 0:
                    print("right arm up")
                    client.send_message("/armL/0", 1.0)
                elif armR == 1:
                    print("right arm down")
                    client.send_message("/armL/1", 1.0)
                elif armR == 2:
                    print("right arm middle")
                    client.send_message("/armL/2", 1.0)

            armRPrev = armR

            


    # render the image
    output.Render(img)

    # update the title bar
    #output.SetStatus("{:s} | Network {:.0f} FPS".format(opt.network, net.GetNetworkFPS()))

    # print out performance info
    #net.PrintProfilerTimes()

    # exit on input/output EOS
    if not input.IsStreaming() or not output.IsStreaming():
        break
