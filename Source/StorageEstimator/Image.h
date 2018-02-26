#pragma once

#include "BaseInterface.h"

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
		typedef std::shared_ptr<class AbstractBase> SharedPtr;
		typedef std::vector<Image::SharedPtr> SharedPtrVector;
		typedef std::vector<class Stack> StackVector;

		enum class Type { JPEG, JPEG2000, BMP, UNKNOWN };
		std::string TypeToString(Image::Type type);
		Image::Type TypeToEnum(std::string type);
		bool FindByIdInVector(Image::SharedPtrVector& images, Image::Id id, Image::SharedPtrVector::iterator& imageLocation);
	}
}

/*
	Classes
*/
namespace StorageEstimator
{
	namespace Image
	{
		class AbstractBase : public StorageEstimator::BaseInterface
		{
		protected:
			Image::Type type = Image::Type::UNKNOWN;
			Image::Id id = 0;
			Image::Dimension width = 0;
			Image::Dimension height = 0;

		public:
			AbstractBase(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
				: id{ imageId }, type{ imageType }, width{ imageWidth }, height{ imageHeight }
			{}
			~AbstractBase() = default;

			Image::Id Id() const;
			virtual StorageSize Size() const = 0;
			virtual std::string ToString() const;
		};

		class AbstractPyramid : public Image::AbstractBase
		{
		public:
			AbstractPyramid(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
				: Image::AbstractBase(imageId, imageType, imageWidth, imageHeight)
			{}
			~AbstractPyramid() = default;

			virtual StorageSize Size() const override final;
			virtual StorageSize PyramidLevelSize(Image::Dimension width, Image::Dimension height) const = 0;
		};
		
		class Stack : public StorageEstimator::BaseInterface
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

	}
}
