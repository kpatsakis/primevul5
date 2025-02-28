STATIC void
S_rck_elide_nothing(pTHX_ regnode *node)
{
    dVAR;

    PERL_ARGS_ASSERT_RCK_ELIDE_NOTHING;

    if (OP(node) != CURLYX) {
        const int max = (reg_off_by_arg[OP(node)]
                        ? I32_MAX
                          /* I32 may be smaller than U16 on CRAYs! */
                        : (I32_MAX < U16_MAX ? I32_MAX : U16_MAX));
        int off = (reg_off_by_arg[OP(node)] ? ARG(node) : NEXT_OFF(node));
        int noff;
        regnode *n = node;

        /* Skip NOTHING and LONGJMP. */
        while (
            (n = regnext(n))
            && (
                (PL_regkind[OP(n)] == NOTHING && (noff = NEXT_OFF(n)))
                || ((OP(n) == LONGJMP) && (noff = ARG(n)))
            )
            && off + noff < max
        ) {
            off += noff;
        }
        if (reg_off_by_arg[OP(node)])
            ARG(node) = off;
        else
            NEXT_OFF(node) = off;
    }
    return;