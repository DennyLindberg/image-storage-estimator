#include "ImageVariants.h"

namespace StorageEstimator
{
	namespace Image
	{
		StorageSize BMP::PyramidLevelSize(Image::Dimension width, Image::Dimension height) const
		{
			return width*height;
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
