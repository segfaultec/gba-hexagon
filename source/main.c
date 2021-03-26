

#include <gba_base.h>
#include <gba_video.h>
#include <gba_interrupt.h>		// for interrupt handling
#include <gba_systemcalls.h>	// for VBlankIntrWait()
#include <gba_types.h>
#include <gba_input.h>

#include "input.h"
#include "game.h"

int main(void) {
//#
	irqInit();
	irqEnable(IRQ_VBLANK);

	game_init();

	while (1) {
		UpdateKeyDown();

		game_update();

		VBlankIntrWait();
	}
}