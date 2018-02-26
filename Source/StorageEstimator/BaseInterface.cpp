#include "BaseInterface.h"

namespace StorageEstimator
{
	std::string StorageSizeToString(StorageSize size)
	{
		// Formats value from 1234567 to string "1 234 567"

		std::string sizeStr(std::to_string(size));
		std::string output(sizeStr.size() + sizeStr.size() / 3, ' ');

		size_t outputIndex = output.size();
		size_t sizeStrIndex = sizeStr.size();
		size_t count = 0;
		while (outputIndex > 0 && sizeStrIndex > 0)
		{
			output[--outputIndex] = sizeStr[--sizeStrIndex];

			count++;
			if (count >= 3)
			{
				count = 0;
				--outputIndex;
			}
		}

		return output;
	}
}


