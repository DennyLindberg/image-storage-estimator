#pragma once

#include "StorageEstimator.h"

using namespace StorageEstimator;

class ImageStorageEstimator
{
private:
	Image::Id idCounter = 0;
	Image::PointerVector images;
	Image::StackVector imageStacks;

public:
	ImageStorageEstimator() = default;
	~ImageStorageEstimator()
	{
	/*	for (auto p : images)
		{
			delete p;
		}*/
	}
	
	void AddImage(Image::Type imageType, Image::Dimension width, Image::Dimension height)
	{
		switch (imageType)
		{
		case Image::Type::BMP:
			images.push_back(new Image::BMP(++idCounter, imageType, width, height));
			break;
		case Image::Type::JPEG:
			images.push_back(new Image::JPEG(++idCounter, imageType, width, height));
			break;
		case Image::Type::JPEG2000:
			images.push_back(new Image::JPEG2000(++idCounter, imageType, width, height));
			break;

		case Image::Type::UNKNOWN:
		default:
			throw std::invalid_argument("Unknown image type supplied to AddImage");
		}
	}

	void AddStack(std::vector<Image::Id>& imageIds)
	{
		Image::PointerVector::iterator imageLocation;
		Image::StackVector::iterator imageStackLocation;

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

	StorageSize Size() const
	{
		StorageSize totalSize = 0;
		for (auto &image : images)
		{
			totalSize += image->Size();
		}

		for (auto &stack : imageStacks)
		{
			totalSize += stack.Size();
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
				outputString += "\t" + image->ToString() + "\n";
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
		outputString += "\n\tTotal Size: " + StorageSizeToString(Size()) + " bytes\n\n";

		return outputString;
	}

private:
	bool FindImageOutsideStacks(Image::Id id, Image::PointerVector::iterator& imageLocation)
	{
		return Image::FindById(images, id, imageLocation);
	}

	bool FindImageInStack(Image::Id id, Image::StackVector::iterator& parentStack, Image::PointerVector::iterator& imageLocation)
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

	void MoveImageToStack(Image::PointerVector::iterator location, Image::Stack& stack)
	{
		stack.AddImage(*location);
		images.erase(location);
	}

	void MoveImageBetweenStacks(Image::PointerVector::iterator imageLocation, Image::Stack& sourceStackLocation, Image::Stack& targetStack)
	{
		targetStack.AddImage(*imageLocation);
		sourceStackLocation.RemoveImage(imageLocation);
	}
};