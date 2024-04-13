static void __exit exit_em86_binfmt(void)
{
	unregister_binfmt(&em86_format);
}