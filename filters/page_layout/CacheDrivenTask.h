/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2007-2008  Joseph Artsimovich <joseph_a@mail.ru>

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

#ifndef PAGE_LAYOUT_CACHEDRIVENTASK_H_
#define PAGE_LAYOUT_CACHEDRIVENTASK_H_

#include "NonCopyable.h"
#include "ref_countable.h"
#include "intrusive_ptr.h"

class QRectF;
class PageInfo;
class AbstractFilterDataCollector;
class ImageTransformation;

namespace output {
    class CacheDrivenTask;
}

namespace page_layout {
    class Settings;

    class CacheDrivenTask : public ref_countable {
    DECLARE_NON_COPYABLE(CacheDrivenTask)

    public:
        CacheDrivenTask(intrusive_ptr<output::CacheDrivenTask> const& next_task,
                        intrusive_ptr<Settings> const& settings);

        virtual ~CacheDrivenTask();

        void process(PageInfo const& page_info,
                     AbstractFilterDataCollector* collector,
                     ImageTransformation const& xform,
                     QRectF const& content_rect);

    private:
        intrusive_ptr<output::CacheDrivenTask> m_ptrNextTask;
        intrusive_ptr<Settings> m_ptrSettings;
    };
}  // namespace page_layout
#endif // ifndef PAGE_LAYOUT_CACHEDRIVENTASK_H_
