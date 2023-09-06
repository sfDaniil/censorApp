#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>
#include <unordered_map>
#include <chrono>


std::unordered_map<std::string, int> dictionaryHigh;
std::unordered_map<std::string, int> dictionaryMedium;
std::unordered_map<std::string, int> dictionaryLow;

auto fileSize = -1;
const int minimumWordSize = 3;

long long getCurrentTimeMillis() {
	auto currentTime = std::chrono::system_clock::now();
	auto millis = std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime);
	return millis.time_since_epoch().count();
}

void testWord(std::unordered_map<std::string, int>* dictionary, const std::string* word)
{
	if (!dictionary->empty())
	{
		if (1 == dictionary->count(*word))
		{
			(*dictionary)[*word] += 1;
		}
	}
}

void processWord(const std::string* word)
{
	testWord(&dictionaryHigh, word);
	testWord(&dictionaryMedium, word);
	testWord(&dictionaryLow, word);

	//std::cout << *word << std::endl;
}

// split the block into words with size from minimumWordSize, and pass it to the word handler
void processBlock(const std::vector<char>& vec) {
	static std::string wordInBlock = ""; // Temporary string to store the current word between 

	for (char c : vec) {
		if (c > 0)
			if (std::isprint(c) && !std::ispunct(c)) {
				wordInBlock += c; // Add a character to the current word
			}
			else
			{
				if (wordInBlock.length() > minimumWordSize)
				{
					processWord(&wordInBlock);
				}
				wordInBlock.clear();
			}
	}

	// Outputs the last word if the vector ends with a printable character
	// skip last word processWord(&word);
}

// memory loading of dictionaries files
bool loadDictionaryIntoMap(const std::string& filename, std::unordered_map<std::string, int>& dictionary) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Could not open the file: " << filename << std::endl;
		return false;
	}

	const int MAX_SIZE = 100; // maximum size of the line
	char line[MAX_SIZE]; // character array to store the line

	// Read lines from the file
	while (!file.eof()) {
		file.getline(line, MAX_SIZE);
		dictionary[line] = 0; // Загружаем слово в map с value 0
	}

	file.close();
	std::cout << filename << " - file loaded successfully, words: " << dictionary.size() << std::endl;
	return true;
}

void loadDicts()
{
	loadDictionaryIntoMap("dict-HIGH.txt", dictionaryHigh);
	loadDictionaryIntoMap("dict-MEDIUM.txt", dictionaryMedium);
	loadDictionaryIntoMap("dict-LOW.txt", dictionaryLow);
	std::cout << std::endl;
}

unsigned int  sumResultsByDict(const std::string& name, std::unordered_map<std::string, int>& dictionary, bool isNeedPrint)
{
	unsigned int wordsSum = 0;
	if (isNeedPrint) {
		std::cout << name << std::endl;
	}
	// Output dictionary contents for verification
	for (const auto& entry : dictionary) {
		if (isNeedPrint && entry.second > 0) {
			std::cout << entry.first << ": " << entry.second << std::endl;
		}
		wordsSum += entry.second;
	}
	if (isNeedPrint) {
		std::cout << std::endl;
	}
	return wordsSum;
}

int main(int argc, char* argv[]) {
	if (argc != 2)
	{
		std::cout << "arguments count = " << argc << std::endl;
		std::cout << "Wait file name in argument." << std::endl;
		exit(1);
	}

	// Load several dictionaries into the memory
	loadDicts();

	// Open the file	
	std::string fileName = argv[1];

	// try to open file
	std::ifstream file(fileName, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Could not open file " << fileName << std::endl << std::endl;
		return 1;
	}
	else
	{
		std::cout << "work with file " << fileName << std::endl;
		// get fileSize
		file.seekg(0, std::ios::end); // Move the file pointer to the end of the file
		std::streampos fileSizeCurrent = file.tellg(); // Get the position, which is the file size
		file.seekg(0, std::ios::beg);// Move the file pointer to the begin of the file
		fileSize = fileSizeCurrent;
		std::cout << "fileSize " << fileSize << std::endl;
	}

	const int blockSize = 1024;
	std::vector<char> block(blockSize);

	//process main work 
	long long currentTime = getCurrentTimeMillis();
	long long startTime = currentTime;
	long long lastPrintTime = currentTime;
	bool flagPrintTime = false;
	while (file.read(block.data(), blockSize)) {
		processBlock(block);
		// for big files - print current progress in %
		currentTime = getCurrentTimeMillis();
		if (currentTime - lastPrintTime > 1000)
		{
			std::streampos fileSizeCurrent = file.tellg(); // Get the position, which is the file size
			int percentOfFile = (100 * fileSizeCurrent) / fileSize;
			std::cout << percentOfFile << " % " << "complete" << std::endl;
			lastPrintTime = currentTime;
			flagPrintTime = true;
		}
	}
	if (flagPrintTime) {
		std::cout << "100 % " << "complete" << std::endl << std::endl;
	}

	// calculate  sum of wors in every dict
	bool printAllWords = true;
	unsigned int  sumWordsHigh = sumResultsByDict("High warning words count:", dictionaryHigh, printAllWords);
	unsigned int  sumWordsMedium = sumResultsByDict("Medium warning words count:", dictionaryMedium, printAllWords);
	unsigned int  sumWordsLow = sumResultsByDict("Low warning words count:", dictionaryLow, printAllWords);

	currentTime = getCurrentTimeMillis();
	std::cout << "workTime = " << currentTime - startTime << std::endl << std::endl;

	// print  sumary of search
	std::cout << std::endl << "Keyword search  sumary:" << std::endl;
	std::cout << "//Words are categorized into three dictionaries with different alert priorities." << std::endl;
	std::cout << " sumWordsHigh=" << sumWordsHigh << std::endl;
	std::cout << " sumWordsMedium=" << sumWordsMedium << std::endl;
	std::cout << " sumWordsLow=" << sumWordsLow << std::endl;


	file.close();
	return 0;
}
