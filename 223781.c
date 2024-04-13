static int WebPEncodeProgress(int percent,const WebPPicture* picture)
{
#define EncodeImageTag  "Encode/Image"

  Image
    *image;

  MagickBooleanType
    status;

  image=(Image *) picture->user_data;
  status=SetImageProgress(image,EncodeImageTag,percent-1,100);
  return(status == MagickFalse ? 0 : 1);
}