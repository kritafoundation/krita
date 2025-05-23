/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
    SPDX-FileCopyrightText: 2006 Peter Simonsson <peter.simonsson@gmail.com>
    SPDX-FileCopyrightText: 2006-2007 C. Boemann <cbo@boemann.dk>

    SPDX-License-Identifier: LGPL-2.0-only
*/
#include "KoZoomAction.h"
#include "KoZoomMode.h"
#include "KoZoomWidget.h"

#include <KoIcon.h>

#include <QString>
#include <QLocale>
#include <QStringList>
#include <QRegExp>
#include <QList>
#include <QSlider>
#include <QLineEdit>
#include <QGridLayout>
#include <QStatusBar>
#include <QButtonGroup>

#include <klocalizedstring.h>
#include <WidgetsDebug.h>
#include <kis_signal_compressor.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include "krita_container_utils.h"

#include <math.h>

class Q_DECL_HIDDEN KoZoomAction::Private
{
public:

    Private(KoZoomAction *_parent)
        : parent(_parent)
        , minimumZoomValue(-1)
        , maximumZoomValue(-1)
        , guiUpdateCompressor(200, KisSignalCompressor::FIRST_ACTIVE)
    {}

    KoZoomAction *parent {nullptr};

    KoZoomMode::Modes zoomModes;
    QList<qreal> sliderLookup;

    qreal effectiveZoom {0.0};

    QList<qreal> generateSliderZoomLevels() const;
    QList<qreal> filterMenuZoomLevels(const QList<qreal> &zoomLevels) const;

    qreal minimumZoomValue {0.0};
    qreal maximumZoomValue {0.0};

    KisSignalCompressor guiUpdateCompressor;
};

QList<qreal> KoZoomAction::Private::generateSliderZoomLevels() const
{
    KConfigGroup config = KSharedConfig::openConfig()->group("");
    int steps = config.readEntry("zoomSteps", 2);
    qreal k = steps / M_LN2;

    int first =  ceil(log(parent->minimumZoom()) * k);
    int size  = floor(log(parent->maximumZoom()) * k) - first + 1;
    QVector<qreal> zoomLevels(size);

    // enforce zoom levels relating to thirds (33.33%, 66.67%, ...)
    QVector<qreal> snap(steps);
    if (steps > 1) {
        qreal third = log(4./ 3.) * k;
        int i = round(third);
        snap[(i - first) % steps] = third - i;
    }

    k = 1./ k;
    for (int i = 0; i < steps; i++) {
        qreal f = exp((i + first + snap[i]) * k);
        f = floor(f * 0x1p48 + 0.5) / 0x1p48; // round off inaccuracies
        for (int j = i; j < size; j += steps, f *= 2.) {
            zoomLevels[j] = f;
        }
    }

    return QList<qreal>::fromVector(zoomLevels);
}

QList<qreal> KoZoomAction::Private::filterMenuZoomLevels(const QList<qreal> &zoomLevels) const
{
    QList<qreal> filteredZoomLevels;

    Q_FOREACH (qreal zoom, zoomLevels) {
        if (zoom >= 0.2 && zoom <= 10) {
            filteredZoomLevels << zoom;
        }
    }

    return filteredZoomLevels;
}

KoZoomAction::KoZoomAction(KoZoomMode::Modes zoomModes, const QString& text, QObject *parent)
    : KSelectAction(text, parent)
    , d(new Private(this))
{
    d->zoomModes = zoomModes;
    setIcon(koIcon("zoom-original"));
    setEditable( true );
    setMaxComboViewCount( 15 );

    d->sliderLookup = d->generateSliderZoomLevels();

    d->effectiveZoom = 1.0;
    regenerateItems(d->effectiveZoom);
    connect( this, SIGNAL(textTriggered(QString)), SLOT(triggered(QString)));
    connect(&d->guiUpdateCompressor, SIGNAL(timeout()), SLOT(slotUpdateGuiAfterZoom()));
}

KoZoomAction::~KoZoomAction()
{
    delete d;
}

qreal KoZoomAction::effectiveZoom() const
{
    return d->effectiveZoom;
}

void KoZoomAction::setZoom(qreal zoom)
{
    setEffectiveZoom(zoom);
}

