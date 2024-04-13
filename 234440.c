style_from_fill(struct fill_style_type *fs)
{
    int fillpar, style;

    switch( fs->fillstyle ) {
    case FS_SOLID:
    case FS_TRANSPARENT_SOLID:
	fillpar = fs->filldensity;
	style = ((fillpar & 0xfff) << 4) + fs->fillstyle;
	break;
    case FS_PATTERN:
    case FS_TRANSPARENT_PATTERN:
	fillpar = fs->fillpattern;
	style = ((fillpar & 0xfff) << 4) + fs->fillstyle;
	break;
    case FS_EMPTY:
    default:
	/* solid fill with background color */
	style = FS_EMPTY;
	break;
    }

    return style;
}