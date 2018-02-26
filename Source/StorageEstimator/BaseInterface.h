#pragma once

#include <string>

typedef unsigned int StorageSize;

namespace StorageEstimator
{
	std::string StorageSizeToString(StorageSize size);

	class BaseInterface
	{
	public:
		BaseInterface() = default;
		~BaseInterface() = default;

		virtual StorageSize Size() const = 0;
		virtual std::string ToString() const = 0;
	};
}
