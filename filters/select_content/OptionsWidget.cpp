/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "OptionsWidget.h"
#include "ApplyDialog.h"
#include "Settings.h"
#include "ScopedIncDec.h"

#include <iostream>

namespace select_content {
    OptionsWidget::OptionsWidget(intrusive_ptr<Settings> const& settings,
                                 PageSelectionAccessor const& page_selection_accessor)
            : m_ptrSettings(settings),
              m_pageSelectionAccessor(page_selection_accessor),
              m_ignoreAutoManualToggle(0) {
        setupUi(this);

        setupUiConnections();
    }

    OptionsWidget::~OptionsWidget() {
    }

    void OptionsWidget::preUpdateUI(PageId const& page_id) {
        removeUiConnections();

        ScopedIncDec<int> guard(m_ignoreAutoManualToggle);

        m_pageId = page_id;
        autoBtn->setEnabled(false);
        manualBtn->setEnabled(false);
        disableBtn->setEnabled(false);
        pageDetectAutoBtn->setEnabled(false);
        pageDetectDisableBtn->setEnabled(false);

        setupUiConnections();
    }

    void OptionsWidget::postUpdateUI(UiData const& ui_data) {
        removeUiConnections();

        Margins m = ui_data.pageBorders();

        m_uiData = ui_data;

        leftBorder->setValue(m.left());
        topBorder->setValue(m.top());
        rightBorder->setValue(m.right());
        bottomBorder->setValue(m.bottom());

        updateModeIndication(ui_data.mode());
        fineTuneBtn->setChecked(ui_data.fineTuning());
        pageDetectAutoBtn->setChecked(ui_data.pageDetection());
        pageDetectDisableBtn->setChecked(!ui_data.pageDetection());
        autoBtn->setEnabled(true);
        manualBtn->setEnabled(true);
        disableBtn->setEnabled(true);
        pageDetectAutoBtn->setEnabled(true);
        pageDetectDisableBtn->setEnabled(true);
        fineTuneBtn->setEnabled(true);

        setupUiConnections();
    }

    void OptionsWidget::manualContentRectSet(QRectF const& content_rect) {
        m_uiData.setContentRect(content_rect);
        m_uiData.setMode(MODE_MANUAL);
        emit modeChanged(false);
        updateModeIndication(MODE_MANUAL);
        commitCurrentParams();

        emit invalidateThumbnail(m_pageId);
    }

    void OptionsWidget::modeChanged(bool const auto_mode) {
        if (auto_mode) {
            m_uiData.setMode(MODE_AUTO);
            m_uiData.setContentDetection(true);
            commitCurrentParams();
            emit reloadRequested();
        } else {
            m_uiData.setMode(MODE_MANUAL);
            m_uiData.setContentDetection(true);
            commitCurrentParams();
            if (m_uiData.pageDetection()) {
                m_uiData.setPageDetection(false);
                emit reloadRequested();
            }
        }
    }

    void OptionsWidget::autoMode() {
        modeChanged(true);
    }

    void OptionsWidget::manualMode() {
        modeChanged(false);
    }

    void OptionsWidget::fineTuningChanged(bool checked) {
        m_uiData.setFineTuneCorners(checked);
        commitCurrentParams();
        if (m_uiData.pageDetection()) {
            emit reloadRequested();
        }
    }

    void OptionsWidget::contentDetectionDisabled(void) {
        bool old = m_ignoreAutoManualToggle;
        m_ignoreAutoManualToggle = true;

        m_uiData.setContentDetection(false);
        commitCurrentParams();
        autoBtn->setChecked(false);
        manualBtn->setChecked(false);
        disableBtn->setChecked(true);
        emit reloadRequested();

        m_ignoreAutoManualToggle = old;
    }

    void OptionsWidget::pageDetectionDisabled(void) {
        m_uiData.setPageDetection(false);
        pageDetectAutoBtn->setChecked(false);
        pageDetectDisableBtn->setChecked(true);
        commitCurrentParams();
        emit reloadRequested();
    }

    void OptionsWidget::pageDetectionEnabled(void) {
        m_uiData.setPageDetection(true);
        pageDetectAutoBtn->setChecked(true);
        pageDetectDisableBtn->setChecked(false);
        commitCurrentParams();
        emit reloadRequested();
    }

