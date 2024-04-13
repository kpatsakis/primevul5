static void set_timer_rand_state(unsigned int irq,
				 struct timer_rand_state *state)
{
	irq_timer_state[irq] = state;
}