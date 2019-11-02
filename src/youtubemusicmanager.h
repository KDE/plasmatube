#ifndef YOUTUBEMUSICMANAGER_H
#define YOUTUBEMUSICMANAGER_H

#include <QObject>

class QNetworkAccessManager;

class YouTubeMusicManager : public QObject
{
    Q_OBJECT

public:
    YouTubeMusicManager(QObject *parent = nullptr);
    ~YouTubeMusicManager();

    Q_INVOKABLE void searchForArtists(const QString &searchQuery);
    Q_INVOKABLE void musicInfo();

private:
    QNetworkAccessManager *m_netManager;
};

#endif // YOUTUBEMUSICMANAGER_H
