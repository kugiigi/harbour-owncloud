#include "owncloudbrowser.h"

OwnCloudBrowser::OwnCloudBrowser(QObject *parent, Settings *settings) :
    QObject(parent)
{
    this->settings = settings;

    // Need to decide on how to securely save password
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(reloadSettings()));
    connect(&parser, SIGNAL(finished()), this, SLOT(printList()));
    connect(&parser, SIGNAL(errorChanged(QString)), this, SLOT(printError(QString)));
    connect(&webdav, SIGNAL(errorChanged(QString)), this, SLOT(printError(QString)));
}

void OwnCloudBrowser::reloadSettings() {
    webdav.setConnectionSettings(settings->isHttps() ? QWebdav::HTTPS : QWebdav::HTTP,
                                 settings->hostname(),
                                 settings->path() + "/remote.php/webdav",
                                 settings->username(),
                                 settings->password(),
                                 settings->port());
}


void OwnCloudBrowser::printList()
{
    QList<QWebdavItem> list = parser.getList();
    QVariantList entries;

    QWebdavItem item;
    foreach(item, list) {
        EntryInfo *entry = new EntryInfo();
        entry->setName(item.name());
        entry->setDirectory(item.isDir());
        entry->setSize(item.size());
        if(!item.isDir())
            entry->setMimeType(item.mimeType());

        QVariant tmpVariant;
        tmpVariant.setValue(entry);
        entries.append(tmpVariant);
    }
    emit directoryContentChanged(currentPath, entries);
}

void OwnCloudBrowser::printError(QString msg)
{
    qDebug() << "ERROR: " << msg;
}

QString OwnCloudBrowser::getCurrentPath()
{
    return currentPath;
}

void OwnCloudBrowser::getDirectoryContent(QString path)
{
    currentPath = path;
    parser.listDirectory(&webdav, path);
}