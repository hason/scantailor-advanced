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

#ifndef OUTPUT_PARAMS_H_
#define OUTPUT_PARAMS_H_

#include "Dpi.h"
#include "ColorParams.h"
#include "DewarpingOptions.h"
#include "dewarping/DistortionModel.h"
#include "DepthPerception.h"
#include "DespeckleLevel.h"
#include "PictureShapeOptions.h"

class QDomDocument;
class QDomElement;

namespace output {
    class Params {
    public:
        Params();

        Params(QDomElement const& el);

        Dpi const& outputDpi() const {
            return m_dpi;
        }

        void setOutputDpi(Dpi const& dpi) {
            m_dpi = dpi;
        }

        ColorParams const& colorParams() const {
            return m_colorParams;
        }

        PictureShapeOptions pictureShapeOptions() const {
            return m_pictureShapeOptions;
        }

        void setPictureShapeOptions(PictureShapeOptions opt) {
            m_pictureShapeOptions = opt;
        }

        void setColorParams(ColorParams const& params) {
            m_colorParams = params;
        }

        const SplittingOptions& splittingOptions() const {
            return m_splittingOptions;
        }

        void setSplittingOptions(const SplittingOptions& opt) {
            m_splittingOptions = opt;
        }

        DewarpingOptions const& dewarpingOptions() const {
            return m_dewarpingOptions;
        }

        void setDewarpingOptions(DewarpingOptions const& opt) {
            m_dewarpingOptions = opt;
        }

        dewarping::DistortionModel const& distortionModel() const {
            return m_distortionModel;
        }

        void setDistortionModel(dewarping::DistortionModel const& model) {
            m_distortionModel = model;
        }

        DepthPerception const& depthPerception() const {
            return m_depthPerception;
        }

        void setDepthPerception(DepthPerception depth_perception) {
            m_depthPerception = depth_perception;
        }

        DespeckleLevel despeckleLevel() const {
            return m_despeckleLevel;
        }

        void setDespeckleLevel(DespeckleLevel level) {
            m_despeckleLevel = level;
        }

        QDomElement toXml(QDomDocument& doc, QString const& name) const;

    private:
        static ColorParams::ColorMode parseColorMode(QString const& str);

        static QString formatColorMode(ColorParams::ColorMode mode);

        Dpi m_dpi;
        ColorParams m_colorParams;
        SplittingOptions m_splittingOptions;
        PictureShapeOptions m_pictureShapeOptions;
        dewarping::DistortionModel m_distortionModel;
        DepthPerception m_depthPerception;
        DewarpingOptions m_dewarpingOptions;
        DespeckleLevel m_despeckleLevel;
    };
}  // namespace output
#endif  // ifndef OUTPUT_PARAMS_H_
