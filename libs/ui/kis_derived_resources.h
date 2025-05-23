/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_DERIVED_RESOURCES_H
#define __KIS_DERIVED_RESOURCES_H

#include "KoDerivedResourceConverter.h"
#include "KoResourceUpdateMediator.h"
#include <QScopedPointer>

class KisPresetUpdateMediator : public KoResourceUpdateMediator
{
    Q_OBJECT
public:
    KisPresetUpdateMediator();
    ~KisPresetUpdateMediator() override;
    void connectResource(QVariant sourceResource) override;

private Q_SLOTS:
    void slotSettingsChanged();
    void slotResourceChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

class KisCompositeOpResourceConverter : public KoDerivedResourceConverter
{
public:
    KisCompositeOpResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisEffectiveCompositeOpResourceConverter : public KoDerivedResourceConverter
{
public:
    KisEffectiveCompositeOpResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisOpacityToPresetOpacityResourceConverter : public KoDerivedResourceConverter, public QObject
{
public:
    KisOpacityToPresetOpacityResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisFlowResourceConverter : public KoDerivedResourceConverter, public QObject
{
public:
    KisFlowResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisFadeResourceConverter : public KoDerivedResourceConverter, public QObject
{
public:
    KisFadeResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisScatterResourceConverter : public KoDerivedResourceConverter, public QObject
{
public:
    KisScatterResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisSizeResourceConverter : public KoDerivedResourceConverter, public QObject
{
public:
    KisSizeResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisBrushRotationResourceConverter : public KoDerivedResourceConverter, public QObject
{
public:
    KisBrushRotationResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisPatternSizeResourceConverter : public KoDerivedResourceConverter, public QObject
{
public:
    KisPatternSizeResourceConverter();

    QVariant fromSource(const QVariant& value) override;
    QVariant toSource(const QVariant& value, const QVariant& sourceValue) override;
};

class KisLodAvailabilityResourceConverter : public KoDerivedResourceConverter
{
public:
    KisLodAvailabilityResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisLodSizeThresholdResourceConverter : public KoDerivedResourceConverter
{
public:
    KisLodSizeThresholdResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisLodSizeThresholdSupportedResourceConverter : public KoDerivedResourceConverter
{
public:
    KisLodSizeThresholdSupportedResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisEraserModeResourceConverter : public KoDerivedResourceConverter
{
public:
    KisEraserModeResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};

class KisBrushNameResourceConverter : public KoDerivedResourceConverter
{
public:
    KisBrushNameResourceConverter();

    QVariant fromSource(const QVariant &value) override;
    QVariant toSource(const QVariant &value, const QVariant &sourceValue) override;
};


#endif /* __KIS_DERIVED_RESOURCES_H */
