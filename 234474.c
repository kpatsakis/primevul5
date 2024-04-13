null_scale(double x, double y)
{
    (void) x;                   /* avoid -Wunused warning */
    (void) y;
    int_error(NO_CARET, "Attempt to call deprecated terminal function");
    return FALSE;               /* can't be done */
}