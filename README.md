# Frequency Analyzer

The Frequency Analyzer is a program written in C that analyzes text files to identify word frequency patterns. 

## Features
- High efficiency 
- Ban list of words can be used
- Macro DEBUG, to check work time
- Low resource consumption

## How to install:

   git clone https://github.com/FallenAngel-coder/Text-Frequency-Analyzer.git
   
   cd Text-Frequency-Analyzer
## Usage:
   gcc -o frequency_analyzer frequency_analyzer.c -lm
   
   ./frequency_analyzer

   Input files: D:/file.txt; D:/banlist.txt
   
   Output files: D:/result.txt; D:/result1.txt
## Methods
- GetText
  Reads the contents of a file specified by the path into a dynamically allocated string and returns it.
