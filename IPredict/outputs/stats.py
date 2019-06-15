import sys
import math

print(sys.argv[1] + "." + sys.argv[2])

sequence_count = 0
dataset = []
average_sequence_length = 0
f=open(str(sys.argv[1]) + '.fold.' + str(sys.argv[2]) + '.training.txt',"r")
for line in f:
    for item in line.split(' '):
    	if item != '':
    		dataset.append(int(item))
    		if (int(item) == 99999):
    			sequence_count += 1
 #   	else:
 #   		print("non arithmetic character")
dataset_length = len(dataset)
average_sequence_length = dataset_length / sequence_count
print("Dataset length: " + str(dataset_length))
alphabet = set(dataset)
print("Alphabet Size: " + str(len(alphabet)))
print("Number of sequences: " + str(sequence_count))
print("Average Sequence Length: " + str(average_sequence_length))

queries_count = 0
query_length = 0
f=open(str(sys.argv[1]) + '.fold.' + str(sys.argv[2]) + '.queries.txt',"r")
for line in f:
	line = line.rstrip('\n')
#	print(line.split(' '))
	for item in line.split(' '):
		if item != '':
			query_length += 1
	queries_count += 1


query_length = query_length / queries_count
print("Number of Queries: " + str(queries_count))
print("Average Query Length: " + str(query_length))



bits_needed =  math.ceil(math.log(len(alphabet),2))
print("Bits Needed / Symbol: " + str(bits_needed))
binary_size = dataset_length * bits_needed
print("Binary Size (BITS): " + str(binary_size))
mega_bytes = binary_size * 1.25 * 10**-7
print ("Binary Size (MB): " + str(mega_bytes))

