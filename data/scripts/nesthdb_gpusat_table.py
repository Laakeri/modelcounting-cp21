#!/usr/bin/env python3 

from sys import argv
from os import path
from subprocess import call

s1 = 0
s2 = 0
s3 = 0

tt = {}
tt["all"] = []
tt["ss"] = []
tt["gpu"] = []
tt["nest"] = []
tt["vbs"] = []

with open("../results/nesthdb_gpusat.csv", 'r') as file:
	for line in file:
		tline = line.strip().split(",")
		if tline[0] == "instance":
			continue
		t1 = 910
		t2 = 910
		t3 = 910
		tw = 0
		if tline[2] != "TO" and tline[2] != "RTE":
			t1 = float(tline[2])
			assert t1 < 900
			s1 += 1
		if tline[3] != "TO" and tline[3] != "RTE":
			t2 = float(tline[3])
			assert t2 < 900
			s2 += 1
		if tline[4] != "TO" and tline[4] != "RTE":
			t3 = float(tline[4])
			assert t3 < 900
			s3 += 1
		if tline[1] != "-":
			tw = int(tline[1])
		else:
			if t1 < 900 or t2 < 900 or t3 < 900:
				tw = 0
			else:
				tw = 999
		tt["all"].append(tw)
		if t1 < 900:
			tt["ss"].append(tw)
		if t2 < 900:
			tt["nest"].append(tw)
		if t3 < 900:
			tt["gpu"].append(tw)
		if t1 < 900 or t2 < 900 or t3 < 900:
			tt["vbs"].append(tw)

for r in [(0, 30), (31, 50), (51, 100), (101, 200), (201, 266), (267, 100000)]:
	x = ["all", "vbs", "gpu", "nest", "ss"]
	c = {}
	for t in x:
		c[t] = 0
	for t in x:
		for h in tt[t]:
			if h>=r[0] and h<=r[1]:
				c[t] += 1
	print(r)
	s = ""
	for t in x:
		s += t + ": " + str(c[t]) + "   "
	print(s)