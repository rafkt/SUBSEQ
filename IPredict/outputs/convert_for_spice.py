# This script will read the exports from CPT+ IPredict framewkork that are currently exported.
# What it will do is to covert them in a form that the SpiCe offline framework can read and do predictions




import sys
import math

print(sys.argv[1] + "." + sys.argv[2] +  "." + sys.argv[3])

sequence_count = 0
dataset = []
sequence = []
all_items = []
max_item_value = -1

f=open('SPICE' + str(sys.argv[1]) + '.fold.' + str(sys.argv[2]) + '.' + str(sys.argv[3]) + '.txt',"r")
if str(sys.argv[3]) == "training":
	for line in f:
	    for item in line.split(' '):
	    	if item != '':
	    		if (int(item) == 99999):
	    			sequence_count += 1
	    			dataset.append(sequence)
	    			sequence = []
	    		else:
	    			sequence.append(int(item))
	    			all_items.append(int(item))
	    			if (int(item) > max_item_value):
	    				max_item_value = int(item)
else: #will try to do read for queries
	for line in f:
		line = line.rstrip('\n')
		for item in line.split(' '):
			if item != '':
				sequence.append(int(item))
				all_items.append(int(item))
				if (int(item) > max_item_value):
					max_item_value = int(item)
		dataset.append(sequence)
		sequence = []
		sequence_count += 1

#print(sequence_count)
#print(len(set(all_items)))


o = open('SPICE' + str(sys.argv[1]) + '.fold.' + str(sys.argv[2]) + '.' + str(sys.argv[3]) + '.SPICE.txt', "w")

o.write(str(sequence_count) + " " + str(max_item_value + 1) + '\n')

for seq in dataset:
	o.write(str(len(seq)) + " " + " ".join(map(str, seq)) + '\n')