    void OptionsWidget::borderChanged() {
        m_uiData.setPageBorders(leftBorder->value(), topBorder->value(), rightBorder->value(), bottomBorder->value());
        commitCurrentParams();
        if (!m_uiData.contentRect().isEmpty()) {
            emit reloadRequested();
        }
    }

    void OptionsWidget::updateModeIndication(AutoManualMode const mode) {
        ScopedIncDec<int> guard(m_ignoreAutoManualToggle);

        if (!m_uiData.contentDetection()) {
            disableBtn->setChecked(true);
            autoBtn->setChecked(false);
            manualBtn->setChecked(false);
        } else {
            disableBtn->setChecked(false);
            if (mode == MODE_AUTO) {
                autoBtn->setChecked(true);
                manualBtn->setChecked(false);
            } else {
                autoBtn->setChecked(false);
                manualBtn->setChecked(true);
                pageDetectDisableBtn->setChecked(true);
                pageDetectAutoBtn->setChecked(false);
            }
        }
    }

    void OptionsWidget::commitCurrentParams() {
        Params params(
                m_uiData.contentRect(), m_uiData.contentSizeMM(),
                Dependencies(), m_uiData.mode(), m_uiData.contentDetection(), m_uiData.pageDetection(),
                m_uiData.fineTuning()
        );
        params.setPageRect(m_uiData.pageRect());
        params.setPageBorders(m_uiData.pageBorders());
        params.computeDeviation(m_ptrSettings->avg());
        m_ptrSettings->setPageParams(m_pageId, params);
    }

    void OptionsWidget::showApplyToDialog() {
        ApplyDialog* dialog = new ApplyDialog(
                this, m_pageId, m_pageSelectionAccessor
        );
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(
                dialog, SIGNAL(applySelection(std::set<PageId> const &, bool)),
                this, SLOT(applySelection(std::set<PageId> const &, bool))
        );
        dialog->show();
    }

    void OptionsWidget::applySelection(std::set<PageId> const& pages, bool apply_content_box) {
        if (pages.empty()) {
            return;
        }

        Params const params(
                m_uiData.contentRect(), m_uiData.contentSizeMM(),
                m_uiData.dependencies(), m_uiData.mode(), m_uiData.contentDetection(),
                m_uiData.pageDetection(), m_uiData.fineTuning(),
                Margins(leftBorder->value(), topBorder->value(), rightBorder->value(), bottomBorder->value())
        );

        for (PageId const& page_id : pages) {
            Params new_params(params);

            std::unique_ptr<Params> old_params = m_ptrSettings->getPageParams(page_id);
            if (old_params.get()) {
                new_params.setPageRect(old_params->pageRect());

                if (new_params.isContentDetectionEnabled() && (new_params.mode() == MODE_MANUAL)) {
                    if (!apply_content_box) {
                        new_params.setContentRect(old_params->contentRect());
                        new_params.setContentSizeMM(old_params->contentSizeMM());
                    } else {
                        // we don't want the content box to be out of the page bounds
                        QRectF fixedContentRect = m_uiData.contentRect().intersected(old_params->pageRect());
                        if (fixedContentRect != m_uiData.contentRect()) {
                            if (fixedContentRect.isValid()) {
                                new_params.setContentRect(fixedContentRect);
                            } else {
                                new_params.setContentRect(old_params->pageRect());
                            }
                            // we need recalculate other params that can't be done here
                            // so we install empty dependencies to force that
                            new_params.setDependencies(Dependencies());
                        }
                    }
                }
            }

            m_ptrSettings->setPageParams(page_id, new_params);
        }

        if (pages.size() > 1) {
            emit invalidateAllThumbnails();
        } else {
            for (PageId const& page_id : pages) {
                emit invalidateThumbnail(page_id);
            }
        }

        emit reloadRequested();
    } // OptionsWidget::applySelection

