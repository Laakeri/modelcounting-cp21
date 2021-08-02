#!/usr/bin/env python3 

from sys import argv
from os import path
from subprocess import call

ins = 0
solved = 0
solved_time = 0.0

bad = {"BAD", "ERR", "UR"}
fail = {"TO", "MO", "RTE"}

with open("../results/competition.csv", 'r') as file:
	for line in file:
		tline = line.strip().split(",")
		if tline[0] == "instance":
			continue
		for i in range(1, 2):
			assert(tline[i] not in bad)
		ins += 1
		if tline[1] not in fail:
			solved += 1
			time = float(tline[1])
			assert time > 10 and time < 1200
			solved_time += time

print("ins: " + str(ins))
print("solved: " + str(solved))
print("solved avgtime: "+str(solved_time/float(solved)))
