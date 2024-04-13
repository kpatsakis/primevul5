static int __init nonx_setup(char *str)
{
	if (!str)
		return -EINVAL;
	if (!strncmp(str, "on", 2)) {
                __supported_pte_mask |= _PAGE_NX; 
 		do_not_nx = 0; 
	} else if (!strncmp(str, "off", 3)) {
		do_not_nx = 1;
		__supported_pte_mask &= ~_PAGE_NX;
        }
	return 0;
} 