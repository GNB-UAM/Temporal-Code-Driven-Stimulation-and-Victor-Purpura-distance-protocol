### Victor Purpura and TCDS closed loop stimulation protocol

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
