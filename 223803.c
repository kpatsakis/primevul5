MagickExport void ConvertHSBToRGB(const double hue,const double saturation,
  const double brightness,Quantum *red,Quantum *green,Quantum *blue)
{
  double
    f,
    h,
    p,
    q,
    t;

  /*
    Convert HSB to RGB colorspace.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  if (fabs(saturation) < MagickEpsilon)
    {
      *red=ClampToQuantum(QuantumRange*brightness);
      *green=(*red);
      *blue=(*red);
      return;
    }
  h=6.0*(hue-floor(hue));
  f=h-floor((double) h);
  p=brightness*(1.0-saturation);
  q=brightness*(1.0-saturation*f);
  t=brightness*(1.0-(saturation*(1.0-f)));
  switch ((int) h)
  {
    case 0:
    default:
    {
      *red=ClampToQuantum(QuantumRange*brightness);
      *green=ClampToQuantum(QuantumRange*t);
      *blue=ClampToQuantum(QuantumRange*p);
      break;
    }
    case 1:
    {
      *red=ClampToQuantum(QuantumRange*q);
      *green=ClampToQuantum(QuantumRange*brightness);
      *blue=ClampToQuantum(QuantumRange*p);
      break;
    }
    case 2:
    {
      *red=ClampToQuantum(QuantumRange*p);
      *green=ClampToQuantum(QuantumRange*brightness);
      *blue=ClampToQuantum(QuantumRange*t);
      break;
    }
    case 3:
    {
      *red=ClampToQuantum(QuantumRange*p);
      *green=ClampToQuantum(QuantumRange*q);
      *blue=ClampToQuantum(QuantumRange*brightness);
      break;
    }
    case 4:
    {
      *red=ClampToQuantum(QuantumRange*t);
      *green=ClampToQuantum(QuantumRange*p);
      *blue=ClampToQuantum(QuantumRange*brightness);
      break;
    }
    case 5:
    {
      *red=ClampToQuantum(QuantumRange*brightness);
      *green=ClampToQuantum(QuantumRange*p);
      *blue=ClampToQuantum(QuantumRange*q);
      break;
    }
  }
}