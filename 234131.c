static struct timer_rand_state *get_timer_rand_state(unsigned int irq)
{
	return irq_timer_state[irq];
}