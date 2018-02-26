#pragma once

#include "Image.h"

namespace StorageEstimator
{
	class CombinedImageStack : public Image::Stack
	{
	private:
		Image::Id idCounter = 0;
		Image::SharedPtrVector images;
		Image::StackVector imageStacks;

	public:
		CombinedImageStack() = default;
		~CombinedImageStack()
		{}
	
		void AddImage(Image::Type imageType, Image::Dimension width, Image::Dimension height);
		void AddStack(std::vector<Image::Id>& imageIds);
		size_t NumberOfImages() const override;
		size_t NumberOfStacks() const;
		StorageSize Size() const override;
		std::string ToString() const override;

	private:
		bool FindImageOutsideStacks(Image::Id id, Image::SharedPtrVector::iterator& imageLocation);
		bool FindImageInStack(Image::Id id, Image::StackVector::iterator& parentStack, Image::SharedPtrVector::iterator& imageLocation);
		void MoveImageToStack(Image::SharedPtrVector::iterator location, Image::Stack& stack);
		void MoveImageBetweenStacks(Image::SharedPtrVector::iterator imageLocation, Image::Stack& sourceStackLocation, Image::Stack& targetStack);
	};
}
