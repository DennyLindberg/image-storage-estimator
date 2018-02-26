/*
	Copyright Denny Lindberg 2018
	www.dennylindberg.com
*/

#include "Image.h"

/*
	Typedefs and procedures
*/
namespace StorageEstimator
{
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

		bool FindByIdInVector(Image::SharedPtrVector& images, Image::Id id, Image::SharedPtrVector::iterator& imageLocation)
		{
			imageLocation = images.end();

			for (int index = 0; index < images.size(); ++index)
			{
				if (images[index]->Id() == id)
				{
					imageLocation = (images.begin() + index);
					break;
				}
			}

			return (imageLocation != images.end());
		}
	}
}


/*
	Classes
*/
namespace StorageEstimator
{
	namespace Image
	{
		Image::Id AbstractBase::Id() const 
		{ 
			return id; 
		}

		std::string AbstractBase::ToString() const
		{
			std::string typeStr = Image::TypeToString(type);
			std::string padding(10 - typeStr.size(), ' ');
			return "[" + std::to_string(id) + "]\t" + typeStr + padding + "\t(" + std::to_string(width) + ", " + std::to_string(height) + ")px" + "\t" + StorageEstimator::StorageSizeToString(Size()) + " bytes";
		}
	}
}

namespace StorageEstimator
{
	namespace Image
	{
		StorageSize AbstractPyramid::Size() const
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
	}
}

namespace StorageEstimator
{
	namespace Image
	{
		bool Stack::IsEmpty() const 
		{ 
			return images.size() == 0; 
		}

		void Stack::AddImage(Image::SharedPtr newImage) 
		{ 
			images.push_back(newImage); 
		}

		void Stack::RemoveImage(Image::SharedPtrVector::iterator imageLocation)
		{
			images.erase(imageLocation);
		}

		bool Stack::FindImage(Image::Id id, Image::SharedPtrVector::iterator& imageLocation)
		{
			return Image::FindByIdInVector(images, id, imageLocation);
		}

		size_t Stack::NumberOfImages() const
		{
			return images.size();
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
	}
}
