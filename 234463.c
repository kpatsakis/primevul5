do_point(unsigned int x, unsigned int y, int number)
{
    int htic, vtic;
    struct termentry *t = term;

    /* use solid lines for point symbols */
    if (term->dashtype != null_dashtype)
	term->dashtype(DASHTYPE_SOLID, NULL);

    if (number < 0) {           /* do dot */
	(*t->move) (x, y);
	(*t->vector) (x, y);
	return;
    }
    number %= POINT_TYPES;
    /* should be in term_tbl[] in later version */
    htic = (term_pointsize * t->h_tic / 2);
    vtic = (term_pointsize * t->v_tic / 2);

    /* point types 1..4 are same as in postscript, png and x11
       point types 5..6 are "similar"
       (note that (number) equals (pointtype-1)
    */
    switch (number) {
    case 4:                     /* do diamond */
	(*t->move) (x - htic, y);
	(*t->vector) (x, y - vtic);
	(*t->vector) (x + htic, y);
	(*t->vector) (x, y + vtic);
	(*t->vector) (x - htic, y);
	(*t->move) (x, y);
	(*t->vector) (x, y);
	break;
    case 0:                     /* do plus */
	(*t->move) (x - htic, y);
	(*t->vector) (x - htic, y);
	(*t->vector) (x + htic, y);
	(*t->move) (x, y - vtic);
	(*t->vector) (x, y - vtic);
	(*t->vector) (x, y + vtic);
	break;
    case 3:                     /* do box */
	(*t->move) (x - htic, y - vtic);
	(*t->vector) (x - htic, y - vtic);
	(*t->vector) (x + htic, y - vtic);
	(*t->vector) (x + htic, y + vtic);
	(*t->vector) (x - htic, y + vtic);
	(*t->vector) (x - htic, y - vtic);
	(*t->move) (x, y);
	(*t->vector) (x, y);
	break;
    case 1:                     /* do X */
	(*t->move) (x - htic, y - vtic);
	(*t->vector) (x - htic, y - vtic);
	(*t->vector) (x + htic, y + vtic);
	(*t->move) (x - htic, y + vtic);
	(*t->vector) (x - htic, y + vtic);
	(*t->vector) (x + htic, y - vtic);
	break;
    case 5:                     /* do triangle */
	(*t->move) (x, y + (4 * vtic / 3));
	(*t->vector) (x - (4 * htic / 3), y - (2 * vtic / 3));
	(*t->vector) (x + (4 * htic / 3), y - (2 * vtic / 3));
	(*t->vector) (x, y + (4 * vtic / 3));
	(*t->move) (x, y);
	(*t->vector) (x, y);
	break;
    case 2:                     /* do star */
	(*t->move) (x - htic, y);
	(*t->vector) (x - htic, y);
	(*t->vector) (x + htic, y);
	(*t->move) (x, y - vtic);
	(*t->vector) (x, y - vtic);
	(*t->vector) (x, y + vtic);
	(*t->move) (x - htic, y - vtic);
	(*t->vector) (x - htic, y - vtic);
	(*t->vector) (x + htic, y + vtic);
	(*t->move) (x - htic, y + vtic);
	(*t->vector) (x - htic, y + vtic);
	(*t->vector) (x + htic, y - vtic);
	break;
    }
}