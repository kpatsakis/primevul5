clause_reset_after_error()
{
    if (clause_depth)
	FPRINTF((stderr,"CLAUSE RESET after error at depth %d\n",clause_depth));
    clause_depth = 0;
    iteration_depth = 0;
}