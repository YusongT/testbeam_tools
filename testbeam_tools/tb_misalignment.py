
from helperFunctions_Yusong import *

#ROOT.gROOT.SetBatch(1)#doesn't show the graph

#May need to source /opt/homebrew/bin/thisroot.sh on mac
#root-config --python-version
#python3.10 print_plots.py

# ------ ------ config ------ ------
'''
inDir="/Users/ytian/hardware/2021_testBeam/tb_2023/reconstruction/"
ouDir="/Users/ytian/code/output_tb/"

inF_names=[
["align_DUT_24_abs50_50.root","24_abs50"]
]

inF_paths=[
["AlignmentDUTResidual/plane"],
["130"],
["/residualsX","/residualsY"],
]

pic_names=[
["_pl"],#extra explanation
["130"],
["_residuals"],
["X","Y"],
]
'''

inDir="/Users/ytian/hardware/tb/tb_2023/misalignment_analysis/"
ouDir="/Users/ytian/code/output_tb/"
ouFname='misalignment_pl24.txt'

runNus_24=[7490,7491,7757,7773,7774,7779,7783,7787,7790,7803,7913,7934]
runNus_130=[7490,7491,7768,7774,7806,7816,7818,7827,7829,7836,7867,7875,7878,7902,7906,7911,7913,7934,7936,7947]
print('#runs =',len(runNus_24)+len(runNus_130))
fileName_prefix='analysis_run'
fileName_suffix='_4Dabs100100_pl24None_minH7_2023-6-26.root'
inF_names=[]
for runNu in runNus_24:
	inF_names.append([fileName_prefix+str(runNu)+fileName_suffix,'run'+str(runNu)])
print(inF_names)
#inF_names=[
#['analysis_run7490_4Dabs100100_pl24None_minH7_2023-6-26.root',"run7490"],
#]

inF_paths=[
["AnalysisDUT"],
#["AnalysisITkPixQuad","AnalysisDUT"],
["/plane110","/plane130"],
["/local_residuals/"],
#["/local_residuals/","/global_residuals/"],
["residualsX","residualsY"],
]

pic_names=[
["_pl"],
["110","130"],
["_local_"],
#["_local_","_global_"],
["residualsX_2","residualsY_2"],
]

# ------ ------ main() ------ ------

#the path to hs after entering file
h_paths1=multiplyList(inF_paths)
#file name for the corresponding figure
pic_names1=multiplyList(pic_names)

# sanity check: 1 hist per figure (name)
if len(h_paths1)!=len(pic_names1):
	print("!!!!!! #hists different from #figure names!")
else:
	print("#hists = #figure names")
	for i in range(len(h_paths1)):
		print(i,h_paths1[i],pic_names1[i])

pic_names2=copyList(pic_names1)

mean_files_li=[]
for i in range(len(inF_names)):
	mean_file_li=[]
	#add prefix to all names
	for j in range(len(pic_names1)):
		newName="h_"+inF_names[i][1]+pic_names1[j]
		pic_names2[j]=newName
		print("hist name:",pic_names2[j],"path:",inF_names[i],h_paths1[j])
	inF_fullPath=addPath(inDir,inF_names[i][0])
	print("inF_fullPath:",inF_fullPath)
	f_inF=TFile.Open(inF_fullPath,"r")
	f_inF.ls()
	#print("h_paths1:",h_paths1)

	h_li=getTH1s(f_inF,ouDir,h_paths1,pic_names2,10)
	for h in h_li:
		# Fit histogram with root #
		print('h.GetEntries =',h.GetEntries())
		if h.GetEntries()!=0:
			h.Fit('gaus')
			# Get Root Fit and Goodness of Fit Parameters #
			f = h.GetFunction('gaus')
			const,mu,sigma = f.GetParameter(0), f.GetParameter(1), f.GetParameter(2)
			econst,emu,esigma = f.GetParError(0), f.GetParError(1), f.GetParError(2)
			ndf,chi2,prob = f.GetNDF(),f.GetChisquare(),f.GetProb()
			print(chi2/ndf,prob)
		else:
			print('empty h')
			const=mu=sigma=-999
		print('const, mu, sigma =',const, mu, sigma)
		mean_file_li.append(mu)
	mean_files_li.append(mean_file_li)
	#printTH1s(h_li,ouDir,pic_names2)
print('all means:',mean_files_li)
mean_files_li=changeType_list(mean_files_li,'string')
print(mean_files_li,type(mean_files_li[0]),type(mean_files_li[0][0]),type(mean_files_li[1][0][1]))
for i in range(len(runNus_24)):
	f = open(ouDir+ouFname, 'a')
	f.write(str(runNus_24[i])+'	')	
	saveList(mean_files_li[i],f,'	','a')
	f.write('\n')
#print2D(f_inF,ouDir,h_paths1,pic_names2,rebX,rebY)
