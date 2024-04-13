time_differs(long t1, long t2)
{
#if defined(__linux__) || defined(MSWIN)
    /* On a FAT filesystem, esp. under Linux, there are only 5 bits to store
     * the seconds.  Since the roundoff is done when flushing the inode, the
     * time may change unexpectedly by one second!!! */
    return (t1 - t2 > 1 || t2 - t1 > 1);
#else
    return (t1 != t2);
#endif
}