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

#include <QCoreApplication>

#include "compositor-renderer-interface.h"
#include "display.h"
#include "server.h"

using namespace Budgie::Compositor;

Server::Server(RendererInterface *renderer)
    : m_renderer(renderer), m_wl_shell(new QWaylandWlShell(this)),
      m_xdg_shell_v5(new QWaylandXdgShellV5(this)), m_seat(nullptr)
{
    // Start with zero focus
    m_keyFocus = nullptr;
    m_mouseFocus = nullptr;
    m_mouseLast = QPoint(0, 0);

    // Hook up basic compositor signals so we know whats going on when we ::create()
    connect(this, &QWaylandCompositor::surfaceCreated, this, &Server::surfaceCreated);
    connect(this, &QWaylandCompositor::surfaceAboutToBeDestroyed, this, &Server::surfaceDestroying);

    // Now hook up each protocol
    connect(m_wl_shell, &QWaylandWlShell::wlShellSurfaceCreated, this, &Server::wlShellCreated);
    connect(m_xdg_shell_v5,
            &QWaylandXdgShellV5::xdgSurfaceCreated,
            this,
            &Server::xdgShellv5Created);

    connect(this, &QWaylandCompositor::defaultSeatChanged, this, &Server::wlSeatChanged);
    connect(this, &QWaylandCompositor::createdChanged, this, &Server::wlCreated);
}

bool Server::start()
{
    // Ask that we get created.
    create();

    // This is where we just fake the hell out of it for now.
    auto output = new QWaylandOutput(this, nullptr);
    QWaylandOutputMode mode(QSize(1024, 768), 60000);
    output->setPosition(QPoint(0, 0));
    output->addMode(mode, true);

    // This is code we WOULD use, i.e. construct a display and store it.
    // We'll actually need to add sorting based on the index but in theory
    // we could swap to a map of index->display
    auto display = m_renderer->createDisplay(this, output);
    if (!display) {
        qWarning() << "Failed to construct a valid display for: " << output;
        return false;
    }

    auto window = display->window();
    if (window == nullptr) {
        qWarning() << "Broken Renderer is not returning the Window";
        return false;
    }

    // Force connection and mode change now
    output->setWindow(window);
    output->setCurrentMode(mode);

    // Store the display now
    m_displays << QSharedPointer<Display>(display);

    // Get it on screen
    window->show();
    return true;
}

/**
 * We return a shallow copy pointer reference to our known windows that should
 * appear on the given display to help the renderers know about rendering order.
 */
QList<Budgie::Compositor::Window *> Server::getRenderables(Compositor::Display *origin)
{
    QList<Window *> ret;

    // Traverse renderable layers. In future optimize this when we have full screen windows.
    for (int i = static_cast<int>(Compositor::MinLayer); i < static_cast<int>(Compositor::MaxLayer);
         i++) {
        RenderLayer eLayer = static_cast<RenderLayer>(i);

        for (const auto window : m_renderables[eLayer]) {
            // TODO: If the window is on the target display ..
            ret << window;
        }
    }

    return ret;
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
