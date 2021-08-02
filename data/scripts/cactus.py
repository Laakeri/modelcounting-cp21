#!/usr/bin/env python3 

import sys
from sys import argv, stdout
from os import path
from subprocess import call

times = {}
ins = {}

bad = {"BAD", "UR", "ERR"}
fail = {"TO", "MO", "RTE"}

def ToBold(yes, s):
	if not yes:
		return s
	else:
		return "\\textbf{" + s + "}"

def PaperFam(s):
	s = s.replace("BayesianNetwork", "BN")
	s = s.replace("/", "-")
	if s == "BN":
		s = "BN-other"
	elif s == "Planning-pr-":
		s = "Plan recog"
	elif s == "Planning":
		s = "Planning-other"
	elif s == "circuit":
		s = "circuit-other"
	elif s == "bmc-pmc-symbolic-markov":
		s = "bmc-symb-markov"
	elif s == "bmc":
		s = "bmc-other"
	elif s == "qif":
		s = "qif-other"
	elif s == "synth-symbolic-sygus":
		s = "symb-sygus"
	return s

def PaperSolver(s):
	s = s.replace("sharpsat", "SharpSAT")
	s = s.replace("ganak", "GANAK")
	s = s.replace("_tw", "-TW")
	return s

label = {}
for line in open("../results/main.csv", 'r'):
	tline = line.strip().split(",")
	assert(len(tline) == 14)
	if tline[0] == "instance":
		for i in range(1, len(tline)):
			label[i] = tline[i]
			times[tline[i]] = {}
		continue
	tins = tline[0]
	if "Planning/" in tins:
		if "step" in tins or "tire-" in tins or "/log-" in tins:
			tins = tins.replace("Planning/", "Planning/pr/")
		if "pddl" in tins:
			tins = tins.replace("Planning/", "Planning/pddl/")
	ins[tins] = 1
	for i in range(1, len(tline)):
		assert tline[i] not in bad
		if tline[i] not in fail:
			if i == 1:
				assert int(tline[i]) >= -1 and int(tline[i]) <= 10000
			else:
				assert float(tline[i]) >= 0 and float(tline[i]) <= 7200
		times[label[i]][tins] = tline[i]


assert(len(ins) == 2424)

sout = sys.stdout

with open("cactus.dat", "w") as f:
	sys.stdout = f
	for s in ["sharpsat", "sharpsat_tw", "ganak", "ganak_tw", "c2d", "c2d_tw", "minic2d", "minic2d_tw", "sharpsat_cen", "d4", "dpmc_lg", "dpmc_htb"]:
		print("#" + s)
		l = []
		for i in ins:
			if times[s][i] not in fail:
				l.append(float(times[s][i]))
		j = 0
		for t in sorted(l):
			j += 1
			print(str(j) + " " + str(t))
		print()
		print()
		print()

sys.stdout = sout

for s in ["sharpsat", "sharpsat_tw", "ganak", "ganak_tw", "c2d", "c2d_tw", "minic2d", "minic2d_tw", "sharpsat_cen", "d4", "dpmc_lg", "dpmc_htb"]:
	so = 0
	for i in ins:
		if times[s][i] not in fail:
			so += 1
	print(s + " : " + str(so))