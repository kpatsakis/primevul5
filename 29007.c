CairoImageOutputDev::~CairoImageOutputDev()
{
  int i;

  for (i = 0; i < numImages; i++)
    delete images[i];
  gfree (images);
}
