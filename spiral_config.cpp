// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "spiral_config.h"
#include "utils.h"
#include <QFile>
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>

namespace SpiralFun {

namespace {
constexpr int CONFIG_VERSION = 1;
}

SpiralConfig::SpiralConfig(const CircleList& circles, qreal defaultRadius) :
    mCircles(circles),
    mDefaultRadius(defaultRadius)
{
}

QJsonDocument SpiralConfig::createJsonDoc() const
{
    QJsonObject root;
    root.insert("ConfigVersion", CONFIG_VERSION);
    root.insert("AppVersion", APP_VERSION);

    QJsonArray circles;
    for (const auto& c : mCircles)
    {
        QJsonObject circle;
        qreal relRadius = std::round((c->getRadius() / mDefaultRadius) * 100) / 100.0;
        circle.insert("Radius", relRadius);
        circle.insert("Speed", c->getSpeed());
        circle.insert("Draw", c->getDraw());
        circle.insert("Color", c->getColor().name(QColor::HexRgb));

        circles.push_back(circle);
    }

    root.insert("Circles", circles);
    QJsonDocument doc;
    doc.setObject(root);

    return doc;
}

bool SpiralConfig::save(const QImage& img) const
{
    const QString path = Utils::getSpiralCongifPath();
    if (path.isNull())
        return false;

    const QString baseName = Utils::createDateTimeName();
    const QString cfgFileName = path + QString("/SPIRAL_%1.json").arg(baseName);
    QFile file(cfgFileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Failed to create:" << cfgFileName;
        return false;
    }
    qDebug() << "Cfg file:" << cfgFileName;

    const auto json = createJsonDoc();
    if (file.write(json.toJson()) == -1)
    {
        qWarning() << "Failed to write:" << cfgFileName;
        return false;
    }
    file.close();

    const QString imgFileName = path + QString("/IMG_%1.jpg").arg(baseName);
    qDebug() << "Image file:" << cfgFileName;
    if (!img.save(imgFileName))
    {
        qWarning() << "Failed to save:" << imgFileName;
        file.remove();
        return false;
    }

    return true;
}

}
