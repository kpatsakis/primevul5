static struct timer_rand_state *get_timer_rand_state(unsigned int irq)
{
	struct irq_desc *desc;

	desc = irq_to_desc(irq);

	return desc->timer_rand_state;
}