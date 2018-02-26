/*
	Copyright Denny Lindberg 2018
	www.dennylindberg.com
*/

#pragma once

#include "Image.h"

namespace StorageEstimator
{
	namespace Image
	{
		template<typename... Args>
		SharedPtr MakeSharedPtrByType(Image::Type type, Args... args)
		{
			// This function expects args to match the arguments for Image::Base for any created instance
			switch (type)
			{
			case Image::Type::JPEG:
				return std::make_shared<Image::JPEG>(args...);
			case Image::Type::JPEG2000:
				return std::make_shared<Image::JPEG2000>(args...);
			case Image::Type::BMP:
				return std::make_shared<Image::BMP>(args...);
			case Image::Type::UNKNOWN:
			default:
				throw std::invalid_argument("Unknown image type supplied to MakeSharedPtrByType");
			}
		}

		class BMP : public Image::AbstractPyramid
		{
		public:
			BMP(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
				: Image::AbstractPyramid(imageId, imageType, imageWidth, imageHeight)
			{}
			~BMP() = default;

			virtual StorageSize PyramidLevelSize(Image::Dimension width, Image::Dimension height) const override;
		};

		class JPEG : public Image::AbstractPyramid
		{
		public:
			JPEG(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
				: Image::AbstractPyramid(imageId, imageType, imageWidth, imageHeight)
			{}
			~JPEG() = default;

			virtual StorageSize PyramidLevelSize(Image::Dimension width, Image::Dimension height) const override;
		};

		class JPEG2000 : public Image::AbstractBase
		{
		public:
			JPEG2000(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
				: Image::AbstractBase(imageId, imageType, imageWidth, imageHeight)
			{}
			~JPEG2000() = default;

			virtual StorageSize Size() const override;
		};
	}
}
