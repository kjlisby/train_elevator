#ifndef DCC_INTERFACE_H
#define DCC_INTERFACE_H

#include <NmraDcc.h>

#define DCC_PIN         34
#define DECODER_ADDR_1 229
#define DECODER_ADDR_2 230
#define DECODER_ADDR_3 231

class DccInterface {
	public:
		void Init ();
		void Loop ();
	private:
		NmraDcc *MyDcc;
};

#endif