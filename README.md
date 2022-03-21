### Principe

Boolean neural network are a set of boolean functions - 2 boolean inputs, 2 pow 4 = 16 possible logical doors per cells. 
Classical terimnology of neural networks still apply, so that we have a set of inputs, some hidden layers and some outputs. 
A demo representation of a boolean neural network can be found in [demo.bnn](/demo.bnn) file.

Boolean neural networks proviode some advantages :
-  boolean neural networks can be paralelized, simulated and trained at zero cost using FPGA and custom ICs; also this totally remove the wall between AI and hardware.
- genetic evolution fits well within 4-bits logical functions cells
- this provides a nice user-friendly hexadecimal representation of the network, also as boolean algebra still aply, QMC and other optimisation algos still apply

And some disavdagantes :
- training seems slower
- require an appropriate initial neural network shape

### Training Algorithm

- A population of random chromosomes (boolean functions) is choosen
- For each chromosomes that fail a test
	- A crossover point of two parents (including non-expressive genes) that succeded test are choosen randomly
		- if zero succeded regenerate the population (it may take a while to find a suitable initial population for multi-output neural network)
	- A child is created usig the union of parents cross-over, it also contains non-expressive parent genes

### About FPGA

This projet also come with an FPGA prototype built in logism (.circ). Current implementation is desgined as a 2D array of a cell that performs both routing and boolean operations. Cells are desgined to be chainable, thus not having identifier, and thus exandable with other circuits. The final goal is to build a dynamycally exandable waffer-sized FPGA chip that could host a neural network trained to reshap other neural networks.

### Current application project

- [ ] neural network trained to simulate a full-adder

### Future application project

- [ ] neural network that plays tik tak toe
- [ ] neural network that recognize handwritten digits
- [ ] neural network that reshap other networks
