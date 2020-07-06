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

#ifndef CHIBIEMBEDWIDGET_H
#define CHIBIEMBEDWIDGET_H

#include <QWidget>

class ChibiEmbedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChibiEmbedWidget(QWidget* parent);
    ~ChibiEmbedWidget() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    bool canEmbed() const;
    bool wasResized() const;

    void setup(void* ptr);
    void resizeView(int width, int height);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    struct PrivateData;
    PrivateData* const pData;
};

#endif // CHIBIEMBEDWIDGET_H