    void OptionsWidget::setupUiConnections() {
        connect(autoBtn, SIGNAL(pressed()), this, SLOT(autoMode()));
        connect(manualBtn, SIGNAL(pressed()), this, SLOT(manualMode()));
        connect(disableBtn, SIGNAL(pressed()), this, SLOT(contentDetectionDisabled()));
        connect(pageDetectAutoBtn, SIGNAL(pressed()), this, SLOT(pageDetectionEnabled()));
        connect(pageDetectDisableBtn, SIGNAL(pressed()), this, SLOT(pageDetectionDisabled()));
        connect(applyToBtn, SIGNAL(clicked()), this, SLOT(showApplyToDialog()));
        connect(fineTuneBtn, SIGNAL(toggled(bool)), this, SLOT(fineTuningChanged(bool)));

        connect(leftBorder, SIGNAL(valueChanged(double)), this, SLOT(borderChanged()));
        connect(rightBorder, SIGNAL(valueChanged(double)), this, SLOT(borderChanged()));
        connect(topBorder, SIGNAL(valueChanged(double)), this, SLOT(borderChanged()));
        connect(bottomBorder, SIGNAL(valueChanged(double)), this, SLOT(borderChanged()));
    }

    void OptionsWidget::removeUiConnections() {
        disconnect(autoBtn, SIGNAL(pressed()), this, SLOT(autoMode()));
        disconnect(manualBtn, SIGNAL(pressed()), this, SLOT(manualMode()));
        disconnect(disableBtn, SIGNAL(pressed()), this, SLOT(contentDetectionDisabled()));
        disconnect(pageDetectAutoBtn, SIGNAL(pressed()), this, SLOT(pageDetectionEnabled()));
        disconnect(pageDetectDisableBtn, SIGNAL(pressed()), this, SLOT(pageDetectionDisabled()));
        disconnect(applyToBtn, SIGNAL(clicked()), this, SLOT(showApplyToDialog()));
        disconnect(fineTuneBtn, SIGNAL(toggled(bool)), this, SLOT(fineTuningChanged(bool)));

        disconnect(leftBorder, SIGNAL(valueChanged(double)), this, SLOT(borderChanged()));
        disconnect(rightBorder, SIGNAL(valueChanged(double)), this, SLOT(borderChanged()));
        disconnect(topBorder, SIGNAL(valueChanged(double)), this, SLOT(borderChanged()));
        disconnect(bottomBorder, SIGNAL(valueChanged(double)), this, SLOT(borderChanged()));
    }
    

/*========================= OptionsWidget::UiData ======================*/

    OptionsWidget::UiData::UiData()
            : m_mode(MODE_AUTO),
              m_contentDetection(true),
              m_pageDetection(false),
              m_fineTuneCorners(false),
              m_borders(0, 0, 0, 0) {
    }

    OptionsWidget::UiData::~UiData() {
    }

    void OptionsWidget::UiData::setSizeCalc(PhysSizeCalc const& calc) {
        m_sizeCalc = calc;
    }

    void OptionsWidget::UiData::setContentRect(QRectF const& content_rect) {
        m_contentRect = content_rect;
    }

    QRectF const& OptionsWidget::UiData::contentRect() const {
        return m_contentRect;
    }

    void OptionsWidget::UiData::setPageRect(QRectF const& page_rect) {
        m_pageRect = page_rect;
    }

    QRectF const& OptionsWidget::UiData::pageRect() const {
        return m_pageRect;
    }

    QSizeF OptionsWidget::UiData::contentSizeMM() const {
        return m_sizeCalc.sizeMM(m_contentRect);
    }

    void OptionsWidget::UiData::setDependencies(Dependencies const& deps) {
        m_deps = deps;
    }

    Dependencies const& OptionsWidget::UiData::dependencies() const {
        return m_deps;
    }

    void OptionsWidget::UiData::setMode(AutoManualMode const mode) {
        m_mode = mode;
    }

    AutoManualMode OptionsWidget::UiData::mode() const {
        return m_mode;
    }

    void OptionsWidget::UiData::setContentDetection(bool detect) {
        m_contentDetection = detect;
    }

    void OptionsWidget::UiData::setPageDetection(bool detect) {
        m_pageDetection = detect;
    }

    void OptionsWidget::UiData::setFineTuneCorners(bool fine_tune) {
        m_fineTuneCorners = fine_tune;
    }

    void OptionsWidget::UiData::setPageBorders(double left, double top, double right, double bottom) {
        m_borders.setLeft(left);
        m_borders.setTop(top);
        m_borders.setRight(right);
        m_borders.setBottom(bottom);
    }
}  // namespace select_content