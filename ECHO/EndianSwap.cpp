#include "EndianSwap.h"

#if defined(JUCE_MAC) && defined(JUCE_LITTLE_ENDIAN)
void swapcopy32(uint32 *dest,uint32 *src,int bytes)
{
	while (bytes > 0)
	{
		*dest = ByteOrder::swap(*src);
		src++;
		dest++;
		bytes -= 4;
	}
}
#endif

void swapblock32(uint32 *data,int bytes)
{
	while (bytes > 0)
	{
		*data = ByteOrder::swap(*data);
		data++;
		bytes -= 4;
	}
}


