static void SFDDumpGradient(FILE *sfd, const char *keyword, struct gradient *gradient) {
    int i;

    /* Use ";" as a coord separator because we treat "," as a potential decimal point */
    fprintf( sfd, "%s %g;%g %g;%g %g %s %d ", keyword,
	    (double) gradient->start.x, (double) gradient->start.y,
	    (double) gradient->stop.x, (double) gradient->stop.y,
	    (double) gradient->radius,
	    spreads[gradient->sm],
	    gradient->stop_cnt );
    for ( i=0 ; i<gradient->stop_cnt; ++i ) {
	fprintf( sfd, "{%g #%06x %g} ", (double) gradient->grad_stops[i].offset,
		gradient->grad_stops[i].col, (double) gradient->grad_stops[i].opacity );
    }
    putc('\n',sfd);
}