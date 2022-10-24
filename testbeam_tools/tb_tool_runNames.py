#runNumList=[2476, 2478, 2479, [2483, 2489], [2491, 2496], 2498, 2499]
#pathPrefix="/eos/atlas/atlascerngroupdisk/pixel-upgrade/itk/BeamTest/cern_2022_april_itk_sps/native/run00"
#runNumList=[2732,2733,[2736,2744],2747,2748,[2750,2754]]
'''
runNumList=[
2732,2733,[2736,2744],2747,2748,[2750,2754],#800e
[2518,2623],[2630,2668],#1000e
[2779,2784],[2786,2802],#1200e
[2669,2673],2675,[2677,2680],[2708,2713],[2719,2730]#1500e
]
pathPrefix="/eos/atlas/atlascerngroupdisk/pixel-upgrade/itk/BeamTest/cern_2022_june_itk/native/run00"
'''
runNumList=[
6219, 6222, 6232, 6237, 6240, 6241, 6251, 6252,
6315, 6331, 6339, 6345, [6347,6350],
[6352,6359],
[6360,6367],
[6368,6373],
[6375,6378],
6380, 6383, 6384, 6386,
]
pathPrefix="/eos/atlas/atlascerngroupdisk/pixel-upgrade/itk/BeamTest/20220928.cern.itk/raw/run00"
pathSuffix=".raw"
runs=[]
for i in range(len(runNumList)):
	if type(runNumList[i])==list:
		for j in range(runNumList[i][0],runNumList[i][1]+1):
			runs.append(j)
	else:
		runs.append(runNumList[i])
print(runs)
print("# of runs:",len(runs))
final_str=""
for i in range(len(runs)):
	if len(runs)!=1 and i!=len(runs)-1:
		final_str=final_str+'"'+pathPrefix+str(runs[i])+pathSuffix+'",'
	else:
		final_str=final_str+'"'+pathPrefix+str(runs[i])+pathSuffix+'"'
	#runs[i]=pathPrefix+str(runs[i])+pathSuffix
final_str="'"+final_str+"'"
print(final_str)
