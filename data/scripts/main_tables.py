#!/usr/bin/env python3 

from sys import argv
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

solvers = ["sharpsat", "sharpsat_tw", "ganak", "ganak_tw", "c2d", "c2d_tw", "minic2d", "minic2d_tw"]

# By instance family
done = {}
fams = ["BayesianNetwork/Ratio", "BayesianNetwork/DQMR", "BayesianNetwork/Ace", "BayesianNetwork", "Planning/pr/", "Planning/pddl", "Planning", "circuit/iscas", "circuit", "bmc/pmc-symbolic-markov", "bmc", "synth/symbolic-sygus", "qif/maxcount-qif", "qif", "Handmade", "Configuration", "random", "scheduling"]

pline = "Family & \\#inst & VBS & VBS-O & VBS-TW"
for s in solvers:
	pline += " & " + PaperSolver(s)
print(pline + "\\\\")
print("\\hline")

tot = {}
for s in solvers:
	tot[s] = 0
tot["vbs"] = 0
tot["vbs-tw"] = 0
tot["vbs-o"] = 0
tot["ins"] = 0

for fam in fams:
	pline = PaperFam(fam)
	ct = 0
	vbs = 0
	vbs_tw = 0
	vbs_o = 0
	solved = {}
	for s in solvers:
		solved[s] = 0
	for i in ins:
		if fam not in i:
			continue
		if i in done:
			continue
		done[i] = 1
		cs = False
		cs_o = False
		cs_tw = False
		for s in solvers:
			if times[s][i] not in fail:
				assert float(times[s][i]) < 7199.0
				cs = True
				if "tw" in s:
					cs_tw = True
				else:
					cs_o = True
		ct += 1
		tot["ins"] += 1
		if cs:
			vbs += 1
			tot["vbs"] += 1
		if cs_o:
			vbs_o += 1
			tot["vbs-o"] += 1
		if cs_tw:
			vbs_tw += 1
			tot["vbs-tw"] += 1
		for s in solvers:
			if times[s][i] not in fail:
				assert float(times[s][i]) < 7199.0
				solved[s] += 1
				tot[s] += 1
	pline += " & " + str(ct) + " & " + str(vbs) + " & " + ToBold(vbs_o > vbs_tw, str(vbs_o)) + " & " + ToBold(vbs_tw > vbs_o, str(vbs_tw))
	for i in range(0, 8):
		tobold = False
		if i < 8:
			xo = i
			if i%2 == 1:
				xo -= 1
			else:
				xo += 1
			if solved[solvers[xo]] < solved[solvers[i]]:
				tobold = True
		pline += " & " + ToBold(tobold, str(solved[solvers[i]]))
	print(pline+"\\\\")

print("\\hline")
pline = "Total & " + str(tot["ins"]) + " & " + str(tot["vbs"]) + " & " + ToBold(tot["vbs-o"] > tot["vbs-tw"], str(tot["vbs-o"])) + " & " + ToBold(tot["vbs-tw"] > tot["vbs-o"], str(tot["vbs-tw"]))

for i in range(0, 8):
	tobold = False
	if i < 8:
		xo = i
		if i%2 == 1:
			xo -= 1
		else:
			xo += 1
		if tot[solvers[xo]] < tot[solvers[i]]:
			tobold = True
	pline += " & " + ToBold(tobold, str(tot[solvers[i]]))
print(pline+"\\\\")


# By treewidth
print()
print()
print()
tot = {}
for s in solvers:
	tot[s] = 0
tot["vbs"] = 0
tot["vbs-tw"] = 0
tot["vbs-o"] = 0
tot["ins"] = 0

pline = "TW & \\#inst & VBS & VBS-O & VBS-TW"
for s in solvers:
	pline += " & " + PaperSolver(s)
print(pline + "\\\\")
print("\\hline")

for r in [(-1,20), (21, 30), (31, 50), (51, 100), (101, 150), (151, 200), (201, 300), (301, 1000000)]:
	pline = ""
	if r[0] == -1:
		pline += "$\\le " + str(r[1]) + " $"
	elif r[1] == 1000000:
		pline += "$ " + str(r[0]) + "\\le$"
	else:
		pline += "$" + str(r[0]) + " \\ldots " + str(r[1]) + "$"
	ct = 0
	vbs = 0
	vbs_tw = 0
	vbs_o = 0
	solved = {}
	for s in solvers:
		solved[s] = 0
	for i in ins:
		tw = -1
		stw = times["treewidth"][i]
		if stw not in fail and stw != "-1":
			tw = int(stw)
		if tw < r[0] or tw > r[1]:
			continue
		cs = False
		cs_o = False
		cs_tw = False
		for s in solvers:
			if times[s][i] not in fail:
				assert float(times[s][i]) < 7199.0
				cs = True
				if "tw" in s:
					cs_tw = True
				else:
					cs_o = True
		ct += 1
		tot["ins"] += 1
		if cs:
			vbs += 1
			tot["vbs"] += 1
		if cs_o:
			vbs_o += 1
			tot["vbs-o"] += 1
		if cs_tw:
			vbs_tw += 1
			tot["vbs-tw"] += 1
		for s in solvers:
			if times[s][i] not in fail:
				assert float(times[s][i]) < 7199.0
				solved[s] += 1
				tot[s] += 1
	pline += " & " + str(ct) + " & " + str(vbs) + " & " + ToBold(vbs_o > vbs_tw, str(vbs_o)) + " & " + ToBold(vbs_tw > vbs_o, str(vbs_tw))
	for i in range(0, 8):
		tobold = False
		if i < 8:
			xo = i
			if i%2 == 1:
				xo -= 1
			else:
				xo += 1
			if solved[solvers[xo]] < solved[solvers[i]]:
				tobold = True
		pline += " & " + ToBold(tobold, str(solved[solvers[i]]))
	print(pline+"\\\\")

print("\\hline")
pline = "Total & " + str(tot["ins"]) + " & " + str(tot["vbs"]) + " & " + ToBold(tot["vbs-o"] > tot["vbs-tw"], str(tot["vbs-o"])) + " & " + ToBold(tot["vbs-tw"] > tot["vbs-o"], str(tot["vbs-tw"]))

for i in range(0, 8):
	tobold = False
	if i < 8:
		xo = i
		if i%2 == 1:
			xo -= 1
		else:
			xo += 1
		if tot[solvers[xo]] < tot[solvers[i]]:
			tobold = True
	pline += " & " + ToBold(tobold, str(tot[solvers[i]]))
print(pline+"\\\\")