void KoZoomAction::triggered(const QString& text)
{
    QString zoomString = text;
    zoomString = zoomString.remove( '&' );

    KoZoomMode::Mode mode = KoZoomMode::toMode(zoomString);
    double zoom = 0;

    if( mode == KoZoomMode::ZOOM_CONSTANT ) {
        bool ok;
        QRegExp regexp( ".*(\\d+(\\.\\d+)?).*" ); // "Captured" non-empty sequence of digits
        int pos = regexp.indexIn( zoomString );
        if( pos > -1 ) {
            zoom = regexp.cap( 1 ).toDouble( &ok );

            if( !ok ) {
                zoom = 0;
            }
        }
    }

    Q_EMIT zoomChanged( mode, zoom/100.0 );
}

void KoZoomAction::regenerateItems(const qreal zoom)
{
    // TODO: refactor this method to become less slow, then
    //       we could reduce the timeout of d->guiUpdateCompressor
    //       to at least 80ms (12.5fps)

    QList<qreal> zoomLevels = d->filterMenuZoomLevels(d->sliderLookup);

    if( !zoomLevels.contains( zoom ) )
        zoomLevels << zoom;

    std::sort(zoomLevels.begin(), zoomLevels.end());

    // update items with new sorted zoom values
    QStringList values;
    if(d->zoomModes & KoZoomMode::ZOOM_PAGE) {
        values << KoZoomMode::toString(KoZoomMode::ZOOM_PAGE);
    }
    if(d->zoomModes & KoZoomMode::ZOOM_WIDTH) {
        values << KoZoomMode::toString(KoZoomMode::ZOOM_WIDTH);
    }
    if(d->zoomModes & KoZoomMode::ZOOM_HEIGHT) {
        values << KoZoomMode::toString(KoZoomMode::ZOOM_HEIGHT);
    }

    Q_FOREACH (qreal value, zoomLevels) {
        const qreal valueInPercent = value * 100;
        const int precision = (value > 10.0) ? 0 : 1;

        values << i18n("%1%", QLocale().toString(valueInPercent, 'f', precision));
    }

    setItems( values );

    Q_EMIT zoomLevelsChanged(values);

    {
        const qreal zoomInPercent = zoom * 100;
        const int precision = (zoom > 10.0) ? 0 : 1;

        const QString valueString = i18n("%1%", QLocale().toString(zoomInPercent, 'f', precision));

        setCurrentAction(valueString);

        Q_EMIT currentZoomLevelChanged(valueString);
    }
}

void KoZoomAction::sliderValueChanged(int value)
{
    if (value < d->sliderLookup.size()) {
        setZoom(d->sliderLookup[value]);
        Q_EMIT zoomChanged(KoZoomMode::ZOOM_CONSTANT, d->sliderLookup[value]);
    }
}

qreal KoZoomAction::nextZoomLevel() const
{
    const qreal eps = 1e-5;
    int i = 0;
    while (i < d->sliderLookup.size() - 1 && d->effectiveZoom > d->sliderLookup[i] - eps) {
        i++;
    }

    return qMax(d->effectiveZoom, d->sliderLookup[i]);
}

qreal KoZoomAction::prevZoomLevel() const
{
    const qreal eps = 1e-5;
    int i = d->sliderLookup.size() - 1;
    while (i > 0 && d->effectiveZoom < d->sliderLookup[i] + eps) i--;

    return qMin(d->effectiveZoom, d->sliderLookup[i]);
}

void KoZoomAction::zoomIn()
{
    qreal zoom = nextZoomLevel();

    if (zoom > d->effectiveZoom) {
        setZoom(zoom);
        Q_EMIT zoomChanged(KoZoomMode::ZOOM_CONSTANT, d->effectiveZoom);
    }
}

void KoZoomAction::zoomOut()
{
    qreal zoom = prevZoomLevel();

    if (zoom < d->effectiveZoom) {
        setZoom(zoom);
        Q_EMIT zoomChanged(KoZoomMode::ZOOM_CONSTANT, d->effectiveZoom);
    }
}

