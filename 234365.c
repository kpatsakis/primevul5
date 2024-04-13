break_command()
{
    c_token++;
    if (iteration_depth == 0)
	return;
    /* Skip to end of current iteration */
    c_token = num_tokens;
    /* request that subsequent iterations should be skipped also */
    requested_break = TRUE;
}