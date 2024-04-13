reread_command()
{
    FILE *fp = lf_top();

    if (fp != (FILE *) NULL)
	rewind(fp);
    c_token++;
}