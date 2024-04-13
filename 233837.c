ms_escher_read_Sp (MSEscherState *state, MSEscherHeader *h)
{
	static char const * const shape_names[] = {
		/* 0 */ "Not a primitive",
		/* 1 */ "Rectangle",
		/* 2 */ "RoundRectangle",
		/* 3 */ "Ellipse",
		/* 4 */ "Diamond",
		/* 5 */ "IsocelesTriangle",
		/* 6 */ "RightTriangle",
		/* 7 */ "Parallelogram",
		/* 8 */ "Trapezoid",
		/* 9 */ "Hexagon",
		/* 10 */ "Octagon",
		/* 11 */ "Plus",
		/* 12 */ "Star",
		/* 13 */ "Arrow",
		/* 14 */ "ThickArrow",
		/* 15 */ "HomePlate",
		/* 16 */ "Cube",
		/* 17 */ "Balloon",
		/* 18 */ "Seal",
		/* 19 */ "Arc",
		/* 20 */ "Line",
		/* 21 */ "Plaque",
		/* 22 */ "Can",
		/* 23 */ "Donut",
		/* 24-31 */	"TextSimple", "TextOctagon", "TextHexagon", "TextCurve", "TextWave",
				"TextRing", "TextOnCurve", "TextOnRing",
		/* 32 */ "StraightConnector1",
		/* 33-36 */	"BentConnector2",	"BentConnector3",
				"BentConnector4",	"BentConnector5",
		/* 37-40 */	"CurvedConnector2",	"CurvedConnector3",
				"CurvedConnector4",	"CurvedConnector5",
		/* 41-43 */ "Callout1",		"Callout2",	"Callout3",
		/* 44-46 */ "AccentCallout1", "AccentCallout2", "AccentCallout3",
		/* 47-49 */ "BorderCallout1", "BorderCallout2", "BorderCallout3",
		/* 50-52 */ "AccentBorderCallout1", "AccentBorderCallout2", "AccentBorderCallout3",
		/* 53-54 */ "Ribbon", "Ribbon2",
		/* 55 */ "Chevron",
		/* 56 */ "Pentagon",
		/* 57 */ "NoSmoking",
		/* 58 */ "Seal8",
		/* 59 */ "Seal16",
		/* 60 */ "Seal32",
		/* 61 */ "WedgeRectCallout",
		/* 62 */ "WedgeRRectCallout",
		/* 63 */ "WedgeEllipseCallout",
		/* 64 */ "Wave",
		/* 65 */ "FoldedCorner",
		/* 66 */ "LeftArrow",
		/* 67 */ "DownArrow",
		/* 68 */ "UpArrow",
		/* 69 */ "LeftRightArrow",
		/* 70 */ "UpDownArrow",
		/* 71 */ "IrregularSeal1",
		/* 72 */ "IrregularSeal2",
		/* 73 */ "LightningBolt",
		/* 74 */ "Heart",
		/* 75 */ "PictureFrame",
		/* 76 */ "QuadArrow",
		/* 77-83 */ "LeftArrowCallout", "RightArrowCallout",
			"UpArrowCallout", "DownArrowCallout", "LeftRightArrowCallout",
			"UpDownArrowCallout", "QuadArrowCallout",
		/* 84 */ "Bevel",
		/* 85 */ "LeftBracket",
		/* 86 */ "RightBracket",
		/* 87 */ "LeftBrace",
		/* 88 */ "RightBrace",
		/* 89 */ "LeftUpArrow",
		/* 90 */ "BentUpArrow",
		/* 91 */ "BentArrow",
		/* 92 */ "Seal24",
		/* 93 */ "StripedRightArrow",
		/* 94 */ "NotchedRightArrow",
		/* 95 */ "BlockArc",
		/* 96 */ "SmileyFace",
		/* 97 */ "VerticalScroll",
		/* 98 */ "HorizontalScroll",
		/* 99 */ "CircularArrow",
		/* 100 */ "NotchedCircularArrow",
		/* 101 */ "UturnArrow",
		/* 102-105 */ "CurvedRightArrow", "CurvedLeftArrow", "CurvedUpArrow", "CurvedDownArrow",
		/* 106 */ "CloudCallout",
		/* 107-108 */ "EllipseRibbon", "EllipseRibbon2",
		/* 109-135 */ "FlowChartProcess", "FlowChartDecision",
			"FlowChartInputOutput", "FlowChartPredefinedProcess",
			"FlowChartInternalStorage", "FlowChartDocument",
			"FlowChartMultidocument", "FlowChartTerminator",
			"FlowChartPreparation", "FlowChartManualInput",
			"FlowChartManualOperation", "FlowChartConnector",
			"FlowChartPunchedCard", "FlowChartPunchedTape",
			"FlowChartSummingJunction", "FlowChartOr", "FlowChartCollate",
			"FlowChartSort", "FlowChartExtract", "FlowChartMerge",
			"FlowChartOfflineStorage", "FlowChartOnlineStorage",
			"FlowChartMagneticTape", "FlowChartMagneticDisk",
			"FlowChartMagneticDrum", "FlowChartDisplay", "FlowChartDelay",
		/* 136 */ "TextPlainText",
		/* 137 */ "TextStop",
		/* 138-139 */ "TextTriangle", "TextTriangleInverted",
		/* 140-141 */ "TextChevron", "TextChevronInverted",
		/* 142-143 */ "TextRingInside", "TextRingOutside",
		/* 144-145 */ "TextArchUpCurve", "TextArchDownCurve",
		/* 146-147 */ "TextCircleCurve", "TextButtonCurve",
		/* 148-149 */ "TextArchUpPour", "TextArchDownPour",
		/* 150 */ "TextCirclePour",
		/* 151 */ "TextButtonPour",
		/* 152-153 */ "TextCurveUp",	"TextCurveDown",
		/* 154-155 */ "TextCascadeUp",	"TextCascadeDown",
		/* 156-159 */ "TextWave1",		"TextWave2", "TextWave3", "TextWave4",
		/* 160-161 */ "TextInflate",	"TextDeflate",
		/* 162-163 */ "TextInflateBottom", "TextDeflateBottom",
		/* 164-165 */ "TextInflateTop",	"TextDeflateTop",
		/* 166-167 */ "TextDeflateInflate",	"TextDeflateInflateDeflate",
		/* 168-171 */ "TextFadeRight", "TextFadeLeft", "TextFadeUp", "TextFadeDown",
		/* 172-174 */ "TextSlantUp", "TextSlantDown", "TextCanUp",
		/* 175 */ "TextCanDown",
		/* 176 */ "FlowChartAlternateProcess",
		/* 177 */ "FlowChartOffpageConnector",
		/* 178 */ "Callout90",
		/* 179 */ "AccentCallout90",
		/* 180 */ "BorderCallout90",
		/* 181 */ "AccentBorderCallout90",
		/* 182 */ "LeftRightUpArrow",
		/* 183-184 */ "Sun",	"Moon",
		/* 185 */ "BracketPair",
		/* 186 */ "BracePair",
		/* 187 */ "Seal4",
		/* 188 */ "DoubleWave",
		/* 189-200 */ "ActionButtonBlank", "ActionButtonHome",
			"ActionButtonHelp", "ActionButtonInformation",
			"ActionButtonForwardNext", "ActionButtonBackPrevious",
			"ActionButtonEnd", "ActionButtonBeginning",
			"ActionButtonReturn", "ActionButtonDocument",
			"ActionButtonSound", "ActionButtonMovie",
		/* 201 */ "HostControl",
		/* 202 */ "TextBox"
	};

	gboolean needs_free;
	guint32 spid, flags;
	guint8 const *data;

	g_return_val_if_fail (h->instance <= 202, TRUE);

	d (0, g_printerr ("%s (0x%x);\n", shape_names[h->instance],
		      h->instance););

	data = ms_escher_get_data (state,
		h->offset + COMMON_HEADER_LEN, 8, &needs_free);
	if (data == NULL)
		return TRUE;

	spid  = GSF_LE_GET_GUINT32 (data+0);
	flags = GSF_LE_GET_GUINT32 (data+4);
	d (0, g_printerr ("SPID %d, Type %d,%s%s%s%s%s%s%s%s%s%s%s%s;\n",
		      spid, h->instance,
			(flags&0x01) ? " Group": "",
			(flags&0x02) ? " Child": "",
			(flags&0x04) ? " Patriarch": "",
			(flags&0x08) ? " Deleted": "",
			(flags&0x10) ? " OleShape": "",
			(flags&0x20) ? " HaveMaster": "",
			(flags&0x40) ? " FlipH": "",
			(flags&0x80) ? " FlipV": "",
			(flags&0x100) ? " Connector":"",
			(flags&0x200) ? " HasAnchor": "",
			(flags&0x400) ? " HasBackground": "",
			(flags&0x800) ? " HasSpt": ""
		       ););

	if (flags & 0x40)
		ms_escher_header_add_attr (h,
			ms_obj_attr_new_flag (MS_OBJ_ATTR_FLIP_H));
	if (flags & 0x80)
		ms_escher_header_add_attr (h,
			ms_obj_attr_new_flag (MS_OBJ_ATTR_FLIP_V));
	if (needs_free)
		g_free ((guint8*)data);

	return FALSE;
}