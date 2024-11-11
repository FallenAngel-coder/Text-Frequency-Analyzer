# Frequency Analyzer

The Frequency Analyzer is a program written in C that analyzes text files to identify word frequency patterns. 

## Examples
- Basic ban list example
- Text example(~1600 words text about USA)
- Result file

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
   
   ./frequency_analyzer inputFile.txt banlist.txt result.txt result1.txt
   
   Where result - usage of all words and unique words in %, result1 - words with more than 10% usage
## Another way
   Just download FAnalyzer.exe, run it and input path of files in next order: inputFile.txt banlist.txt result.txt result1.txt
   
## Methods

- GetText: Reads the contents of a file specified by the path into a dynamically allocated string and returns it.
- ClearText: Transforms the input string by converting all characters to lowercase and removing any non-alphabetic characters, except for spaces.
- ToMultiMap: Function processes a space-separated string of words, storing each unique word along with its count into dynamically allocated arrays. It maintains a list of words and their corresponding counts.
- BanList: Function filters out words from the given word list that are present in a ban list. It reads the ban list from a file, processes it, and updates the original word list to exclude banned words. It returns the filtered list of words and their counts.
- SaveResult: Saves analysis results to two files: one file contains the percentage of unique words and the frequency of all words, while another file lists words with frequencies greater than 10%. It also reads and prints the content of the second file to the console, displaying the most used words or a message if none exceed the threshold.
