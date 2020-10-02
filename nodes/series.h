#ifndef SERIES_H
#define SERIES_H

#include "node.h"

#include <QDate>



class Series : public Node
{
    Q_OBJECT
    Q_PROPERTY(QString status READ getStatus NOTIFY statusChanged)
    Q_PROPERTY(int productionYear READ getProductionYear NOTIFY productionYearChanged)
    Q_PROPERTY(QDate premiereDate READ getPremiereDate NOTIFY premiereDateChanged)

public:
    Series();

    void fromJson(const QJsonObject &jsonData) override;

    // GETTERS SETTERS
    QString getStatus() const;
    void setStatus(const QString &value);

    int getProductionYear() const;
    void setProductionYear(int value);

    QDate getPremiereDate() const;
    void setPremiereDate(const QDate &value);

    // INVOKABLES
    Q_INVOKABLE void updateChildren() override;

private:
    QString status;
    int productionYear;
    QDate premiereDate;

signals:
    void statusChanged();
    void productionYearChanged();
    void premiereDateChanged();

};

#endif // SERIES_H
