MagickExport ChannelStatistics *GetImageStatistics(const Image *image,
  ExceptionInfo *exception)
{
  ChannelStatistics
    *channel_statistics;

  double
    area,
    *histogram,
    standard_deviation;

  MagickStatusType
    status;

  MemoryInfo
    *median_info;

  Quantum
    *median;

  QuantumAny
    range;

  ssize_t
    i;

  size_t
    depth;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  histogram=(double *) AcquireQuantumMemory(MaxMap+1UL,GetPixelChannels(image)*
    sizeof(*histogram));
  channel_statistics=(ChannelStatistics *) AcquireQuantumMemory(
    MaxPixelChannels+1,sizeof(*channel_statistics));
  if ((channel_statistics == (ChannelStatistics *) NULL) ||
      (histogram == (double *) NULL))
    {
      if (histogram != (double *) NULL)
        histogram=(double *) RelinquishMagickMemory(histogram);
      if (channel_statistics != (ChannelStatistics *) NULL)
        channel_statistics=(ChannelStatistics *) RelinquishMagickMemory(
          channel_statistics);
      return(channel_statistics);
    }
  (void) memset(channel_statistics,0,(MaxPixelChannels+1)*
    sizeof(*channel_statistics));
  for (i=0; i <= (ssize_t) MaxPixelChannels; i++)
  {
    channel_statistics[i].depth=1;
    channel_statistics[i].maxima=(-MagickMaximumValue);
    channel_statistics[i].minima=MagickMaximumValue;
  }
  (void) memset(histogram,0,(MaxMap+1)*GetPixelChannels(image)*
    sizeof(*histogram));
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    const Quantum
      *magick_restrict p;

    ssize_t
      x;

    /*
      Compute pixel statistics.
    */
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      ssize_t
        i;

      if (GetPixelReadMask(image,p) <= (QuantumRange/2))
        {
          p+=GetPixelChannels(image);
          continue;
        }
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if (traits == UndefinedPixelTrait)
          continue;
        if ((traits & UpdatePixelTrait) == 0)
          continue;
        if (channel_statistics[channel].depth != MAGICKCORE_QUANTUM_DEPTH)
          {
            depth=channel_statistics[channel].depth;
            range=GetQuantumRange(depth);
            status=p[i] != ScaleAnyToQuantum(ScaleQuantumToAny(p[i],range),
              range) ? MagickTrue : MagickFalse;
            if (status != MagickFalse)
              {
                channel_statistics[channel].depth++;
                if (channel_statistics[channel].depth >
                    channel_statistics[CompositePixelChannel].depth)
                  channel_statistics[CompositePixelChannel].depth=
                    channel_statistics[channel].depth;
                i--;
                continue;
              }
          }
        if ((double) p[i] < channel_statistics[channel].minima)
          channel_statistics[channel].minima=(double) p[i];
        if ((double) p[i] > channel_statistics[channel].maxima)
          channel_statistics[channel].maxima=(double) p[i];
        channel_statistics[channel].sum+=p[i];
        channel_statistics[channel].sum_squared+=(double) p[i]*p[i];
        channel_statistics[channel].sum_cubed+=(double) p[i]*p[i]*p[i];
        channel_statistics[channel].sum_fourth_power+=(double) p[i]*p[i]*p[i]*
          p[i];
        channel_statistics[channel].area++;
        if ((double) p[i] < channel_statistics[CompositePixelChannel].minima)
          channel_statistics[CompositePixelChannel].minima=(double) p[i];
        if ((double) p[i] > channel_statistics[CompositePixelChannel].maxima)
          channel_statistics[CompositePixelChannel].maxima=(double) p[i];
        histogram[GetPixelChannels(image)*ScaleQuantumToMap(
          ClampToQuantum((double) p[i]))+i]++;
        channel_statistics[CompositePixelChannel].sum+=(double) p[i];
        channel_statistics[CompositePixelChannel].sum_squared+=(double)
          p[i]*p[i];
        channel_statistics[CompositePixelChannel].sum_cubed+=(double)
          p[i]*p[i]*p[i];
        channel_statistics[CompositePixelChannel].sum_fourth_power+=(double)
          p[i]*p[i]*p[i]*p[i];
        channel_statistics[CompositePixelChannel].area++;
      }
      p+=GetPixelChannels(image);
    }
  }
  for (i=0; i <= (ssize_t) MaxPixelChannels; i++)
  {
    /*
      Normalize pixel statistics.
    */
    area=PerceptibleReciprocal(channel_statistics[i].area);
    channel_statistics[i].sum*=area;
    channel_statistics[i].sum_squared*=area;
    channel_statistics[i].sum_cubed*=area;
    channel_statistics[i].sum_fourth_power*=area;
    channel_statistics[i].mean=channel_statistics[i].sum;
    channel_statistics[i].variance=channel_statistics[i].sum_squared;
    standard_deviation=sqrt(channel_statistics[i].variance-
      (channel_statistics[i].mean*channel_statistics[i].mean));
    standard_deviation=sqrt(PerceptibleReciprocal(channel_statistics[i].area-
      1.0)*channel_statistics[i].area*standard_deviation*standard_deviation);
    channel_statistics[i].standard_deviation=standard_deviation;
  }
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    double
      number_bins;

    ssize_t
      j;

    /*
      Compute pixel entropy.
    */
    PixelChannel channel = GetPixelChannelChannel(image,i);
    number_bins=0.0;
    for (j=0; j <= (ssize_t) MaxMap; j++)
      if (histogram[GetPixelChannels(image)*j+i] > 0.0)
        number_bins++;
    area=PerceptibleReciprocal(channel_statistics[channel].area);
    for (j=0; j <= (ssize_t) MaxMap; j++)
    {
      double
        count;

      count=area*histogram[GetPixelChannels(image)*j+i];
      channel_statistics[channel].entropy+=-count*MagickLog10(count)*
        PerceptibleReciprocal(MagickLog10(number_bins));
      channel_statistics[CompositePixelChannel].entropy+=-count*
        MagickLog10(count)*PerceptibleReciprocal(MagickLog10(number_bins))/
        GetPixelChannels(image);
    }
  }
  histogram=(double *) RelinquishMagickMemory(histogram);
  for (i=0; i <= (ssize_t) MaxPixelChannels; i++)
  {
    /*
      Compute kurtosis & skewness statistics.
    */
    standard_deviation=PerceptibleReciprocal(
      channel_statistics[i].standard_deviation);
    channel_statistics[i].skewness=(channel_statistics[i].sum_cubed-3.0*
      channel_statistics[i].mean*channel_statistics[i].sum_squared+2.0*
      channel_statistics[i].mean*channel_statistics[i].mean*
      channel_statistics[i].mean)*(standard_deviation*standard_deviation*
      standard_deviation);
    channel_statistics[i].kurtosis=(channel_statistics[i].sum_fourth_power-4.0*
      channel_statistics[i].mean*channel_statistics[i].sum_cubed+6.0*
      channel_statistics[i].mean*channel_statistics[i].mean*
      channel_statistics[i].sum_squared-3.0*channel_statistics[i].mean*
      channel_statistics[i].mean*1.0*channel_statistics[i].mean*
      channel_statistics[i].mean)*(standard_deviation*standard_deviation*
      standard_deviation*standard_deviation)-3.0;
  }
  median_info=AcquireVirtualMemory(image->columns,image->rows*sizeof(*median));
  if (median_info == (MemoryInfo *) NULL)
    (void) ThrowMagickException(exception,GetMagickModule(),
      ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
  else
    {
      ssize_t
        i;

      median=(Quantum *) GetVirtualMemoryBlob(median_info);
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        size_t
          n = 0;

        /*
          Compute median statistics for each channel.
        */
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if (traits == UndefinedPixelTrait)
          continue;
        if ((traits & UpdatePixelTrait) == 0)
          continue;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          const Quantum
            *magick_restrict p;

          ssize_t
            x;

          p=GetVirtualPixels(image,0,y,image->columns,1,exception);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            if (GetPixelReadMask(image,p) <= (QuantumRange/2))
              {
                p+=GetPixelChannels(image);
                continue;
              }
            median[n++]=p[i];
          }
          p+=GetPixelChannels(image);
        }
        channel_statistics[channel].median=(double) median[
          GetMedianPixel(median,n)];
      }
      median_info=RelinquishVirtualMemory(median_info);
    }
  channel_statistics[CompositePixelChannel].mean=0.0;
  channel_statistics[CompositePixelChannel].median=0.0;
  channel_statistics[CompositePixelChannel].standard_deviation=0.0;
  channel_statistics[CompositePixelChannel].entropy=0.0;
  for (i=0; i < (ssize_t) MaxPixelChannels; i++)
  {
    channel_statistics[CompositePixelChannel].mean+=
      channel_statistics[i].mean;
    channel_statistics[CompositePixelChannel].median+=
      channel_statistics[i].median;
    channel_statistics[CompositePixelChannel].standard_deviation+=
      channel_statistics[i].standard_deviation;
    channel_statistics[CompositePixelChannel].entropy+=
      channel_statistics[i].entropy;
  }
  channel_statistics[CompositePixelChannel].mean/=(double)
    GetImageChannels(image);
  channel_statistics[CompositePixelChannel].median/=(double)
    GetImageChannels(image);
  channel_statistics[CompositePixelChannel].standard_deviation/=(double)
    GetImageChannels(image);
  channel_statistics[CompositePixelChannel].entropy/=(double)
    GetImageChannels(image);
  if (y < (ssize_t) image->rows)
    channel_statistics=(ChannelStatistics *) RelinquishMagickMemory(
      channel_statistics);
  return(channel_statistics);
}