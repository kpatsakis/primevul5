help_command()
{
    while (!(END_OF_COMMAND))
	c_token++;
    fputs("This gnuplot was not built with inline help\n", stderr);
}