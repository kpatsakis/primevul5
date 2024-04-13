static inline Quantum PlasmaPixel(RandomInfo *magick_restrict random_info,
  const double pixel,const double noise)
{
  MagickRealType
    plasma;

  plasma=pixel+noise*GetPseudoRandomValue(random_info)-noise/2.0;
  return(ClampToQuantum(plasma));
}