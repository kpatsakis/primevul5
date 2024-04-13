static void freeString(slist * stk)
{
    slist *sp;

    while (stk) {
	sp = stk->next;
	free(stk);
	stk = sp;
    }
}