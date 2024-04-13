static rfbBool CheckRect(rfbClient* client, int x, int y, int w, int h) {
  return x + w <= client->width && y + h <= client->height;
}