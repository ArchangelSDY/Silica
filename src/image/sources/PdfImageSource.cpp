#include <QByteArray>
#include <QBuffer>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QPdfDocument>
#include <QTextStream>

#include "PdfImageSource.h"

PdfImageSource::PdfImageSource(ImageSourceFactory* factory,
	const QString& arcPath, int page) :
	ImageSource(factory),
	m_page(page)
{
	m_arcPath = findRealPath(arcPath);
	m_name = QString::number(page).rightJustified(5, '0');

	// Compute hash
	QString hashStr;
	QTextStream(&hashStr) << "pdf#" << m_arcPath << "#" << m_page;
	m_hash = QCryptographicHash::hash(
		QByteArray(hashStr.toUtf8()), QCryptographicHash::Sha1).toHex();

	// Compute url
	m_url = QUrl::fromLocalFile(m_arcPath);
	m_url.setScheme("pdf");
	m_url.setFragment(m_name);
}

bool PdfImageSource::open()
{
	m_doc.reset(new QPdfDocument());
	QPdfDocument::DocumentError err = m_doc->load(m_arcPath);
	return err == QPdfDocument::DocumentError::NoError;
}

void PdfImageSource::close()
{
	if (m_doc) {
		m_doc->close();
	}
	m_doc.reset();
}

bool PdfImageSource::readFrames(QList<QImage>& images, QList<int>& durations)
{
	Q_ASSERT_X(!m_doc.isNull(), "PdfImageSource::readFrames", "m_doc should not be null");

	QSizeF pageSize = m_doc->pageSize(m_page);
	QImage image = m_doc->render(m_page, pageSize.toSize());

	images << image;
	durations << 0;

	return true;
}

bool PdfImageSource::exists()
{
	return QFileInfo::exists(m_arcPath);
}

bool PdfImageSource::copy(const QString& destPath)
{
	return false;
}
