#include <iostream>
#include <string>
//#include <string_view>
#include <regex>
#include <vector>

template <class T>
void PrintVector(const std::vector<T>& vec)
{
	std::cout << "[";

	if (vec.size() == 0)
	{
		std::cout << "]";
	}
	else
	{
		std::for_each(vec.begin(), vec.end() - 1, [](const T& element) {
			std::cout << element << ", ";
		});
		std::cout << vec.back() << "]";
	}
}

void TokenizeString(const std::string &str, std::vector<std::string>& tokens)
{
	const std::regex findWhitespaces("\\s+");
	std::sregex_token_iterator tokensIterator(str.begin(), str.end(), findWhitespaces, -1);
	tokens.assign(tokensIterator, std::sregex_token_iterator());
}

bool ValidateCommand(const std::string &command)
{
	std::vector<std::string> validCommands = {
		"Q",				// Quit (finish input)
		"G",				// Image Group (Stack)
		"J", "JPEG",		// Image formats
		"JP2", "JPEG200",
		"BMP"
	};

	bool isValidCommand = (std::find(validCommands.begin(), validCommands.end(), command) != validCommands.end());
	if (!isValidCommand)
	{
		std::cout << "\n The input [" << command << "] is not a valid command!\n\n";
	}

	return isValidCommand;
}

bool ValidateInput(const std::vector<std::string>& inputTokens)
{
	return (inputTokens.size() > 0) && ValidateCommand(inputTokens[0]);
}

int main()
{
	std::string userInput;
	std::vector<std::string> images;
	std::vector<std::string> imageGroups;

	std::cout << 
R"(Storage calculator by Denny Lindberg. 
Enter one line for each image/group using the formats: 
	"type width height"
	"G i, i, ..." 

Exit with "Q"

)";

	std::vector<std::string> inputTokens;
	while (true)
	{
		std::cout << "Add image/group: ";
		std::getline(std::cin, userInput);

		if (userInput == "Q") break;

		TokenizeString(userInput, inputTokens);
		if (ValidateInput(inputTokens))
		{
			images.push_back(userInput);
		}
	}
	
	std::getline(std::cin, userInput);

    return 0;
}
