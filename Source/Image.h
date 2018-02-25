#pragma once

#include <vector>
#include <string>
#include <assert.h>

namespace Image
{
	typedef unsigned int Id;
	typedef unsigned int Dimension;
	typedef unsigned int StorageSize;
	typedef std::vector<class Info> Vector;

	enum Type { JPEG, JPEG2000, BMP, UNKNOWN };

	Image::Type TypeToEnum(std::string type)
	{
		if (type == "JPEG")		return Image::JPEG;
		if (type == "JPEG2000")	return Image::JPEG2000;
		if (type == "BMP")		return Image::BMP;

		return Image::UNKNOWN;
	}

	std::string TypeToString(Image::Type type)
	{
		const std::vector<std::string> enumNames = { "JPEG", "JPEG2000", "BMP" };
		return enumNames[(int)type];
	}

	std::string StorageSizeToString(Image::StorageSize size)
	{
		// Formats value from 1234567 to string "1 234 567"

		std::string sizeStr(std::to_string(size));
		std::string output(sizeStr.size() + sizeStr.size() / 3, ' ');

		size_t outputIndex  = output.size();
		size_t sizeStrIndex = sizeStr.size();
		size_t count = 0;
		while (outputIndex > 0 && sizeStrIndex > 0)
		{
			output[--outputIndex] = sizeStr[--sizeStrIndex];

			count++;
			if (count >= 3)
			{
				count = 0;
				--outputIndex;
			}
		}

		return output;
	}

	class Info
	{
	public:
		Image::Id id = 0;
		Image::Type type = Image::BMP;
		Image::Dimension width = 0;
		Image::Dimension height = 0;

	public:
		~Info() = default;

		Info(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
			: id{ imageId }, type {	imageType }, width{ imageWidth }, height{ imageHeight }
		{}

		Info(Image::Id imageId, Image::Type imageType, int imageWidth, int imageHeight)
			: id{ imageId }, type{ imageType }
		{
			assert(imageWidth >= 0 && imageHeight >= 0);
			width = abs(imageWidth);
			height = abs(imageHeight);
		}

		Image::StorageSize StorageSize() const
		{
			switch (type)
			{
			case Image::JPEG2000:
				return (Image::StorageSize) (width * height * 0.4 / log(log(width * height + 16)));

			case Image::JPEG:
			case Image::BMP:
			default:
				return CalculatePyramidSizeInBytes();
			}
		}

		std::string ToString() const
		{
			std::string typeStr = Image::TypeToString(type);
			std::string padding(10 - typeStr.size(), ' ');
			return "[" + std::to_string(id) + "]\t" + typeStr + padding + "\t(" + std::to_string(width) + ", " + std::to_string(height) + ")px" + "\t" + Image::StorageSizeToString(StorageSize()) + " bytes";
		}

	private:
		Image::StorageSize CalculatePyramidSizeInBytes() const
		{
			assert(type == Image::JPEG || type == Image::BMP);
			double compressionFactor = (type == Image::JPEG) ? 0.2 : 1.0;

			const int minimumPyramidDimension = 128;

			Image::Dimension pyramidWidth  = width;
			Image::Dimension pyramidHeight = height;
			Image::StorageSize totalSize = 0;
			do
			{
				totalSize += (Image::StorageSize) (pyramidWidth * pyramidHeight * compressionFactor);
				pyramidWidth /= 2;
				pyramidHeight /= 2;
			} while (pyramidWidth >= minimumPyramidDimension && pyramidHeight >= minimumPyramidDimension);

			return totalSize;
		}
	};

	bool FindById(Image::Vector& images, Image::Id id, Image::Vector::iterator& imageLocation)
	{
		imageLocation = images.end();

		for (int index = 0; index<images.size(); ++index)
		{
			if (images[index].id == id)
			{
				imageLocation = (images.begin() + index);
			}
		}

		return (imageLocation != images.end());
	}
}