QWidget * KoZoomAction::createWidget(QWidget *parent)
{
    KoZoomWidget* zoomWidget = new KoZoomWidget(parent, d->sliderLookup.size() - 1);

    connect(this, SIGNAL(zoomLevelsChanged(QStringList)), zoomWidget, SLOT(setZoomLevels(QStringList)));
    connect(this, SIGNAL(sliderZoomLevelsChanged(int)), zoomWidget, SLOT(setSliderSize(int)));
    connect(this, SIGNAL(currentZoomLevelChanged(QString)), zoomWidget, SLOT(setCurrentZoomLevel(QString)));
    connect(this, SIGNAL(sliderChanged(int)), zoomWidget, SLOT(setSliderValue(int)));
    connect(this, SIGNAL(canvasMappingModeChanged(bool)), zoomWidget, SLOT(setCanvasMappingMode(bool)));

    connect(zoomWidget, SIGNAL(sliderValueChanged(int)), this, SLOT(sliderValueChanged(int)));
    connect(zoomWidget, SIGNAL(zoomLevelChanged(QString)), this, SLOT(triggered(QString)));
    connect(zoomWidget, SIGNAL(canvasMappingModeChanged(bool)), this, SIGNAL(canvasMappingModeChanged(bool)));
    connect(zoomWidget, SIGNAL(zoomedToSelection()), this, SIGNAL(zoomedToSelection()));
    connect(zoomWidget, SIGNAL(zoomedToAll()), this, SIGNAL(zoomedToAll()));

    regenerateItems(d->effectiveZoom);

    syncSliderWithZoom();

    return zoomWidget;
}

void KoZoomAction::setEffectiveZoom(qreal zoom)
{
    if(d->effectiveZoom == zoom)
        return;

    zoom = clampZoom(zoom);
    d->effectiveZoom = zoom;
    d->guiUpdateCompressor.start();
}

void KoZoomAction::slotUpdateGuiAfterZoom()
{
    syncSliderWithZoom();

    // TODO: don't regenerate when only mode changes
    regenerateItems(d->effectiveZoom);
}

void KoZoomAction::slotUpdateZoomLevels()
{
    qreal currentZoom = d->effectiveZoom;
    d->sliderLookup = d->generateSliderZoomLevels();
    regenerateItems(currentZoom);
    syncSliderWithZoom();

    Q_EMIT sliderZoomLevelsChanged(d->sliderLookup.size() - 1);

}

void KoZoomAction::setSelectedZoomMode(KoZoomMode::Mode mode)
{
    QString modeString(KoZoomMode::toString(mode));
    setCurrentAction(modeString);

    Q_EMIT currentZoomLevelChanged(modeString);
}

void KoZoomAction::setCanvasMappingMode(bool status)
{
    Q_EMIT canvasMappingModeChanged(status);
}

void KoZoomAction::syncSliderWithZoom()
{
    const qreal eps = 1e-5;
    int i = d->sliderLookup.size() - 1;
    while (d->effectiveZoom < d->sliderLookup[i] + eps && i > 0) i--;
    
    Q_EMIT sliderChanged(i);
}

qreal KoZoomAction::minimumZoom()
{
    if (d->minimumZoomValue < 0) {
        return KoZoomMode::minimumZoom();
    }
    return d->minimumZoomValue;
}

qreal KoZoomAction::maximumZoom()
{
    if (d->maximumZoomValue < 0) {
        return KoZoomMode::maximumZoom();
    }
    return d->maximumZoomValue;
}

qreal KoZoomAction::clampZoom(qreal zoom)
{
    return qMin(maximumZoom(), qMax(minimumZoom(), zoom));
}

void KoZoomAction::setMinimumZoom(qreal zoom)
{
    Q_ASSERT(zoom > 0.0f);
    KoZoomMode::setMinimumZoom(zoom);
    d->minimumZoomValue = zoom;
    d->sliderLookup = d->generateSliderZoomLevels();
    regenerateItems(d->effectiveZoom);
    syncSliderWithZoom();
}

void KoZoomAction::setMaximumZoom(qreal zoom)
{
    Q_ASSERT(zoom > 0.0f);
    KoZoomMode::setMaximumZoom(zoom);
    d->maximumZoomValue = zoom;
    d->sliderLookup = d->generateSliderZoomLevels();
    regenerateItems(d->effectiveZoom);
    syncSliderWithZoom();
}

void KoZoomAction::setMinMaxZoom(qreal min, qreal max)
{
    Q_ASSERT(min > 0.0f);
    Q_ASSERT(max > 0.0f);
    KoZoomMode::setMinimumZoom(min);
    KoZoomMode::setMaximumZoom(max);
    d->minimumZoomValue = min;
    d->maximumZoomValue = max;
    d->sliderLookup = d->generateSliderZoomLevels();
    regenerateItems(d->effectiveZoom);
    syncSliderWithZoom();
}
