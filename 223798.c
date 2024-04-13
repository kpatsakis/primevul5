MagickExport void ConvertRGBToHSI(const Quantum red,const Quantum green,
  const Quantum blue,double *hue,double *saturation,double *intensity)
{
  double
    alpha,
    beta;

  /*
    Convert RGB to HSI colorspace.
  */
  assert(hue != (double *) NULL);
  assert(saturation != (double *) NULL);
  assert(intensity != (double *) NULL);
  *intensity=(QuantumScale*red+QuantumScale*green+QuantumScale*blue)/3.0;
  if (*intensity <= 0.0)
    {
      *hue=0.0;
      *saturation=0.0;
      return;
    }
  *saturation=1.0-MagickMin(QuantumScale*red,MagickMin(QuantumScale*green,
    QuantumScale*blue))/(*intensity);
  alpha=0.5*(2.0*QuantumScale*red-QuantumScale*green-QuantumScale*blue);
  beta=0.8660254037844385*(QuantumScale*green-QuantumScale*blue);
  *hue=atan2(beta,alpha)*(180.0/MagickPI)/360.0;
  if (*hue < 0.0)
    *hue+=1.0;
}