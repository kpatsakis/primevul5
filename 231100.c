static ssize_t SmushXGap(const Image *smush_image,const Image *images,
  const ssize_t offset,ExceptionInfo *exception)
{
  CacheView
    *left_view,
    *right_view;

  const Image
    *left_image,
    *right_image;

  RectangleInfo
    left_geometry,
    right_geometry;

  register const PixelPacket
    *p;

  register ssize_t
    i,
    y;

  size_t
    gap;

  ssize_t
    x;

  if (images->previous == (Image *) NULL)
    return(0);
  right_image=images;
  SetGeometry(smush_image,&right_geometry);
  GravityAdjustGeometry(right_image->columns,right_image->rows,
    right_image->gravity,&right_geometry);
  left_image=images->previous;
  SetGeometry(smush_image,&left_geometry);
  GravityAdjustGeometry(left_image->columns,left_image->rows,
    left_image->gravity,&left_geometry);
  gap=right_image->columns;
  left_view=AcquireVirtualCacheView(left_image,exception);
  right_view=AcquireVirtualCacheView(right_image,exception);
  for (y=0; y < (ssize_t) smush_image->rows; y++)
  {
    for (x=(ssize_t) left_image->columns-1; x > 0; x--)
    {
      p=GetCacheViewVirtualPixels(left_view,x,left_geometry.y+y,1,1,exception);
      if ((p == (const PixelPacket *) NULL) ||
          (GetPixelOpacity(p) != TransparentOpacity) ||
          ((left_image->columns-x-1) >= gap))
        break;
    }
    i=(ssize_t) left_image->columns-x-1;
    for (x=0; x < (ssize_t) right_image->columns; x++)
    {
      p=GetCacheViewVirtualPixels(right_view,x,right_geometry.y+y,1,1,
        exception);
      if ((p == (const PixelPacket *) NULL) ||
          (GetPixelOpacity(p) != TransparentOpacity) ||
          ((x+i) >= (ssize_t) gap))
        break;
    }
    if ((x+i) < (ssize_t) gap)
      gap=(size_t) (x+i);
  }
  right_view=DestroyCacheView(right_view);
  left_view=DestroyCacheView(left_view);
  if (y < (ssize_t) smush_image->rows)
    return(offset);
  return((ssize_t) gap-offset);
}