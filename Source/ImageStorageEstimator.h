#pragma once

#include "Image.h"
#include "ImageStack.h"

typedef std::vector<Image::Stack> ImageStackVector;

class ImageStorageEstimator
{
private:
	Image::Id idCounter = 0;
	Image::Vector images;
	Image::StackVector imageStacks;

public:
	ImageStorageEstimator() = default;
	~ImageStorageEstimator() = default;
	
	void AddImage(Image::Type imageType, Image::Dimension width, Image::Dimension height)
	{
		images.push_back(Image::Info(++idCounter, imageType, width, height));
	}

	void AddStack(std::vector<Image::Id>& imageIds)
	{
		Image::Vector::iterator imageLocation;
		ImageStackVector::iterator imageStackLocation;

		Image::Stack newStack;
		for (auto id : imageIds)
		{ 
			if (FindImageOutsideStacks(id, imageLocation))
			{
				MoveImageToStack(imageLocation, newStack);
			}
			else
			{
				if (FindImageInStack(id, imageStackLocation, imageLocation))
				{
					Image::Stack& sourceStack = *imageStackLocation;
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
		return idCounter;
	}

	size_t NumberOfStacks() const
	{
		return imageStacks.size();
	}

	Image::StorageSize StorageSize() const
	{
		Image::StorageSize totalSize = 0;
		for (auto &image : images)
		{
			totalSize += image.StorageSize();
		}

		for (auto &stack : imageStacks)
		{
			totalSize += stack.StorageSize();
		}

		return totalSize;
	}

	std::string ToString() const
	{
		std::string outputString;

		if (images.size() == 0)
		{
			outputString += "\tNo images outside stacks\n";
		}
		else
		{
			for (const auto& image : images)
			{
				outputString += "\t" + image.ToString() + "\n";
			}
		}
		outputString += "\n";


		if (NumberOfStacks() == 0)
		{
			outputString += "\tNo image stacks\n";
		}
		else
		{
			for (const auto& stack : imageStacks)
			{
				outputString += "\tStack:\n" + stack.ToString() + "\n";
			}
		}
		outputString += "\n\tTotal Size: " + Image::StorageSizeToString(StorageSize()) + " bytes\n\n";

		return outputString;
	}

private:
	bool FindImageOutsideStacks(Image::Id id, Image::Vector::iterator& imageLocation)
	{
		return Image::FindById(images, id, imageLocation);
	}

	bool FindImageInStack(Image::Id id, Image::StackVector::iterator& parentStack, Image::Vector::iterator& imageLocation)
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

	void MoveImageToStack(Image::Vector::iterator location, Image::Stack& stack)
	{
		stack.AddImage(*location);
		images.erase(location);
	}

	void MoveImageBetweenStacks(Image::Vector::iterator imageLocation, Image::Stack& sourceStackLocation, Image::Stack& targetStack)
	{
		targetStack.AddImage(*imageLocation);
		sourceStackLocation.RemoveImage(imageLocation);
	}
};