do_arrow(
    unsigned int usx, unsigned int usy,   /* start point */
    unsigned int uex, unsigned int uey,   /* end point (point of arrowhead) */
    int headstyle)
{
    /* Clipping and angle calculations do not work if coords are unsigned! */
    int sx = (int)usx;
    int sy = (int)usy;
    int ex = (int)uex;
    int ey = (int)uey;

    struct termentry *t = term;
    double len_tic = ((double) (t->h_tic + t->v_tic)) / 2.0;
    /* average of tic sizes */
    /* (dx,dy) : vector from end to start */
    double dx = sx - ex;
    double dy = sy - ey;
    double len_arrow = sqrt(dx * dx + dy * dy);
    gpiPoint head_points[5];
    int xm = 0, ym = 0;
    BoundingBox *clip_save;

    /* The arrow shaft was clipped already in draw_clip_arrow() but we still */
    /* need to clip the head here. */
    clip_save = clip_area;
    if (term->flags & TERM_CAN_CLIP)
	clip_area = NULL;
    else
	clip_area = &canvas;

    /* Calculate and draw arrow heads.
     * Draw no head for arrows with length = 0, or, to be more specific,
     * length < DBL_EPSILON, because len_arrow will almost always be != 0.
     */
    if ((headstyle & BOTH_HEADS) != NOHEAD && fabs(len_arrow) >= DBL_EPSILON) {
	int x1, y1, x2, y2;
	if (curr_arrow_headlength <= 0) {
	    /* An arrow head with the default size and angles */
	    double coeff_shortest = len_tic * HEAD_SHORT_LIMIT / len_arrow;
	    double coeff_longest = len_tic * HEAD_LONG_LIMIT / len_arrow;
	    double head_coeff = GPMAX(coeff_shortest,
				      GPMIN(HEAD_COEFF, coeff_longest));
	    /* we put the arrowhead marks at 15 degrees to line */
	    x1 = (int) ((COS15 * dx - SIN15 * dy) * head_coeff);
	    y1 = (int) ((SIN15 * dx + COS15 * dy) * head_coeff);
	    x2 = (int) ((COS15 * dx + SIN15 * dy) * head_coeff);
	    y2 = (int) ((-SIN15 * dx + COS15 * dy) * head_coeff);
	    /* backangle defaults to 90 deg */
	    xm = (int) ((x1 + x2)/2);
	    ym = (int) ((y1 + y2)/2);
	} else {
	    /* An arrow head with the length + angle specified explicitly.	*/
	    /* Assume that if the arrow is shorter than the arrowhead, this is	*/
	    /* because of foreshortening in a 3D plot.                  	*/
	    double alpha = curr_arrow_headangle * DEG2RAD;
	    double beta = curr_arrow_headbackangle * DEG2RAD;
	    double phi = atan2(-dy,-dx); /* azimuthal angle of the vector */
	    double backlen, effective_length;
	    double dx2, dy2;

	    effective_length = curr_arrow_headlength;
	    if (!curr_arrow_headfixedsize && (curr_arrow_headlength > len_arrow/2.)) {
		effective_length = len_arrow/2.;
		alpha = atan(tan(alpha)*((double)curr_arrow_headlength/effective_length));
		beta = atan(tan(beta)*((double)curr_arrow_headlength/effective_length));
	    }
	    backlen = sin(alpha) / sin(beta);

	    /* anticlock-wise head segment */
	    x1 = -(int)(effective_length * cos( alpha - phi ));
	    y1 =  (int)(effective_length * sin( alpha - phi ));
	    /* clock-wise head segment */
	    dx2 = -effective_length * cos( phi + alpha );
	    dy2 = -effective_length * sin( phi + alpha );
	    x2 = (int) (dx2);
	    y2 = (int) (dy2);
	    /* back point */
	    xm = (int) (dx2 + backlen*effective_length * cos( phi + beta ));
	    ym = (int) (dy2 + backlen*effective_length * sin( phi + beta ));
	}

	if ((headstyle & END_HEAD) && !clip_point(ex, ey)) {
	    head_points[0].x = ex + xm;
	    head_points[0].y = ey + ym;
	    head_points[1].x = ex + x1;
	    head_points[1].y = ey + y1;
	    head_points[2].x = ex;
	    head_points[2].y = ey;
	    head_points[3].x = ex + x2;
	    head_points[3].y = ey + y2;
	    head_points[4].x = ex + xm;
	    head_points[4].y = ey + ym;
	    if (!((headstyle & SHAFT_ONLY))) {
		if (curr_arrow_headfilled >= AS_FILLED) {
		    /* draw filled forward arrow head */
		    head_points->style = FS_OPAQUE;
		    if (t->filled_polygon)
			(*t->filled_polygon) (5, head_points);
		}
		/* draw outline of forward arrow head */
		if (curr_arrow_headfilled == AS_NOFILL) {
		    draw_clip_polygon(3, head_points+1);
		} else if (curr_arrow_headfilled != AS_NOBORDER) {
		    draw_clip_polygon(5, head_points);
		}
	    }
	}

	/* backward arrow head */
	if ((headstyle & BACKHEAD) && !clip_point(sx,sy)) {
	    head_points[0].x = sx - xm;
	    head_points[0].y = sy - ym;
	    head_points[1].x = sx - x1;
	    head_points[1].y = sy - y1;
	    head_points[2].x = sx;
	    head_points[2].y = sy;
	    head_points[3].x = sx - x2;
	    head_points[3].y = sy - y2;
	    head_points[4].x = sx - xm;
	    head_points[4].y = sy - ym;
	    if (!((headstyle & SHAFT_ONLY))) {
		if (curr_arrow_headfilled >= AS_FILLED) {
		    /* draw filled backward arrow head */
		    head_points->style = FS_OPAQUE;
		    if (t->filled_polygon)
			(*t->filled_polygon) (5, head_points);
		}
		/* draw outline of backward arrow head */
		if (curr_arrow_headfilled == AS_NOFILL) {
		    draw_clip_polygon(3, head_points+1);
		} else if (curr_arrow_headfilled != AS_NOBORDER) {
		    draw_clip_polygon(5, head_points);
		}
	    }
	}
    }

    /* Adjust the length of the shaft so that it doesn't overlap the head */
    if ((headstyle & BACKHEAD)
    &&  (fabs(len_arrow) >= DBL_EPSILON) && (curr_arrow_headfilled != AS_NOFILL) ) {
	sx -= xm;
	sy -= ym;
    }
    if ((headstyle & END_HEAD)
    &&  (fabs(len_arrow) >= DBL_EPSILON) && (curr_arrow_headfilled != AS_NOFILL) ) {
	ex += xm;
	ey += ym;
    }

    /* Draw the line for the arrow. */
    if (!((headstyle & HEADS_ONLY)))
	draw_clip_line(sx, sy, ex, ey);

    /* Restore previous clipping box */
    clip_area = clip_save;
}