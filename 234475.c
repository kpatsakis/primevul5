do_enh_writec(int c)
{
    /* note: c is meant to hold a char, but is actually an int, for
     * the same reasons applying to putc() and friends */
    *enhanced_cur_text++ = c;
}