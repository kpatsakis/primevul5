MagickExport void ConvertHSVToRGB(const double hue,const double saturation,
  const double value,Quantum *red,Quantum *green,Quantum *blue)
{
  double
    b,
    c,
    g,
    h,
    min,
    r,
    x;

  /*
    Convert HSV to RGB colorspace.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  h=hue*360.0;
  c=value*saturation;
  min=value-c;
  h-=360.0*floor(h/360.0);
  h/=60.0;
  x=c*(1.0-fabs(h-2.0*floor(h/2.0)-1.0));
  switch ((int) floor(h))
  {
    case 0:
    {
      r=min+c;
      g=min+x;
      b=min;
      break;
    }
    case 1:
    {
      r=min+x;
      g=min+c;
      b=min;
      break;
    }
    case 2:
    {
      r=min;
      g=min+c;
      b=min+x;
      break;
    }
    case 3:
    {
      r=min;
      g=min+x;
      b=min+c;
      break;
    }
    case 4:
    {
      r=min+x;
      g=min;
      b=min+c;
      break;
    }
    case 5:
    {
      r=min+c;
      g=min;
      b=min+x;
      break;
    }
    default:
    {
      r=0.0;
      g=0.0;
      b=0.0;
    }
  }
  *red=ClampToQuantum(QuantumRange*r);
  *green=ClampToQuantum(QuantumRange*g);
  *blue=ClampToQuantum(QuantumRange*b);
}