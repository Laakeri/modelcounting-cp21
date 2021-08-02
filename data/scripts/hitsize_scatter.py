#!/usr/bin/env python3 

from sys import argv
from os import path
from subprocess import call


bad = {"BAD", "UR", "ERR"}
fail = {"TO", "MO", "RTE"}

p = []

with open("../results/cache_hits.csv", 'r') as file:
	for line in file:
		tline = line.strip().split(",")
		if tline[0] == "instance" or tline[0] == "totals":
			continue
		assert (tline[1] not in bad) and (tline[2] not in bad)
		t1 = 8000
		t2 = 8000
		if tline[1] not in fail:
			#SharpSAT default
			t1 = float(tline[1])
			assert t1 < 7200
		if tline[2] not in fail:
			#SharpSAT-tw
			t2 = float(tline[2])
			assert t2 < 7200
		if t1 < 60 and t2 < 960:
			continue
		if t1 > 7200 or t2 > 7200:
			continue
		p.append((tline[3], tline[4]))


for r in p:
	print(r[0] + " " + r[1])