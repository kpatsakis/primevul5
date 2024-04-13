trace_eval_jmp_to_tail(union trace_eval_map_item *ptr)
{
	/* Return tail of array given the head */
	return ptr + ptr->head.length + 1;
}