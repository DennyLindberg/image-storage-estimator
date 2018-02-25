#pragma once

#include <vector>
#include <string>
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

		bool IsEmpty() const { return images.size() == 0;	}

		void AddImage(Image::Info& newImage)  { images.push_back(newImage); }

		void RemoveImage(Image::Vector::iterator imageLocation) 
		{ 
			images.erase(imageLocation); 
		}

		bool FindImage(Image::Id id, Image::Vector::iterator& imageLocation)
		{
			return Image::FindById(images, id, imageLocation);
		}

		Image::StorageSize StorageSize() const
		{
			Image::StorageSize totalSize = 0;
			for (const auto& image : images)
			{
				totalSize += image.StorageSize();
			}
		
			// Apply compression to stack according to requirements
			totalSize = (Image::StorageSize) (totalSize / log(images.size() + 3));
		
			return totalSize;
		}

		std::string ToString() const
		{
			std::string output;

			for (const auto& image : images)
			{
				output += "\t  " + image.ToString() + "\n";
			}

			output += "\t\t" + std::to_string(images.size()) + " images, compressed to " + Image::StorageSizeToString(StorageSize()) + " bytes\n";

			return output;
		}
	};
};