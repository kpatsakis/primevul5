ms_escher_read_OPT (MSEscherState *state, MSEscherHeader *h)
{
	int const num_properties = h->instance;
	gboolean needs_free;
	guint8 const *data = ms_escher_get_data (state,
		h->offset + COMMON_HEADER_LEN, h->len - COMMON_HEADER_LEN, &needs_free);
	guint8 const *fopte = data;
	guint8 const *extra = fopte + 6*num_properties;
	guint prev_pid = 0; /* A debug tool */
	char const *name;
	int i;

	/* let's be really careful */
	g_return_val_if_fail (6*num_properties + COMMON_HEADER_LEN <= h->len, TRUE);
	g_return_val_if_fail (data != NULL, TRUE);

	for (i = 0; i < num_properties; ++i, fopte += 6) {
		guint16 const tmp = GSF_LE_GET_GUINT32(fopte);
		guint const pid = tmp & 0x3fff;
		gboolean const is_blip = (tmp & 0x4000) != 0;
		gboolean const is_complex = (tmp & 0x8000) != 0;
		guint32 val = GSF_LE_GET_GUINT32 (fopte+2);
		MSObjAttrID id = MS_OBJ_ATTR_NONE;

		/* container is sorted by pid. Use this as sanity test */
		if (prev_pid >= pid) {
			g_printerr ("Pids not monotonic %d >= %d;\n", prev_pid, pid);
			if (needs_free)
				g_free ((guint8 *)data);
			return TRUE;
		}
		prev_pid = pid;

		name = NULL;
		switch (pid) {
	/* Transofrmation */
		/* 0 : fixed point: 16.16 degrees */
		case 4 : name = "long rotation"; break;

	/* Protection */
		case 119 : case 120 : case 121 : case 122 : case 123 :
		case 124 : case 125 : case 126 : case 127 :
			ms_escher_read_OPT_bools (h,
				protection_bools, G_N_ELEMENTS (protection_bools),
				pid, val);
			break;

	/* Text */
		/* 0 : id for the text, value determined by the host */
		case 128 : name = "long Txid"; break;
		/* 1/10" : margins relative to shape's inscribed text rectangle (in EMUs) */
		case 129 : name = "long dxTextLeft"; break;
		/* 1/20" :  */
		case 130 : name = "long dyTextTop"; break;
		/* 1/10" :  */
		case 131 : name = "long dxTextRight"; break;
		/* 1/20" :  */
		case 132 : name = "long dyTextBottom"; break;
		/* Square */
		case 133 : name = "WrapMode wrap_text_at_margin"; break;
		/* 0 : Text zoom/scale (used if fFitTextToShape) */
		case 134 : name = "long scaleText"; break;
		/* anchor_Top : How to anchor the text */
		case 135 : name = "AnchorType anchorText"; break;
		/* HorzN : Text flow */
		case 136 : name = "TextFlow txflTextFlow"; break;
		/* msocdir0 : Font rotation */
		case 137 : name = "RotationType cdirFont"; break;
		/* NULL : ID of the next shape (used by Word for linked textboxes) */
		case 138 : name = "MSOHSP hspNext"; break;
		/* LTR : Bi-Di Text direction */
		case 139 : name = "TextDirection txdir"; break;

		case 187 : case 188 : case 189 : case 190 : case 191 :
			ms_escher_read_OPT_bools (h,
				text_bools, G_N_ELEMENTS (text_bools),
				pid, val);
			break;

	/* GeoText */
		/* NULL : UNICODE text string */
		case 192 : name = "wchar* gtextUNICODE"; break;
		/* NULL : RTF text string */
		case 193 : name = "char* gtextRTF"; break;
		/* Center : alignment on curve */
		case 194 : name = "Alignment gtextAlign"; break;
		/* 36<<16 : default point size */
		case 195 : name = "long gtextSize"; break;
		/* 1<<16 : fixed point 16.16 */
		case 196 : name = "long gtextSpacing"; break;
		/* NULL : font family name */
		case 197 : name = "wchar* gtextFont"; break;

		case 240 : case 241 : case 242 : case 243 : case 244 :
		case 245 : case 246 : case 247 : case 248 : case 249 :
		case 250 : case 251 : case 252 : case 253 : case 254 :
		case 255 :
			ms_escher_read_OPT_bools (h,
				geotext_bools, G_N_ELEMENTS (geotext_bools),
				pid, val);
			break;

	/* Blip */
		/* 0 : 16.16 fraction times total image width or height, as appropriate. */
		case 256 : name = "fixed16_16 cropFromTop";
			   id = MS_OBJ_ATTR_BLIP_CROP_TOP;
			   break;
		case 257 : name = "fixed16_16 cropFromBottom";
			   id = MS_OBJ_ATTR_BLIP_CROP_BOTTOM;
			   break;
		case 258 : name = "fixed16_16 cropFromLeft";
			   id = MS_OBJ_ATTR_BLIP_CROP_LEFT;
			   break;
		case 259 : name = "fixed16_16 cropFromRight";
			   id = MS_OBJ_ATTR_BLIP_CROP_RIGHT;
			   break;

		/* NULL : Blip to display */
		case 260 : id = MS_OBJ_ATTR_BLIP_ID;
			   name = "Blip * pib";
			   break;

		/* NULL : Blip file name */
		case 261 : name = "wchar * pibName"; break;
		/* What are BlipFlags ? */
		/* Comment Blip flags */
		case 262 : name = "BlipType pibFlags"; break;
		/* ~0 : transparent color (none if ~0UL)  */
		case 263 : name = "long pictureTransparent"; break;
		/* 1<<16 : contrast setting */
		case 264 : name = "long pictureContrast"; break;
		/* 0 : brightness setting */
		case 265 : name = "long pictureBrightness"; break;
		/* 0 : 16.16 gamma */
		case 266 : name = "fixed16_16 pictureGamma"; break;
		/* 0 : Host-defined ID for OLE objects (usually a pointer) */
		case 267 : name = "Long pictureId"; break;
		/* undefined : Double shadow Colour */
		case 268 : name = "Colour pictureDblCrMod"; break;
		/* undefined : */
		case 269 : name = "Colour pictureFillCrMod"; break;
		/* undefined : */
		case 270 : name = "Colour pictureLineCrMod"; break;
		/* NULL : Blip to display when printing */
		case 271 : name = "Blip * pibPrint"; break;
		/* NULL : Blip file name */
		case 272 : name = "wchar * pibPrintName"; break;
		/* Comment Blip flags */
		case 273 : name = "BlipType pibPrintFlags"; break;

		case 316 : case 317 : case 318 : case 319 :
			ms_escher_read_OPT_bools (h,
				picture_bools, G_N_ELEMENTS (picture_bools),
				pid, val);
			break;

	/* Geometry */
		/* 0 : Defines the G (geometry) coordinate space. */
		case 320 : name = "long geoLeft"; break;
		/* 0 :  */
		case 321 : name = "long geoTop"; break;
		/* 21600 :  */
		case 322 : name = "long geoRight"; break;
		/* 21600 :  */
		case 323 : name = "long geoBottom"; break;
		/* msoshapeLinesClosed :  */
		case 324 : name = "ShapePath shapePath"; break;
		/* NULL : An array of points, in G units. */
		case 325 : name = "IMsoArray pVertices"; break;
		/* NULL :  */
		case 326 : name = "IMsoArray pSegmentInfo"; break;
		/* 0 : Adjustment values corresponding to the positions of the
		 * adjust handles of the shape. The number of values used and
		 * their allowable ranges vary from shape type to shape type.
		 */
		case 327 : name = "long adjustValue"; break;
		/* 0 :  */
		case 328 : name = "long adjust2Value"; break;
		/* 0 :  */
		case 329 : name = "long adjust3Value"; break;
		/* 0 :  */
		case 330 : name = "long adjust4Value"; break;
		/* 0 :  */
		case 331 : name = "long adjust5Value"; break;
		/* 0 :  */
		case 332 : name = "long adjust6Value"; break;
		/* 0 :  */
		case 333 : name = "long adjust7Value"; break;
		/* 0 :  */
		case 334 : name = "long adjust8Value"; break;
		/* 0 :  */
		case 335 : name = "long adjust9Value"; break;
		/* 0 :  */
		case 336 : name = "long adjust10Value"; break;

		case 378 : case 379 : case 380 : case 381 : case 382 :
		case 383 :
			ms_escher_read_OPT_bools (h,
				geometry_bools, G_N_ELEMENTS (geometry_bools),
				pid, val);
			break;

	/* FillStyle */
		/* Solid : Type of fill */
		case 384 : id = MS_OBJ_ATTR_FILL_TYPE;
			   name = "FillType fillType";
			   break;

		/* white : Foreground color */
		case 385 : id = MS_OBJ_ATTR_FILL_COLOR;
			   name = "Colour fillColor";
			   break;

		/* 1<<16 : Fixed 16.16 */
		case 386 : id = MS_OBJ_ATTR_FILL_ALPHA;
			   name = "long fillOpacity";
			   break;
		/* white : Background color */
		case 387 : id = MS_OBJ_ATTR_FILL_BACKGROUND;
			   name = "Colour fillBackColor";
			   break;
		/* 1<<16 : Shades only */
		case 388 : id = MS_OBJ_ATTR_FILL_BACKGROUND_ALPHA;
			   name = "long fillBackOpacity";
			   break;
		/* undefined : Modification for BW views */
		case 389 : name = "Colour fillCrMod"; break;
		/* NULL : Pattern/texture */
		case 390 : name = "IMsoBlip* fillBlip"; break;
		/* NULL : Blip file name */
		case 391 : name = "wchar* fillBlipName"; break;
		/* Comment : Blip flags */
		case 392 : name = "BlipFlags fillBlipFlags"; break;
		/* 0 : How big (A units) to make a metafile texture. */
		case 393 : name = "long fillWidth"; break;
		/* 0 :  */
		case 394 : name = "long fillHeight"; break;
		/* 0 : Fade angle - degrees in 16.16 */
		case 395 : id = MS_OBJ_ATTR_FILL_ANGLE;
			   name = "long fillAngle";
			   break;
		/* 0 : Linear shaded fill focus percent */
		case 396 : id = MS_OBJ_ATTR_FILL_FOCUS;
			   name = "long fillFocus";
			   break;
		/* 0 : Fraction 16.16 */
		case 397 : name = "long fillToLeft"; break;
		/* 0 : Fraction 16.16 */
		case 398 : name = "long fillToTop"; break;
		/* 0 : Fraction 16.16 */
		case 399 : name = "long fillToRight"; break;
		/* 0 : Fraction 16.16 */
		case 400 : name = "long fillToBottom"; break;
		/* 0 : For shaded fills, use the specified rectangle instead of
		 * the shape's bounding rect to define how large the fade is
		 * going to be. */
		case 401 : name = "long fillRectLeft"; break;
		/* 0 :  */
		case 402 : name = "long fillRectTop"; break;
		/* 0 :  */
		case 403 : name = "long fillRectRight"; break;
		/* 0 :  */
		case 404 : name = "long fillRectBottom"; break;
		/* measure_Default :  */
		case 405 : name = "LengthMeasure fillDztype"; break;
		/* 0 : Special shades */
		case 406 : id = MS_OBJ_ATTR_FILL_PRESET;
			   name = "long fillShadePreset";
			   break;
		/* NULL : a preset array of colors */
		case 407 : name = "IMsoArray fillShadeColors"; break;
		/* 0 :  */
		case 408 : name = "long fillOriginX"; break;
		/* 0 :  */
		case 409 : name = "long fillOriginY"; break;
		/* 0 :  */
		case 410 : name = "long fillShapeOriginX"; break;
		/* 0 :  */
		case 411 : name = "long fillShapeOriginY"; break;
		/* shade_Default : Type of shading, if a shaded (gradient) fill. */
		case 412 : id = MS_OBJ_ATTR_FILL_SHADE_TYPE;
			   name = "ShadeType fillShadeType";
			   break;

		case 443 : case 444 : case 445 : case 446 : case 447 :
			ms_escher_read_OPT_bools (h,
				fill_bools, G_N_ELEMENTS (fill_bools),
				pid, val);
			break;

	/* LineStyle */
		/* black : Color of line */
		case 448 : id = MS_OBJ_ATTR_OUTLINE_COLOR;
			   name = "Colour lineColor";
			   break;
		/* 1<<16 : Not implemented */
		case 449 : name = "long lineOpacity"; break;
		/* white : Background color */
		case 450 : name = "Colour lineBackColor"; break;
		/* undefined : Modification for BW views */
		case 451 : name = "Colour lineCrMod"; break;
		/* Solid : Type of line */
		case 452 : name = "LineFill lineType"; break;
		/* NULL : Pattern/texture */
		case 453 : name = "IMsoBlip* lineFillBlip"; break;
		/* NULL : Blip file name */
		case 454 : name = "wchar* lineFillBlipName"; break;
		/* Comment : Blip flags */
		case 455 : name = "BlipFlags lineFillBlipFlags"; break;
		/* 0 : How big (A units) to make a metafile texture. */
		case 456 : name = "long lineFillWidth"; break;
		/* 0 :  */
		case 457 : name = "long lineFillHeight"; break;
		/* measure_Default : How to interpret fillWidth/Height numbers. */
		case 458 : name = "LengthMeasure lineFillDztype"; break;
		/* 9525 : A units; 1pt == 12700 EMUs */
		case 459 : id = MS_OBJ_ATTR_OUTLINE_WIDTH;
			   name = "long lineWidth";
			   val = val * 256. / 12700.; /* convert to 256th of pt */
			   break;
		/* 8<<16 : ratio (16.16) of width */
		case 460 : name = "long lineMiterLimit"; break;
		/* Simple : Draw parallel lines? */
		case 461 : name = "LineStyle lineStyle"; break;
		/* Solid : Can be overridden by: */
		case 462 : id = MS_OBJ_ATTR_OUTLINE_STYLE;
			   name = "DashedLineStyle lineDashing";
			   break;
		/* NULL : As Win32 ExtCreatePen */
		case 463 : name = "IMsoArray lineDashStyle"; break;
		/* NoEnd : Arrow at start */
		case 464 : id = MS_OBJ_ATTR_ARROW_START;
			   name = "LineEndStyle lineStartArrowhead";
			   break;
		/* NoEnd : Arrow at end */
		case 465 : id = MS_OBJ_ATTR_ARROW_END;
			   name = "LineEndStyle lineEndArrowhead";
			   break;
		/* MediumWidthArrow : Arrow at start */
		case 466 : id = MS_OBJ_ATTR_ARROW_START_WIDTH;
			   name = "ArrowWidth lineStartArrowWidth"; break;
		/* MediumLenArrow : Arrow at end */
		case 467 : id = MS_OBJ_ATTR_ARROW_START_LENGTH;
			   name = "ArrowLength lineStartArrowLength"; break;
		/* MediumWidthArrow : Arrow at start */
		case 468 : id = MS_OBJ_ATTR_ARROW_END_WIDTH;
			   name = "ArrowWidth lineEndArrowWidth"; break;
		/* MediumLenArrow : Arrow at end */
		case 469 : id = MS_OBJ_ATTR_ARROW_END_LENGTH;
			   name = "ArrowLength lineEndArrowLength"; break;
		/* JoinRound : How to join lines */
		case 470 : name = "LineJoin lineJoinStyle"; break;
		/* EndCapFlat : How to end lines */
		case 471 : name = "LineCap lineEndCapStyle"; break;

		case 507 : case 508 : case 509 : case 510 : case 511 :
			ms_escher_read_OPT_bools (h,
				line_bools, G_N_ELEMENTS (line_bools),
				pid, val);
			break;

	/* ShadowStyle */
		/* Offset : Type of effect */
		case 512 : name = "Shadow shadowType"; break;
		/* 0x808080 : Foreground color */
		case 513 : name = "Colour shadowColor"; break;
		/* 0xCBCBCB : Embossed color */
		case 514 : name = "Colour shadowHighlight"; break;
		/* undefined : Modification for BW views */
		case 515 : name = "Colour shadowCrMod"; break;
		/* 1<<16 : Fixed 16.16 */
		case 516 : name = "long shadowOpacity"; break;
		/* 25400 : Offset shadow */
		case 517 : name = "long shadowOffsetX"; break;
		/* 25400 : Offset shadow */
		case 518 : name = "long shadowOffsetY"; break;
		/* 0 : Double offset shadow */
		case 519 : name = "long shadowSecondOffsetX"; break;
		/* 0 : Double offset shadow */
		case 520 : name = "long shadowSecondOffsetY"; break;
		/* 1<<16 : 16.16 */
		case 521 : name = "long shadowScaleXToX"; break;
		/* 0 : 16.16 */
		case 522 : name = "long shadowScaleYToX"; break;
		/* 0 : 16.16 */
		case 523 : name = "long shadowScaleXToY"; break;
		/* 1<<16 : 16.16 */
		case 524 : name = "long shadowScaleYToY"; break;
		/* 0 : 16.16 / weight */
		case 525 : name = "long shadowPerspectiveX"; break;
		/* 0 : 16.16 / weight */
		case 526 : name = "long shadowPerspectiveY"; break;
		/* 1<<8 : scaling factor */
		case 527 : name = "long shadowWeight"; break;
		/* 0 :  */
		case 528 : name = "long shadowOriginX"; break;
		/* 0 :  */
		case 529 : name = "long shadowOriginY"; break;
		case 574 : case 575 :
			ms_escher_read_OPT_bools (h,
				shadow_bools, G_N_ELEMENTS (shadow_bools),
				pid, val);
			break;

	/* PerspectiveStyle */
		/* Shape : Where transform applies */
		case 576 : name = "Transform perspectiveType"; break;
		/* 0 : The long values define a transformation matrix,
		 * effectively, each value is scaled by the perspectiveWeight
		 * parameter. */
		case 577 : name = "long perspectiveOffsetX"; break;
		/* 0 :  */
		case 578 : name = "long perspectiveOffsetY"; break;
		/* 1<<16 :  */
		case 579 : name = "long perspectiveScaleXToX"; break;
		/* 0 :  */
		case 580 : name = "long perspectiveScaleYToX"; break;
		/* 0 :  */
		case 581 : name = "long perspectiveScaleXToY"; break;
		/* 1<<16 :  */
		case 582 : name = "long perspectiveScaleYToY"; break;
		/* 0 :  */
		case 583 : name = "long perspectivePerspectiveX"; break;
		/* 0 :  */
		case 584 : name = "long perspectivePerspectiveY"; break;
		/* 1<<8 : Scaling factor */
		case 585 : name = "long perspectiveWeight"; break;
		/* 1<<15 :  */
		case 586 : name = "long perspectiveOriginX"; break;
		/* 1<<15 :  */
		case 587 : name = "long perspectiveOriginY"; break;
		/* FALSE : On/off */
		case 639 : name = "bool fPerspective"; break;

	/* 3D Object */
		/* 0 : Fixed-point 16.16 */
		case 640 : name = "long DSpecularAmt"; break;
		/* 65536 : Fixed-point 16.16 */
		case 641 : name = "long c3DDiffuseAmt"; break;
		/* 5 : Default gives OK results */
		case 642 : name = "long c3DShininess"; break;
		/* 12700 : Specular edge thickness */
		case 643 : name = "long c3DEdgeThickness"; break;
		/* 0 : Distance of extrusion in EMUs */
		case 644 : name = "long c3DExtrudeForward"; break;
		/* 457200 :  */
		case 645 : name = "long c3DExtrudeBackward"; break;
		/* 0 : Extrusion direction */
		case 646 : name = "long c3DExtrudePlane"; break;
		/* FillThenLine : Basic color of extruded part of shape; the
		 * lighting model used will determine the exact shades used
		 * when rendering.  */
		case 647 : name = "Colour c3DExtrusionColor"; break;
		/* undefined : Modification for BW views */
		case 648 : name = "Colour c3DCrMod"; break;
		case 700 : case 701 : case 702 : case 703 :
			ms_escher_read_OPT_bools (h,
				three_d_obj_bools, G_N_ELEMENTS (three_d_obj_bools),
				pid, val);
			break;

	/* 3D Style */
		/* 0 : degrees (16.16) about y axis */
		case 704 : name = "long c3DYRotationAngle"; break;
		/* 0 : degrees (16.16) about x axis */
		case 705 : name = "long c3DXRotationAngle"; break;
		/* 100 : These specify the rotation axis; only their relative magnitudes matter. */
		case 706 : name = "long c3DRotationAxisX"; break;
		/* 0 :  */
		case 707 : name = "long c3DRotationAxisY"; break;
		/* 0 :  */
		case 708 : name = "long c3DRotationAxisZ"; break;
		/* 0 : degrees (16.16) about axis */
		case 709 : name = "long c3DRotationAngle"; break;
		/* 0 : rotation center x (16.16 or g-units) */
		case 710 : name = "long c3DRotationCenterX"; break;
		/* 0 : rotation center y (16.16 or g-units) */
		case 711 : name = "long c3DRotationCenterY"; break;
		/* 0 : rotation center z (absolute (emus)) */
		case 712 : name = "long c3DRotationCenterZ"; break;
		/* FullRender : Full,wireframe, or bcube */
		case 713 : name = "RenderMode c3DRenderMode"; break;
		/* 30000 : pixels (16.16) */
		case 714 : name = "long c3DTolerance"; break;
		/* 1250000 : X view point (emus) */
		case 715 : name = "long c3DXViewpoint"; break;
		/* -1250000 : Y view point (emus) */
		case 716 : name = "long c3DYViewpoint"; break;
		/* 9000000 : Z view distance (emus) */
		case 717 : name = "long c3DZViewpoint"; break;
		/* 32768 :  */
		case 718 : name = "long c3DOriginX"; break;
		/* -32768 :  */
		case 719 : name = "long c3DOriginY"; break;
		/* -8847360 : degree (16.16) skew angle */
		case 720 : name = "long c3DSkewAngle"; break;
		/* 50 : Percentage skew amount */
		case 721 : name = "long c3DSkewAmount"; break;
		/* 20000 : Fixed point intensity */
		case 722 : name = "long c3DAmbientIntensity"; break;
		/* 50000 : Key light source direc- */
		case 723 : name = "long c3DKeyX"; break;
		/* 0 : tion; only their relative */
		case 724 : name = "long c3DKeyY"; break;
		/* 10000 : magnitudes matter */
		case 725 : name = "long c3DKeyZ"; break;
		/* 38000 : Fixed point intensity */
		case 726 : name = "long c3DKeyIntensity"; break;
		/* -50000 : Fill light source direc- */
		case 727 : name = "long c3DFillX"; break;
		/* 0 : tion; only their relative */
		case 728 : name = "long c3DFillY"; break;
		/* 10000 : magnitudes matter */
		case 729 : name = "long c3DFillZ"; break;
		/* 38000 : Fixed point intensity */
		case 730 : name = "long c3DFillIntensity"; break;

		case 763 : case 764 : case 765 : case 766 : case 767 :
			ms_escher_read_OPT_bools (h,
				three_d_style_bools, G_N_ELEMENTS (three_d_style_bools),
				pid, val);
			break;

	/* Shape */
		/* NULL : master shape */
		case 769 : name = "MSOHSP pMaster"; break;
		/* None : Type of connector */
		case 771 : name = "ConnectStyle"; break;
		/* Automatic : Settings for modifications to be made when in
		 * different forms of black-and-white mode. */
		case 772 : name = "BlackWhiteMode bWMode"; break;
		/* Automatic :  */
		case 773 : name = "BlackWhiteMode bWModePureBW"; break;
		/* Automatic :  */
		case 774 : name = "BlackWhiteMode bWModeBW"; break;
		case 826 : case 827 : case 828 : case 830 : case 831 :
			ms_escher_read_OPT_bools (h,
				shape_bools, G_N_ELEMENTS (shape_bools),
				pid, val);
			break;

	/* CallOut */
		/* TwoSegment : CalloutType */
		case 832 : name = "CalloutType spcot"; break;
		/* 1/12" : Distance from box to first point.(EMUs) */
		case 833 : name = "long dxyCalloutGap"; break;
		/* Any : Callout angle */
		case 834 : name = "CallOutAngle spcoa"; break;
		/* Specified : Callout drop type */
		case 835 : name = "CalloutDrop spcod"; break;
		/* 9 points : if msospcodSpecified, the actual drop distance */
		case 836 : name = "long dxyCalloutDropSpecified"; break;
		/* 0 : if fCalloutLengthSpecified, the actual distance */
		case 837 : name = "long dxyCalloutLengthSpecified"; break;

		case 889 : case 890 : case 891 : case 892 : case 893 :
		case 894 : case 895 :
			ms_escher_read_OPT_bools (h,
				callout_bools, G_N_ELEMENTS (callout_bools),
				pid, val);
			break;

	/* GroupShape */
		/* NULL : Shape Name (present only if explicitly set) */
		case 896 : id = MS_OBJ_ATTR_OBJ_NAME;
			   name = "wchar* wzName";
			   break;
		/* NULL : alternate text */
		case 897 : id = MS_OBJ_ATTR_OBJ_ALT_TEXT;
			   name = "wchar* wzDescription";
			   break;

		/* NULL : The hyperlink in the shape. */
		case 898 : name = "IHlink* pihlShape"; break;
		/* NULL : The polygon that text will be wrapped around (Word) */
		case 899 : name = "IMsoArray pWrapPolygonVertices"; break;
		/* 1/8" : Left wrapping distance from text (Word) */
		case 900 : name = "long dxWrapDistLeft"; break;
		/* 0 : Top wrapping distance from text (Word) */
		case 901 : name = "long dyWrapDistTop"; break;
		/* 1/8" : Right wrapping distance from text (Word) */
		case 902 : name = "long dxWrapDistRight"; break;
		/* 0 : Bottom wrapping distance from text (Word) */
		case 903 : name = "long dyWrapDistBottom"; break;
		/* 0 : Regroup ID  */
		case 904 : name = "long lidRegroup"; break;

		case 953 : case 954 : case 955 : case 956 : case 957 :
		case 958 : case 959 :
			ms_escher_read_OPT_bools (h,
				group_bools, G_N_ELEMENTS (group_bools),
				pid, val);
			break;

		default : name = "UnknownID";
		}

		d (0, {
			if (NULL != name)
				   g_printerr ("%s %d = 0x%08x (=%d) %s%s;\n", name, pid, val, val,
			      is_blip ? " is blip" : "",
					   is_complex ? " is complex" : "");
		});

		/* TODO : use this for something */
		if (is_complex) {
			/* check for over run */
			g_return_val_if_fail (extra + val - data + COMMON_HEADER_LEN <= h->len, TRUE);

			d (5, gsf_mem_dump (extra, val););
			d (11, {
				static int count = 0;
				char *name = g_strdup_printf ("gnumeric-complex-opt-[%d]-%d", pid, count++);
				FILE *f = g_fopen (name, "w");
				if (f != NULL) {
					fwrite (extra, 1, val, f);
					fclose (f);
				}
				g_free (name);
			});
			if (id & MS_OBJ_ATTR_IS_PTR_MASK) {
				char *s = g_utf16_to_utf8 ((gunichar2*)extra, val / 2,
							   NULL, NULL, NULL);
				ms_escher_header_add_attr (h,
					ms_obj_attr_new_ptr (id, s));
				id = MS_OBJ_ATTR_NONE;
			}
			extra += val;
		}

		if (id & MS_OBJ_ATTR_IS_INT_MASK)
			ms_escher_header_add_attr (h,
				ms_obj_attr_new_uint (id, val));
		else if (id != MS_OBJ_ATTR_NONE) {
			g_warning ("We know attr[%d] with id == 0x%x, but do not store it ?", pid, id);
		}
	}
	if (needs_free)
		g_free ((guint8 *)data);

	return FALSE;
}