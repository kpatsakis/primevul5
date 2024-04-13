static int rand_initialize(void)
{
	init_std_data(&input_pool);
	init_std_data(&blocking_pool);
	init_std_data(&nonblocking_pool);
	return 0;
}