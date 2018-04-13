/**
	@mainpage Algorithm Fano

	Here are the documentation for the program that implements the algorithm of Fano, with the interface at the command prompt.
*/


#ifndef FANO
#define FANO
#include <string>
#include <fstream>
#include <iostream>
#include <map>
/**
	@brief The structure is intended to describe the nodes of the list of frequencies
*/
struct frequencyNode {
	unsigned char symbol;
	int frequency;
};

/**
	@brief Function to compare two nodes
	@param left The first node to compare
	@param right The second node to compare
*/
static int frequencyCompare(const void *left, const void *right) {
	frequencyNode a = *(frequencyNode*)left;
	frequencyNode b = *(frequencyNode*)right;
	if (a.frequency < b.frequency) return 1;
	else if (a.frequency > b.frequency) return -1;
	return 0;
}

/**
	@class Fano
	@brief Class which implements a command line interface for the algorithm Fano

	Class Fano is used for building codes, Fano and further archiving/unzipping.

	Example:
	@code
	try {
		if (argc == 3) {
			Fano fileForEncode(argv[1], argv[2]);
			fileForEncode.encode();
		}
		else if (argc == 4) {
			std::string arg = argv[3];
			if (arg == "-d") {
				Fano fileForDecode(argv[1], argv[2]);
				fileForDecode.decode();
			}
			else {
				Fano::showUsage();
			}
		}
		else {
			Fano::showUsage();
		}
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}
	@endcode
*/

class Fano {
private:
	int size;
	int alphabetPower;
	int totalFrequency;
	frequencyNode *frequencies;
	std::map<unsigned char, std::string> fanoCodes;

	std::ifstream inputFile;
	std::ofstream outputFile;
public:
	/**
		@brief Creates a Fano and opens input and output file, calculates the size of the file
		@param inputFilename name of the input file
		@param outputFilename name of the output file
		@exception std::ios_base::failure parameter is opening error
	*/
	Fano(std::string inputFilename, std::string outputFilename);
	/**
		@brief Closes the files
	*/
	~Fano();
	/**
		@brief Encoded original file into outputFile
	*/
	void encode();
	/**
		@brief Decoded archived file into outputFile
	*/
	void decode();
	/**
		@brief Recursively building a binary tree codes Fano 
		@param leftLimit left border character group
		@param rightLimit right border character group
		@param totalNodeFrequency frequency of the current node
		@param code fano codes 
	*/
	void binaryTree(int leftLimit, int rightLimit, int totalNodeFrequency, std::string code);
	/**
		@brief Runs through the whole file and counts the frequency
	*/
	void countFrequencies();
	/**
		@brief The entry in the output file the table of frequencies
	*/
	void writeHeader();
	/**
		@brief Archiving by replacing the original codes to the codes Fano
	*/
	void writeBody();
	/**
		@brief Read frequency table from source file
	*/
	void readHeader();
	/**
		@brief Unzipping by replacing codes of Fano on the source
	*/
	void readBody();
	/**
		@brief Checks significant bit in the given position
		@param symbol Byte to check
		@param position Position of the bit
		@return True if the most significant bit is present at the specified point and false if missing
	*/
	bool checkBit(const unsigned char symbol, const int position) const;
	/**
		@brief Sets the specified bit set to 1
		@param symbol Byte to set
		@param position Position of the bit
	*/
	void setBit(unsigned char& symbol, const int position) const;
	/**
		@brief Help on using the program
	*/
	static void showUsage();
};

#endif