ms_escher_line_type_to_xl (GOLineDashType ld)
{
	switch (ld) {
	default:
	case GO_LINE_NONE:		return -1;
	case GO_LINE_SOLID:		return 0;
	case GO_LINE_S_DOT:		return 2;
	case GO_LINE_S_DASH_DOT:	return 3;
	case GO_LINE_S_DASH_DOT_DOT:	return 4;
	case GO_LINE_DASH_DOT_DOT_DOT:	return 4;
	case GO_LINE_DOT:		return 5;
	case GO_LINE_S_DASH:		return 6;
	case GO_LINE_DASH:		return 7;
	case GO_LINE_LONG_DASH:		return 8;
	case GO_LINE_DASH_DOT:		return 9;
	case GO_LINE_DASH_DOT_DOT:	return 10;
	}
}