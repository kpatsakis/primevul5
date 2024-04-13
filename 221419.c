MagickExport void RemoveZeroDelayLayers(Image **images,
     ExceptionInfo *exception)
{
  Image
    *i;

  assert((*images) != (const Image *) NULL);
  assert((*images)->signature == MagickCoreSignature);
  if ((*images)->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",(*images)->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);

  i=GetFirstImageInList(*images);
  for ( ; i != (Image *) NULL; i=GetNextImageInList(i))
    if ( i->delay != 0L ) break;
  if ( i == (Image *) NULL ) {
    (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
       "ZeroTimeAnimation","`%s'",GetFirstImageInList(*images)->filename);
    return;
  }
  i=GetFirstImageInList(*images);
  while ( i != (Image *) NULL )
  {
    if ( i->delay == 0L ) {
      (void) DeleteImageFromList(&i);
      *images=i;
    }
    else
      i=GetNextImageInList(i);
  }
  *images=GetFirstImageInList(*images);
}