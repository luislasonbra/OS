/******************************************************************************
 *	Kernel entry file
 *
 *		Author: Arvydas Sidorenko
 *****************************************************************************/

#include <libc.h>
#include <x86/i8259.h>
#include <x86/idt.h>
#include <x86/irq.h>


static char* logo =
"\
    ___              ____   ____\n\
   /   |_  _  __    / __ \\/ ___/\n\
  / /| |\\\\//||--\\\\ / / / /\\__ \\\n\
 / /_| | \\\\ |||_||/ /_/ /___/ /\n\
/_/  |_|//\\\\||__//\\____/\\____/\n";

struct boot_info {
	int mem_size;
	int krnl_size;
} __attribute__((__packed__));

static int screen_init()
{
	set_color(VID_CLR_LIGHT_BLUE, VID_CLR_WHITE);
	clear_screen();
	goto_xy(0, 0);
	puts(logo);
	return 0;
}

static int x86_init(struct boot_info *binfo)
{
	i8259_init();
	reg_irq(32, x86_i8253_irq_handle);
	install_idt();
	irq_enable();
	
	return binfo->mem_size;
}

/* Kernel entry point */
int kmain(struct boot_info binfo)
{
	/* keep interrupts disabled until handlers are present */
	irq_disable();

	/* set segment values */
	__asm__ __volatile__("movw $0x10, %%ax \n"
						 "movw %%ax, %%ds \n"
						 "movw %%ax, %%es \n"
						 "movw %%ax, %%fs \n"
						 "movw %%ax, %%gs \n"
						: : : "ax");

	screen_init();
	x86_init(&binfo);

	goto_xy(10,10);
	printf("Memory size: %dKb\n", binfo.mem_size);
	goto_xy(10,11);
	printf("Kernel size: %dKb\n", binfo.krnl_size);


	for (;;);
	return 0;
}
