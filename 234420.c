command()
{
    int i;

    for (i = 0; i < MAX_NUM_VAR; i++)
	c_dummy_var[i][0] = NUL;	/* no dummy variables */

    if (is_definition(c_token))
	define();
    else if (is_array_assignment())
	;
    else
	(*lookup_ftable(&command_ftbl[0],c_token))();

    return;
}