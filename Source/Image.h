#pragma once

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
		if (type == "J" || type == "JPEG")		 return Image::JPEG;
		if (type == "JP2" || type == "JPEG2000") return Image::JPEG2000;
		if (type == "BMP")						 return Image::BMP;

		return Image::UNKNOWN;
	}

	std::vector<std::string> TypesToStringVector()
	{
		return { "JPEG", "JPEG2000", "BMP" };
	}

	std::string TypeToString(Image::Type type)
	{
		const std::vector<std::string> enumNames = TypesToStringVector();
		return enumNames[(int)type];
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

		Image::StorageSize GetSizeInBytes() const
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
			return "[" + std::to_string(id) + "]\t" + typeStr + padding + "\t(" + std::to_string(width) + ", " + std::to_string(height) + ")px" + "\t" + std::to_string(GetSizeInBytes()) + " bytes";
		}

	private:
		Image::StorageSize CalculatePyramidSizeInBytes() const
		{
			const int minimumDimension = 128;
			Image::Dimension pyramidWidth  = width;
			Image::Dimension pyramidHeight = height;

			Image::StorageSize totalSize = 0;
			do
			{
				switch (type)
				{
				case Image::JPEG:
					totalSize += (Image::StorageSize) (width * height * 0.2);
					break;

				case Image::BMP:
				default:
					totalSize += pyramidWidth * pyramidHeight;
					break;
				}

				pyramidWidth /= 2;
				pyramidHeight /= 2;
			} while (pyramidWidth >= minimumDimension && pyramidHeight >= minimumDimension);

			return totalSize;
		}
	};

	bool FindById(Image::Vector& images, Image::Id id, Image::Vector::iterator& imageLocation)
	{
		imageLocation = images.end();

		if (images.size() == 0) return false;

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


