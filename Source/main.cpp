/*
	Copyright Denny Lindberg 2018
	www.dennylindberg.com
*/

#include <string>
#include <vector>
#include <regex>

#include "ConsoleUtils.h"
#include "StorageEstimator/CombinedImageStack.h"

using namespace StorageEstimator;

typedef std::vector<std::string> InputParameters;
enum class InputCommand { NoInput, EndProcess, AddImageStack, AddImageType, Unknown };
enum class InputResponse { Failed, Success };

void SplitStringToCommandAndParameters(const std::string& userInputStr, std::string& commandStr, InputParameters& parameters);
void SplitStringUsingRegex(const std::string& str, std::vector<std::string>& stringTokens, const std::regex expression);
InputCommand InterpretStringAsCommand(std::string command);
InputResponse AttemptToAddImageFromInput(const std::string& userInputImageTypeStr, const InputParameters& parameters, StorageEstimator::CombinedImageStack& storageEstimator);
InputResponse AttemptToAddImageStackFromInput(const InputParameters& parameters, StorageEstimator::CombinedImageStack& storageEstimator);

int main()
{
	std::cout << 
R"(######################################################################

	Storage calculator by Denny Lindberg

	Enter one line for each image/group using the formats: 
		"type width height"
		"G i, i, ..." 
		
		Exit with "Q"

######################################################################

)";

	StorageEstimator::CombinedImageStack storageEstimator;

	std::string userInputStr;
	std::string commandStr;
	InputParameters parameters;
	InputCommand command = InputCommand::NoInput;
	InputResponse response = InputResponse::Failed;

	while (command != InputCommand::EndProcess)
	{
		// Fetch input
		PrintLine("Add image/group: ");
		std::getline(std::cin, userInputStr);
		SplitStringToCommandAndParameters(userInputStr, commandStr, parameters);

		// Evaluate command
		command = InterpretStringAsCommand(commandStr);
		switch (command)
		{
		case InputCommand::NoInput:
		case InputCommand::EndProcess:
			response = InputResponse::Success;
			break;

		case InputCommand::AddImageStack:
			response = AttemptToAddImageStackFromInput(parameters, storageEstimator);
			break;

		case InputCommand::AddImageType:
			response = AttemptToAddImageFromInput(commandStr, parameters, storageEstimator);
			break;

		case InputCommand::Unknown:
		default:
			PrintWarning("The input [" + commandStr + "] is not a valid command.");
			response = InputResponse::Failed;
			break;
		}

		// Print updated contents
		if (response == InputResponse::Success && command != InputCommand::EndProcess)
		{
			PrintLine(storageEstimator.ToString());
		}
	} 

    return 0;
}

void SplitStringToCommandAndParameters(const std::string& userInputStr, std::string& commandStr, InputParameters& parameters)
{
	std::vector<std::string> inputStrTokens;
	SplitStringUsingRegex(userInputStr, inputStrTokens, std::regex("\\s+")); // match whitespaces
	
	// Get command in upper case form (allow both upper/lower case)
	commandStr = inputStrTokens[0];
	std::transform(commandStr.begin(), commandStr.end(), commandStr.begin(), ::toupper);

	if (inputStrTokens.size() > 1)
	{
		parameters.assign(inputStrTokens.begin() + 1, inputStrTokens.end());
	}
	else
	{
		parameters.clear();
	}
}

void SplitStringUsingRegex(const std::string& str, std::vector<std::string>& stringTokens, const std::regex expression)
{
	std::sregex_token_iterator tokensBegin(str.begin(), str.end(), expression, -1);
	std::sregex_token_iterator tokensEnd;
	stringTokens.assign(tokensBegin, tokensEnd);
}

InputCommand InterpretStringAsCommand(std::string command)
{
	if (command == "") return InputCommand::NoInput;

	std::vector<std::string> validCommands = {
		"Q",				// Quit (end of input)
		"G",				// Image Group (stack)
		"J", "JPG", "JPEG", // Image types
		"JP2", "JPEG2000",
		"BMP"
	};

	auto elementIter = std::find(validCommands.begin(), validCommands.end(), command);

	if (elementIter == validCommands.end())			return InputCommand::Unknown;
	else if (*elementIter == validCommands[0])		return InputCommand::EndProcess;
	else if (*elementIter == validCommands[1])		return InputCommand::AddImageStack;
	else											return InputCommand::AddImageType;
}

InputResponse AttemptToAddImageFromInput(const std::string& userInputImageTypeStr, const InputParameters& parameters, StorageEstimator::CombinedImageStack& storageEstimator)
{
	std::string internalImageType(userInputImageTypeStr);
	if (userInputImageTypeStr == "J" || userInputImageTypeStr == "JPG")
	{
		internalImageType = "JPEG";
	}
	else if (userInputImageTypeStr == "JP2")
	{
		internalImageType = "JPEG2000";
	}

	Image::Type imageType = Image::TypeToEnum(internalImageType);

	if (imageType == Image::Type::UNKNOWN)
	{
		PrintWarning("The input [" + userInputImageTypeStr + "] is an unknown image type.");
	}
	else if (parameters.size() != 2)
	{
		PrintWarning("Invalid image dimensions. Type the command in this form: [" + userInputImageTypeStr + " width height]");
	}
	else
	{
		int width = std::stoi(parameters[0]);
		int height = std::stoi(parameters[1]);

		if (width < 0 || height < 0)
		{
			PrintWarning("Image dimensions must have positive values!");
		}
		else
		{
			storageEstimator.AddImage(imageType, abs(width), abs(height));
			return InputResponse::Success;
		}
	}

	return InputResponse::Failed;
}

InputResponse AttemptToAddImageStackFromInput(const InputParameters& parameters, StorageEstimator::CombinedImageStack& storageEstimator)
{
	if (parameters.size() == 0)
	{
		PrintWarning("You must supply at least one image id to the image group: [G i, i, ...]");
		return InputResponse::Failed;
	}
	else
	{
		std::vector<Image::Id> imageIds(0);

		// Extract ids from parameters
		for (const auto& param : parameters)
		{
			try
			{
				int id = std::stoi(param);
				int arrayIndex = id - 1;
				if (arrayIndex < 0 || arrayIndex >= storageEstimator.NumberOfImages())
				{
					PrintWarning("" + param + " does not match any of the images.");
					return InputResponse::Failed;
				}

				imageIds.push_back(abs(id));
			}
			catch (...)
			{
				PrintWarning("'" + param + "' is not a valid parameter.");
				return InputResponse::Failed;
			}
		}

		if (imageIds.size() <= 1)
		{
			PrintWarning("You must add at least two images to a group.");
			return InputResponse::Failed;
		}
		else
		{
			storageEstimator.AddStack(imageIds);
			return InputResponse::Success;
		}
	}
}
