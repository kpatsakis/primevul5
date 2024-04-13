static int __init init_em86_binfmt(void)
{
	register_binfmt(&em86_format);
	return 0;
}