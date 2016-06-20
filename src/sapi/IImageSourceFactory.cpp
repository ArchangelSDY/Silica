#include "IImageSourceFactory.h"

#include <QRegExp>

namespace sapi {

IImageSourceFactory::~IImageSourceFactory()
{
}

bool IImageSourceFactory::isValidFileName(const QString &name)
{
    QStringList patterns = fileNamePattern().split(' ');
    foreach (const QString &pattern, patterns) {
        QRegExp reg(pattern, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (reg.exactMatch(name)) {
            return true;
        }
    }

    return false;
}

}
