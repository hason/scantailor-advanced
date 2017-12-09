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

#include "Thumbnail.h"
#include <QPainter>

namespace select_content {
    Thumbnail::Thumbnail(intrusive_ptr<ThumbnailPixmapCache> const& thumbnail_cache,
                         QSizeF const& max_size,
                         ImageId const& image_id,
                         ImageTransformation const& xform,
                         QRectF const& content_rect,
                         QRectF const& page_rect,
                         bool page_rect_enabled,
                         bool deviant)
            : ThumbnailBase(thumbnail_cache, max_size, image_id, xform),
              m_contentRect(content_rect),
              m_pageRect(page_rect),
              m_pageRectEnabled(page_rect_enabled),
              m_deviant(deviant) {
    }

    void Thumbnail::paintOverImage(QPainter& painter, QTransform const& image_to_display,
                                   QTransform const& thumb_to_display) {
        if (m_contentRect.isNull()) {
            return;
        }

        QRectF page_rect(virtToThumb().mapRect(m_pageRect));

        painter.setRenderHint(QPainter::Antialiasing, false);

        if (m_pageRectEnabled) {
            QPen pen(QColor(0xee, 0xee, 0x00));
            pen.setWidth(1);
            pen.setCosmetic(true);
            painter.setPen(pen);

            painter.setBrush(Qt::NoBrush);

            painter.drawRect(page_rect);
        }

        QPen pen(QColor(0x00, 0x00, 0xff));
        pen.setWidth(1);
        pen.setCosmetic(true);
        painter.setPen(pen);

        painter.setBrush(QColor(0x00, 0x00, 0xff, 50));

        QRectF content_rect(virtToThumb().mapRect(m_contentRect));

        // Adjust to compensate for pen width.
        content_rect.adjust(-1, -1, 1, 1);
        content_rect = content_rect.intersected(page_rect);

        painter.drawRect(content_rect);

        if (m_deviant) {
            paintDeviant(painter);
        }
    }
}  // namespace select_content