# Dictionary-based-Decompression-and-compression
a Simulator to compress and decompress given files using Run length encoding, direct match, Bitmask, consecutive or anywhere mismatch


Command Line and Input/Output Formats: The simulator should be executed with the following command line. Please use parameters “1” and “2” to indicate compression, and decompression, respectively.
./SIM 1 (or java SIM 1) for compression
./SIM 2 (or java SIM 2) for decompression
Please hardcode the input and output files as follows:
1. Input file for your compression function: original.txt
2. Output produced by your compression function: cout.txt
3. Input file for your decompression function: compressed.txt
4. Output produced by your decompression function: dout.txt

Please compile to produce an executable named SIM.
g++ -std=c++11 SIM_f.cpp –o SIM

Assume hardcoded input/output files as outlined in the project description.
o Compress the input file (original.txt) and check with the expected output (compressed.txt)
 ./SIM 1 (or java SIM 1)
 diff –w –B cout.txt compressed.txt
o Decompress the input file (compressed.txt) and check with the expected output (original.txt)
 ./SIM 2 (or java SIM 2)
 diff –w –B dout.txt original.txt
