#pragma once

#include "Image.h"
#include "ImageStack.h"
#include "StringUtils.h"

typedef std::vector<class ImageStack> ImageStackVector;

class ImageStorageEstimator
{
private:
	ImageId idCounter = 0;
	ImageVector images;
	ImageStackVector imageStacks;

public:
	ImageStorageEstimator() = default;
	~ImageStorageEstimator() = default;
	
	void AddImage(ImageType imageType, ImageDimension width, ImageDimension height)
	{
		images.push_back(Image(++idCounter, imageType, width, height));
	}

	void AddStack(std::vector<ImageId>& imageIds)
	{
		ImageVector::iterator imageLocation;
		ImageStackVector::iterator imageStackLocation;

		ImageStack newStack;
		for (auto id : imageIds)
		{ 
			if (FindImageOutsideStacks(id, imageLocation))
			{
				MoveImageToStack(imageLocation, newStack);
			}
			else
			{
				if (FindImageInParentStack(id, imageStackLocation, imageLocation))
				{
					ImageStack& sourceStack = *imageStackLocation;
					MoveImageBetweenStacks(imageLocation, sourceStack, newStack);

					if (sourceStack.IsEmpty())
					{
						imageStacks.erase(imageStackLocation);
					}
				}
			}
		}

		imageStacks.push_back(newStack);
	}

	size_t NumberOfImages() const
	{
		return idCounter; // images can be hidden inside stacks, don't use vector size
	}

	size_t NumberOfStacks() const
	{
		return imageStacks.size();
	}

	std::string ToString() const
	{
		std::string outputString;

		outputString += "\tImages\n";
		if (NumberOfImages() == 0)
		{
			outputString += "\tNo images";
		}
		for (const auto& image : images)
		{
			outputString += "\t" + image.ToString() + "\n";
		}
		outputString += "\n\n";



		outputString += "\tImage Groups\n";
		if (NumberOfStacks() == 0)
		{
			outputString += "\tNo image groups";
		}
		for (const auto& stack : imageStacks)
		{
			outputString += stack.ToString() + "\n";
		}
		outputString += "\n\n";

		return outputString;
	}

private:
	bool FindImageOutsideStacks(ImageId id, ImageVector::iterator& imageLocation)
	{
		return Image::FindImageById(images, id, imageLocation);
	}

	bool FindImageInParentStack(ImageId id, ImageStackVector::iterator& parentStack, ImageVector::iterator& imageLocation)
	{
		parentStack = imageStacks.end();

		for (int index = 0; index<imageStacks.size(); ++index)
		{
			if (imageStacks[index].FindImage(id, imageLocation))
			{
				parentStack = imageStacks.begin() + index;
				return true;
			}
		}

		return false;
	}

	void MoveImageToStack(ImageVector::iterator location, ImageStack& stack)
	{
		stack.AddImage(*location);
		images.erase(location);
	}

	void MoveImageBetweenStacks(ImageVector::iterator imageLocation, ImageStack& sourceStackLocation, ImageStack& targetStack)
	{
		targetStack.AddImage(*imageLocation);
		sourceStackLocation.RemoveImage(imageLocation);
	}
};