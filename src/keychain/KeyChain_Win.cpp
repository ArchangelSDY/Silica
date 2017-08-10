#include "KeyChain.h"

#include <QDebug>

#include <windows.h>
#include <wincred.h>

bool KeyChain::write(const QString &key, const QByteArray &data, const QString &comment)
{
    CREDENTIALW cred;
    memset(&cred, 0, sizeof(cred));

    cred.Comment = (LPWSTR)comment.utf16();
    cred.Type = CRED_TYPE_GENERIC;
    cred.TargetName = (LPWSTR)key.utf16();
    cred.CredentialBlobSize = data.size();
    cred.CredentialBlob = (LPBYTE)data.constData();
    cred.Persist = CRED_PERSIST_LOCAL_MACHINE;

    return CredWriteW(&cred, 0) == TRUE;
}

QByteArray KeyChain::read(const QString &key)
{
    PCREDENTIALW cred;

    if (!CredReadW((LPCWSTR)key.utf16(), CRED_TYPE_GENERIC, 0, &cred)) {
        return QByteArray();
    }

    QByteArray result((char *)cred->CredentialBlob, cred->CredentialBlobSize);
    CredFree(cred);

    return result;
}

bool KeyChain::remove(const QString &key)
{
    return CredDeleteW((LPCWSTR)key.utf16(), CRED_TYPE_GENERIC, 0) == TRUE;
}

QString KeyChain::errorMessage() const
{
    DWORD error = GetLastError();
    if (error == 0) {
        return QString();
    }

    LPWSTR buf = nullptr;
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buf, 0, NULL);
    std::wstring message(buf, size);

    LocalFree(buf);

    return QString::fromStdWString(message);
}
