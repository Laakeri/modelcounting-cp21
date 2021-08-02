#!/usr/bin/env python3 

from sys import argv
from os import path
from subprocess import call

s5 = 0
s60 = 0
s900 = 0
s1800 = 0

pos = 0
neg = 0
ins = 0
diff = 0

bad = {"BAD", "ERR", "UR"}
fail = {"TO", "MO", "RTE"}

with open("../results/tw_time.csv", 'r') as file:
	for line in file:
		tline = line.strip().split(",")
		if tline[0] == "instance":
			continue
		for i in range(1, 5):
			assert(tline[i] not in bad)
		ins += 1
		if tline[1] not in fail:
			s5 += 1
		if tline[2] not in fail:
			s60 += 1
		if tline[3] not in fail:
			s900 += 1
		if tline[4] not in fail:
			s1800 += 1
		if tline[1] not in fail and tline[3] in fail:
			pos += 1
			diff += 1
		if tline[1] in fail and tline[3] not in fail:
			neg += 1
			diff += 1

print("ins: " + str(ins))
print("solve 5s: " + str(s5))
print("solve 60s: " + str(s60))
print("solve 900s: " + str(s900))
print("solve 1800s: " + str(s1800))
print("diff 5,900: " + str(diff))
print("pos 5,900: " + str(pos))
print("neg 5,900: " + str(neg))