static void SFDDumpPattern(FILE *sfd, const char *keyword, struct pattern *pattern) {

    fprintf( sfd, "%s %s %g;%g [%g %g %g %g %g %g]\n", keyword,
	    pattern->pattern,
	    (double) pattern->width, (double) pattern->height,
	    (double) pattern->transform[0], (double) pattern->transform[1],
	    (double) pattern->transform[2], (double) pattern->transform[3],
	    (double) pattern->transform[4], (double) pattern->transform[5] );
}