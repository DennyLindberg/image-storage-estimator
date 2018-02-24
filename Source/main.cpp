#include <iostream>
#include <string>
//#include <string_view>
#include <regex>
#include <vector>
#include <assert.h>
#include <memory>


// TODO: Typedef imageid
typedef std::vector<class Image> ImageVector;
typedef std::vector<class ImageStack> ImageStackVector;

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

class Image
{
public:
	ImageType type = ImageType::BMP;
	unsigned int width = 0;
	unsigned int height = 0;

public:
	~Image() = default;

	Image(ImageType imageType, unsigned int imageWidth, unsigned int imageHeight)
		: type { imageType }, width{ imageWidth }, height{ imageHeight }
	{}

	Image(ImageType imageType, int imageWidth, int imageHeight)
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

class ImageStack
{
private:
	std::vector<const Image*> imagePtrs;

public:
	ImageStack(const ImageVector& _images, std::vector<unsigned int>& _ImageIds)
	{
		for (auto& id : _ImageIds)
		{
			imagePtrs.push_back(&_images[id-1]); // TODO: Fix code or input so that IDs are displayed from 1...?
		}
	};

	~ImageStack() = default;

	bool ContainsImage(const Image& image) const
	{
		for (const auto& imagePtr : imagePtrs)
		{
			if (imagePtr == &image) return true;
		}
		
		return false;
	}

	bool ContainsAnyImage(const ImageVector& images) const
	{
		for (const auto& image : images)
		{
			if (std::find(imagePtrs.begin(), imagePtrs.end(), &image) != imagePtrs.end())
			{
				return true;
			}
		}

		return false;
	}

	unsigned int GetSizeInBytes() const
	{
		unsigned int totalSize = 0;
		for (const auto& imagePtr : imagePtrs)
		{
			if (imagePtr)
			{
				totalSize += imagePtr->GetSizeInBytes();
			}
		}
		// TODO: Make sure this is correct
		return totalSize;
	}

	std::string ToString() const
	{
		std::string output;
		output += "\t(" + std::to_string(imagePtrs.size()) + " images, " + std::to_string(GetSizeInBytes()) + " bytes)\n";

		for (const auto& imagePtr : imagePtrs)
		{
			if (imagePtr)
			{
				output += "\t\t" + imagePtr->ToString() + "\n";
			}
		}

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

enum class InputCommand { NoInput, EndOfInput, AddImageStack, AddImageType, Invalid, Successful };
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
	else if (*elementIter == validCommands[1])		return InputCommand::AddImageStack;
	else											return InputCommand::AddImageType;
}

// TODO: typedef parameters?
InputCommand AttemptAddImageFromInput(const std::string& imageTypeStr, const std::vector<std::string>& parameters, ImageVector& Images)
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
			Images.push_back(Image(imageType, width, height));
			return InputCommand::Successful;
		}
	}

	return InputCommand::Invalid;
}

InputCommand AttemptAddImageStackFromInput(const std::vector<std::string>& parameters, const ImageVector& images, ImageStackVector& imageStacks)
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
				if (arrayIndex < 0 || arrayIndex >= images.size())
				{
					std::cout << " " << param << " does not match any of the image indices. Try again.\n";
					return InputCommand::Invalid;
				}

				imageIds.push_back(abs(id));
			}
			catch (...)
			{
				std::cout << " " << param << " is not a valid parameter. Try again.\n";
				return InputCommand::Invalid;
			}
		}

		// Verify that images does not exist in other stacks
		for (const auto& imageStack : imageStacks)
		{
			for (const auto& id : imageIds)
			{
				if (imageStack.ContainsImage(images[id]))
				{
					std::cout << " The image [" + std::to_string(id) + "] already exists in another group. Try again.\n";
					return InputCommand::Invalid;
				}
			}
		}

		imageStacks.push_back(ImageStack(images, imageIds));
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
	ImageVector images;
	ImageStackVector stacks;

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

		case InputCommand::AddImageStack:
			command = AttemptAddImageStackFromInput(parameters, images, stacks);
			break;

		case InputCommand::AddImageType:
			command = AttemptAddImageFromInput(commandstr, parameters, images);
			break;
		}

		std::cout << "\n";

		if (command == InputCommand::Successful)
		{
			std::cout << "\tImages\n";
			if (images.size() == 0) std::cout << "\tNo images";

			int counter = 0;
			for (const auto& image : images)
			{
				std::cout << "\t[" << ++counter << "]\t" << image.ToString() << std::endl;
			}
			std::cout << "\n\n";

			std::cout << "\tImage Groups\n";
			if (stacks.size() == 0) std::cout << "\tNo image groups";
			for (const auto& stack : stacks)
			{
				std::cout << stack.ToString() << std::endl;
			}

			std::cout << "\n\n\n";
		}
	} 
	
	//std::getline(std::cin, userInput);

    return 0;
}
