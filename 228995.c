CairoFont *CairoFont::create(GfxFont *gfxFont, XRef *xref, FT_Library lib, GBool useCIDs) {
  Ref embRef;
  Object refObj, strObj;
  GooString *tmpFileName, *fileName,*tmpFileName2;
  DisplayFontParam *dfp;
  FILE *tmpFile;
  int c, i, n;
  GfxFontType fontType;
  char **enc;
  char *name;
  FoFiTrueType *ff;
  FoFiType1C *ff1c;
  Ref ref;
  static cairo_user_data_key_t cairo_font_face_key;
  cairo_font_face_t *cairo_font_face;
  FT_Face face;

  Gushort *codeToGID;
  int codeToGIDLen;
  
  dfp = NULL;
  codeToGID = NULL;
  codeToGIDLen = 0;
  cairo_font_face = NULL;
  
  ref = *gfxFont->getID();
  fontType = gfxFont->getType();

  tmpFileName = NULL;

  if (gfxFont->getEmbeddedFontID(&embRef)) {
    if (!openTempFile(&tmpFileName, &tmpFile, "wb", NULL)) {
      error(-1, "Couldn't create temporary font file");
      goto err2;
    }
    
    refObj.initRef(embRef.num, embRef.gen);
    refObj.fetch(xref, &strObj);
    refObj.free();
    if (!strObj.isStream()) {
      error(-1, "Embedded font object is wrong type");
      strObj.free();
      fclose(tmpFile);
      goto err2;
    }
    strObj.streamReset();
    while ((c = strObj.streamGetChar()) != EOF) {
      fputc(c, tmpFile);
    }
    strObj.streamClose();
    strObj.free();
    fclose(tmpFile);
    fileName = tmpFileName;
    
  } else if (!(fileName = gfxFont->getExtFontFile())) {
    // look for a display font mapping or a substitute font
    dfp = NULL;
    if (gfxFont->getName()) {
      dfp = globalParams->getDisplayFont(gfxFont);
    }
    if (!dfp) {
      error(-1, "Couldn't find a font for '%s'",
	    gfxFont->getName() ? gfxFont->getName()->getCString()
	    : "(unnamed)");
      goto err2;
    }
    switch (dfp->kind) {
    case displayFontT1:
      fileName = dfp->t1.fileName;
      fontType = gfxFont->isCIDFont() ? fontCIDType0 : fontType1;
      break;
    case displayFontTT:
      fileName = dfp->tt.fileName;
      fontType = gfxFont->isCIDFont() ? fontCIDType2 : fontTrueType;
      break;
    }
  }

  switch (fontType) {
  case fontType1:
  case fontType1C:
    if (FT_New_Face(lib, fileName->getCString(), 0, &face)) {
      error(-1, "could not create type1 face");
      goto err2;
    }
    
    enc = ((Gfx8BitFont *)gfxFont)->getEncoding();
    
    codeToGID = (Gushort *)gmallocn(256, sizeof(int));
    codeToGIDLen = 256;
    for (i = 0; i < 256; ++i) {
      codeToGID[i] = 0;
      if ((name = enc[i])) {
	codeToGID[i] = (Gushort)FT_Get_Name_Index(face, name);
      }
    }
    break;
    
  case fontCIDType2:
    codeToGID = NULL;
    n = 0;
    if (((GfxCIDFont *)gfxFont)->getCIDToGID()) {
      n = ((GfxCIDFont *)gfxFont)->getCIDToGIDLen();
      if (n) {
	codeToGID = (Gushort *)gmallocn(n, sizeof(Gushort));
	memcpy(codeToGID, ((GfxCIDFont *)gfxFont)->getCIDToGID(),
		n * sizeof(Gushort));
      }
    } else {
      ff = FoFiTrueType::load(fileName->getCString());
      if (! ff)
	goto err2;
      codeToGID = ((GfxCIDFont *)gfxFont)->getCodeToGIDMap(ff, &n);
      delete ff;
    }
    codeToGIDLen = n;
    /* Fall through */
  case fontTrueType:
    if (!(ff = FoFiTrueType::load(fileName->getCString()))) {
      error(-1, "failed to load truetype font\n");
      goto err2;
    }
    /* This might be set already for the CIDType2 case */
    if (fontType == fontTrueType) {
      codeToGID = ((Gfx8BitFont *)gfxFont)->getCodeToGIDMap(ff);
      codeToGIDLen = 256;
    }
    if (!openTempFile(&tmpFileName2, &tmpFile, "wb", NULL)) {
      delete ff;
      error(-1, "failed to open truetype tempfile\n");
      goto err2;
    }
    ff->writeTTF(&fileWrite, tmpFile);
    fclose(tmpFile);
    delete ff;

    if (FT_New_Face(lib, tmpFileName2->getCString(), 0, &face)) {
      error(-1, "could not create truetype face\n");
      goto err2;
    }
    unlink (tmpFileName2->getCString());
    delete tmpFileName2;
    break;
    
  case fontCIDType0:
  case fontCIDType0C:

    codeToGID = NULL;
    codeToGIDLen = 0;

    if (!useCIDs)
    {
      if ((ff1c = FoFiType1C::load(fileName->getCString()))) {
        codeToGID = ff1c->getCIDToGIDMap(&codeToGIDLen);
        delete ff1c;
      }
    }

    if (FT_New_Face(lib, fileName->getCString(), 0, &face)) {
      gfree(codeToGID);
      codeToGID = NULL;
      error(-1, "could not create cid face\n");
      goto err2;
    }
    break;
    
  default:
    printf ("font type not handled\n");
    goto err2;
    break;
  }

  // delete the (temporary) font file -- with Unix hard link
  // semantics, this will remove the last link; otherwise it will
  // return an error, leaving the file to be deleted later
  if (fileName == tmpFileName) {
    unlink (fileName->getCString());
    delete tmpFileName;
  }

  cairo_font_face = cairo_ft_font_face_create_for_ft_face (face,
							   FT_LOAD_NO_HINTING |
							   FT_LOAD_NO_BITMAP);
  if (cairo_font_face == NULL) {
    error(-1, "could not create cairo font\n");
    goto err2; /* this doesn't do anything, but it looks like we're
		* handling the error */
  } {
  CairoFont *ret = new CairoFont(ref, cairo_font_face, face, codeToGID, codeToGIDLen);
  cairo_font_face_set_user_data (cairo_font_face, 
				 &cairo_font_face_key,
				 ret,
				 cairo_font_face_destroy);

  return ret;
  }
 err2:
  /* hmm? */
  printf ("some font thing failed\n");
  return NULL;
}