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
## Methods

- GetText: Reads the contents of a file specified by the path into a dynamically allocated string and returns it.
- ClearText: Transforms the input string by converting all characters to lowercase and removing any non-alphabetic characters, except for spaces.
- ToMultiMap: Function processes a space-separated string of words, storing each unique word along with its count into dynamically allocated arrays. It maintains a list of words and their corresponding counts.
- BanList: Function filters out words from the given word list that are present in a ban list. It reads the ban list from a file, processes it, and updates the original word list to exclude banned words. It returns the filtered list of words and their counts.
- hash: Computes a hash index for a given string to place it into the hash table.
- createHashTable: Allocates and initializes a hash table with a specified capacity.
- insertOrUpdate: Adds a word to the hash table or updates its count if it already exists, using chaining for collision resolution.
- hashTableToArray: Converts the hash table entries into two parallel arrays: one for words and one for their counts.
- ToMap: Constructs a hash table from a list of words, updates the counts of each word, and then converts the hash table into arrays of unique words and their counts for further processing.
- SaveResult: Saves analysis results to two files: one file contains the percentage of unique words and the frequency of all words, while another file lists words with frequencies greater than 10%. It also reads and prints the content of the second file to the console, displaying the most used words or a message if none exceed the threshold.
