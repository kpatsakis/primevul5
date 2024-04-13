static void set_timer_rand_state(unsigned int irq,
				 struct timer_rand_state *state)
{
	struct irq_desc *desc;

	desc = irq_to_desc(irq);

	desc->timer_rand_state = state;
}