#pragma once

#include <vector>
#include <string>
#include <assert.h>

typedef unsigned int StorageSize;

namespace StorageEstimator
{
	std::string StorageSizeToString(StorageSize size);

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
		typedef std::vector<class Base*> Vector;
		typedef std::vector<class Stack> StackVector;


		enum class Type { JPEG, JPEG2000, BMP, UNKNOWN };
		std::string TypeToString(Image::Type type);
		Image::Type TypeToEnum(std::string type);
		bool FindById(Image::Vector& images, Image::Id id, Image::Vector::iterator& imageLocation);

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
			Image::Vector images;

		public:
			Stack() = default;
			~Stack() = default;

			bool IsEmpty() const;

			void AddImage(Image::Base* newImage);

			void RemoveImage(Image::Vector::iterator imageLocation);

			bool FindImage(Image::Id id, Image::Vector::iterator& imageLocation);

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

		class BMP : virtual public Image::Pyramid
		{
		public:
			BMP(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
				: Image::Pyramid(imageId, imageType, imageWidth, imageHeight)
			{}
			~BMP() = default;
		};

		class JPEG : virtual public Image::Pyramid
		{
		public:
			JPEG(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
				: Image::Pyramid(imageId, imageType, imageWidth, imageHeight)
			{}
			~JPEG() = default;

			virtual StorageSize PyramidLevelSize(Image::Dimension width, Image::Dimension height) const override;
		};

		class JPEG2000 : public Image::Base
		{
		public:
			JPEG2000(Image::Id imageId, Image::Type imageType, Image::Dimension imageWidth, Image::Dimension imageHeight)
				: Image::Base(imageId, imageType, imageWidth, imageHeight)
			{}
			~JPEG2000() = default;

			virtual StorageSize Size() const override;
		};

	}
}












