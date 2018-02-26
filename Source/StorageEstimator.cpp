#include "StorageEstimator.h"

#include <vector>
#include <string>
#include <assert.h>

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

	namespace Image
	{
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


		bool FindById(Image::SharedPtrVector& images, Image::Id id, Image::SharedPtrVector::iterator& imageLocation)
		{
			imageLocation = images.end();

			for (int index = 0; index<images.size(); ++index)
			{
				if (images[index]->Id() == id)
				{
					imageLocation = (images.begin() + index);
				}
			}

			return (imageLocation != images.end());
		}





		Image::Id Base::Id() const { return id; }

		StorageSize Base::Size() const
		{
			return width * height;
		}

		std::string Base::ToString() const
		{
			std::string typeStr = Image::TypeToString(type);
			std::string padding(10 - typeStr.size(), ' ');
			return "[" + std::to_string(id) + "]\t" + typeStr + padding + "\t(" + std::to_string(width) + ", " + std::to_string(height) + ")px" + "\t" + StorageEstimator::StorageSizeToString(Size()) + " bytes";
		}





		bool Stack::IsEmpty() const { return images.size() == 0; }

		void Stack::AddImage(Image::SharedPtr newImage) { images.push_back(newImage); }

		void Stack::RemoveImage(Image::SharedPtrVector::iterator imageLocation)
		{
			images.erase(imageLocation);
		}

		bool Stack::FindImage(Image::Id id, Image::SharedPtrVector::iterator& imageLocation)
		{
			return Image::FindById(images, id, imageLocation);
		}

		StorageSize Stack::Size() const
		{
			StorageSize totalSize = 0;
			for (const auto& image : images)
			{
				totalSize += image->Size();
			}

			// Apply compression to stack according to requirements
			totalSize = (StorageSize)(totalSize / log(images.size() + 3));

			return totalSize;
		}

		std::string Stack::ToString() const
		{
			std::string output;

			for (const auto& image : images)
			{
				output += "\t  " + image->ToString() + "\n";
			}

			output += "\t\t" + std::to_string(images.size()) + " images, compressed to " + StorageEstimator::StorageSizeToString(Size()) + " bytes\n";

			return output;
		}





		StorageSize Pyramid::Size() const
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

		StorageSize Pyramid::PyramidLevelSize(Image::Dimension width, Image::Dimension height) const
		{
			return width * height;
		}





		StorageSize JPEG::PyramidLevelSize(Image::Dimension width, Image::Dimension height) const
		{
			return (StorageSize)(width * height * 0.2);
		}





		StorageSize JPEG2000::Size() const
		{
			return (StorageSize)(width * height * 0.4 / log(log(width * height + 16)));
		}
	}
}


