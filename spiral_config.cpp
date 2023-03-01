// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "spiral_config.h"
#include "utils.h"
#include <QDir>
#include <QFile>
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>
#include <QSharedPointer>

namespace SpiralFun {

namespace {
constexpr int CONFIG_VERSION = 1;
}

const std::initializer_list<CircleConfig> DEFAULT_CONFIG = {
    { 4.0, 0, false, Qt::blue },
    { 2.0, 1, false, Qt::green },
    { 0.2, -5, true, Qt::white }
};
const std::initializer_list<CircleConfig> EXAMPLE1_CONFIG = {
    { 6.0, 0, false, Qt::blue },
    { 2.6, 1, false, Qt::green },
    { 1.6, 5, false, Qt::yellow },
    { 0.8, 40, true, Qt::red }
};
const std::initializer_list<CircleConfig> EXAMPLE2_CONFIG = {
    { 5.0, 0, false, Qt::blue },
    { 3.0, 1, false, Qt::yellow },
    { 1.6, 4, true, Qt::red },
    { 0.2, 200, true, Qt::white }
};
const std::initializer_list<CircleConfig> EXAMPLE3_CONFIG = {
    { 6.0, 0, false, Qt::blue },
    { 2.1, 1, false, Qt::green },
    { 1.6, -5, false, Qt::yellow },
    { 0.8, 25, true, Qt::red },
    { 0.4, -125, false, Qt::cyan },
    { 0.08, 625, true, Qt::white }
};
const std::initializer_list<CircleConfig> EXAMPLE4_CONFIG = {
    { 4.8, 0, false, Qt::cyan },
    { 2.8, 1, false, Qt::yellow },
    { 1.6, -3, true, Qt::green },
    { 0.8, 9, false, Qt::magenta },
    { 0.4, -27, true, Qt::blue },
    { 0.2, 81, false, Qt::red },
    { 0.04, -243, true, Qt::white }
};

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

QObjectList SpiralConfig::getConfigFiles() const
{
    const QString path = Utils::getSpiralCongifPath();
    QDir dir(path);
    const auto files = dir.entryList({"SPIRAL_*.json"}, QDir::Files, QDir::Time);

    QObjectList configFileList;
    for (const auto& f : files)
    {
        auto* entry = new ConfigFileEntry;
        entry->mJsonFileName = dir.absoluteFilePath(f);
        entry->mDisplayName = f.sliced(7, f.size() - 7 - 5); // part between SPIRAL_ and .json
        entry->mImageFileName = dir.absoluteFilePath("IMG_" + entry->mDisplayName + ".jpg");
        if (!QFile::exists(entry->mImageFileName))
            entry->mImageFileName.clear();

        configFileList.push_back(entry);
    }

    return configFileList;
}

CircleConfigList SpiralConfig::load(const QString& fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot open file:" << fileName;
        return {};
    }

    const QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull())
    {
        qWarning() << "Not a valid config file:" << fileName;
        return {};
    }

    return createConfig(doc);
}

CircleConfigList SpiralConfig::createConfig(const QJsonDocument& doc) const
{
    if (!doc.isObject())
    {
        qWarning() << "JSON document is not an object";
        return {};
    }

    QJsonObject root = doc.object();
    QJsonValue circlesValue = root["Circles"];
    if (!circlesValue.isArray())
    {
        qWarning() << "JSON Circles array missing";
        return {};
    }

    CircleConfigList circleCfgList;
    QJsonArray circlesArray = circlesValue.toArray();
    for (const QJsonValue& val : circlesArray)
    {
        QJsonObject circle = val.toObject();
        if (!checkField(circle, "Color", QJsonValue::String) &&
            !checkField(circle, "Draw", QJsonValue::Bool) &&
            !checkField(circle, "Radius", QJsonValue::Double) &&
            !checkField(circle, "Speed", QJsonValue::Double))
        {
            return {};
        }

        CircleConfig circleCfg;
        circleCfg.mColor = QColor(circle["Color"].toString());
        circleCfg.mDraw = circle["Draw"].toBool();
        circleCfg.mRelRadius = circle["Radius"].toDouble();
        circleCfg.mSpeed = circle["Speed"].toInt();
        circleCfgList.push_back(circleCfg);
    }

    if (!isValid(circleCfgList))
        return {};

    return circleCfgList;
}

bool SpiralConfig::checkField(const QJsonObject& object, const QString& key, QJsonValue::Type type) const
{
    if (!object.contains(key))
    {
        qWarning() << "JSON field missing:" << key;
        return false;
    }

    if (object.value(key).type() != type)
    {
        qWarning() << "JSON field" << key << "does not have type:" << type;
        return false;
    }

    return true;
}

bool SpiralConfig::isValid(const CircleConfigList& cfgList) const
{
    if (cfgList.size() < MIN_CIRCLES)
    {
        qWarning() << "Config must contain at least" << MIN_CIRCLES << "circles";
        return false;
    }

    if (cfgList.size() > MAX_CIRCLES)
    {
        qWarning() << "Config must contain at most" << MAX_CIRCLES << "circles";
        return false;
    }

    if (cfgList[0].mSpeed != 0)
    {
        qWarning() << "Speed of first circle must be 0";
        return false;
    }

    if (cfgList[0].mDraw)
    {
        qWarning() << "First circle cannot draw";
        return false;
    }

    if (cfgList[1].mSpeed != 1)
    {
        qWarning() << "Speed of seconds circle must be 1";
        return false;
    }

    for (unsigned i = 0; i < cfgList.size(); ++i)
    {
        const CircleConfig& cfg = cfgList[i];
        if (!cfg.mColor.isValid())
        {
            qWarning() << "Circle" << i << "invalid color";
            return false;
        }
        if (cfg.mRelRadius > MAX_REL_RADIUS)
        {
            qWarning() << "Circle" << i << "radius >" << MAX_REL_RADIUS;
            return false;
        }
        if (std::abs(cfg.mSpeed) > MAX_SPEED)
        {
            qWarning() << "Circle" << i << "speed >" << MAX_SPEED;
            return false;
        }
    }

    return true;
}

}
