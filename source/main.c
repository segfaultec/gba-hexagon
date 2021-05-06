

#include <gba_base.h>
#include <gba_interrupt.h>		// for interrupt handling
#include <gba_systemcalls.h>	// for VBlankIntrWait()

#include "input.h"
#include "game.h"
#include "lcd_types.h"

static bool work_is_done = true;
const Color red = {31, 0, 0};


#define	REG_BASE	0x04000000
#define	REG_VCOUNT	*((vu16 *)(REG_BASE + 0x06))

void vcount() {
	BG_PALETTE[0] = red;
}

int main(void) {
//#
	irqInit();
	irqEnable(IRQ_VBLANK | IRQ_VCOUNT);
	//irqSet(IRQ_VCOUNT, vcount);

	game_init();

	while (1) {

		IntrWait(0, IRQ_VCOUNT); // VCOUNT

		game_vmain_update();

		VBlankIntrWait(); // VBLANK

		UpdateKeyDown();
		game_vblank_update();
	}
}