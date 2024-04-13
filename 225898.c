MagickExport MagickBooleanType PlasmaImageProxy(Image *image,
  CacheView *image_view,CacheView *u_view,CacheView *v_view,
  RandomInfo *magick_restrict random_info,
  const SegmentInfo *magick_restrict segment,size_t attenuate,size_t depth)
{
  ExceptionInfo
    *exception;

  double
    plasma;

  MagickStatusType
    status;

  PixelPacket
    u,
    v;

  ssize_t
    x,
    x_mid,
    y,
    y_mid;

  if ((fabs(segment->x2-segment->x1) < MagickEpsilon) &&
      (fabs(segment->y2-segment->y1) < MagickEpsilon))
    return(MagickTrue);
  if (depth != 0)
    {
      SegmentInfo
        local_info;

      /*
        Divide the area into quadrants and recurse.
      */
      depth--;
      attenuate++;
      x_mid=CastDoubleToLong(ceil((segment->x1+segment->x2)/2-0.5));
      y_mid=CastDoubleToLong(ceil((segment->y1+segment->y2)/2-0.5));
      local_info=(*segment);
      local_info.x2=(double) x_mid;
      local_info.y2=(double) y_mid;
      status=PlasmaImageProxy(image,image_view,u_view,v_view,random_info,
        &local_info,attenuate,depth);
      local_info=(*segment);
      local_info.y1=(double) y_mid;
      local_info.x2=(double) x_mid;
      status&=PlasmaImageProxy(image,image_view,u_view,v_view,random_info,
        &local_info,attenuate,depth);
      local_info=(*segment);
      local_info.x1=(double) x_mid;
      local_info.y2=(double) y_mid;
      status&=PlasmaImageProxy(image,image_view,u_view,v_view,random_info,
        &local_info,attenuate,depth);
      local_info=(*segment);
      local_info.x1=(double) x_mid;
      local_info.y1=(double) y_mid;
      status&=PlasmaImageProxy(image,image_view,u_view,v_view,random_info,
        &local_info,attenuate,depth);
      return(status == 0 ? MagickFalse : MagickTrue);
    }
  x_mid=CastDoubleToLong(ceil((segment->x1+segment->x2)/2-0.5));
  y_mid=CastDoubleToLong(ceil((segment->y1+segment->y2)/2-0.5));
  if ((fabs(segment->x1-x_mid) < MagickEpsilon) &&
      (fabs(segment->x2-x_mid) < MagickEpsilon) &&
      (fabs(segment->y1-y_mid) < MagickEpsilon) &&
      (fabs(segment->y2-y_mid) < MagickEpsilon))
    return(MagickFalse);
  /*
    Average pixels and apply plasma.
  */
  status=MagickTrue;
  exception=(&image->exception);
  plasma=(double) QuantumRange/(2.0*attenuate);
  if ((fabs(segment->x1-x_mid) >= MagickEpsilon) ||
      (fabs(segment->x2-x_mid) >= MagickEpsilon))
    {
      PixelPacket
        *magick_restrict q;

      /*
        Left pixel.
      */
      x=CastDoubleToLong(ceil(segment->x1-0.5));
      (void) GetOneCacheViewVirtualPixel(u_view,x,CastDoubleToLong(ceil(
        segment->y1-0.5)),&u,exception);
      (void) GetOneCacheViewVirtualPixel(v_view,x,CastDoubleToLong(ceil(
        segment->y2-0.5)),&v,exception);
      q=QueueCacheViewAuthenticPixels(image_view,x,y_mid,1,1,exception);
      if (q == (PixelPacket *) NULL)
        return(MagickTrue);
      SetPixelRed(q,PlasmaPixel(random_info,((MagickRealType) u.red+
        v.red)/2.0,plasma));
      SetPixelGreen(q,PlasmaPixel(random_info,((MagickRealType) u.green+
        v.green)/2.0,plasma));
      SetPixelBlue(q,PlasmaPixel(random_info,((MagickRealType) u.blue+
        v.blue)/2.0,plasma));
      status=SyncCacheViewAuthenticPixels(image_view,exception);
      if (fabs(segment->x1-segment->x2) >= MagickEpsilon)
        {
          /*
            Right pixel.
          */
          x=CastDoubleToLong(ceil(segment->x2-0.5));
          (void) GetOneCacheViewVirtualPixel(u_view,x,CastDoubleToLong(ceil(
            segment->y1-0.5)),&u,exception);
          (void) GetOneCacheViewVirtualPixel(v_view,x,CastDoubleToLong(ceil(
            segment->y2-0.5)),&v,exception);
          q=QueueCacheViewAuthenticPixels(image_view,x,y_mid,1,1,exception);
          if (q == (PixelPacket *) NULL)
            return(MagickFalse);
          SetPixelRed(q,PlasmaPixel(random_info,((MagickRealType) u.red+
            v.red)/2.0,plasma));
          SetPixelGreen(q,PlasmaPixel(random_info,((MagickRealType) u.green+
            v.green)/2.0,plasma));
          SetPixelBlue(q,PlasmaPixel(random_info,((MagickRealType) u.blue+
            v.blue)/2.0,plasma));
          status=SyncCacheViewAuthenticPixels(image_view,exception);
        }
    }
  if ((fabs(segment->y1-y_mid) >= MagickEpsilon) ||
      (fabs(segment->y2-y_mid) >= MagickEpsilon))
    {
      if ((fabs(segment->x1-x_mid) >= MagickEpsilon) ||
          (fabs(segment->y2-y_mid) >= MagickEpsilon))
        {
          PixelPacket
            *magick_restrict q;

          /*
            Bottom pixel.
          */
          y=CastDoubleToLong(ceil(segment->y2-0.5));
          (void) GetOneCacheViewVirtualPixel(u_view,CastDoubleToLong(ceil(
            segment->x1-0.5)),y,&u,exception);
          (void) GetOneCacheViewVirtualPixel(v_view,CastDoubleToLong(ceil(
            segment->x2-0.5)),y,&v,exception);
          q=QueueCacheViewAuthenticPixels(image_view,x_mid,y,1,1,exception);
          if (q == (PixelPacket *) NULL)
            return(MagickTrue);
          SetPixelRed(q,PlasmaPixel(random_info,((MagickRealType) u.red+
            v.red)/2.0,plasma));
          SetPixelGreen(q,PlasmaPixel(random_info,((MagickRealType) u.green+
            v.green)/2.0,plasma));
          SetPixelBlue(q,PlasmaPixel(random_info,((MagickRealType) u.blue+
            v.blue)/2.0,plasma));
          status=SyncCacheViewAuthenticPixels(image_view,exception);
        }
      if (fabs(segment->y1-segment->y2) >= MagickEpsilon)
        {
          PixelPacket
            *magick_restrict q;

          /*
            Top pixel.
          */
          y=CastDoubleToLong(ceil(segment->y1-0.5));
          (void) GetOneCacheViewVirtualPixel(u_view,CastDoubleToLong(ceil(
            segment->x1-0.5)),y,&u,exception);
          (void) GetOneCacheViewVirtualPixel(v_view,CastDoubleToLong(ceil(
            segment->x2-0.5)),y,&v,exception);
          q=QueueCacheViewAuthenticPixels(image_view,x_mid,y,1,1,exception);
          if (q == (PixelPacket *) NULL)
            return(MagickTrue);
          SetPixelRed(q,PlasmaPixel(random_info,((MagickRealType) u.red+
            v.red)/2.0,plasma));
          SetPixelGreen(q,PlasmaPixel(random_info,((MagickRealType) u.green+
            v.green)/2.0,plasma));
          SetPixelBlue(q,PlasmaPixel(random_info,((MagickRealType) u.blue+
            v.blue)/2.0,plasma));
          status=SyncCacheViewAuthenticPixels(image_view,exception);
        }
    }
  if ((fabs(segment->x1-segment->x2) >= MagickEpsilon) ||
      (fabs(segment->y1-segment->y2) >= MagickEpsilon))
    {
      PixelPacket
        *magick_restrict q;

      /*
        Middle pixel.
      */
      x=CastDoubleToLong(ceil(segment->x1-0.5));
      y=CastDoubleToLong(ceil(segment->y1-0.5));
      (void) GetOneCacheViewVirtualPixel(u_view,x,y,&u,exception);
      x=CastDoubleToLong(ceil(segment->x2-0.5));
      y=CastDoubleToLong(ceil(segment->y2-0.5));
      (void) GetOneCacheViewVirtualPixel(v_view,x,y,&v,exception);
      q=QueueCacheViewAuthenticPixels(image_view,x_mid,y_mid,1,1,exception);
      if (q == (PixelPacket *) NULL)
        return(MagickTrue);
      SetPixelRed(q,PlasmaPixel(random_info,((MagickRealType) u.red+
        v.red)/2.0,plasma));
      SetPixelGreen(q,PlasmaPixel(random_info,((MagickRealType) u.green+
        v.green)/2.0,plasma));
      SetPixelBlue(q,PlasmaPixel(random_info,((MagickRealType) u.blue+
        v.blue)/2.0,plasma));
      status=SyncCacheViewAuthenticPixels(image_view,exception);
    }
  if ((fabs(segment->x2-segment->x1) < 3.0) &&
      (fabs(segment->y2-segment->y1) < 3.0))
    return(status == 0 ? MagickFalse : MagickTrue);
  return(MagickFalse);
}