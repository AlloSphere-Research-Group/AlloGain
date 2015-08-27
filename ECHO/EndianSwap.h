#ifndef _endian_swap_h_
#define _endian_swap_h_

#if defined(JUCE_MAC) && defined(JUCE_LITTLE_ENDIAN)
void swapcopy32(uint32 *dest,uint32 *src,int bytes);
#endif

void swapblock32(uint32 *data,int bytes);

#endif