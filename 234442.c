line_and_point(unsigned int x, unsigned int y, int number)
{
    /* temporary(?) kludge to allow terminals with bad linetypes
       to make nice marks */

    (*term->linetype) (NICE_LINE);
    do_point(x, y, number);
}