static void get_tr_index(void *data, struct trace_array **ptr,
			 unsigned int *pindex)
{
	*pindex = *(unsigned char *)data;

	*ptr = container_of(data - *pindex, struct trace_array,
			    trace_flags_index);
}