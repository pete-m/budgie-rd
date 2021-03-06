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

#pragma once

#include "base-interface.h"
#include "shell-interface.h"

#define BudgieServiceInterfaceIID "org.budgie-desktop.ServiceInterface"

namespace Budgie
{
    /**
     * Plugins implementing ServiceInterface have a simple lifecycle
     * and are simply start/stopped as appropriate.
     *
     * A ServiceInterface is used to implement vital support services
     * within the desktop environment, such as the window manager or
     * notifications.
     */
    class ServiceInterface : BaseInterface
    {
    public:
        virtual ~ServiceInterface()
        {
        }

        inline const QString interfaceName() override
        {
            return QStringLiteral(BudgieServiceInterfaceIID);
        }

        /**
         * Ask the service to initialise with the given ShellInterface
         */
        virtual bool init(ShellInterface *shell) = 0;

        /**
         * Ask the service interface to start itself
         */
        virtual bool start() = 0;

        /**
         * Ask the service interface to stop itself.
         */
        virtual bool stop() = 0;
    };
}

Q_DECLARE_INTERFACE(Budgie::ServiceInterface, BudgieServiceInterfaceIID)

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
