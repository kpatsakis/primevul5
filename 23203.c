static int __init setup_noefi(char *arg)
{
	disable_runtime = true;
	return 0;
}