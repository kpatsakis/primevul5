static int ReadAnimatedWEBPImage(const ImageInfo *image_info,Image *image,
  uint8_t *stream,size_t length,WebPDecoderConfig *configure,
  ExceptionInfo *exception)
{
  Image
    *original_image;

  int
    image_count,
    webp_status;

  size_t
    canvas_width,
    canvas_height;

  WebPData
    data;

  WebPDemuxer
    *demux;

  WebPIterator
    iter;

  image_count=0;
  webp_status=0;
  original_image=image;
  webp_status=FillBasicWEBPInfo(image,stream,length,configure);
  canvas_width=image->columns;
  canvas_height=image->rows;
  data.bytes=stream;
  data.size=length;
  {
    WebPMux
      *mux;

    WebPMuxAnimParams
      params;

    WebPMuxError
      status;

    mux=WebPMuxCreate(&data,0);
    status=WebPMuxGetAnimationParams(mux,&params);
    if (status >= 0)
      image->iterations=params.loop_count;
    WebPMuxDelete(mux);
  }
  demux=WebPDemux(&data);
  if (WebPDemuxGetFrame(demux,1,&iter)) {
    do {
      if (image_count != 0)
        {
          AcquireNextImage(image_info,image);
          if (GetNextImageInList(image) == (Image *) NULL)
            break;
          image=SyncNextImageInList(image);
          CloneImageProperties(image, original_image);
          image->page.x=iter.x_offset;
          image->page.y=iter.y_offset;
          webp_status=ReadSingleWEBPImage(image,iter.fragment.bytes,
            iter.fragment.size,configure,exception,MagickFalse);
        }
      else
        {
          image->page.x=iter.x_offset;
          image->page.y=iter.y_offset;
          webp_status=ReadSingleWEBPImage(image,iter.fragment.bytes,
            iter.fragment.size,configure,exception,MagickTrue);
        }
      if (webp_status != VP8_STATUS_OK)
        break;

      image->page.width=canvas_width;
      image->page.height=canvas_height;
      image->ticks_per_second=100;
      image->delay=iter.duration/10;
      image->dispose=NoneDispose;
      if (iter.dispose_method == WEBP_MUX_DISPOSE_BACKGROUND)
        image->dispose=BackgroundDispose;
      image_count++;
    } while (WebPDemuxNextFrame(&iter));
    WebPDemuxReleaseIterator(&iter);
  }
  WebPDemuxDelete(demux);
  return(webp_status);
}