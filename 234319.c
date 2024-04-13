static void pushString(slist ** stk, const char *s)
{
    int sz = ROUND2(sizeof(slist) + strlen(s), sizeof(void *));
    slist *sp = (slist *) N_NEW(sz, char);
    strcpy(sp->buf, s);
    sp->next = *stk;
    *stk = sp;
}