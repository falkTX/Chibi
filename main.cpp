/*
 * Chibi - Carla's mini-host plugin loader
 * Copyright (C) 2020-2023 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "chibiwindow.h"
#include "CarlaFrontend.h"

#include <QApplication>

// TODO
#include "CarlaUtils.h"
uint carla_get_cached_plugin_count(PluginType, const char*) { return 0; }
const CarlaCachedPluginInfo* carla_get_cached_plugin_info(PluginType, uint) { return nullptr; }

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    const PluginListDialogResults* const res = carla_frontend_createAndExecPluginListDialog(nullptr);

    if (res == nullptr)
        return 1;

    ChibiWindow w(res);
    w.show();

    return app.exec();
}
