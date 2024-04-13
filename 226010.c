MagickExport ChannelMoments *GetImageMoments(const Image *image,
  ExceptionInfo *exception)
{
#define MaxNumberImageMoments  8

  CacheView
    *image_view;

  ChannelMoments
    *channel_moments;

  double
    M00[MaxPixelChannels+1],
    M01[MaxPixelChannels+1],
    M02[MaxPixelChannels+1],
    M03[MaxPixelChannels+1],
    M10[MaxPixelChannels+1],
    M11[MaxPixelChannels+1],
    M12[MaxPixelChannels+1],
    M20[MaxPixelChannels+1],
    M21[MaxPixelChannels+1],
    M22[MaxPixelChannels+1],
    M30[MaxPixelChannels+1];

  PointInfo
    centroid[MaxPixelChannels+1];

  ssize_t
    channel,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  channel_moments=(ChannelMoments *) AcquireQuantumMemory(MaxPixelChannels+1,
    sizeof(*channel_moments));
  if (channel_moments == (ChannelMoments *) NULL)
    return(channel_moments);
  (void) memset(channel_moments,0,(MaxPixelChannels+1)*
    sizeof(*channel_moments));
  (void) memset(centroid,0,sizeof(centroid));
  (void) memset(M00,0,sizeof(M00));
  (void) memset(M01,0,sizeof(M01));
  (void) memset(M02,0,sizeof(M02));
  (void) memset(M03,0,sizeof(M03));
  (void) memset(M10,0,sizeof(M10));
  (void) memset(M11,0,sizeof(M11));
  (void) memset(M12,0,sizeof(M12));
  (void) memset(M20,0,sizeof(M20));
  (void) memset(M21,0,sizeof(M21));
  (void) memset(M22,0,sizeof(M22));
  (void) memset(M30,0,sizeof(M30));
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    const Quantum
      *magick_restrict p;

    ssize_t
      x;

    /*
      Compute center of mass (centroid).
    */
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if (traits == UndefinedPixelTrait)
          continue;
        if ((traits & UpdatePixelTrait) == 0)
          continue;
        M00[channel]+=QuantumScale*p[i];
        M00[MaxPixelChannels]+=QuantumScale*p[i];
        M10[channel]+=x*QuantumScale*p[i];
        M10[MaxPixelChannels]+=x*QuantumScale*p[i];
        M01[channel]+=y*QuantumScale*p[i];
        M01[MaxPixelChannels]+=y*QuantumScale*p[i];
      }
      p+=GetPixelChannels(image);
    }
  }
  for (channel=0; channel <= MaxPixelChannels; channel++)
  {
    /*
       Compute center of mass (centroid).
    */
    centroid[channel].x=M10[channel]*PerceptibleReciprocal(M00[channel]);
    centroid[channel].y=M01[channel]*PerceptibleReciprocal(M00[channel]);
  }
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    const Quantum
      *magick_restrict p;

    ssize_t
      x;

    /*
      Compute the image moments.
    */
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if (traits == UndefinedPixelTrait)
          continue;
        if ((traits & UpdatePixelTrait) == 0)
          continue;
        M11[channel]+=(x-centroid[channel].x)*(y-centroid[channel].y)*
          QuantumScale*p[i];
        M11[MaxPixelChannels]+=(x-centroid[channel].x)*(y-centroid[channel].y)*
          QuantumScale*p[i];
        M20[channel]+=(x-centroid[channel].x)*(x-centroid[channel].x)*
          QuantumScale*p[i];
        M20[MaxPixelChannels]+=(x-centroid[channel].x)*(x-centroid[channel].x)*
          QuantumScale*p[i];
        M02[channel]+=(y-centroid[channel].y)*(y-centroid[channel].y)*
          QuantumScale*p[i];
        M02[MaxPixelChannels]+=(y-centroid[channel].y)*(y-centroid[channel].y)*
          QuantumScale*p[i];
        M21[channel]+=(x-centroid[channel].x)*(x-centroid[channel].x)*
          (y-centroid[channel].y)*QuantumScale*p[i];
        M21[MaxPixelChannels]+=(x-centroid[channel].x)*(x-centroid[channel].x)*
          (y-centroid[channel].y)*QuantumScale*p[i];
        M12[channel]+=(x-centroid[channel].x)*(y-centroid[channel].y)*
          (y-centroid[channel].y)*QuantumScale*p[i];
        M12[MaxPixelChannels]+=(x-centroid[channel].x)*(y-centroid[channel].y)*
          (y-centroid[channel].y)*QuantumScale*p[i];
        M22[channel]+=(x-centroid[channel].x)*(x-centroid[channel].x)*
          (y-centroid[channel].y)*(y-centroid[channel].y)*QuantumScale*p[i];
        M22[MaxPixelChannels]+=(x-centroid[channel].x)*(x-centroid[channel].x)*
          (y-centroid[channel].y)*(y-centroid[channel].y)*QuantumScale*p[i];
        M30[channel]+=(x-centroid[channel].x)*(x-centroid[channel].x)*
          (x-centroid[channel].x)*QuantumScale*p[i];
        M30[MaxPixelChannels]+=(x-centroid[channel].x)*(x-centroid[channel].x)*
          (x-centroid[channel].x)*QuantumScale*p[i];
        M03[channel]+=(y-centroid[channel].y)*(y-centroid[channel].y)*
          (y-centroid[channel].y)*QuantumScale*p[i];
        M03[MaxPixelChannels]+=(y-centroid[channel].y)*(y-centroid[channel].y)*
          (y-centroid[channel].y)*QuantumScale*p[i];
      }
      p+=GetPixelChannels(image);
    }
  }
  M00[MaxPixelChannels]/=GetImageChannels(image);
  M01[MaxPixelChannels]/=GetImageChannels(image);
  M02[MaxPixelChannels]/=GetImageChannels(image);
  M03[MaxPixelChannels]/=GetImageChannels(image);
  M10[MaxPixelChannels]/=GetImageChannels(image);
  M11[MaxPixelChannels]/=GetImageChannels(image);
  M12[MaxPixelChannels]/=GetImageChannels(image);
  M20[MaxPixelChannels]/=GetImageChannels(image);
  M21[MaxPixelChannels]/=GetImageChannels(image);
  M22[MaxPixelChannels]/=GetImageChannels(image);
  M30[MaxPixelChannels]/=GetImageChannels(image);
  for (channel=0; channel <= MaxPixelChannels; channel++)
  {
    /*
      Compute elliptical angle, major and minor axes, eccentricity, & intensity.
    */
    channel_moments[channel].centroid=centroid[channel];
    channel_moments[channel].ellipse_axis.x=sqrt((2.0*
      PerceptibleReciprocal(M00[channel]))*((M20[channel]+M02[channel])+
      sqrt(4.0*M11[channel]*M11[channel]+(M20[channel]-M02[channel])*
      (M20[channel]-M02[channel]))));
    channel_moments[channel].ellipse_axis.y=sqrt((2.0*
      PerceptibleReciprocal(M00[channel]))*((M20[channel]+M02[channel])-
      sqrt(4.0*M11[channel]*M11[channel]+(M20[channel]-M02[channel])*
      (M20[channel]-M02[channel]))));
    channel_moments[channel].ellipse_angle=RadiansToDegrees(1.0/2.0*atan(2.0*
      M11[channel]*PerceptibleReciprocal(M20[channel]-M02[channel])));
    if (fabs(M11[channel]) < 0.0)
      {
        if ((fabs(M20[channel]-M02[channel]) >= 0.0) &&
            ((M20[channel]-M02[channel]) < 0.0))
          channel_moments[channel].ellipse_angle+=90.0;
      }
    else
      if (M11[channel] < 0.0)
        {
          if (fabs(M20[channel]-M02[channel]) >= 0.0)
            {
              if ((M20[channel]-M02[channel]) < 0.0)
                channel_moments[channel].ellipse_angle+=90.0;
              else
                channel_moments[channel].ellipse_angle+=180.0;
            }
        }
      else
        if ((fabs(M20[channel]-M02[channel]) >= 0.0) &&
            ((M20[channel]-M02[channel]) < 0.0))
          channel_moments[channel].ellipse_angle+=90.0;
    channel_moments[channel].ellipse_eccentricity=sqrt(1.0-(
      channel_moments[channel].ellipse_axis.y*
      channel_moments[channel].ellipse_axis.y*PerceptibleReciprocal(
      channel_moments[channel].ellipse_axis.x*
      channel_moments[channel].ellipse_axis.x)));
    channel_moments[channel].ellipse_intensity=M00[channel]*
      PerceptibleReciprocal(MagickPI*channel_moments[channel].ellipse_axis.x*
      channel_moments[channel].ellipse_axis.y+MagickEpsilon);
  }
  for (channel=0; channel <= MaxPixelChannels; channel++)
  {
    /*
      Normalize image moments.
    */
    M10[channel]=0.0;
    M01[channel]=0.0;
    M11[channel]*=PerceptibleReciprocal(pow(M00[channel],1.0+(1.0+1.0)/2.0));
    M20[channel]*=PerceptibleReciprocal(pow(M00[channel],1.0+(2.0+0.0)/2.0));
    M02[channel]*=PerceptibleReciprocal(pow(M00[channel],1.0+(0.0+2.0)/2.0));
    M21[channel]*=PerceptibleReciprocal(pow(M00[channel],1.0+(2.0+1.0)/2.0));
    M12[channel]*=PerceptibleReciprocal(pow(M00[channel],1.0+(1.0+2.0)/2.0));
    M22[channel]*=PerceptibleReciprocal(pow(M00[channel],1.0+(2.0+2.0)/2.0));
    M30[channel]*=PerceptibleReciprocal(pow(M00[channel],1.0+(3.0+0.0)/2.0));
    M03[channel]*=PerceptibleReciprocal(pow(M00[channel],1.0+(0.0+3.0)/2.0));
    M00[channel]=1.0;
  }
  image_view=DestroyCacheView(image_view);
  for (channel=0; channel <= MaxPixelChannels; channel++)
  {
    /*
      Compute Hu invariant moments.
    */
    channel_moments[channel].invariant[0]=M20[channel]+M02[channel];
    channel_moments[channel].invariant[1]=(M20[channel]-M02[channel])*
      (M20[channel]-M02[channel])+4.0*M11[channel]*M11[channel];
    channel_moments[channel].invariant[2]=(M30[channel]-3.0*M12[channel])*
      (M30[channel]-3.0*M12[channel])+(3.0*M21[channel]-M03[channel])*
      (3.0*M21[channel]-M03[channel]);
    channel_moments[channel].invariant[3]=(M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])+(M21[channel]+M03[channel])*
      (M21[channel]+M03[channel]);
    channel_moments[channel].invariant[4]=(M30[channel]-3.0*M12[channel])*
      (M30[channel]+M12[channel])*((M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])-3.0*(M21[channel]+M03[channel])*
      (M21[channel]+M03[channel]))+(3.0*M21[channel]-M03[channel])*
      (M21[channel]+M03[channel])*(3.0*(M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])-(M21[channel]+M03[channel])*
      (M21[channel]+M03[channel]));
    channel_moments[channel].invariant[5]=(M20[channel]-M02[channel])*
      ((M30[channel]+M12[channel])*(M30[channel]+M12[channel])-
      (M21[channel]+M03[channel])*(M21[channel]+M03[channel]))+
      4.0*M11[channel]*(M30[channel]+M12[channel])*(M21[channel]+M03[channel]);
    channel_moments[channel].invariant[6]=(3.0*M21[channel]-M03[channel])*
      (M30[channel]+M12[channel])*((M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])-3.0*(M21[channel]+M03[channel])*
      (M21[channel]+M03[channel]))-(M30[channel]-3*M12[channel])*
      (M21[channel]+M03[channel])*(3.0*(M30[channel]+M12[channel])*
      (M30[channel]+M12[channel])-(M21[channel]+M03[channel])*
      (M21[channel]+M03[channel]));
    channel_moments[channel].invariant[7]=M11[channel]*((M30[channel]+
      M12[channel])*(M30[channel]+M12[channel])-(M03[channel]+M21[channel])*
      (M03[channel]+M21[channel]))-(M20[channel]-M02[channel])*
      (M30[channel]+M12[channel])*(M03[channel]+M21[channel]);
  }
  if (y < (ssize_t) image->rows)
    channel_moments=(ChannelMoments *) RelinquishMagickMemory(channel_moments);
  return(channel_moments);
}