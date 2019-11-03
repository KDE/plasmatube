#ifndef YTMSEARCHRESULT_H
#define YTMSEARCHRESULT_H

#include <QVector>

class YTMSearchResult
{
public:
    class ShelfItem
    {
    private:
        QVector<QString> m_tags;
    };

    class Shelf
    {
    private:
        QString m_title;
    };

    static YTMSearchResult &fromJson(const QByteArray &json);

private:
    QVector<Shelf> m_shelves;
};

#endif // YTMSEARCHRESULT_H
