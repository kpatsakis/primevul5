end_clause()
{
    if (clause_depth == 0)
	int_error(c_token, "unexpected }");
    else
	clause_depth--;
    c_token++;
    return;
}