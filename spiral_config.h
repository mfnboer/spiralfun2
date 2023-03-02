// Copyright (C) 2023 Michel de Boer
// License: GPLv3
#pragma once
#include "circle.h"
#include <QJsonDocument>
#include <vector>

namespace SpiralFun {

struct CircleConfig
{
    qreal mRelRadius;
    int mSpeed;
    bool mDraw;
    QColor mColor;
};

using CircleConfigList = std::vector<CircleConfig>;

extern const std::initializer_list<CircleConfig> DEFAULT_CONFIG;
extern const std::initializer_list<CircleConfig> EXAMPLE1_CONFIG;
extern const std::initializer_list<CircleConfig> EXAMPLE2_CONFIG;
extern const std::initializer_list<CircleConfig> EXAMPLE3_CONFIG;
extern const std::initializer_list<CircleConfig> EXAMPLE4_CONFIG;

class ConfigFileEntry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString imgFileName MEMBER mImageFileName CONSTANT)
    Q_PROPERTY(QString jsonFileName MEMBER mJsonFileName CONSTANT)
    Q_PROPERTY(QString displayName MEMBER mDisplayName CONSTANT)

public:
    QString mImageFileName;
    QString mJsonFileName;
    QString mDisplayName;
};

class SpiralConfig
{
public:
    static constexpr int MIN_CIRCLES = 2;
    static constexpr int MAX_CIRCLES = 10;
    static constexpr int MAX_REL_RADIUS = 12;
    static constexpr int MAX_SPEED = 9999;

    SpiralConfig(const CircleList& circles, qreal defaultRadius);

    void save(const QImage& img) const;
    QObjectList getConfigFiles() const;
    CircleConfigList load(const QString& fileName) const;
    void remove(const QStringList& fileNameList) const;

private:
    QJsonDocument createJsonDoc() const;
    CircleConfigList createConfig(const QJsonDocument& doc) const;
    void checkField(const QJsonObject& object, const QString& key, QJsonValue::Type type) const;
    bool isValid(const CircleConfigList& cfgList, QString& error) const;

    const CircleList& mCircles;
    const qreal mDefaultRadius;
};

}
