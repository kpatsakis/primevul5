void LibRaw::fixupArri()
{
    struct alist_t
    {
        const char *a_model;
        const char *a_software;
        ushort a_width,a_height;
        int a_black;
        unsigned a_filters;
        float a_aspect;
    }
    alist[] =
    {
        {"ALEXA65", "Alexa65  XT", 6560 ,3100, 256,0x49494949,1.f},

        {"ALEXALF", "Alexa LF Plus W", 3840 ,2160, 256,0x49494949,1.0f },
        {"ALEXALF", "Alexa LF Plus W", 4448 ,1856, 256,0x49494949,0.75f },
        {"ALEXALF", "Alexa LF Plus W", 4448 ,3096, 256,0x49494949,1.f },

        {"ALEXA", "Alexa Plus 4:3 SXT", 2880 ,1620, 256,0x61616161,.75f},
        {"ALEXA", "Alexa Plus 4:3 SXT", 3168 ,1782, 256,0x61616161,0.75f},
        {"ALEXA", "Alexa Plus 4:3 SXT", 3424 ,2202, 256,0x61616161,1.f},
        {"ALEXA", "Alexa Plus 4:3 SXT", 2592 ,2160, 256,0x61616161,1.12f},

        {"ALEXA", "Alexa Plus 4:3 XT", 2592 ,2160, 256,0x61616161,1.12f},
        {"ALEXA", "Alexa Plus 4:3 XT", 2880 ,2160, 256,0x61616161,1.f},
        {"ALEXA", "Alexa Plus 4:3 XT", 2880 ,1620, 256,0x61616161,0.75f},
        {"ALEXA", "Alexa Plus 4:3 XT", 3424 ,2202, 256,0x61616161,1.f},
    };
    for(int i = 0; i < int(sizeof(alist)/sizeof(alist[0])); i++)
        if(!strncasecmp(model,alist[i].a_model,strlen(alist[i].a_model)) && software
            && !strncasecmp(software,alist[i].a_software,strlen(alist[i].a_software))
            && width == alist[i].a_width && height == alist[i].a_height)
        {
            filters = alist[i].a_filters;
            black = alist[i].a_black;
            pixel_aspect = alist[i].a_aspect;
            strcpy(model,software);
            software[0]=0;
            return;
        }
}