MagickExport void ConvertRGBToLCHab(const Quantum red,const Quantum green,
  const Quantum blue,double *luma,double *chroma,double *hue)
{
  double
    X,
    Y,
    Z;

  /*
   Convert RGB to LCHab colorspace.
  */
  assert(luma != (double *) NULL);
  assert(chroma != (double *) NULL);
  assert(hue != (double *) NULL);
  ConvertRGBToXYZ(red,green,blue,&X,&Y,&Z);
  ConvertXYZToLCHab(X,Y,Z,luma,chroma,hue);
}