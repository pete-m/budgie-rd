/*
 * This file is part of budgie-rd
 *
 * Copyright © 2017-2018 Budgie Desktop Developers
 *
 * budgie-rd is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "root-window.h"

#include <KWindowSystem>
#include <QApplication>
#include <QDesktopWidget>

namespace Desktop
{
    RootWindow::RootWindow(QQmlEngine *engine, int screenNumber)
        : QQuickView(engine, nullptr), screenNumber(screenNumber)
    {
        // https://bugreports.qt.io/browse/QTBUG-54886
        //         setAttribute(Qt::WA_ShowWithoutActivating);
        setProperty("_q_showWithoutActivating", QVariant(true));
        setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint | Qt::Tool);
        resize(0, 0);
        // Parent size dictates QML size
        setResizeMode(QQuickView::SizeRootObjectToView);
        setSource(QUrl("qrc:/qml/backdrop.qml"));
    }

    void RootWindow::updateGeometry()
    {
        hide();
        auto desktop = QApplication::desktop();
        // Copy our geom from the target geom
        this->visibleArea = desktop->screenGeometry(this->screenNumber);
        setMaximumWidth(visibleArea.width());
        setMaximumHeight(visibleArea.width());
        setWidth(visibleArea.width());
        setHeight(visibleArea.height());
        setPosition(visibleArea.x(), visibleArea.y());
        show();

        // TODO: Track wID changes
        KWindowSystem::setType(winId(), NET::WindowType::Desktop);
    }
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=4 expandtab:
 * :indentSize=4:tabSize=4:noTabs=true:
 */
