# CSC-512-Compiler-Project-3-Detect-Integer-Overflow
Project 3: Detecting integer overflow

Use DrCCTProf to pinpoint integer overflows at runtime.
Remember DrCCTProf is available via GitHub
In this project, you will use DrCCTProf to analyze integer instructions. One can determine an integer instruction via its operator. We focus on two types of instructions: (1) arithmetic instruction (add, sub), and (2) left bit shifting instruciton (shl). We will add the necessary APIs to help you filter out necessary instructions for analysis. You will need to write the overflow check for each instruction of interest. The algorithm is described as follows.

For each instruction, we need to investigate the integer type (e.g., 64 bit, 32 bit, 16 bit, 8 bit). We can reason the source and target operands to identify the integer type. We will provide an API to provide the type (length) for each source and target operand. You need to think about an algorithm to figure out the type of the entire computation from the operands. There are some examples to help you understand the problem. You may need to cover other cases.

1. int sig_overflow(int a, int b) { return a + b; }
If we call this instruction as sig_overflow(1<<30, 1 << 30), an integer overflow can happen.
The assmebly code related to sig_overflow can be found as follows:

add %eax %ebx
The instruction performs the add computation: %eax holds a and %ebx holds b and %ebx = %eax + %ebx. Here you need to get the type of %eax and %ebx, both of which are 32-bit (4 bytes). Then you check the type of the target operand %ebx, which is 32-bit. We use target operand type to know the computation is based on 32-bit integer type. Finally, you use the algorithm below to check the overflow happens or not.

2. We have an expression a - b. The assembly code can be

sub %eax %ebx

The source operands are %eax and %ebx that represent a and b; the target operand is %ebx. Here we can also use the target operand to reason about the integer type of the computation, which is 32-bit. We then can use the following algorithm to evaluate the values to identify the overflow.

Algorithm: You will leverage the algorithm in Slide 12 in the integer overflow lecture file. Thus, you need to obtain the values of each source operands. There are three operand types as follow. You need to check their values and perform the comparison. For the left bit shifting instruction, you need to derive your own check for the overflow. In this project, you can assume each value is always a signed value.

Register
Memory
Immediate number
Output: The full calling context (printed with the context handle) in a text file showing that where integer overflow happens. This output should be similar to what you have done in project 0, but you don't need to count the occurrences. Moreover, integer overflow can happen in multiple contexts, which means integer overflows can happen at more than one location in a program.
Test: We will release a test program with multiple overflows. Make sure your code can pass the test. We will use more test cases for grading, especially changing the integer sizes (int64_t, int32_t, int16_t, int8_t).

Extra credits: Integer overflow is still an on-going research, so there are many open questions. I list some directions you can explore with some extra credits. We have some facilities (APIs) in DrCCTProf to help you complete these tasks. More details will be added.

Visualizing analysis in a GUI (5 points): DrCCTProf can generate output in a specific format that can be visualized using DrCCTProf View extesion in VSCode. You need to implement this and use VSCode to visualize the results. Besides the client file, you also need to give a screen snapshot of the VSCode that visualizes the analysis result. The details about how to support VSCode and visualize it will be added soon.
Using the overflow bit in the state register to detect (5 points) and compare (5 points) it with the approach implemented in this project (i.e., the algorithm in Slide 12). For the detection, you need to implement it. For the comparison, you need to provide a document to compare the results for different test cases you designed. A starting point for testing is designing overflows with both signed and unsigned integers to check which solution is better. Moreover, in the document, you should describe how you implement the overflow bit solution.
You can describe your own thoughts about improving the detection of integer overflows. Please document your thoughts and try to implement them. We will give extra credits (up to 10 points) according to your efforts and insights.
