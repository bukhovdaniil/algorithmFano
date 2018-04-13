#include "Fano.h"

int main(int argc, char* argv[])
{
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
	return 0;
}