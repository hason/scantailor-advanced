
#include "SplittingOptions.h"

namespace output {

    SplittingOptions::SplittingOptions() :
            splitOutput(false),
            foregroundType(BLACK_AND_WHITE_FOREGROUND) {
    }

    SplittingOptions::SplittingOptions(QDomElement const& el) :
            splitOutput(el.attribute("splitOutput") == "1"),
            foregroundType(parseForegroundType(el.attribute("foregroundType"))) {
    }

    QDomElement SplittingOptions::toXml(QDomDocument& doc, QString const& name) const {
        QDomElement el(doc.createElement(name));
        el.setAttribute("splitOutput", splitOutput ? "1" : "0");
        el.setAttribute("foregroundType", formatForegroundType(foregroundType));

        return el;
    }

    bool SplittingOptions::isSplitOutput() const {
        return splitOutput;
    }

    void SplittingOptions::setSplitOutput(bool splitOutput) {
        SplittingOptions::splitOutput = splitOutput;
    }

    SplittingOptions::ForegroundType SplittingOptions::getForegroundType() const {
        return foregroundType;
    }

    void SplittingOptions::setForegroundType(SplittingOptions::ForegroundType foregroundType) {
        SplittingOptions::foregroundType = foregroundType;
    }

    SplittingOptions::ForegroundType SplittingOptions::parseForegroundType(const QString& str) {
        if (str == "color") {
            return COLOR_FOREGROUND;
        } else {
            return BLACK_AND_WHITE_FOREGROUND;
        }
    }

    QString SplittingOptions::formatForegroundType(const SplittingOptions::ForegroundType type) {
        QString str = "";
        switch (type) {
            case BLACK_AND_WHITE_FOREGROUND:
                str = "bw";
                break;
            case COLOR_FOREGROUND:
                str = "color";
                break;
        }

        return str;
    }

    bool SplittingOptions::operator==(const SplittingOptions& other) const {
        return (splitOutput == other.splitOutput)
               && (foregroundType == other.foregroundType);
    }

    bool SplittingOptions::operator!=(const SplittingOptions& other) const {
        return !(*this == other);
    }
}