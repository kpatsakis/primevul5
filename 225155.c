const char *v4l2_norm_to_name(v4l2_std_id id)
{
	u32 myid = id;
	int i;

	/* HACK: ppc32 architecture doesn't have __ucmpdi2 function to handle
	   64 bit comparisons. So, on that architecture, with some gcc
	   variants, compilation fails. Currently, the max value is 30bit wide.
	 */
	BUG_ON(myid != id);

	for (i = 0; standards[i].std; i++)
		if (myid == standards[i].std)
			break;
	return standards[i].descr;
}