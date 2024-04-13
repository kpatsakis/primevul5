bm_delta2_search (char const **tpp, char const *ep, char const *sp, int len,
                  char const *trans, char gc1, char gc2,
                  unsigned char const *d1, kwset_t kwset)
{
  char const *tp = *tpp;
  int d = len, skip = 0;

  while (true)
    {
      int i = 2;
      if (tr (trans, tp[-2]) == gc2)
        {
          while (++i <= d)
            if (tr (trans, tp[-i]) != tr (trans, sp[-i]))
              break;
          if (i > d)
            {
              for (i = d + skip + 1; i <= len; ++i)
                if (tr (trans, tp[-i]) != tr (trans, sp[-i]))
                  break;
              if (i > len)
                {
                  *tpp = tp - len;
                  return true;
                }
            }
        }

      tp += d = kwset->shift[i - 2];
      if (tp > ep)
        break;
      if (tr (trans, tp[-1]) != gc1)
        {
          if (d1)
            tp += d1[U(tp[-1])];
          break;
        }
      skip = i - 1;
    }

  *tpp = tp;
  return false;
}