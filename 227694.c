    void Jp2Image::printStructure(std::ostream& out, PrintStructureOption option, int depth)
    {
        if (io_->open() != 0)
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());

        // Ensure that this is the correct image type
        if (!isJp2Type(*io_, false)) {
            if (io_->error() || io_->eof())
                throw Error(kerFailedToReadImageData);
            throw Error(kerNotAJpeg);
        }

        bool bPrint = option == kpsBasic || option == kpsRecursive;
        bool bRecursive = option == kpsRecursive;
        bool bICC = option == kpsIccProfile;
        bool bXMP = option == kpsXMP;
        bool bIPTCErase = option == kpsIptcErase;

        if (bPrint) {
            out << "STRUCTURE OF JPEG2000 FILE: " << io_->path() << std::endl;
            out << " address |   length | box       | data" << std::endl;
        }

        if ( bPrint || bXMP || bICC || bIPTCErase ) {

            long              position  = 0;
            Jp2BoxHeader      box       = {1,1};
            Jp2BoxHeader      subBox    = {1,1};
            Jp2UuidBox        uuid      = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
            bool              bLF       = false;

            while (box.length && box.type != kJp2BoxTypeClose && io_->read((byte*)&box, sizeof(box)) == sizeof(box))
            {
                position   = io_->tell();
                box.length = getLong((byte*)&box.length, bigEndian);
                box.type = getLong((byte*)&box.type, bigEndian);
                enforce(box.length <= io_->size()-io_->tell() , Exiv2::kerCorruptedMetadata);

                if (bPrint) {
                    out << Internal::stringFormat("%8ld | %8ld | ", position - sizeof(box),
                                                  (size_t)box.length)
                        << toAscii(box.type) << "      | ";
                    bLF = true;
                    if (box.type == kJp2BoxTypeClose)
                        lf(out, bLF);
                }
                if (box.type == kJp2BoxTypeClose)
                    break;

                switch (box.type) {
                    case kJp2BoxTypeJp2Header: {
                        lf(out, bLF);

                        while (io_->read((byte*)&subBox, sizeof(subBox)) == sizeof(subBox) &&
                               io_->tell() < position + (long)box.length)  // don't read beyond the box!
                        {
                            int address = io_->tell() - sizeof(subBox);
                            subBox.length = getLong((byte*)&subBox.length, bigEndian);
                            subBox.type = getLong((byte*)&subBox.type, bigEndian);

                            if (subBox.length < sizeof(box) || subBox.length > io_->size() - io_->tell()) {
                                throw Error(kerCorruptedMetadata);
                            }

                            DataBuf data(subBox.length - sizeof(box));
                            io_->read(data.pData_, data.size_);
                            if (bPrint) {
                                out << Internal::stringFormat("%8ld | %8ld |  sub:", (size_t)address,
                                                              (size_t)subBox.length)
                                    << toAscii(subBox.type) << " | "
                                    << Internal::binaryToString(makeSlice(data, 0, std::min(30l, data.size_)));
                                bLF = true;
                            }

                            if (subBox.type == kJp2BoxTypeColorHeader) {
                                long pad = 3;  // don't know why there are 3 padding bytes
                                if (bPrint) {
                                    out << " | pad:";
                                    for (int i = 0; i < 3; i++)
                                        out << " " << (int)data.pData_[i];
                                }
                                long iccLength = getULong(data.pData_ + pad, bigEndian);
                                if (bPrint) {
                                    out << " | iccLength:" << iccLength;
                                }
                                if (bICC) {
                                    out.write((const char*)data.pData_ + pad, iccLength);
                                }
                            }
                            lf(out, bLF);
                        }
                    } break;

                    case kJp2BoxTypeUuid: {
                        if (io_->read((byte*)&uuid, sizeof(uuid)) == sizeof(uuid)) {
                            bool bIsExif = memcmp(uuid.uuid, kJp2UuidExif, sizeof(uuid)) == 0;
                            bool bIsIPTC = memcmp(uuid.uuid, kJp2UuidIptc, sizeof(uuid)) == 0;
                            bool bIsXMP = memcmp(uuid.uuid, kJp2UuidXmp, sizeof(uuid)) == 0;

                            bool bUnknown = !(bIsExif || bIsIPTC || bIsXMP);

                            if (bPrint) {
                                if (bIsExif)
                                    out << "Exif: ";
                                if (bIsIPTC)
                                    out << "IPTC: ";
                                if (bIsXMP)
                                    out << "XMP : ";
                                if (bUnknown)
                                    out << "????: ";
                            }

                            DataBuf rawData;
                            rawData.alloc(box.length - sizeof(uuid) - sizeof(box));
                            long bufRead = io_->read(rawData.pData_, rawData.size_);
                            if (io_->error())
                                throw Error(kerFailedToReadImageData);
                            if (bufRead != rawData.size_)
                                throw Error(kerInputDataReadFailed);

                            if (bPrint) {
                                out << Internal::binaryToString(
                                        makeSlice(rawData, 0, rawData.size_>40?40:rawData.size_));
                                out.flush();
                            }
                            lf(out, bLF);

                            if (bIsExif && bRecursive && rawData.size_ > 8) { // "II*\0long"
                                if ((rawData.pData_[0] == rawData.pData_[1]) &&
                                    (rawData.pData_[0] == 'I' || rawData.pData_[0] == 'M')) {
                                    BasicIo::UniquePtr p = BasicIo::UniquePtr(new MemIo(rawData.pData_, rawData.size_));
                                    printTiffStructure(*p, out, option, depth);
                                }
                            }

                            if (bIsIPTC && bRecursive) {
                                IptcData::printStructure(out, makeSlice(rawData.pData_, 0, rawData.size_), depth);
                            }

                            if (bIsXMP && bXMP) {
                                out.write((const char*)rawData.pData_, rawData.size_);
                            }
                        }
                    } break;

                    default:
                        break;
                }

                // Move to the next box.
                io_->seek(static_cast<long>(position - sizeof(box) + box.length), BasicIo::beg);
                if (io_->error())
                    throw Error(kerFailedToReadImageData);
                if (bPrint)
                    lf(out, bLF);
            }
        }
    }  // JpegBase::printStructure