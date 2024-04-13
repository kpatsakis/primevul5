stats_command()
{
#ifdef USE_STATS
    statsrequest();
#else
    int_error(NO_CARET,"This copy of gnuplot was not configured with support for the stats command");
#endif
}