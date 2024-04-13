find_clause(int *clause_start, int *clause_end)
{
    int i, depth;

    *clause_start = token[c_token].start_index;
    for (i=++c_token, depth=1; i<num_tokens; i++) {
	if (equals(i,"{"))
	    depth++;
	else if (equals(i,"}"))
	    depth--;
	if (depth == 0)
	    break;
    }
    *clause_end = token[i].start_index;

    return (i+1);
}