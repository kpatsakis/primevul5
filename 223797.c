MagickExport size_t GetOptimalKernelWidth2D(const double radius,
  const double sigma)
{
  double
    alpha,
    beta,
    gamma,
    normalize,
    value;

  size_t
    width;

  ssize_t
    j,
    u,
    v;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  if (radius > MagickEpsilon)
    return((size_t) (2.0*ceil(radius)+1.0));
  gamma=fabs(sigma);
  if (gamma <= MagickEpsilon)
    return(3UL);
  alpha=PerceptibleReciprocal(2.0*gamma*gamma);
  beta=(double) PerceptibleReciprocal((double) Magick2PI*gamma*gamma);
  for (width=5; ; )
  {
    normalize=0.0;
    j=(ssize_t) (width-1)/2;
    for (v=(-j); v <= j; v++)
      for (u=(-j); u <= j; u++)
        normalize+=exp(-((double) (u*u+v*v))*alpha)*beta;
    value=exp(-((double) (j*j))*alpha)*beta/normalize;
    if ((value < QuantumScale) || (value < MagickEpsilon))
      break;
    width+=2;
  }
  return((size_t) (width-2));
}