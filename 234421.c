continue_command()
{
    c_token++;
    if (iteration_depth == 0)
	return;
    /* Skip to end of current clause */
    c_token = num_tokens;
    /* request that remainder of this iteration be skipped also */
    requested_continue = TRUE;
}