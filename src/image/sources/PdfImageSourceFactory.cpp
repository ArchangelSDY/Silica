#include <QString>
#include <QPdfDocument>

#include "PdfImageSource.h"
#include "PdfImageSourceFactory.h"

PdfImageSourceFactory::PdfImageSourceFactory(ImageSourceManager *mgr) :
    ImageSourceFactory(mgr)
{
}

QString PdfImageSourceFactory::name() const
{
    return "Pdf";
}

QString PdfImageSourceFactory::fileNamePattern() const
{
    return "*.pdf";
}

QString PdfImageSourceFactory::urlScheme() const
{
    return "pdf";
}

ImageSource *PdfImageSourceFactory::createSingle(const QUrl &url)
{
    if (url.scheme() == urlScheme()) {
		QUrl pdfUrl = url;
		pdfUrl.setScheme("file");
		pdfUrl.setFragment("");
		QString pdfPath = pdfUrl.toLocalFile();
        int page = url.fragment().toInt();

		return new PdfImageSource(this, pdfPath, page);
    } else {
        return 0;
    }
}

ImageSource *PdfImageSourceFactory::createSingle(const QString &packagePath)
{
    if (!isValidFileName(packagePath)) {
        return nullptr;
    }

    QPdfDocument doc;
	QPdfDocument::DocumentError err = doc.load(packagePath);
    if (err != QPdfDocument::DocumentError::NoError) {
        return nullptr;
    }

    if (doc.pageCount() > 0) {
        doc.close();
        return new PdfImageSource(this, packagePath, 0);
    }

    doc.close();
    return nullptr;
}

QList<ImageSource *> PdfImageSourceFactory::createMultiple(const QUrl &url)
{
    QList<ImageSource *> imageSources;

    if (!isValidFileName(url.path())) {
        return imageSources;
    }

    if (url.hasFragment()) {
        ImageSource *source = createSingle(url);
        if (source) {
            imageSources << source;
        }
    } else {
        // Add all files in the pdf
        QUrl fileUrl = url;
        fileUrl.setScheme("file");
        QString packagePath = fileUrl.toLocalFile();

        QPdfDocument doc;
        QPdfDocument::DocumentError err = doc.load(packagePath);
        if (err == QPdfDocument::DocumentError::NoError) {
            int pageCount = doc.pageCount();
            for (int i = 0; i < pageCount; i++) {
                QUrl imageUrl = url;
                imageUrl.setFragment(QString::number(i));

                ImageSource *source = createSingle(imageUrl);
                if (source) {
                    imageSources << source;
                }
            }

            doc.close();
        }
    }

    return imageSources;
}

QList<ImageSource *> PdfImageSourceFactory::createMultiple(const QString &path)
{
    QUrl url = QUrl::fromLocalFile(path);
    url.setScheme(urlScheme());
    return createMultiple(url);
}
