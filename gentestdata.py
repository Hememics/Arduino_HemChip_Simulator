import sys

# This is from Carl. 

num_circuits = 81
num_samples = 1500

if len(sys.argv) < 2:
    print("Please supply a file name.")
    exit()

file = open(sys.argv[1], "wb")

if not file:
    print("Failed to open file!")
    exit()

for iSample in range(0, num_samples):
    for iCircuit in range(0, num_circuits):
        file.write(bytearray([(iCircuit + iSample) % 256]))

file.close()
