ppm_load_read_image(FILE       *fp,
                    pnm_struct *img)
{
    guint   i;

    if (img->type == PIXMAP_RAW)
      {
        fread (img->data, img->bpc, img->numsamples, fp);

        /* Fix endianness if necessary */
        if (img->bpc > 1)
          {
            gushort *ptr = (gushort *) img->data;

            for (i=0; i < img->numsamples; i++)
              {
                *ptr = GUINT16_FROM_BE (*ptr);
                ptr++;
              }
          }
      }
    else
      {
        /* Plain PPM format */

        if (img->bpc == sizeof (guchar))
          {
            guchar *ptr = img->data;

            for (i = 0; i < img->numsamples; i++)
              {
                guint sample;
                fscanf (fp, " %u", &sample);
                *ptr++ = sample;
              }
          }
        else if (img->bpc == sizeof (gushort))
          {
            gushort *ptr = (gushort *) img->data;

            for (i = 0; i < img->numsamples; i++)
              {
                guint sample;
                fscanf (fp, " %u", &sample);
                *ptr++ = sample;
              }
          }
        else
          {
            g_warning ("%s: Programmer stupidity error", G_STRLOC);
          }
      }
}