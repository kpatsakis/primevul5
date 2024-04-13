void *aggetuserptr(Agraph_t * g, Agcbdisc_t * cbd)
{
    Agcbstack_t *stack_ent;

    for (stack_ent = g->clos->cb; stack_ent; stack_ent = stack_ent->prev)
	if (stack_ent->f == cbd)
	    return stack_ent->state;
    return NIL(void *);
}