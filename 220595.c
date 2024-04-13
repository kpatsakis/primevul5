static inline void HatTransform(const float *magick_restrict pixels,
  const size_t stride,const size_t extent,const size_t scale,float *kernel)
{
  const float
    *magick_restrict p,
    *magick_restrict q,
    *magick_restrict r;

  ssize_t
    i;

  p=pixels;
  q=pixels+scale*stride;
  r=pixels+scale*stride;
  for (i=0; i < (ssize_t) scale; i++)
  {
    kernel[i]=0.25f*(*p+(*p)+(*q)+(*r));
    p+=stride;
    q-=stride;
    r+=stride;
  }
  for ( ; i < (ssize_t) (extent-scale); i++)
  {
    kernel[i]=0.25f*(2.0f*(*p)+*(p-scale*stride)+*(p+scale*stride));
    p+=stride;
  }
  q=p-scale*stride;
  r=pixels+stride*(extent-2);
  for ( ; i < (ssize_t) extent; i++)
  {
    kernel[i]=0.25f*(*p+(*p)+(*q)+(*r));
    p+=stride;
    q+=stride;
    r-=stride;
  }
}