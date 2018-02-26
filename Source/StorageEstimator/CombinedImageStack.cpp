#pragma once

#include "CombinedImageStack.h"
#include "ImageVariants.h"

namespace StorageEstimator
{
	void CombinedImageStack::AddImage(Image::Type imageType, Image::Dimension width, Image::Dimension height)
	{
		images.push_back(Image::MakeSharedPtrByType(imageType, ++idCounter, imageType, width, height));
	}

	void CombinedImageStack::AddStack(std::vector<Image::Id>& imageIds)
	{
		Image::SharedPtrVector::iterator imageLocation;
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

	size_t CombinedImageStack::NumberOfImages() const
	{
		return idCounter;
	}

	size_t CombinedImageStack::NumberOfStacks() const
	{
		return imageStacks.size();
	}

	StorageSize CombinedImageStack::Size() const
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

	std::string CombinedImageStack::ToString() const
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

	bool CombinedImageStack::FindImageOutsideStacks(Image::Id id, Image::SharedPtrVector::iterator& imageLocation)
	{
		return Image::FindById(images, id, imageLocation);
	}

	bool CombinedImageStack::FindImageInStack(Image::Id id, Image::StackVector::iterator& parentStack, Image::SharedPtrVector::iterator& imageLocation)
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

	void CombinedImageStack::MoveImageToStack(Image::SharedPtrVector::iterator location, Image::Stack& stack)
	{
		stack.AddImage(*location);
		images.erase(location);
	}

	void CombinedImageStack::MoveImageBetweenStacks(Image::SharedPtrVector::iterator imageLocation, Image::Stack& sourceStackLocation, Image::Stack& targetStack)
	{
		targetStack.AddImage(*imageLocation);
		sourceStackLocation.RemoveImage(imageLocation);
	}
}
