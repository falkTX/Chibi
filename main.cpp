/*
 * Chibi - Carla's mini-host plugin loader
 * Copyright (C) 2020 Filipe Coelho <falktx@falktx.com>
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

#include <QApplication>
#include <QInputDialog>

#include <cstdlib>

#include "CarlaUtils.h"

int main(int argc, char *argv[])
{
    BinaryType btype;
    PluginType ptype;
    QString filename;
    QString name;
    QString label;
    int64_t uniqueId;

    QApplication app(argc, argv);

    if (const char* const NSM_URL = std::getenv("NSM_URL"))
    {
        // if under NSM, wait for it to provide a path
        // TODO
        qFatal("Not implemented yet");
        return 1;
    }
    else
    {
        // Otherwise, check CLI args
        const QStringList args = app.arguments();

        if (args.size() > 1)
        {
            // we got some args, lets try them
            // TODO
            qWarning("CLI argument received, assumed to be VST2 path");

            btype = CarlaBackend::BINARY_NATIVE;
            ptype = CarlaBackend::PLUGIN_VST2;
            filename = args[1];
        }
        else
        {
            // we have nothing, give user something to pick from

            // TODO use carla full-blown selector instead of this
            QStringList items;
            QStringList uris;

            if (const uint count = carla_get_cached_plugin_count(CarlaBackend::PLUGIN_LV2, nullptr))
            {
                for (uint i=0; i<count; ++i)
                {
                    const CarlaCachedPluginInfo* pinfo = carla_get_cached_plugin_info(CarlaBackend::PLUGIN_LV2, i);

                    if (! pinfo->valid)
                        continue;

                    items.append(pinfo->name);
                    uris.append(pinfo->label);
                }
            }
            else
            {
                qWarning("No plugins available, bailing out");
                return 0;
            }

            const QString result = QInputDialog::getItem(nullptr, "Chibi - pick plugin", "Plugin", items);

            if (result.isEmpty())
            {
                qWarning("No plugin selected, bailing out");
                return 0;
            }

            btype = CarlaBackend::BINARY_NATIVE;
            ptype = CarlaBackend::PLUGIN_LV2;
            name = result;
            label = uris.at(items.indexOf(result)).section('/', 1);
            uniqueId = 0;
        }
    }

    ChibiWindow w(btype, ptype, filename, name, label, uniqueId);
    w.show();

    return app.exec();
}
