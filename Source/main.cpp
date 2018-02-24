#include <iostream>
#include <string>
//#include <string_view>
#include <regex>
#include <vector>
#include <assert.h>
#include <memory>

enum class ImageType { JPEG, JPEG2000, BMP, UNKNOWN };
std::string ImageTypeToString(ImageType type)
{
	const std::string names[4] = {"JPEG", "JPEG2000", "BMP", "UNKNOWN"};
	return names[(int)type];
}
ImageType ImageTypeStringToEnum(std::string type)
{
	if (type == "J"   || type == "JPEG")	 return ImageType::JPEG;
	if (type == "JP2" || type == "JPEG2000") return ImageType::JPEG2000;
	if (type == "BMP")						 return ImageType::BMP;

	return ImageType::UNKNOWN;
}

class ImageInfo
{
public:
	ImageType type = ImageType::BMP;
	unsigned int width = 0;
	unsigned int height = 0;

public:
	~ImageInfo() = default;

	ImageInfo(ImageType imageType, unsigned int imageWidth, unsigned int imageHeight)
		: type{ imageType }, width{ imageWidth }, height{ imageHeight }
	{}

	ImageInfo(ImageType imageType, int imageWidth, int imageHeight)
		: type{ imageType }
	{
		assert(imageWidth >= 0 && imageHeight >= 0);
		width  = abs(imageWidth);
		height = abs(imageHeight);
	}

	unsigned int GetSizeInBytes() const
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

	std::string ToString() const
	{
		return ImageTypeToString(type) + "\t(" + std::to_string(width) + ", " + std::to_string(height) + ")px" + "\t" + std::to_string(GetSizeInBytes()) + " bytes";
	}

private:
	unsigned int CalculatePyramidSizeInBytes(unsigned int baseWidth, unsigned int baseHeight) const
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
	std::vector<unsigned int> imageInfoIds;
	//std::vector<ImageInfo> images;

public:
	ImageInfoStack(std::vector<unsigned int>& _imageInfoIds) : imageInfoIds{_imageInfoIds} {};
	~ImageInfoStack() = default;

	bool ContainsImageInfo(unsigned int id) const
	{
		return (std::find(imageInfoIds.begin(), imageInfoIds.end(), id) != imageInfoIds.end());
	}

	bool ContainsAnyImageInfo(std::vector<unsigned int>& ids) const
	{
		auto result = std::find_first_of(ids.begin(), ids.end(), imageInfoIds.begin(), imageInfoIds.end());
		return (result != ids.end());
	}

	unsigned int GetSizeInBytes(const std::vector<ImageInfo>& imageInfos) const
	{
		if (!ContainsImageInfo(imageInfos.size()))
		{
			throw std::invalid_argument("ImageInfoStack contains invalid references to ImageInfos.");
		}

		unsigned int totalSize = 0;
		for (const auto& id : imageInfoIds)
		{
			totalSize += imageInfos[id].GetSizeInBytes();
		}

		return totalSize;
	}

	std::string ToString() const
	{
		std::string output;
		for (auto v : imageInfoIds)
		{
			output += std::to_string(v) + " ";
		}

		//output += "\t\t" + std::to_string(GetSizeInBytes());

		return output;
	}
};

void CoutVector(const std::vector<std::string>& strings)
{
	std::cout << "[";

	if (strings.size() == 0)
	{
		std::cout << "]";
	}
	else
	{
		std::for_each(strings.begin(), strings.end() - 1, [](const std::string& element) {
			std::cout << element << ", ";
		});
		std::cout << strings.back() << "]";
	}
}

void CoutVector(const std::vector<unsigned int>& values)
{
	std::vector<std::string> strings;
	std::transform(values.begin(), values.end(), std::back_inserter(strings), [](const int& value) { return std::to_string(value); });

	CoutVector(strings);
}

void SplitStringUsingRegex(const std::string &str, std::vector<std::string>& tokens, const std::regex expression)
{
	std::sregex_token_iterator tokensBegin(str.begin(), str.end(), expression, -1);
	std::sregex_token_iterator tokensEnd;
	tokens.assign(tokensBegin, tokensEnd);
}

enum class InputCommand { NoInput, EndOfInput, AddImageGroup, AddImageType, Invalid, Successful };
InputCommand InterpretCommand(const std::string &command)
{
	if (command == "") return InputCommand::NoInput;

	std::vector<std::string> validCommands = {
		"Q",				// Quit (end of input)
		"G",				// Image Group (stack)
		"J", "JPEG",		// Image formats
		"JP2", "JPEG200",
		"BMP"
	};

	auto elementIter = std::find(validCommands.begin(), validCommands.end(), command);

	if      (elementIter == validCommands.end())	return InputCommand::Invalid;
	else if (*elementIter == validCommands[0])		return InputCommand::EndOfInput;
	else if (*elementIter == validCommands[1])		return InputCommand::AddImageGroup;
	else											return InputCommand::AddImageType;
}

