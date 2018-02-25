#pragma once

#include <string>
#include <assert.h>

typedef unsigned int ImageId;
typedef unsigned int ImageDimension;
typedef std::vector<class Image> ImageVector;

enum class ImageType { JPEG, JPEG2000, BMP, UNKNOWN };
std::string ImageTypeToString(ImageType type)
{
	const std::string names[4] = { "JPEG", "JPEG2000", "BMP", "UNKNOWN" };
	return names[(int)type];
}
ImageType ImageTypeStringToEnum(std::string type)
{
	if (type == "J" || type == "JPEG")	 return ImageType::JPEG;
	if (type == "JP2" || type == "JPEG2000") return ImageType::JPEG2000;
	if (type == "BMP")						 return ImageType::BMP;

	return ImageType::UNKNOWN;
}


class Image
{
public:
	ImageId id = 0;
	ImageType type = ImageType::BMP;
	ImageDimension width = 0;
	ImageDimension height = 0;

public:
	~Image() = default;

	Image(ImageId imageId, ImageType imageType, ImageDimension imageWidth, ImageDimension imageHeight)
		: id{ imageId }, type {	imageType }, width{ imageWidth }, height{ imageHeight }
	{}

	Image(ImageId imageId, ImageType imageType, int imageWidth, int imageHeight)
		: id{ imageId }, type{ imageType }
	{
		assert(imageWidth >= 0 && imageHeight >= 0);
		width = abs(imageWidth);
		height = abs(imageHeight);
	}

	unsigned int GetSizeInBytes() const
	{
		switch (type)
		{
		case ImageType::JPEG2000:
			return (unsigned int)(width * height * 0.4 / log(log(width * height + 16)));

		case ImageType::JPEG:
		case ImageType::BMP:
		default:
			return CalculatePyramidSizeInBytes(width, height);
		}
	}

	std::string ToString() const
	{
		std::string typeStr = ImageTypeToString(type);
		std::string padding(10 - typeStr.size(), ' ');
		return "[" + std::to_string(id) + "]\t" + typeStr + padding + "\t(" + std::to_string(width) + ", " + std::to_string(height) + ")px" + "\t" + std::to_string(GetSizeInBytes()) + " bytes";
	}

public:
	static bool FindImageById(ImageVector& images, ImageId id, ImageVector::iterator& imageLocation)
	{
		if (images.size() == 0)
		{
			imageLocation = images.end();
			return false;
		}

		for (int index = 0; index<images.size(); ++index)
		{
			if (images[index].id == id)
			{
				imageLocation = (images.begin() + index);
			}
		}

		return (imageLocation != images.end());
	}

private:
	unsigned int CalculatePyramidSizeInBytes(ImageDimension baseWidth, ImageDimension baseHeight) const
	{
		const int minimumDimension = 128;
		unsigned int totalSize = 0;
		do
		{
			switch (type)
			{
			case ImageType::JPEG:
				totalSize += (unsigned int)(width * height * 0.2);
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
