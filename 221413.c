static inline void CompositeCanvas(Image *destination,
  const CompositeOperator compose,Image *source,ssize_t x_offset,
  ssize_t y_offset,ExceptionInfo *exception)
{
  const char
    *value;

  x_offset+=source->page.x-destination->page.x;
  y_offset+=source->page.y-destination->page.y;
  value=GetImageArtifact(source,"compose:outside-overlay");
  (void) CompositeImage(destination,source,compose,
    (value != (const char *) NULL) && (IsStringTrue(value) != MagickFalse) ?
    MagickFalse : MagickTrue,x_offset,y_offset,exception);
}