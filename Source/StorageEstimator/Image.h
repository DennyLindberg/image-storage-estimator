#pragma once

#include "StorageEstimator.h"

#include <vector>
#include <string>
#include <memory>

/*
	Typedefs and procedures
*/
namespace StorageEstimator
{
	namespace Image
	{
		typedef unsigned int Id;
		typedef unsigned int Dimension;
		typedef std::shared_ptr<class Base> SharedPtr;
		typedef std::vector<Image::SharedPtr> SharedPtrVector;
		typedef std::vector<class Stack> StackVector;

		enum class Type { JPEG, JPEG2000, BMP, UNKNOWN };
		std::string TypeToString(Image::Type type);
		Image::Type TypeToEnum(std::string type);
		bool FindById(Image::SharedPtrVector& images, Image::Id id, Image::SharedPtrVector::iterator& imageLocation);
	}
}

/*
	Classes
*/
namespace StorageEstimator
{
	namespace Image
	{
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

			Image::Id Id() const;
			virtual StorageSize Size() const;
			virtual std::string ToString() const;
		};

		class Stack : public StorageEstimator::BaseClass
		{
		protected:
			Image::SharedPtrVector images;

		public:
			Stack() = default;
			~Stack() = default;

			bool IsEmpty() const;
			void AddImage(Image::SharedPtr newImage);
			void RemoveImage(Image::SharedPtrVector::iterator imageLocation);
			bool FindImage(Image::Id id, Image::SharedPtrVector::iterator& imageLocation);
			virtual size_t NumberOfImages() const;
			virtual StorageSize Size() const override;
			virtual std::string ToString() const override;
		};

		class Pyramid : public Image::Base
		{
		public:
			Pyramid(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
				: Image::Base(imageId, imageType, imageWidth, imageHeight)
			{}
			~Pyramid() = default;

			virtual StorageSize Size() const override final;
			virtual StorageSize PyramidLevelSize(Image::Dimension width, Image::Dimension height) const;
		};
	}
}
