void add_interrupt_randomness(int irq)
{
	struct timer_rand_state *state;

	state = get_timer_rand_state(irq);

	if (state == NULL)
		return;

	DEBUG_ENT("irq event %d\n", irq);
	add_timer_randomness(state, 0x100 + irq);
}