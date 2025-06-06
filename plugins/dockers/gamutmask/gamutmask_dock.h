/*
 *  SPDX-FileCopyrightText: 2018 Anna Medonosova <anna.medonosova@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef H_GAMUT_MASK_DOCK_H
#define H_GAMUT_MASK_DOCK_H

#include <QDockWidget>
#include <QMessageBox>

#include <KoCanvasObserverBase.h>
#include <KoResourceServerProvider.h>
#include <KoResourceServer.h>
#include <resources/KoGamutMask.h>
#include <KisView.h>
#include <kis_types.h>
#include <KisResourceItemChooser.h>

#include <kis_mainwindow_observer.h>

class KisDocument;
class KisCanvasResourceProvider;
class QButtonGroup;
class QMenu;

struct GamutMaskChooserUI;

class GamutMaskDock: public QDockWidget, public KisMainwindowObserver, public KoResourceServerObserver<KoGamutMask>
{
    Q_OBJECT

public:
    GamutMaskDock();
    ~GamutMaskDock() override;
    QString observerName() override { return "GamutMaskDock"; }
    void setViewManager(KisViewManager* kisview) override;
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

public: // KoResourceServerObserver
    void unsetResourceServer() override;
    void resourceAdded(KoGamutMaskSP /*resource*/) override {}
    void removingResource(KoGamutMaskSP resource) override;
    void resourceChanged(KoGamutMaskSP resource) override;

Q_SIGNALS:
    void sigGamutMaskSet(KoGamutMaskSP mask);
    void sigGamutMaskChanged(KoGamutMaskSP mask);
    void sigGamutMaskUnset();
    void sigGamutMaskPreviewUpdate();

private Q_SLOTS:
    void slotGamutMaskEdit();
    void slotGamutMaskSave();
    void slotGamutMaskCancelEdit();
    void slotGamutMaskSelected(KoGamutMaskSP mask);
    void slotGamutMaskPreview();
    void slotGamutMaskCreateNew();
    void slotGamutMaskDuplicate();
    void slotGamutMaskDelete();

    void slotDocumentRemoved(QString filename);
    void slotViewChanged();
    void slotDocumentSaved();

private:
    void closeMaskDocument();
    bool openMaskEditor();
    void cancelMaskEdit();
    void selectMask(KoGamutMaskSP mask, bool notifyItemChooser = true);
    bool saveSelectedMaskResource();
    void deleteMask();
    int getUserFeedback(QString text, QString informativeText = "",
                        QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No,
                        QMessageBox::StandardButton defaultButton = QMessageBox::Yes,
                        QMessageBox::Icon severity = QMessageBox::Warning);

    int saveOrCancel(QMessageBox::StandardButton defaultAction = QMessageBox::Save);

    KoGamutMaskSP createMaskResource(KoGamutMaskSP sourceMask, QString newTitle);

    QPair<QString, QFileInfo> resolveMaskTitle(QString suggestedTitle);

    QList<KoShape*> getShapesFromLayer();
    KisShapeLayerSP getShapeLayer();

    KisCanvasResourceProvider* m_resourceProvider {nullptr};

    bool m_selfClosingTemplate {false};
    bool m_externalTemplateClose {false};
    bool m_creatingNewMask {false};
    bool m_templatePrevSaved {false};
    bool m_selfSelectingMask {false};

    GamutMaskChooserUI* m_dockerUI {nullptr};
    KisResourceItemChooser* m_maskChooser {nullptr};

    KoGamutMaskSP m_selectedMask;

    KisDocument* m_maskDocument {nullptr};
    KisView* m_view {nullptr};
};


#endif // H_GAMUT_MASK_DOCK_H
