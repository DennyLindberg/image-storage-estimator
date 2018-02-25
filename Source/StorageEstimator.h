#pragma once

#include <vector>
#include <string>
#include <assert.h>

typedef unsigned int StorageSize;

namespace StorageEstimator
{
	std::string StorageSizeToString(StorageSize size)
	{
		// Formats value from 1234567 to string "1 234 567"

		std::string sizeStr(std::to_string(size));
		std::string output(sizeStr.size() + sizeStr.size() / 3, ' ');

		size_t outputIndex = output.size();
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

	class BaseClass
	{
	public:
		BaseClass() = default;
		~BaseClass() = default;

		virtual StorageSize Size() const = 0;
		virtual std::string ToString() const = 0;

	};

	namespace Image
	{
		typedef unsigned int Id;
		typedef unsigned int Dimension;
		typedef std::vector<class Image::Base> Vector;
		typedef std::vector<class Image::Stack> StackVector;


		enum Type { JPEG, JPEG2000, BMP, UNKNOWN };
		std::string TypeToString(Image::Type type)
		{
			const std::vector<std::string> enumNames = { "JPEG", "JPEG2000", "BMP" };
			return enumNames[(int)type];
		}

		Image::Type TypeToEnum(std::string type)
		{
			if (type == "JPEG")		return Image::Type::JPEG;
			if (type == "JPEG2000")	return Image::Type::JPEG2000;
			if (type == "BMP")		return Image::Type::BMP;

			return Image::Type::UNKNOWN;
		}

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

		class Base : public StorageEstimator::BaseClass
		{
		protected:
			Image::Type type = Image::Type::UNKNOWN;
			Image::Id id = 0;
			Image::Dimension width = 0;
			Image::Dimension height = 0;

		public:
			Base(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
				: id{ imageId }, type{ imageType }, width{ imageWidth }, height{ imageHeight }
			{}

			~Base() = default;

			virtual std::string ToString() const
			{
				std::string typeStr = Image::TypeToString(type);
				std::string padding(10 - typeStr.size(), ' ');
				return "[" + std::to_string(id) + "]\t" + typeStr + padding + "\t(" + std::to_string(width) + ", " + std::to_string(height) + ")px" + "\t" + StorageEstimator::StorageSizeToString(Size()) + " bytes";
			}
		};


		class Stack : public StorageEstimator::BaseClass
		{
		protected:
			Image::Vector images;

		public:
			bool IsEmpty() const { return images.size() == 0; }

			void AddImage(Image::Base& newImage) { images.push_back(newImage); }

			void RemoveImage(Image::Vector::iterator imageLocation)
			{
				images.erase(imageLocation);
			}

			bool FindImage(Image::Id id, Image::Vector::iterator& imageLocation)
			{
				return Image::FindById(images, id, imageLocation);
			}

			virtual StorageSize Size() const override
			{
				StorageSize totalSize = 0;
				for (const auto& image : images)
				{
					totalSize += image.Size();
				}

				// Apply compression to stack according to requirements
				totalSize = (StorageSize) (totalSize / log(images.size() + 3));

				return totalSize;
			}

			virtual std::string ToString() const
			{
				std::string output;

				for (const auto& image : images)
				{
					output += "\t  " + image.ToString() + "\n";
				}

				output += "\t\t" + std::to_string(images.size()) + " images, compressed to " + StorageEstimator::StorageSizeToString(Size()) + " bytes\n";

				return output;
			}
		};

		class Pyramid : public Image::Base
		{
		public:
			virtual StorageSize Size() const override final
			{
				const int minimumPyramidDimension = 128;

				Image::Dimension pyramidWidth = width;
				Image::Dimension pyramidHeight = height;
				StorageSize totalSize = 0;
				do
				{
					totalSize += PyramidLevelSize(pyramidWidth, pyramidHeight);
					pyramidWidth /= 2;
					pyramidHeight /= 2;
				} while (pyramidWidth >= minimumPyramidDimension && pyramidHeight >= minimumPyramidDimension);

				return totalSize;
			}

			virtual StorageSize PyramidLevelSize(Image::Dimension width, Image::Dimension height) const = 0;
		};

		class BMP : public Image::Pyramid
		{
		public:
			BMP() = default;
			~BMP() = default;

			virtual StorageSize PyramidLevelSize(Image::Dimension width, Image::Dimension height) const override
			{
				return width * height;
			}
		};

		class JPEG : public Image::Pyramid
		{
		public:
			JPEG() = default;
			~JPEG() = default;

			virtual StorageSize PyramidLevelSize(Image::Dimension width, Image::Dimension height) const override
			{
				return (StorageSize)(width * height * 0.2);
			}
		};

		class JPEG2000 : public Image::Base
		{
		public:
			JPEG2000() = default;
			~JPEG2000() = default;

			virtual StorageSize Size() const override
			{
				return (StorageSize)(width * height * 0.4 / log(log(width * height + 16)));
			}
		};

	}
}












