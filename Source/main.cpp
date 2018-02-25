#include <iostream>
#include <string>
#include <regex>
#include <vector>

#include "ImageStorageEstimator.h"

typedef std::vector<std::string> InputParameters;

enum class InputCommand { NoInput, EndOfInput, AddImageStack, AddImageType, Invalid, Successful };
InputCommand InterpretCommand(const std::string &command)
{
	if (command == "") return InputCommand::NoInput;

	std::vector<std::string> validCommands = {
		"Q",				// Quit (end of input)
		"G",				// Image Group (stack)
		"J", "JPEG",		// Image formats
		"JP2", "JPEG2000",
		"BMP"
	};
	// TODO: Make use of ENUM names
	auto elementIter = std::find(validCommands.begin(), validCommands.end(), command);

	if      (elementIter == validCommands.end())	return InputCommand::Invalid;
	else if (*elementIter == validCommands[0])		return InputCommand::EndOfInput;
	else if (*elementIter == validCommands[1])		return InputCommand::AddImageStack;
	else											return InputCommand::AddImageType;
}

InputCommand AttemptAddImageFromInput(const std::string& imageTypeStr, const InputParameters& parameters, ImageStorageEstimator& storageEstimator)
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
			storageEstimator.AddImage(imageType, abs(width), abs(height));
			return InputCommand::Successful;
		}
	}

	return InputCommand::Invalid;
}

InputCommand AttemptAddImageStackFromInput(const InputParameters& parameters, ImageStorageEstimator& storageEstimator)
{
	if (parameters.size() == 0)
	{
		std::cout << "\n You must supply at least one image id to the image group: [G i, i, ...]\n\n";
	}
	else
	{
		std::vector<ImageId> imageIds(0);

		// Extract ids from parameters
		// TODO: commas?
		for (const auto& param : parameters)
		{
			try 
			{
				int id = std::stoi(param);
				int arrayIndex = id - 1;
				if (arrayIndex < 0 || arrayIndex >= storageEstimator.NumberOfImages())
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
		//for (const auto& imageStack : imageStacks)
		//{
		//	for (const auto& id : imageIds)
		//	{
		//		if (imageStack.ContainsImage(images[id]))
		//		{
		//			std::cout << " The image [" + std::to_string(id) + "] already exists in another group. Try again.\n";
		//			return InputCommand::Invalid;
		//		}
		//	}
		//}

		//imageStacks.push_back(ImageStack(images, imageIds));

		storageEstimator.AddStack(imageIds);

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
	ImageStorageEstimator storageEstimator;

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
		InputParameters parameters;
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
			command = AttemptAddImageStackFromInput(parameters, storageEstimator);
			break;

		case InputCommand::AddImageType:
			command = AttemptAddImageFromInput(commandstr, parameters, storageEstimator);
			break;
		}

		std::cout << "\n";

		if (command == InputCommand::Successful)
		{
			std::cout << storageEstimator.ToString() + "\n";
		}
	} 
	
	//std::getline(std::cin, userInput);

    return 0;
}
