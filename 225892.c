MagickExport MagickBooleanType PlasmaImage(Image *image,
  const SegmentInfo *segment,size_t attenuate,size_t depth)
{
  CacheView
    *image_view,
    *u_view,
    *v_view;

  MagickBooleanType
    status;

  RandomInfo
    *random_info;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    return(MagickFalse);
  image_view=AcquireAuthenticCacheView(image,&image->exception);
  u_view=AcquireVirtualCacheView(image,&image->exception);
  v_view=AcquireVirtualCacheView(image,&image->exception);
  random_info=AcquireRandomInfo();
  status=PlasmaImageProxy(image,image_view,u_view,v_view,random_info,segment,
    attenuate,depth);
  random_info=DestroyRandomInfo(random_info);
  v_view=DestroyCacheView(v_view);
  u_view=DestroyCacheView(u_view);
  image_view=DestroyCacheView(image_view);
  return(status);
}