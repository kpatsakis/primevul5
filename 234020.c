query (void)
{
  static const GimpParamDef load_args[] =
  {
    { GIMP_PDB_INT32,  "run-mode",     "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_STRING, "filename",     "The name of the file to load" },
    { GIMP_PDB_STRING, "raw-filename", "The name entered"             }
  };

  static const GimpParamDef load_return_vals[] =
  {
    { GIMP_PDB_IMAGE, "image", "Output image" }
  };

  static const GimpParamDef save_args[] =
  {
    { GIMP_PDB_INT32,    "run-mode",     "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_IMAGE,    "image",        "Input image"                  },
    { GIMP_PDB_DRAWABLE, "drawable",     "Drawable to save"             },
    { GIMP_PDB_STRING,   "filename",     "The name of the file to save the image in" },
    { GIMP_PDB_STRING,   "raw-filename", "The name of the file to save the image in" },
    { GIMP_PDB_INT32,    "rle",          "Use RLE compression"          },
    { GIMP_PDB_INT32,    "origin",       "Image origin (0 = top-left, 1 = bottom-left)"}
  } ;

  gimp_install_procedure (LOAD_PROC,
                          "Loads files of Targa file format",
                          "FIXME: write help for tga_load",
                          "Raphael FRANCOIS, Gordon Matzigkeit",
                          "Raphael FRANCOIS, Gordon Matzigkeit",
                          "1997,2000,2007",
                          N_("TarGA image"),
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (load_args),
                          G_N_ELEMENTS (load_return_vals),
                          load_args, load_return_vals);

  gimp_register_file_handler_mime (LOAD_PROC, "image/x-tga");
  gimp_register_magic_load_handler (LOAD_PROC,
                                    "tga,vda,icb,vst",
                                    "",
                                    "-18&,string,TRUEVISION-XFILE.,-1,byte,0");

  gimp_install_procedure (SAVE_PROC,
                          "saves files in the Targa file format",
                          "FIXME: write help for tga_save",
                          "Raphael FRANCOIS, Gordon Matzigkeit",
                          "Raphael FRANCOIS, Gordon Matzigkeit",
                          "1997,2000",
                          N_("TarGA image"),
                          "RGB*, GRAY*, INDEXED*",
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (save_args), 0,
                          save_args, NULL);

  gimp_register_file_handler_mime (SAVE_PROC, "image/x-tga");
  gimp_register_save_handler (SAVE_PROC, "tga", "");
}