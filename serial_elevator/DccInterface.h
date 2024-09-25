#ifndef DCC_INTERFACE_H
#define DCC_INTERFACE_H

#include <NmraDcc.h>

#define DCC_PIN         27
#define DECODER_ADDR_1 229
#define DECODER_ADDR_2 230
#define DECODER_ADDR_3 231
#define DECODER_ADDR_4 232
#define DECODER_ADDR_5 233
#define DECODER_ADDR_6 234
#define DECODER_ADDR_7 235
#define DECODER_ADDR_8 236
#define DECODER_ADDR_9 237
#define DECODER_ADDR_10 238
#define DECODER_ADDR_11 239
#define DECODER_ADDR_12 240

class DccInterface {
	public:
		void Init ();
		void Loop ();
	private:
		NmraDcc *MyDcc;
};

#endif