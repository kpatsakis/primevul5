static ssize_t GetMedianPixel(Quantum *pixels,const size_t n)
{
#define SwapPixels(alpha,beta) \
{ \
  Quantum gamma=(alpha); \
  (alpha)=(beta);(beta)=gamma; \
}

  ssize_t
    low = 0,
    high = (ssize_t) n-1,
    median = (low+high)/2;

  for ( ; ; )
  {
    ssize_t
      l = low+1,
      h = high,
      mid = (low+high)/2;

    if (high <= low)
      return(median);
    if (high == (low+1))
      {
        if (pixels[low] > pixels[high])
          SwapPixels(pixels[low],pixels[high]);
        return(median);
      }
    if (pixels[mid] > pixels[high])
      SwapPixels(pixels[mid],pixels[high]);
    if (pixels[low] > pixels[high])
      SwapPixels(pixels[low], pixels[high]);
    if (pixels[mid] > pixels[low])
      SwapPixels(pixels[mid],pixels[low]);
    SwapPixels(pixels[mid],pixels[low+1]);
    for ( ; ; )
    {
      do l++; while (pixels[low] > pixels[l]);
      do h--; while (pixels[h] > pixels[low]);
      if (h < l)
        break;
      SwapPixels(pixels[l],pixels[h]);
    }
    SwapPixels(pixels[low],pixels[h]);
    if (h <= median)
      low=l;
    if (h >= median)
      high=h-1;
  }
}