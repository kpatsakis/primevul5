new_clause(int clause_start, int clause_end)
{
    char *clause = gp_alloc(clause_end - clause_start, "clause");
    memcpy(clause, &gp_input_line[clause_start+1], clause_end - clause_start);
    clause[clause_end - clause_start - 1] = '\0';
    return clause;
}