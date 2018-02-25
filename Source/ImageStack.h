#pragma once

#include <vector>
#include "Image.h"

namespace Image
{
	typedef std::vector<class Stack> StackVector;

	class Stack
	{
	private:
		std::vector<Image::Info> images;

	public:
		Stack() = default;
		~Stack() = default;

		void AddImage(Image::Info&& newImage) { images.push_back(newImage); }
		void AddImage(Image::Info& newImage)  { images.push_back(newImage); }

		void RemoveImage(Image::Vector::iterator imageLocation) 
		{ 
			images.erase(imageLocation); 
		}

		bool FindImage(Image::Id id, Image::Vector::iterator& imageLocation)
		{
			return Image::FindById(images, id, imageLocation);
		}

		bool IsEmpty() const { return images.size() == 0;	}

		Image::StorageSize GetSizeInBytes() const
		{
			Image::StorageSize totalSize = 0;
			for (const auto& image : images)
			{
				totalSize += image.GetSizeInBytes();
			}
		
			// Apply compression to stack according to requirements
			totalSize = (Image::StorageSize) (totalSize / log(images.size() + 3));
		
			return totalSize;
		}

		std::string ToString() const
		{
			std::string output;
			output += "\t(" + std::to_string(images.size()) + " images, compressed to " + Image::StorageSizeToString(GetSizeInBytes()) + " bytes)\n";

			for (const auto& image : images)
			{
				output += "\t\t" + image.ToString() + "\n";
			}

			return output;
		}
	};
};