xz_reset(xz_statep state)
{
    state->have = 0;            /* no output data available */
    state->eof = 0;             /* not at end of file */
    state->how = LOOK;          /* look for gzip header */
    state->direct = 1;          /* default for empty file */
    state->seek = 0;            /* no seek request pending */
    xz_error(state, LZMA_OK, NULL);     /* clear error */
    state->pos = 0;             /* no uncompressed data yet */
    state->strm.avail_in = 0;   /* no input data yet */
#ifdef HAVE_ZLIB_H
    state->zstrm.avail_in = 0;  /* no input data yet */
#endif
}