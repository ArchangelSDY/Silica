#include "CGColleReader.h"

#include "CGColleV0Reader.h"
#include "CGColleV1Reader.h"

CGColleReader *CGColleReader::create(const QString &path)
{
    if (CGColleV0Reader::isValidFormat(path)) {
        return new CGColleV0Reader(path);
    } else if (CGColleV1Reader::isValidFormat(path)) {
        return new CGColleV1Reader(path);
    } else {
        return nullptr;
    }
}