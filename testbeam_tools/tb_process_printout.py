import sys

#sys.dont_write_bytecode = True

import ROOT
import os
import json
from ROOT import gStyle
from ROOT import TFile

from helperFunctions_Yusong import *

#ROOT.gROOT.SetBatch(1)#doesn't show the graph

runN=6219
inDir = "/eos/user/t/tiany/testbeam/tbanalysis_2022-10_run"+str(runN)+"_betterAlign/"
ouDir="/eos/user/t/tiany/code/correlations_tb/"
inFname="printout_analysis.txt"
ouFname="printout_organised.txt"
confName="07_analysis.conf"

fullPath_in=addPath(inDir,inFname)
print(fullPath_in)

fullPath_ou=addPath(ouDir,ouFname)
print(fullPath_ou)

effTable_select=[
	[["Track selection flow:       "],["\n"]],
	[["* rejected by chi2          "],["\n"]],
	[["* track outside ROI         "],["\n"]],
	[["* track outside DUT         "],["\n"]],
	[["* track close to masked px  "],["\n"]],
	[["* track close to frame edge "],["\n"]],
	[["* track outside lv1 limits "],["\n"]],
	[["* track without an associated cluster on required detector - "],["\n"]],
	[["Accepted tracks:            "],["\n"]],
	[["Total efficiency of detector plane"],["%, measured with "]],
	[["%, measured with "],[" matched/total tracks\n"]],
	[["interchip region chip1-2: "],[" matched/total tracks\n"]],
	[["interchip region chip2-3: "],[" matched/total tracks\n"]],
	[["interchip region chip3-4: "],[" matched/total tracks\n"]],
	[["interchip region chip1-4: "],[" matched/total tracks\n"]],
	[["center of the interchip region: "],[" matched/total tracks\n"]],
	[["interchip region: "],[" matched/total tracks\n"]],
	[["default size pixels: "],[" matched/total tracks\n"]]
]
effTable_transposed=list(map(list, zip(*effTable_select)))
effTable_start=effTable_transposed[0]
effTable_end=effTable_transposed[1]

effTable_select_quad=[
	[["Track selection flow:       "],["\n"]],
	[["* rejected by chi2          "],["\n"]],
	[["* track outside roi         "],["\n"]],
	[["* track outside DUT         "],["\n"]],
	[["* track close to masked px  "],["\n"]],
	[["* track close to frame edge "],["\n"]],
	[["* track outside lv1 limits "],["\n"]],
	[["* track without an associated cluster on required detector - "],["\n"]],
	[["Accepted tracks:            "],["\n"]],
	[["Total efficiency of detector plane"],["%, measured with "]],
	[["%, measured with "],[" matched/total tracks\n"]],
	[["interchip region chip1-2: "],[" matched/total tracks\n"]],
	[["interchip region chip2-3: "],[" matched/total tracks\n"]],
	[["interchip region chip3-4: "],[" matched/total tracks\n"]],
	[["interchip region chip1-4: "],[" matched/total tracks\n"]],
	[["center of the interchip region: "],[" matched/total tracks\n"]],
	[["interchip region: "],[" matched/total tracks\n"]],
	[["default size pixels: "],[" matched/total tracks\n"]]
]
effTable_transposed_quad=list(map(list, zip(*effTable_select_quad)))
effTable_start_quad=effTable_transposed_quad[0]
effTable_end_quad=effTable_transposed_quad[1]
#print(effTable_start)

printoutF=open(fullPath_in,"r")
allLines=printoutF.readlines()

ouLines=[]
i=0
while i<len(allLines):
	aLine=allLines[i]
	if confName in aLine:
		ouLines.append(aLine)
		i=i+1
	elif "(STATUS) Ev: " in aLine:
		ouLines.append(aLine[aLine.find("Ev: "):len(aLine):1])
		i=i+1
	# find the 1st line of DUTAssociation
	# loop through the rest of the analysis
	# start to write in table
	elif "(STATUS) [F:DUTAssociation:" in aLine:
		DUTs=[]
		aTable=[["DUT"],["cl to tr"]]
		effTable=effTable_start
		j=i
		while j<len(allLines):
			if "(STATUS) [F:DUTAssociation:" in allLines[j]:
				pl=findBetween(allLines[j],"[F:DUTAssociation:plane","] In total")
				cl=findBetween(allLines[j],pl+"] In total, "," clusters")
				tr=findBetween(allLines[j],cl+" clusters are associated to "," tracks")
				aTable[0].append("\t")
				aTable[0].append("pl"+pl)
				DUTs.append(pl)
				aTable[1].append("\t")
				aTable[1].append(cl+" to "+tr)
				j=j+1
			# find the first time when AnalysisEfficiency gives result
			elif "(STATUS) [F:AnalysisEfficiency:plane" in allLines[j] and "Track selection flow:" in allLines[j]:
				for k in range(len(DUTs)):
					res_1DUT=cutList(allLines,"[F:AnalysisEfficiency:plane","Total efficiency of detector plane",j)
					effTable=fillTable(effTable_start, effTable_end, res_1DUT, effTable, "	")
					j=j+len(res_1DUT)
				#print("effTable:",effTable)
			elif "(STATUS) [F:AnalysisITkPixQuad:plane" in allLines[j] and "Track selection flow:" in allLines[j]:
				for k in range(len(DUTs)):
					aTable[0].append("\t")
					aTable[0].append("pl"+DUTs[k]+"AnalysisITkPixQuad")
					res_1DUT=cutList(allLines,"(STATUS) [F:AnalysisITkPixQuad:plane"," for the default size pixels: ",j)
					effTable=fillTable(effTable_start_quad, effTable_end_quad, res_1DUT, effTable, "	")
					j=j+len(res_1DUT)
				#print("effTable:",effTable)
				effTable=addLineBreak_list(effTable)
				aTable.append(effTable)
			# loop through until end of this analysis
			elif "(STATUS) Wrote histogram output file to" in allLines[j]:
				print("!!!!!!end of 1 run!!!!!!!")
				aTable[0].append("\n")
				aTable[1].append("\n")
				i=j
				j=len(allLines)+1
				break
			else:
				j=j+1
			print(j,"finding data")
			i=j
		print(i,", got the data table for 1 file!")
		ouLines.append(aTable)
	else:
		i=i+1
#print(ouLines)
#print("DUTs: ",DUTs)
ouLines=addLineBreak_list(ouLines)
ouf=open(fullPath_ou,"w")
writeList(ouLines,ouf)
ouf.close()

