test_term()
{
    struct termentry *t = term;
    const char *str;
    int x, y, xl, yl, i;
    int xmax_t, ymax_t, x0, y0;
    char label[MAX_ID_LEN];
    int key_entry_height;
    int p_width;
    TBOOLEAN already_in_enhanced_text_mode;
    static t_colorspec black = BLACK_COLORSPEC;

    already_in_enhanced_text_mode = t->flags & TERM_ENHANCED_TEXT;
    if (!already_in_enhanced_text_mode)
	do_string("set termopt enh");

    term_start_plot();
    screen_ok = FALSE;
    xmax_t = (t->xmax * xsize);
    ymax_t = (t->ymax * ysize);
    x0 = (xoffset * t->xmax);
    y0 = (yoffset * t->ymax);

    p_width = pointsize * t->h_tic;
    key_entry_height = pointsize * t->v_tic * 1.25;
    if (key_entry_height < t->v_char)
	key_entry_height = t->v_char;

    /* Sync point for epslatex text positioning */
    (*t->layer)(TERM_LAYER_FRONTTEXT);

    /* border linetype */
    (*t->linewidth) (1.0);
    (*t->linetype) (LT_BLACK);
    newpath();
    (*t->move) (x0, y0);
    (*t->vector) (x0 + xmax_t - 1, y0);
    (*t->vector) (x0 + xmax_t - 1, y0 + ymax_t - 1);
    (*t->vector) (x0, y0 + ymax_t - 1);
    (*t->vector) (x0, y0);
    closepath();

    /* Echo back the current terminal type */
    if (!strcmp(term->name,"unknown"))
	int_error(NO_CARET, "terminal type is unknown");
    else {
	char tbuf[64];
	(void) (*t->justify_text) (LEFT);
	sprintf(tbuf,"%s  terminal test", term->name);
	(*t->put_text) (x0 + t->h_char * 2, y0 + ymax_t - t->v_char, tbuf);
	sprintf(tbuf, "gnuplot version %s.%s  ", gnuplot_version, gnuplot_patchlevel);
	(*t->put_text) (x0 + t->h_char * 2, y0 + ymax_t - t->v_char * 2.25, tbuf);
    }

    (*t->linetype) (LT_AXIS);
    (*t->move) (x0 + xmax_t / 2, y0);
    (*t->vector) (x0 + xmax_t / 2, y0 + ymax_t - 1);
    (*t->move) (x0, y0 + ymax_t / 2);
    (*t->vector) (x0 + xmax_t - 1, y0 + ymax_t / 2);

    /* How well can we estimate width and height of characters?
     * Textbox fill shows true size, surrounding box shows the generic estimate
     * used to reserve space during plot layout.
     */
    if (TRUE) {
	struct text_label sample = EMPTY_LABELSTRUCT;
	struct textbox_style save_opts = textbox_opts[0];
	textbox_style *textbox = &textbox_opts[0];
	sample.text = "12345678901234567890";
	sample.pos = CENTRE;
	sample.boxed = -1;
	textbox->opaque = TRUE;
	textbox->noborder = TRUE;
	textbox->fillcolor.type = TC_RGB;
	textbox->fillcolor.lt = 0xccccee;
	/* disable extra space around text */
	textbox->xmargin = 0;
	textbox->ymargin = 0;

	(*t->linetype) (LT_SOLID);
	write_label(xmax_t/2, ymax_t/2, &sample);
	textbox_opts[0] = save_opts;

	sample.boxed = 0;
	sample.text = "true vs. estimated text dimensions";
	write_label(xmax_t/2, ymax_t/2 + 1.5 * t->v_char, &sample);

	newpath();
	(*t->move) (x0 + xmax_t / 2 - t->h_char * 10, y0 + ymax_t / 2 + t->v_char / 2);
	(*t->vector) (x0 + xmax_t / 2 + t->h_char * 10, y0 + ymax_t / 2 + t->v_char / 2);
	(*t->vector) (x0 + xmax_t / 2 + t->h_char * 10, y0 + ymax_t / 2 - t->v_char / 2);
	(*t->vector) (x0 + xmax_t / 2 - t->h_char * 10, y0 + ymax_t / 2 - t->v_char / 2);
	(*t->vector) (x0 + xmax_t / 2 - t->h_char * 10, y0 + ymax_t / 2 + t->v_char / 2);
	closepath();
    }

    /* Test for enhanced text */
    (*t->linetype) (LT_BLACK);
    if (t->flags & TERM_ENHANCED_TEXT) {
	char *tmptext1 =   "Enhanced text:   {x@_{0}^{n+1}}";
	char *tmptext2 = "&{Enhanced text:  }{/:Bold Bold}{/:Italic  Italic}";
	(*t->put_text) (x0 + xmax_t * 0.5, y0 + ymax_t * 0.40, tmptext1);
	(*t->put_text) (x0 + xmax_t * 0.5, y0 + ymax_t * 0.35, tmptext2);
	(*t->set_font)("");
	if (!already_in_enhanced_text_mode)
	    do_string("set termopt noenh");
    }

    /* test justification */
    (void) (*t->justify_text) (LEFT);
    (*t->put_text) (x0 + xmax_t / 2, y0 + ymax_t / 2 + t->v_char * 6, "left justified");
    str = "centre+d text";
    if ((*t->justify_text) (CENTRE))
	(*t->put_text) (x0 + xmax_t / 2,
			y0 + ymax_t / 2 + t->v_char * 5, str);
    else
	(*t->put_text) (x0 + xmax_t / 2 - strlen(str) * t->h_char / 2,
			y0 + ymax_t / 2 + t->v_char * 5, str);
    str = "right justified";
    if ((*t->justify_text) (RIGHT))
	(*t->put_text) (x0 + xmax_t / 2,
			y0 + ymax_t / 2 + t->v_char * 4, str);
    else
	(*t->put_text) (x0 + xmax_t / 2 - strlen(str) * t->h_char,
			y0 + ymax_t / 2 + t->v_char * 4, str);

    /* test tic size */
    (*t->linetype)(2);
    (*t->move) ((unsigned int) (x0 + xmax_t / 2 + t->h_tic * (1 + axis_array[FIRST_X_AXIS].ticscale)), y0 + (unsigned int) ymax_t - 1);
    (*t->vector) ((unsigned int) (x0 + xmax_t / 2 + t->h_tic * (1 + axis_array[FIRST_X_AXIS].ticscale)),
		  (unsigned int) (y0 + ymax_t - axis_array[FIRST_X_AXIS].ticscale * t->v_tic));
    (*t->move) ((unsigned int) (x0 + xmax_t / 2), y0 + (unsigned int) (ymax_t - t->v_tic * (1 + axis_array[FIRST_X_AXIS].ticscale)));
    (*t->vector) ((unsigned int) (x0 + xmax_t / 2 + axis_array[FIRST_X_AXIS].ticscale * t->h_tic),
		  (unsigned int) (y0 + ymax_t - t->v_tic * (1 + axis_array[FIRST_X_AXIS].ticscale)));
    (void) (*t->justify_text) (RIGHT);
    (*t->put_text) (x0 + (unsigned int) (xmax_t / 2 - 1* t->h_char),
		    y0 + (unsigned int) (ymax_t - t->v_char),
		    "show ticscale");
    (void) (*t->justify_text) (LEFT);
    (*t->linetype)(LT_BLACK);

    /* test line and point types */
    x = x0 + xmax_t - t->h_char * 7 - p_width;
    y = y0 + ymax_t - key_entry_height;
    (*t->pointsize) (pointsize);
    for (i = -2; y > y0 + key_entry_height; i++) {
	struct lp_style_type ls = DEFAULT_LP_STYLE_TYPE;
	ls.l_width = 1;
	load_linetype(&ls,i+1);
	term_apply_lp_properties(&ls);

	(void) sprintf(label, "%d", i + 1);
	if ((*t->justify_text) (RIGHT))
	    (*t->put_text) (x, y, label);
	else
	    (*t->put_text) (x - strlen(label) * t->h_char, y, label);
	(*t->move) (x + t->h_char, y);
	(*t->vector) (x + t->h_char * 5, y);
	if (i >= -1)
	    (*t->point) (x + t->h_char * 6 + p_width / 2, y, i);
	y -= key_entry_height;
    }

    /* test arrows (should line up with rotated text) */
    (*t->linewidth) (1.0);
    (*t->linetype) (0);
    (*t->dashtype) (DASHTYPE_SOLID, NULL);
    x = x0 + 2. * t->v_char;
    y = y0 + ymax_t/2;
    xl = t->h_tic * 7;
    yl = t->v_tic * 7;
    i = curr_arrow_headfilled;
    curr_arrow_headfilled = AS_NOBORDER;
    (*t->arrow) (x, y-yl, x, y+yl, BOTH_HEADS);
    curr_arrow_headfilled = AS_EMPTY;
    (*t->arrow) (x, y, x + xl, y + yl, END_HEAD);
    curr_arrow_headfilled = AS_NOFILL;
    (*t->arrow) (x, y, x + xl, y - yl, END_HEAD);
    curr_arrow_headfilled = i;

    /* test text angle (should match arrows) */
    (*t->linetype)(0);
    str = "rotated ce+ntred text";
    if ((*t->text_angle) (TEXT_VERTICAL)) {
	if ((*t->justify_text) (CENTRE))
	    (*t->put_text) (x0 + t->v_char,
			    y0 + ymax_t / 2, str);
	else
	    (*t->put_text) (x0 + t->v_char,
			    y0 + ymax_t / 2 - strlen(str) * t->h_char / 2, str);
	(*t->justify_text) (LEFT);
	str = "  rotate by +45";
	(*t->text_angle)(45);
	(*t->put_text)(x0 + t->v_char * 3, y0 + ymax_t / 2, str);
	(*t->justify_text) (LEFT);
	str = "  rotate by -45";
	(*t->text_angle)(-45);
	(*t->put_text)(x0 + t->v_char * 3, y0 + ymax_t / 2, str);
    } else {
	(void) (*t->justify_text) (LEFT);
	(*t->put_text) (x0 + t->h_char * 2, y0 + ymax_t / 2, "cannot rotate text");
    }
    (void) (*t->justify_text) (LEFT);
    (void) (*t->text_angle) (0);

    /* test line widths */
    (void) (*t->justify_text) (LEFT);
    xl = xmax_t / 10;
    yl = ymax_t / 25;
    x = x0 + xmax_t * .075;
    y = y0 + yl;

    for (i=1; i<7; i++) {
	(*t->linewidth) ((double)(i)); (*t->linetype)(LT_BLACK);
	(*t->move) (x, y); (*t->vector) (x+xl, y);
	sprintf(label,"  lw %1d", i);
	(*t->put_text) (x+xl, y, label);
	y += yl;
    }
    (*t->put_text) (x, y, "linewidth");

    /* test native dashtypes (_not_ the 'set mono' sequence) */
    (void) (*t->justify_text) (LEFT);
    xl = xmax_t / 10;
    yl = ymax_t / 25;
    x = x0 + xmax_t * .3;
    y = y0 + yl;

    for (i=0; i<5; i++) {
 	(*t->linewidth) (1.0);
	(*t->linetype) (LT_SOLID);
	(*t->dashtype) (i, NULL);
	(*t->set_color)(&black);
	(*t->move) (x, y); (*t->vector) (x+xl, y);
	sprintf(label,"  dt %1d", i+1);
	(*t->put_text) (x+xl, y, label);
	y += yl;
    }
    (*t->put_text) (x, y, "dashtype");

    /* test fill patterns */
    x = x0 + xmax_t * 0.5;
    y = y0;
    xl = xmax_t / 40;
    yl = ymax_t / 8;
    (*t->linewidth) (1.0);
    (*t->linetype)(LT_BLACK);
    (*t->justify_text) (CENTRE);
    (*t->put_text)(x+xl*7, y + yl+t->v_char*1.5, "pattern fill");
    for (i=0; i<9; i++) {
	int style = ((i<<4) + FS_PATTERN);
	if (t->fillbox)
	    (*t->fillbox) ( style, x, y, xl, yl );
	newpath();
	(*t->move)  (x,y);
	(*t->vector)(x,y+yl);
	(*t->vector)(x+xl,y+yl);
	(*t->vector)(x+xl,y);
	(*t->vector)(x,y);
	closepath();
	sprintf(label, "%2d", i);
	(*t->put_text)(x+xl/2, y+yl+t->v_char*0.5, label);
	x += xl * 1.5;
    }

    {
	int cen_x = x0 + (int)(0.70 * xmax_t);
	int cen_y = y0 + (int)(0.83 * ymax_t);
	int radius = xmax_t / 20;

	/* test pm3d -- filled_polygon(), but not set_color() */
	if (t->filled_polygon) {
	    int i, j;
#define NUMBER_OF_VERTICES 6
	    int n = NUMBER_OF_VERTICES;
	    gpiPoint corners[NUMBER_OF_VERTICES+1];
#undef  NUMBER_OF_VERTICES

	    for (j=0; j<=1; j++) {
		int ix = cen_x + j*radius;
		int iy = cen_y - j*radius/2;
		for (i = 0; i < n; i++) {
		    corners[i].x = ix + radius * cos(2*M_PI*i/n);
		    corners[i].y = iy + radius * sin(2*M_PI*i/n);
		}
		corners[n].x = corners[0].x;
		corners[n].y = corners[0].y;
		if (j == 0) {
		    (*t->linetype)(2);
		    corners->style = FS_OPAQUE;
		} else {
		    (*t->linetype)(1);
		    corners->style = FS_TRANSPARENT_SOLID + (50<<4);
		}
		term->filled_polygon(n+1, corners);
	    }
	    str = "filled polygons:";
	} else
	    str = "No filled polygons";
	(*t->linetype)(LT_BLACK);
	i = ((*t->justify_text) (CENTRE)) ? 0 : t->h_char * strlen(str) / 2;
	(*t->put_text) (cen_x - i, cen_y + radius + t->v_char * 0.5, str);
    }

    term_end_plot();
}