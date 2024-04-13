PSOutputDev::PSOutputDev(const char *fileName, XRef *xrefA, Catalog *catalog,
			 char *psTitle,
			 int firstPage, int lastPage, PSOutMode modeA,
			 int paperWidthA, int paperHeightA, GBool duplexA,
			 int imgLLXA, int imgLLYA, int imgURXA, int imgURYA,
			 GBool forceRasterizeA,
			 GBool manualCtrlA) {
  FILE *f;
  PSFileType fileTypeA;

  underlayCbk = NULL;
  underlayCbkData = NULL;
  overlayCbk = NULL;
  overlayCbkData = NULL;

  fontIDs = NULL;
  fontFileIDs = NULL;
  fontFileNames = NULL;
  font8Info = NULL;
  font16Enc = NULL;
  imgIDs = NULL;
  formIDs = NULL;
  xobjStack = NULL;
  embFontList = NULL;
  customColors = NULL;
  haveTextClip = gFalse;
  t3String = NULL;

  forceRasterize = forceRasterizeA;

  if (!strcmp(fileName, "-")) {
    fileTypeA = psStdout;
    f = stdout;
  } else if (fileName[0] == '|') {
    fileTypeA = psPipe;
#ifdef HAVE_POPEN
#ifndef WIN32
    signal(SIGPIPE, (SignalFunc)SIG_IGN);
#endif
    if (!(f = popen(fileName + 1, "w"))) {
      error(-1, "Couldn't run print command '%s'", fileName);
      ok = gFalse;
      return;
    }
#else
    error(-1, "Print commands are not supported ('%s')", fileName);
    ok = gFalse;
    return;
#endif
  } else {
    fileTypeA = psFile;
    if (!(f = fopen(fileName, "w"))) {
      error(-1, "Couldn't open PostScript file '%s'", fileName);
      ok = gFalse;
      return;
    }
  }

  init(outputToFile, f, fileTypeA, psTitle,
       xrefA, catalog, firstPage, lastPage, modeA,
       imgLLXA, imgLLYA, imgURXA, imgURYA, manualCtrlA,
       paperWidthA, paperHeightA, duplexA);
}
