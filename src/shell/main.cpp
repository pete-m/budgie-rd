/*
 * This file is part of budgie-rd
 *
 * Copyright © 2017 Budgie Desktop Developers
 *
 * budgie-rd is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QScopedPointer>

#include "config.h"
#include "shell.h"

/**
 * Will expand in future, but we get this object from startup()
 * which allows us to control the rest of our session startup,
 * and control specific details before we attempt to initialise
 * a display context.
 */
struct ShellStartupInfo {
    QString sessionName;
};

/**
 * Handle our basic CLI parsing with a throwaway QCoreApplication
 *
 * This ensures we don't yet use any display-context sensitive event
 * loops. Unfortunately QCommandLineParser will still try to use
 * the QCoreApplication::arguments() even if you process manually,
 * hence this entry.
 */
static ShellStartupInfo *startup(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("budgie-shell");
    QCoreApplication::setApplicationVersion(PACKAGE_VERSION);

    // Set up parser
    QCommandLineParser p;
    p.addHelpOption();
    p.addVersionOption();
    p.setApplicationDescription("Budgie Desktop R&D Shell");

    // Allow setting the session type here
    QCommandLineOption optSession({ "s", "session" },
                                  QCoreApplication::translate("main", "Set the session to launch"),
                                  "session-name",
                                  "budgie-desktop");
    p.addOption(optSession);

    p.process(app);

    // Return what we know so far.
    auto ret = new ShellStartupInfo();
    ret->sessionName = p.value(optSession);
    return ret;
}

/**
 * Handle the GUI specific startup routine now
 */
static QApplication *createApplication(int argc, char **argv)
{
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    QApplication *ret = nullptr;

    // TODO: Maybe do something useful here
    ret = new QApplication(argc, argv);

    return ret;
}

/**
 * Handle any session-specific desktop overrides we need to put in place.
 * Currently this is setting the xdg desktop name/session name for this
 * process and all descendants.
 */
static void initEnvironment()
{
    QString xdgDesktopName = "Budgie";
    auto xdgLocal = xdgDesktopName.toLocal8Bit();

    // Ensure desktop session is correct
    qputenv("DESKTOP_SESSION", xdgLocal);

    // Must ensure XDG_DESKTOP_NAME is correct, display managers may end up
    // changing this which we very much do not want.
    qputenv("XDG_DESKTOP_NAME", xdgLocal);
}

/**
 * Main entry: Process args, begin main
 */
int main(int argc, char **argv)
{
    // Before any Qt usage
    initEnvironment();

    QScopedPointer<QApplication> gui;
    QScopedPointer<ShellStartupInfo> info;
    QScopedPointer<Budgie::Shell> shell;

    // Basic arg handling first
    info.reset(startup(argc, argv));

    shell.reset(new Budgie::Shell(info->sessionName));

    qInfo() << "Starting shell session: " << shell->name();

    // Ideally we know what to do by this point, do GUI cruft.
    gui.reset(createApplication(argc, argv));
    return gui->exec();
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