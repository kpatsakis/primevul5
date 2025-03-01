MagickExport void RemoveDuplicateLayers(Image **images,ExceptionInfo *exception)
{
  RectangleInfo
    bounds;

  register Image
    *image,
    *next;

  assert((*images) != (const Image *) NULL);
  assert((*images)->signature == MagickCoreSignature);
  if ((*images)->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      (*images)->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=GetFirstImageInList(*images);
  for ( ; (next=GetNextImageInList(image)) != (Image *) NULL; image=next)
  {
    if ((image->columns != next->columns) || (image->rows != next->rows) ||
        (image->page.x != next->page.x) || (image->page.y != next->page.y))
      continue;
    bounds=CompareImagesBounds(image,next,CompareAnyLayer,exception);
    if (bounds.x < 0)
      {
        /*
          Two images are the same, merge time delays and delete one.
        */
        size_t
          time;

        time=(size_t) (1000.0*image->delay*
          PerceptibleReciprocal((double) image->ticks_per_second));
        time+=(size_t) (1000.0*next->delay*
          PerceptibleReciprocal((double) next->ticks_per_second));
        next->ticks_per_second=100L;
        next->delay=time*image->ticks_per_second/1000;
        next->iterations=image->iterations;
        *images=image;
        (void) DeleteImageFromList(images);
      }
  }
  *images=GetFirstImageInList(*images);
}