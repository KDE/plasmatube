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

    Q_INVOKABLE void fetchApiKey();
    Q_INVOKABLE void searchForArtists(const QString &searchQuery);
    Q_INVOKABLE void musicInfo();

signals:
    void apiKeyFetched(bool success);

private:
    QNetworkAccessManager *m_netManager;
    QString m_apiKey;
};

#endif // YOUTUBEMUSICMANAGER_H
