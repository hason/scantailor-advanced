
#ifndef SCANTAILOR_OTSUBINARIZATIONOPTIONSWIDGET_H
#define SCANTAILOR_OTSUBINARIZATIONOPTIONSWIDGET_H

#include "ui_OtsuBinarizationOptionsWidget.h"
#include "BinarizationOptionsWidget.h"
#include "ColorParams.h"
#include "intrusive_ptr.h"
#include "Settings.h"
#include <QtCore>

namespace output {

    class OtsuBinarizationOptionsWidget : public BinarizationOptionsWidget, private Ui::OtsuBinarizationOptionsWidget {
    Q_OBJECT

    private:
        intrusive_ptr<Settings> m_ptrSettings;
        PageId m_pageId;
        ColorParams m_colorParams;
        int m_ignoreThresholdChanges;

    public:
        explicit OtsuBinarizationOptionsWidget(intrusive_ptr<Settings> settings);

        ~OtsuBinarizationOptionsWidget() override = default;

        void preUpdateUI(const PageId& m_pageId) override;

    private slots:

        void bwThresholdChanged();

        void thresholdValueChanged(int value);

        void setLighterThreshold();

        void setDarkerThreshold();

        void setNeutralThreshold();

    private:
        void updateView();

        void setupUiConnections();

        void removeUiConnections();
    };


}


#endif //SCANTAILOR_OTSUBINARIZATIONOPTIONSWIDGET_H
