undefine_command()
{
    char key[MAX_ID_LEN+1];
    TBOOLEAN wildcard;

    c_token++;               /* consume the command name */

    while (!END_OF_COMMAND) {
	/* copy next var name into key */
	copy_str(key, c_token, MAX_ID_LEN);

	/* Peek ahead - must do this, because a '*' is returned as a
	   separate token, not as part of the 'key' */
	wildcard = equals(c_token+1,"*");
	if (wildcard)
	    c_token++;

	/* The '$' starting a data block name is a separate token */
	else if (*key == '$')
	    copy_str(&key[1], ++c_token, MAX_ID_LEN-1);

	/* Other strange stuff on command line */
	else if (!isletter(c_token))
	    int_error(c_token, "Not a variable name");

	/* This command cannot deal with array elements or functions */
	if (equals(c_token+1, "[") || equals(c_token+1, "("))
	    int_error(c_token, "Cannot undefine function or array element");

	/* ignore internal variables */
	if (strncmp(key, "GPVAL_", 6) && strncmp(key, "MOUSE_", 6))
	    del_udv_by_name( key, wildcard );

	c_token++;
    }
}