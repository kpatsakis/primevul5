static void popString(slist ** stk)
{
    slist *sp = *stk;
    if (!sp) {
	fprintf(stderr, "PANIC: graphml2gv: empty element stack\n");
	exit(1);
    }
    *stk = sp->next;
    free(sp);
}