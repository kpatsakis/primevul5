ReadVeNCryptSecurityType(rfbClient* client, uint32_t *result)
{
    uint8_t count=0;
    uint8_t loop=0;
    uint8_t flag=0;
    uint32_t tAuth[256], t;
    char buf1[500],buf2[10];
    uint32_t authScheme;

    if (!ReadFromRFBServer(client, (char *)&count, 1)) return FALSE;

    if (count==0)
    {
        rfbClientLog("List of security types is ZERO. Giving up.\n");
        return FALSE;
    }

    rfbClientLog("We have %d security types to read\n", count);
    authScheme=0;
    /* now, we have a list of available security types to read ( uint8_t[] ) */
    for (loop=0;loop<count;loop++)
    {
        if (!ReadFromRFBServer(client, (char *)&tAuth[loop], 4)) return FALSE;
        t=rfbClientSwap32IfLE(tAuth[loop]);
        rfbClientLog("%d) Received security type %d\n", loop, t);
        if (flag) continue;
        if (t==rfbVeNCryptTLSNone ||
            t==rfbVeNCryptTLSVNC ||
            t==rfbVeNCryptTLSPlain ||
#ifdef LIBVNCSERVER_HAVE_SASL
            t==rfbVeNCryptTLSSASL ||
            t==rfbVeNCryptX509SASL ||
#endif /*LIBVNCSERVER_HAVE_SASL */
            t==rfbVeNCryptX509None ||
            t==rfbVeNCryptX509VNC ||
            t==rfbVeNCryptX509Plain)
        {
            flag++;
            authScheme=t;
            rfbClientLog("Selecting security type %d (%d/%d in the list)\n", authScheme, loop, count);
            /* send back 4 bytes (in original byte order!) indicating which security type to use */
            if (!WriteToRFBServer(client, (char *)&tAuth[loop], 4)) return FALSE;
        }
        tAuth[loop]=t;
    }
    if (authScheme==0)
    {
        memset(buf1, 0, sizeof(buf1));
        for (loop=0;loop<count;loop++)
        {
            if (strlen(buf1)>=sizeof(buf1)-1) break;
            snprintf(buf2, sizeof(buf2), (loop>0 ? ", %d" : "%d"), (int)tAuth[loop]);
            strncat(buf1, buf2, sizeof(buf1)-strlen(buf1)-1);
        }
        rfbClientLog("Unknown VeNCrypt authentication scheme from VNC server: %s\n",
               buf1);
        return FALSE;
    }
    *result = authScheme;
    return TRUE;
}