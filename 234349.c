pwd_command()
{
    char *save_file = NULL;

    save_file = gp_alloc(PATH_MAX, "print current dir");
    if (GP_GETCWD(save_file, PATH_MAX) == NULL)
	fprintf(stderr, "<invalid>\n");
    else
	fprintf(stderr, "%s\n", save_file);
    free(save_file);
    c_token++;
}