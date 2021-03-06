/* Copyright (C) 2006 - 2011 Thomas Gahr <thomas.gahr@physik.uni-muenchen.de>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <QApplication>
#include <QCursor> // for Util::centerWidgetOnScreen
#include <QDesktopWidget> // for Util::centerWidgetOnScreen

#include "Util.h"

namespace Gui
{

namespace Util
{

void centerWidgetOnScreen(QWidget *widget, bool centerOnCursorScreen)
{
    widget->adjustSize();
    // Regarding the option to center widget on screen containing mousepointer (only relevant for dual-screen-setups):
    // If some day we'll have (configurable) key shortcuts, there might be situations when the mousepointer (and
    // therefore most likely the user's attention) is not on the screen containg widget's parentWidget.   So by
    // centerning the widget on the screen containing the mousepointer we assure to be as close to the user's focus as
    // possible.  For single screen setups this doesn't make any difference at all.  If the widget to be centered is
    // shown as a result of a mouseClick this makes no difference, too, since the mouseClick most probably happened on
    // the widget's parentWidget so the widget will be centered on the screen containing it.  Still for the sake of
    // completeness the option to pass false is kept open for any case where it might be needed.
    if (centerOnCursorScreen) {
        widget->move(QApplication::desktop()->screenGeometry(QCursor::pos()).center()
                     - widget->rect().center());
    } else {
        widget->move(QApplication::desktop()->screenGeometry(widget->parentWidget()).center()
                     - widget->rect().center());
    }
}

/** @short Path to the "package data directory"

This path shall contain various files (like the localization data).  In case we're running without being installed
(or on some funny^Hnon-X11 platform), this function returns an empty QString.  Please also note that the returned
value might contain data for a completely different version of Trojita.
*/
QString pkgDataDir()
{
#ifdef PKGDATADIR
    return QLatin1String(PKGDATADIR);
#else
    return QString();
#endif
}

} // namespace Util

} // namespace Gui


