#include "alg.h"

uint8_t Count_setbit(uint8_t num)
{
	uint8_t  count = 0;
	for (int i = 0; i < 8; i++)
	{
		if (num&0x01)
		{
			count++;
			//std::cout << "num = " << count << std::endl;
		}
		num >>= 1;
	}
	return count;
}
