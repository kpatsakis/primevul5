static char *topString(slist * stk)
{
    if (!stk) {
	fprintf(stderr, "PANIC: graphml2gv: empty element stack\n");
	exit(1);
    }
    return stk->buf;
}