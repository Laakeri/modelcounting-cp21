#!/usr/bin/env python3 

from sys import argv
from os import path
from subprocess import call

bad = {"BAD", "ERR", "UR"}
fail = {"TO", "MO", "RTE"}

ss = {}
label = {}

with open("../results/var_sel_eq.csv", 'r') as file:
	for line in file:
		tline = line.strip().split(",")
		assert(len(tline) == 8)
		if tline[0] == "instance":
			for i in range(1, 8):
				label[i] = tline[i]
				ss[tline[i]] = 0
			continue
		for i in range(1, 8):
			assert(tline[i] not in bad)
			if tline[i] not in fail:
				ss[label[i]] += 1

print("C = 1e3: " + str(ss["sharpsat_tw_const1e3"]))
print("C = 1e7: " + str(ss["sharpsat_tw_const1e7"]))
print("C = 100 exp(tw/n): " + str(ss["sharpsat_tw_form1"]))
print("C = 100 exp(tw/n)/n: " + str(ss["sharpsat_tw_form2"]))
print("C as above, no activity: " + str(ss["sharpsat_tw_form2_noact"]))
print("C as above, no frequency: " + str(ss["sharpsat_tw_form2_nofreq"]))
print("C as above, no activity nor frequency: " + str(ss["sharpsat_tw_form2_noact_nofreq"]))