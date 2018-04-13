#include "Fano.h"

Fano::Fano(std::string inputFilename, std::string outputFilename) {
	try {
		inputFile.open(inputFilename, std::ifstream::binary);
		inputFile.exceptions(std::ios::failbit | std::ios::badbit);
		outputFile.open(outputFilename, std::ofstream::binary);
		outputFile.exceptions(std::ios::failbit | std::ios::badbit);
	}
	catch (std::ios_base::failure&)
	{
		throw std::runtime_error("Exception opening file\n");
	}
	

	inputFile.seekg(0, std::ifstream::end);
	size = inputFile.tellg();
	inputFile.seekg(0, std::ifstream::beg);
	totalFrequency = 0;
}

Fano::~Fano() {
	inputFile.close();
	outputFile.close();
}

void Fano::encode() {
	countFrequencies();
	
	std::string emptyString;
	binaryTree(0, alphabetPower - 1, totalFrequency, emptyString);
	writeHeader();
	writeBody();
}

void Fano::decode() {
	readHeader();

	std::string emptyString;
	binaryTree(0, alphabetPower - 1, totalFrequency, emptyString);

	readBody();
}

void Fano::binaryTree(int leftLimit, int rightLimit, int totalNodeFrequency, std::string code)
{
	if (leftLimit == rightLimit) {
		fanoCodes[frequencies[leftLimit].symbol] = code;
		return;
	}
	else if (rightLimit - leftLimit == 1) {
		binaryTree(leftLimit, leftLimit, totalNodeFrequency, code + '0');
		binaryTree(rightLimit, rightLimit, totalNodeFrequency, code + '1');
	}
	else {
		int currentFrequency = 0;
		int splitPosition = leftLimit;
		for (int i = leftLimit; i <= rightLimit; ++i) {
			currentFrequency += frequencies[i].frequency;
			if ((currentFrequency) >= (totalNodeFrequency / 2)) {
				splitPosition = i;
				break;
			}
		}
		binaryTree(leftLimit, splitPosition, currentFrequency, code + '0');
		binaryTree(splitPosition + 1, rightLimit, totalNodeFrequency - currentFrequency, code + '1');
	}
}

void Fano::countFrequencies() {
	unsigned char inputChar;
	std::map <unsigned char, int> freqs;
	for (int i = 0; i < size; ++i) {
		inputChar = inputFile.get();
		freqs[inputChar]++;
	}

	alphabetPower = freqs.size();
	frequencies = new frequencyNode[alphabetPower];
	int freqsCount = 0;
	for (const auto& elem : freqs) {
		frequencies[freqsCount].symbol = elem.first;
		frequencies[freqsCount].frequency = elem.second;
		freqsCount++;
		totalFrequency += elem.second;
	}
	qsort(frequencies, alphabetPower, sizeof(frequencyNode), frequencyCompare);
	inputFile.seekg(0, std::ifstream::beg);
}

void Fano::writeHeader() {
	unsigned char byteOut = 0;
	for (int i = 0; i < 8; ++i) {
		if ((alphabetPower - 1) & (1 << i)) {
			setBit(byteOut, i);
		}
	}
	outputFile << byteOut;

	int temporaryVar, bitCount, difCount;
	for (int i = 0; i < alphabetPower; ++i) {
		byteOut = frequencies[i].symbol;
		outputFile << byteOut;
		temporaryVar = frequencies[i].frequency;
		bitCount = 0;
		byteOut = 0;
		difCount = 0;
		while (temporaryVar != 0) {
			if (temporaryVar & (1 << bitCount - difCount)) {
				setBit(byteOut, bitCount % 8);
				temporaryVar -= pow(2, bitCount - difCount);
			}
			
			++bitCount;
			if ((bitCount + 1) % 8 == 0) {
				if (temporaryVar != 0) setBit(byteOut, 7);
				outputFile << byteOut;
				byteOut = 0;
				++bitCount;
				++difCount;
			}
		}

		if (bitCount % 8 != 0) {
			outputFile << byteOut;
		}
	}
}

void Fano::writeBody() {
	unsigned char inputChar, outputChar = 0;
	int bitCount = 7;
	for (int i = 0; i < size; ++i) {
		inputChar = inputFile.get();
		for (int j = 0; j < fanoCodes[inputChar].size(); ++j) {
			if (bitCount == -1) {
				bitCount = 7;
				outputFile << outputChar;
				outputChar = 0;
			}
			if (fanoCodes[inputChar][j] == '1') {
				setBit(outputChar, bitCount);
			}
			--bitCount;
		}
	}
	if (bitCount != -1) {
		outputFile << outputChar;
	}
}

void Fano::readHeader() {
	unsigned char inputChar;
	inputChar = inputFile.get();
	for (int i = 0; i < 8; ++i) {
		if (checkBit(inputChar, i)) {
			alphabetPower += pow(2, i);
		}
	}
	++alphabetPower;

	frequencies = new frequencyNode[alphabetPower];
	bool nextFrequencyFlag;
	int frequencyBitCount, tempFreq;
	for (int i = 0; i < alphabetPower; ++i) {
		nextFrequencyFlag = false;
		inputChar = inputFile.get();
		frequencies[i].symbol = inputChar;
		frequencyBitCount = 0; tempFreq = 0;
		while (!nextFrequencyFlag) {
			inputChar = inputFile.get();
			for (int j = 0; j < 7; ++j) {
				if (checkBit(inputChar, j)) {
					tempFreq += pow(2, frequencyBitCount);
				}
				++frequencyBitCount;
			}
			if (!checkBit(inputChar, 7)) nextFrequencyFlag = true;
		}
		frequencies[i].frequency = tempFreq;
		totalFrequency += frequencies[i].frequency;
	}
}

void Fano::readBody() {
	std::map<std::string, unsigned char> decodesFano;
	std::map<std::string, unsigned char>::iterator check;
	for (const auto& elem : fanoCodes) {
		decodesFano[elem.second] = elem.first;
	}

	int totalBitCount = 0;
	for (int i = 0; i < alphabetPower; ++i) {
		totalBitCount += (fanoCodes[frequencies[i].symbol].size() * frequencies[i].frequency);
	}

	unsigned char inputChar;
	std::string currentCode;
	int bitCount = 0;
	for (int i = inputFile.tellg(); i < size; ++i) {
		inputChar = inputFile.get();
		for (int j = 7; j > -1; --j) {
			if (checkBit(inputChar, j)) {
				currentCode += '1';
			}
			else {
				currentCode += '0';
			}
			check = decodesFano.find(currentCode);
			if (check != decodesFano.end()) {
				outputFile << decodesFano[currentCode];
				currentCode.clear();
			}
			bitCount++;
			if (bitCount == totalBitCount) return;
		}
	}
}

bool Fano::checkBit(const unsigned char symbol, const int position) const {
	return symbol & (1 << position);
}

void Fano::setBit(unsigned char& symbol, const int position) const {
	symbol |= (1 << position);
}

void Fano::showUsage() {
	std::cout << "\nThe program is designed for zipping/unzipping files\n"
		<< "Example for encode:Fano.exe inputFilename outPutfilename\n"
		<< "Example for decode:Fano.exe inputFilename outPutfilename -d\n";
}