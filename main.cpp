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

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

CARLA_BACKEND_USE_NAMESPACE;

int main(int argc, char *argv[])
{
   #ifdef CARLA_OS_MAC
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
   #endif
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_X11InitThreads);

   #ifdef CARLA_OS_WIN
    QApplication::addLibraryPath(QString::fromUtf8(carla_get_library_folder()));
   #endif

    QApplication app(argc, argv);
    app.setApplicationName("Chibi");
    app.setApplicationVersion(CARLA_VERSION_STRING);
    app.setOrganizationName("falkTX");

    const CarlaHostHandle handle = carla_standalone_host_init();

    if (handle == nullptr || !carla_engine_init(handle, "JACK", "Chibi"))
    {
        QMessageBox::critical(nullptr, "Error", carla_get_last_error(handle));
        return 1;
    }

    carla_set_engine_option(handle, ENGINE_OPTION_OSC_ENABLED, 0, nullptr);
    carla_set_engine_option(handle, ENGINE_OPTION_PATH_BINARIES, 0, carla_get_library_folder());
    carla_set_engine_option(handle, ENGINE_OPTION_PREFER_UI_BRIDGES, 0, nullptr);

    // TODO check CLI args and use it instead of plugin list dialog

    const PluginListDialogResults* const res = carla_frontend_createAndExecPluginListDialog(nullptr);

    int r = 1;

    if (res == nullptr)
        goto cleanup;

    if (! carla_add_plugin(handle,
                           static_cast<BinaryType>(res->build),
                           static_cast<PluginType>(res->type),
                           res->filename, res->name, res->label,
                           res->uniqueId, nullptr, PLUGIN_OPTIONS_NULL))
    {
        QMessageBox::critical(nullptr, "Error", carla_get_last_error(handle));
        goto cleanup;
    }

    {
        ChibiWindow w(handle, res->name);
        w.show();
        r = app.exec();
    }

cleanup:
    if (carla_is_engine_running(handle))
        carla_engine_close(handle);

    return r;
}
