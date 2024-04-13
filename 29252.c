DeviceNRecoder::DeviceNRecoder(Stream *strA, int widthA, int heightA,
			       GfxImageColorMap *colorMapA):
    FilterStream(strA) {
  width = widthA;
  height = heightA;
  colorMap = colorMapA;
  imgStr = NULL;
  pixelIdx = 0;
  bufIdx = gfxColorMaxComps;
  bufSize = ((GfxDeviceNColorSpace *)colorMap->getColorSpace())->
              getAlt()->getNComps();
  func = ((GfxDeviceNColorSpace *)colorMap->getColorSpace())->
           getTintTransformFunc();
}
