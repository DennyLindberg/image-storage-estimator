#pragma once

#include <vector>
#include "Image.h"

typedef unsigned int StackId;

class ImageStack
{
private:
	std::vector<Image> images;

public:
	ImageStack() = default;
	~ImageStack() = default;

	void AddImage(Image&& newImage)
	{
		images.push_back(newImage);
	}

	void AddImage(Image& newImage)
	{
		images.push_back(newImage);
	}

	void RemoveImage(ImageVector::iterator imageLocation)
	{
		images.erase(imageLocation);
	}

	bool FindImage(ImageId id, ImageVector::iterator& imageLocation)
	{
		return Image::FindImageById(images, id, imageLocation);
	}

	bool IsEmpty()
	{
		return images.size() == 0;
	}

	bool ContainsImage(const ImageId imageId) const
	{
		for (const auto& innerImage : images)
		{
			if (innerImage.id == imageId) return true;
		}

		return false;
	}

	bool ContainsImage(const Image& outerImage) const
	{
		for (const auto& innerImage : images)
		{
			if (innerImage.id == outerImage.id) return true;
		}

		return false;
	}

	bool ContainsAnyImage(const ImageVector& outerImages) const
	{
		for (const auto& outerImage : outerImages)
		{
			for (const auto& innerImage : images)
			{
				if (innerImage.id == outerImage.id) return true;
			}
		}

		return false;
	}

	unsigned int GetSizeInBytes() const
	{
		unsigned int totalSize = 0;
		for (const auto& image : images)
		{
			totalSize += image.GetSizeInBytes();
		}
		// TODO: Make sure this is correct
		return totalSize;
	}

	std::string ToString() const
	{
		std::string output;
		output += "\t(" + std::to_string(images.size()) + " images, " + std::to_string(GetSizeInBytes()) + " bytes)\n";

		for (const auto& image : images)
		{
			output += "\t\t" + image.ToString() + "\n";
		}

		return output;
	}
};