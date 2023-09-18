#input run numbers as list, print string used to insert to filename

'''
pathPrefix="/eos/atlas/atlascerngroupdisk/pixel-upgrade/itk/BeamTest/cern_2022_april_itk_sps/native/run00"
runNumList=[2476, 2478, 2479, [2483, 2489], [2491, 2496], 2498, 2499]
runNumList=[2732,2733,[2736,2744],2747,2748,[2750,2754]]

pathPrefix="/eos/atlas/atlascerngroupdisk/pixel-upgrade/itk/BeamTest/cern_2022_june_itk/native/run00"
runNumList=[
2732,2733,[2736,2744],2747,2748,[2750,2754],#800e
[2518,2623],[2630,2668],#1000e
[2779,2784],[2786,2802],#1200e
[2669,2673],2675,[2677,2680],[2708,2713],[2719,2730]#1500e
]

#pathPrefix="/eos/atlas/atlascerngroupdisk/pixel-upgrade/itk/BeamTest/20220928.cern.itk/raw/run00"
#46 runs
runNumList=[
6219, 6222, 6232, 6237, 6240, 6241, 6251, 6252,
6315, 6331, 6339, 6345, [6347,6350],
[6352,6359],
[6360,6367],
[6368,6373],
[6375,6378],
6380, 6383, 6384, 6386,
]
runNumList=[
6219, 6222, 6232, 6237, 6240, 6241, 6251, 6252, #0V 1ke
6315, 6331, 6339, 6345, [6347,6350], #1V 1ke
[6352,6359], #2V 1ke
[6360,6367], #3V 1ke
[6368,6373], #4V 1ke
[6375,6378], 6383, 6384, #6V 1ke
6380, 6386, #8V 1ke
6389, 6391, [6398,6400], 6402, 6405, 6407, 6409, #10V 1ke
[6754,6764], [6766,6769], #15V 1ke
[6777,6782], #20V 1ke
[6784,6795], #30V 1ke
[6797,6801], #40V 1ke
[6804,6807], #50V 1ke
]
'''
'''
#pathPrefix="/eos/atlas/atlascerngroupdisk/pixel-upgrade/itk/BeamTest/20221102.cern.itk/raw/run00"
runNumList=[
#7487,7488,[7490,7495],7497,7498,7521,7523,7524,7534,7536,7540,7542
#[7790,7798],[7800,7803]
]
pathPrefix='/eos/atlas/atlascerngroupdisk/det-itk/general/pixels/testbeam/20221102.cern.itk/raw/run00'#2022-11 data moved here
runNumList=[
#100V 2ke, 17 runs
7487,7488,[7490,7495],7497,7498,7521,7523,7524,7534,7536,7540,7542,
#100V 2ke, the rest of the runs
#7757,7758,[7760,7786],[7768,7770],
7757,7758,[7760,7766],[7768,7770],
7773,
7774,[7779,7787],
[7913,7916],7918,7919,[7921,7934],
#120V 2ke
[7866,7875],
#80V 2ke
[7790,7798],[7800,7803],
[7936,7947],
#60V 2ke
[7805,7809],[7811,7816],
#40V 2ke
[7818,7823],7826,7827,
#20V 2ke
[7829,7832],[7834,7836],
#80V 1.5ke
[7906,7911],
#100V 1.5ke
[7878,7885],[7887,7890],
#120V 1.5ke
[7892,7898],[7900,7902],
]
'''

'''
pathPrefix='/eos/atlas/atlascerngroupdisk/det-itk/general/pixels/testbeam/20221102.cern.itk/raw/run00'#2022-11 data moved here
runNumList=[
#120V 2ke
#[7866,7875],
#40V 2ke
#[7818,7823],7826,7827,
#60V 2ke
#[7806,7809],[7811,7813],[7815,7816],
#80V 2ke
#[7936,7947],
#[7790,7798],[7800,7803],
#120V 2ke
#[7867,7875],
#100V 1.5ke
#[7878,7902],
#20V 2ke
[7829,7832],[7834,7836],
#20V 2ke the rest
[7837,7839],[7841,7842],[7844,7848],[7851,7853],[7855,7858],[7860,7861]
]
'''

pathPrefix='/eos/atlas/atlascerngroupdisk/det-itk/general/pixels/testbeam/cern_2023_august2nd_itk/raw/run0'
runNumList=[
[11785, 11810],[11812,11815]
]

pathSuffix=".raw"
runs=[]
# decode the 2D list with single&continuous run numbers into 1 list
for i in range(len(runNumList)):
	if type(runNumList[i])==list:
		for j in range(runNumList[i][0],runNumList[i][1]+1):
			runs.append(j)
	else:
		runs.append(runNumList[i])
# print all run numbers with self-selected separation
#sep="\n"
sep=", "
for aRun in runs:
	print(str(aRun)+sep,end = '')
print("tot # of runs:",len(runs))

# assemble the full names with path
final_str=""
for i in range(len(runs)):
	if len(runs)!=1 and i!=len(runs)-1:
		final_str=final_str+'"'+pathPrefix+str(runs[i])+pathSuffix+'",'
	else:
		final_str=final_str+'"'+pathPrefix+str(runs[i])+pathSuffix+'"'
	#runs[i]=pathPrefix+str(runs[i])+pathSuffix
final_str="'"+final_str+"'"
print(final_str)
