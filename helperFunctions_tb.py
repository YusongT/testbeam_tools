

def expand2Dlist(theLi, sep=", "):
	runs=[]
	# decode the 2D list with single&continuous run numbers into 1 list
	for i in range(len(theLi)):
		if type(theLi[i])==list:
			for j in range(theLi[i][0],theLi[i][1]+1):
				runs.append(j)
		else:
			runs.append(theLi[i])
	# print all run numbers with self-selected separation
	for aRun in runs:
		print(str(aRun)+sep,end = '') 
	print("tot # of runs:",len(runs))
	return runs

