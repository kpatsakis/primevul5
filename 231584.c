detectOptimizationMode (const vector<sliceOptimizationData>& optData)
{
    OptimizationMode w;
    
    // need to be compiled with SSE optimisations: if not, just returns false
#ifdef IMF_HAVE_SSE2
    
    
    // only handle reading 3,4,6 or 8 channels
    switch(optData.size())
    {
        case 3 : break;
        case 4 : break;
        case 6 : break;
        case 8 : break;
        default :
            return w;
    }
    
    //
    // the point at which data switches between the primary and secondary bank
    //
    size_t bankSize = optData.size()>4 ? optData.size()/2 : optData.size();
    
    for(size_t i=0;i<optData.size();i++)
    {
        const sliceOptimizationData& data = optData[i];
        // can't fill anything other than channel 3 or channel 7
        if(data.fill)
        {
            if(i!=3 && i!=7)
            {
                return w;
            }
        }
        
        // cannot have gaps in the channel layout, so the stride must be (number of channels written in the bank)*2
        if(data.xStride !=bankSize*2)
        {
            return w;
        }
        
        // each bank of channels must be channel interleaved: each channel base pointer must be (previous channel+2)
        // this also means channel sampling pattern must be consistent, as must yStride
        if(i!=0 && i!=bankSize)
        {
            if(data.base!=optData[i-1].base+2)
            {
                return w;
            }
        }
        if(i!=0)
        {
            
            if(data.yStride!=optData[i-1].yStride)
            {
                return w;
            }
        }
    }
    

    w._ySampling=optData[0].ySampling;
    w._optimizable=true;
    
#endif

    return w;
}