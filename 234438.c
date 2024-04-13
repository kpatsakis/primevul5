term_start_multiplot()
{
    FPRINTF((stderr, "term_start_multiplot()\n"));
    multiplot_start();
#ifdef USE_MOUSE
    UpdateStatusline();
#endif
}