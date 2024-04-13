static bool arg_type_is_alloc_mem_ptr(enum bpf_arg_type type)
{
	return type == ARG_PTR_TO_ALLOC_MEM ||
	       type == ARG_PTR_TO_ALLOC_MEM_OR_NULL;
}