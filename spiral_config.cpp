// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#include "spiral_config.h"
#include "exception.h"
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

void SpiralConfig::save(const QImage& img) const
{
    const QString path = Utils::getSpiralCongifPath();
    if (path.isNull())
        throw RuntimeException("Failed to access storage.");

    const QString baseName = Utils::createDateTimeName();
    const QString cfgFileName = path + QString("/SPIRAL_%1.json").arg(baseName);
    QFile file(cfgFileName);
    if (!file.open(QIODevice::WriteOnly))
        throw RuntimeException(QString("Failed to create: %1").arg(cfgFileName));

    qDebug() << "Cfg file:" << cfgFileName;

    const auto json = createJsonDoc();
    if (file.write(json.toJson()) == -1)
        throw RuntimeException(QString("Failed to write: %1").arg(cfgFileName));

    file.close();

    const QString imgFileName = path + QString("/IMG_%1.jpg").arg(baseName);
    qDebug() << "Image file:" << cfgFileName;
    if (!img.save(imgFileName))
    {
        file.remove();
        throw RuntimeException(QString("Failed to save: %1").arg(imgFileName));
    }
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
        throw RuntimeException(QString("Cannot open file: %1").arg(fileName));

    const QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull())
        throw RuntimeException(QString("Not a valid config file: %1").arg(fileName));

    return createConfig(doc);
}

void SpiralConfig::remove(const QStringList& fileNameList) const
{
    static const QRegularExpression jsonFileRe("SPIRAL_(.*)\\.json");
    for (const auto& jsonFileName : fileNameList)
    {
        QString imgFileName(jsonFileName);
        imgFileName.replace(jsonFileRe, "IMG_\\1.jpg");
        qDebug() << "Delete:" << imgFileName;
        qDebug() << "Delete:" << jsonFileName;
        QFile::remove(imgFileName);
        QFile::remove(jsonFileName);
    }
}

CircleConfigList SpiralConfig::createConfig(const QJsonDocument& doc) const
{
    if (!doc.isObject())
        throw RuntimeException("JSON document is not an object");

    QJsonObject root = doc.object();
    QJsonValue circlesValue = root["Circles"];

    if (!circlesValue.isArray())
        throw RuntimeException("JSON Circles array missing");

    CircleConfigList circleCfgList;
    QJsonArray circlesArray = circlesValue.toArray();
    for (const QJsonValue& val : circlesArray)
    {
        QJsonObject circle = val.toObject();
        checkField(circle, "Color", QJsonValue::String);
        checkField(circle, "Draw", QJsonValue::Bool);
        checkField(circle, "Radius", QJsonValue::Double);
        checkField(circle, "Speed", QJsonValue::Double);

        CircleConfig circleCfg;
        circleCfg.mColor = QColor(circle["Color"].toString());
        circleCfg.mDraw = circle["Draw"].toBool();
        circleCfg.mRelRadius = circle["Radius"].toDouble();
        circleCfg.mSpeed = circle["Speed"].toInt();
        circleCfgList.push_back(circleCfg);
    }

    QString error;
    if (!isValid(circleCfgList, error))
    {
        qDebug() << error;
        throw RuntimeException("Malformed config file: " + error);
    }

    return circleCfgList;
}

void SpiralConfig::checkField(const QJsonObject& object, const QString& key, QJsonValue::Type type) const
{
    if (!object.contains(key))
        throw RuntimeException(QString("JSON field missing: %1").arg(key));

    if (object.value(key).type() != type)
        throw RuntimeException(QString("JSON field %1 does not have type: %2").arg(key).arg(type));
}

bool SpiralConfig::isValid(const CircleConfigList& cfgList, QString& error) const
{
    if (cfgList.size() < MIN_CIRCLES)
    {
        error = QString("Config must contain at least %1 circles").arg(MIN_CIRCLES);
        return false;
    }

    if (cfgList.size() > MAX_CIRCLES)
    {
        error = QString("Config must contain at most %1 circles").arg(MAX_CIRCLES);
        return false;
    }

    if (cfgList[0].mSpeed != 0)
    {
        error = QString("Circle[0] speed(%1) must be 0").arg(cfgList[0].mSpeed);
        return false;
    }

    if (cfgList[0].mDraw)
    {
        error = "Circle[0] cannot draw";
        return false;
    }

    for (unsigned i = 0; i < cfgList.size(); ++i)
    {
        const CircleConfig& cfg = cfgList[i];
        if (!cfg.mColor.isValid())
        {
            error = QString("Circle[%1] invalid color").arg(i);
            return false;
        }
        if (cfg.mRelRadius > MAX_REL_RADIUS)
        {
            error = QString("Circle[%1] radius(%2) > %3").arg(i).arg(cfg.mRelRadius).arg(MAX_REL_RADIUS);
            return false;
        }
        if (std::abs(cfg.mSpeed) > MAX_SPEED)
        {
            error = QString("Circle[%1] speed(%2) > %3").arg(i).arg(cfg.mSpeed).arg(MAX_SPEED);
            return false;
        }
    }

    return true;
}

}
