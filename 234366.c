test_palette_subcommand()
{
    enum {test_palette_colors = 256};
    struct udvt_entry *datablock;
    char *save_replot_line;
    TBOOLEAN save_is_3d_plot;
    int i;

    static const char pre1[] = "\
reset;\
uns border; se tics scale 0;\
se cbtic 0,0.1,1 mirr format '' scale 1;\
se xr[0:1];se yr[0:1];se zr[0:1];se cbr[0:1];\
set colorbox hor user orig 0.05,0.02 size 0.925,0.12;";

    static const char pre2[] = "\
se lmarg scre 0.05;se rmarg scre 0.975; se bmarg scre 0.22; se tmarg scre 0.86;\
se grid; se xtics 0,0.1;se ytics 0,0.1;\
se key top right at scre 0.975,0.975 horizontal \
title 'R,G,B profiles of the current color palette';";

    static const char pre3[] = "\
p NaN lc palette notit,\
$PALETTE u 1:2 t 'red' w l lt 1 lc rgb 'red',\
'' u 1:3 t 'green' w l lt 1 lc rgb 'green',\
'' u 1:4 t 'blue' w l lt 1 lc rgb 'blue',\
'' u 1:5 t 'NTSC' w l lt 1 lc rgb 'black'\
\n";

    FILE *f = tmpfile();

#if defined(_MSC_VER) || defined(__MINGW32__)
    /* On Vista/Windows 7 tmpfile() fails. */
    if (!f) {
	char buf[PATH_MAX];
	/* We really want the "ANSI" version */
	GetTempPathA(sizeof(buf), buf);
	strcat(buf, "gnuplot-pal.tmp");
	f = fopen(buf, "w+");
    }
#endif

    while (!END_OF_COMMAND)
	c_token++;
    if (!f)
	int_error(NO_CARET, "cannot write temporary file");

    /* Store R/G/B/Int curves in a datablock */
    datablock = add_udv_by_name("$PALETTE");
    if (datablock->udv_value.type != NOTDEFINED)
	gpfree_datablock(&datablock->udv_value);
    datablock->udv_value.type = DATABLOCK;
    datablock->udv_value.v.data_array = NULL;

    /* Part of the purpose for writing these values into a datablock */
    /* is so that the user can read them back if desired.  But data  */
    /* will be read back using the current numeric locale, so for    */
    /* consistency we must also use the locale when creating it.     */
    set_numeric_locale();
    for (i = 0; i < test_palette_colors; i++) {
	char dataline[64];
	rgb_color rgb;
	double ntsc;
	double z = (double)i / (test_palette_colors - 1);
	double gray = (sm_palette.positive == SMPAL_NEGATIVE) ? 1. - z : z;
	rgb1_from_gray(gray, &rgb);
	ntsc = 0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
	sprintf(dataline, "%0.4f %0.4f %0.4f %0.4f %0.4f %c",
		z, rgb.r, rgb.g, rgb.b, ntsc, '\0');
	append_to_datablock(&datablock->udv_value, strdup(dataline));
    }
    reset_numeric_locale();

    /* commands to setup the test palette plot */
    enable_reset_palette = 0;
    save_replot_line = gp_strdup(replot_line);
    save_is_3d_plot = is_3d_plot;
    fputs(pre1, f);
    fputs(pre2, f);
    fputs(pre3, f);

    /* save current gnuplot 'set' status because of the tricky sets
     * for our temporary testing plot.
     */
    save_set(f);

    /* execute all commands from the temporary file */
    rewind(f);
    load_file(f, NULL, 1); /* note: it does fclose(f) */

    /* enable reset_palette() and restore replot line */
    enable_reset_palette = 1;
    free(replot_line);
    replot_line = save_replot_line;
    is_3d_plot = save_is_3d_plot;
}