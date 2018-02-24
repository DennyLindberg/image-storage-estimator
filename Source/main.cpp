#include <iostream>
#include <string>
//#include <string_view>
#include <regex>
#include <vector>
#include <assert.h>

enum class ImageType { JPEG, JPEG2000, BMP };
class ImageInfo
{
public:
	ImageType type = ImageType::BMP;
	unsigned int width = 0;
	unsigned int height = 0;

public:
	ImageInfo(ImageType imageType, unsigned int imageWidth, unsigned int imageHeight)
		: type{ imageType }, width{ imageWidth }, height{ imageHeight }
	{}

	~ImageInfo() = default;

	unsigned int GetSizeInBytes()
	{
		switch (type)
		{
		case ImageType::JPEG2000:
			return (unsigned int) (width * height * 0.4 / log(log(width * height + 16)));

		case ImageType::JPEG:
		case ImageType::BMP:
		default:
			return CalculatePyramidSizeInBytes(width, height);
		}
	}

private:
	unsigned int CalculatePyramidSizeInBytes(unsigned int baseWidth, unsigned int baseHeight)
	{
		const int minimumDimension = 128;
		unsigned int totalSize = 0;
		do
		{
			switch (type)
			{
			case ImageType::JPEG:
				totalSize += (unsigned int) (width * height * 0.2);
				break;

			case ImageType::BMP:
			default:
				totalSize += width * height;
				break;
			}

			baseWidth /= 2;
			baseHeight /= 2;
		} while (baseWidth >= minimumDimension && baseHeight >= minimumDimension);

		return totalSize;
	}
};

class ImageInfoStack
{
private:
	std::vector<ImageInfo> stack;

public:
	ImageInfoStack() = default;
	~ImageInfoStack() = default;

	void Push(ImageInfo& newImageInfo)
	{

	}
};

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

void SplitStringUsingRegex(const std::string &str, std::vector<std::string>& tokens, const std::regex expression)
{
	std::sregex_token_iterator tokensBegin(str.begin(), str.end(), expression, -1);
	std::sregex_token_iterator tokensEnd;
	tokens.assign(tokensBegin, tokensEnd);
}

bool ValidateCommand(const std::string &command)
{
	std::vector<std::string> validCommands = {
		"Q",				// Quit (end of input)
		"G",				// Image Group (stack)
		"J", "JPEG",		// Image formats
		"JP2", "JPEG200",
		"BMP"
	};

	bool isValidCommand = (std::find(validCommands.begin(), validCommands.end(), command) != validCommands.end());
	return isValidCommand;
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
	const std::regex matchWhitespaces("\\s+");
	while (true)
	{
		std::cout << "Add image/group: ";
		std::getline(std::cin, userInput);

		if (userInput.size() == 0)	continue;
		if (userInput == "Q")		break;

		SplitStringUsingRegex(userInput, inputTokens, matchWhitespaces);
		std::string command = inputTokens[0];

		if (!ValidateCommand(command))
		{
			std::cout << "\n The input [" << command << "] is not a valid command!\n\n";
		}
		else
		{

		}
		//images.push_back(userInput);
	}
	
	//std::getline(std::cin, userInput);

    return 0;
}
