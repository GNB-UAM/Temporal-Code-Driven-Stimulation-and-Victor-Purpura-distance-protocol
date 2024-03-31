### Victor Purpura and TCDS closed loop stimulation protocol

**Requirements:** None  
**Limitations:** None  


<!--start-->
Implementation of a real-time closed-loop stimulation protocol to study activity codes with flexibility using the Real-Time eXperiment Interface (RTXI). This new and easier-to-use implementation is based on the TCDS protocol and the Victor-Purpura distance.
<!--end-->

#### Input
1. input(0) - IV : Weakly electric fish voltage (V)

#### Output
1. output(0) - Now : Task startup time
2. output(1) - output : Stimulus enable flag
3. output(2) - bit detected : Bit detected within current time window

#### Parameters
1. Bin Time (ns) - Time window
2. Threshold (V) - Minimum voltage to detect a spike
3. Word length - Number of bits of the word
4. q - Relative sensitivity of the metric to the precise timing of the spikes
5. Limit - Victor Purpura distance threshold
6. Word - Word to detect

#### States

#### References

If you use this code, please reference:

1. A. Lareo, C. G. Forlim, R. D. Pinto, P. Varona, and F. B. Rodriguez. Temporal code-driven stimulation: definition and application to electric fish signaling. Frontiers in neuroinformatics, 10:41, 2016.

2. A. Lareo, C. G. Forlim, R. D. Pinto, P. Varona, and F. B. Rodríguez, “Analysis of electroreception with temporal code-driven stimulation” Lecture Notes in Computer Science (including subseries Lecture Notes in Artificial Intelligence and Lecture Notes in Bioinformatics), vol. 10305 LNCS, pp. 101–111, 2017

3. A. Ayala, A. Lareo, P. Varona, and F. B. Rodriguez. Closed-loop temporal code-driven stimulation implemented and tested using real-time experimental interface (rtxi). 30th Annual Computational Neuroscience Meeting: CNS*2021–Meeting Abstracts. J Comput Neurosci 49, 3–208: P153. DOI: 10.1007/s10827-021-00801-9., 2021

4. A. Ayala, A. Lareo, P. Varona, and F. B. Rodriguez. Closed-loop temporal code-driven stimulation implemented and tested using real-time experimental interface (rtxi). 30th Annual Computational Neuroscience Meeting: CNS*2021–Meeting Abstracts. J Comput Neurosci 49, 3–208: P153. DOI: 10.1007/s10827-021-00801-9., 2021
