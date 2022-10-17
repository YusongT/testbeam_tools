#runNumList=[2476, 2478, 2479, [2483, 2489], [2491, 2496], 2498, 2499]
#runNumList=[2476]
#pathPrefix="/eos/atlas/atlascerngroupdisk/pixel-upgrade/itk/BeamTest/cern_2022_april_itk_sps/native/run00"
#runNumList=[2732,2733,[2736,2744],2747,2748,[2750,2754]]
runNumList=[
2732,2733,[2736,2744],2747,2748,[2750,2754],#800e
[2518,2623],[2630,2668],#1000e
[2779,2784],[2786,2802],#1200e
[2669,2673],2675,[2677,2680],[2708,2713],[2719,2730]#1500e
]
pathPrefix="/eos/atlas/atlascerngroupdisk/pixel-upgrade/itk/BeamTest/cern_2022_june_itk/native/run00"
pathSuffix=".raw"
runs=[]
for i in range(len(runNumList)):
	if type(runNumList[i])==list:
		for j in range(runNumList[i][0],runNumList[i][1]+1):
			runs.append(j)
	else:
		runs.append(runNumList[i])
print(runs)
for i in range(len(runs)):
	'''
	if len(runs)!=1 and i!=len(runs)-1:
		runs[i]=pathPrefix+str(runs[i])+pathSuffix+","
	else:
		runs[i]=pathPrefix+str(runs[i])+pathSuffix
	'''
	runs[i]=pathPrefix+str(runs[i])+pathSuffix
print(runs)