// TODO: typedef parameters?
InputCommand AttemptAddImageInfoFromInput(const std::string& imageTypeStr, const std::vector<std::string>& parameters, std::vector<ImageInfo>& imageInfos)
{
	ImageType imageType = ImageTypeStringToEnum(imageTypeStr);

	if (imageType == ImageType::UNKNOWN)
	{
		std::cout << "\n The input [" << imageTypeStr << "] is an unknown image type. \n\n";
	}
	else if (parameters.size() < 2 || parameters.size() > 2)
	{
		std::cout << "\n Invalid image dimensions. Please type the command in this form: [" + imageTypeStr + " width height]\n\n";
	}
	else
	{
		int width = std::stoi(parameters[0]);
		int height = std::stoi(parameters[1]);

		if (width < 0 || height < 0)
		{
			std::cout << "\n Image dimensions must not be negative! \n\n";
		}
		else
		{
			imageInfos.push_back(ImageInfo(imageType, width, height));
			return InputCommand::Successful;
		}
	}

	return InputCommand::Invalid;
}

InputCommand AttemptAddImageGroupFromInput(const std::vector<std::string>& parameters, const std::vector<ImageInfo>& imageInfos, std::vector<ImageInfoStack>& imageGroups)
{
	// TODO:  An image should not be allowed to belong to multiple groups and 	//		(done by design) a group of images cannot be part of another group.
	if (parameters.size() == 0)
	{
		std::cout << "\n You must supply at least one image id to the image group: [G i, i, ...]\n\n";
	}
	else
	{
		std::vector<unsigned int> imageIds(0);

		// Extract ids from parameters
		for (const auto& param : parameters)
		{
			try 
			{
				int id = std::stoi(param);
				int arrayIndex = id - 1;
				if (arrayIndex < 0 || arrayIndex >= imageInfos.size())
				{
					std::cout << " " << param << " does not match any of the image indices. Try again.\n";
					return InputCommand::Invalid;
				}

				imageIds.push_back(abs(id));
			}
			catch (const std::invalid_argument& ia) 
			{
				std::cout << " " << param << " is not a valid parameter. Try again.\n";
				return InputCommand::Invalid;
			}
		}

		// Verify that images does not exist in other imageGroups
		for (const auto& imageGroup : imageGroups)
		{
			if (imageGroup.ContainsAnyImageInfo(imageIds))
			{
				std::cout << " One of the images "; 
				CoutVector(imageIds);
				std::cout << " already exists in another group. Try again.\n";
				return InputCommand::Invalid;
			}
		}

		imageGroups.push_back(ImageInfoStack(imageIds));
		return InputCommand::Successful;
	}

	return InputCommand::Invalid;
}

int main()
{
	std::cout << 
R"(	Storage calculator by Denny Lindberg

	Enter one line for each image/group using the formats: 
		"type width height"
		"G i, i, ..." 
		
		Exit with "Q"



)";
	std::vector<ImageInfo> images;
	std::vector<ImageInfoStack> imageGroups;

	std::string userInputStr;
	std::vector<std::string> inputTokens;

	InputCommand command = InputCommand::NoInput;
	while (command != InputCommand::EndOfInput)
	{
		// Request input
		std::cout << "Add image/group: ";
		std::getline(std::cin, userInputStr);

		// Extract command
		const std::regex matchWhitespaces("\\s+");
		SplitStringUsingRegex(userInputStr, inputTokens, matchWhitespaces);
		std::string commandstr = inputTokens[0];

		// Extract parameters
		std::vector<std::string> parameters;
		if (inputTokens.size() > 1) parameters.assign(inputTokens.begin() + 1, inputTokens.end());

		// Execute command
		command = InterpretCommand(commandstr);
		switch (command)
		{
		case InputCommand::NoInput:
			command = InputCommand::Successful;
			break;

		case InputCommand::Invalid:
			std::cout << "\n The input [" << commandstr << "] is not a valid command!\n\n";
			break;

		case InputCommand::AddImageGroup:
			command = AttemptAddImageGroupFromInput(parameters, images, imageGroups);
			break;

		case InputCommand::AddImageType:
			command = AttemptAddImageInfoFromInput(commandstr, parameters, images);
			break;
		}

		std::cout << "\n";

		if (command == InputCommand::Successful)
		{
			std::cout << "\tImages\n";
			if (images.size() == 0) std::cout << "\tNo images";

			int count = 1;
			for (const auto& image : images)
			{
				std::cout << "\t[" << count++ << "]\t" << image.ToString() << std::endl;
			}
			std::cout << "\n\n";

			std::cout << "\tImage Groups\n";
			if (imageGroups.size() == 0) std::cout << "\tNo image groups";
			for (const auto& group : imageGroups)
			{
				std::cout << "\tG\t" << group.ToString() << std::endl;
			}

			std::cout << "\n\n\n";
		}
	} 
	
	//std::getline(std::cin, userInput);

    return 0;
}
