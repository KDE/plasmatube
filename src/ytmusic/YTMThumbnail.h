#ifndef YTMTHUMBNAIL_H
#define YTMTHUMBNAIL_H

#include <QSize>
#include <QUrl>

class QJsonObject;

class YTMThumbnail
{
public:
    static YTMThumbnail &fromJson(const QJsonObject &json);

    QSize size() const;
    QSize &size();
    void setSize(const QSize &size);

    QUrl url() const;
    void setUrl(const QUrl &url);

    QJsonObject toJson() const;

private:
    QSize m_size;
    QUrl m_url;
};

#endif // YTMTHUMBNAIL_H
