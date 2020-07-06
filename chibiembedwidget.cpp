#include "chibiembedwidget.h"

#include <QResizeEvent>

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

struct ChibiEmbedWidget::PrivateData
{
    bool wasResized;
    ::Window window;

    PrivateData()
        : window(0) {}
};

ChibiEmbedWidget::ChibiEmbedWidget(QWidget* const parent)
    : QWidget(nullptr, Qt::Window)
    , pData(new PrivateData())
{
    setAttribute(Qt::WA_OpaquePaintEvent);
}

ChibiEmbedWidget::~ChibiEmbedWidget()
{
    delete pData;
}

bool ChibiEmbedWidget::canEmbed() const
{
    return QX11Info::isPlatformX11();
}

bool ChibiEmbedWidget::wasResized() const
{
    return pData->wasResized;
}

void ChibiEmbedWidget::setup(void* const ptr)
{
    if (const ::Window window = reinterpret_cast<::Window>(ptr))
    {
        ::Display* const display = QX11Info::display();

        ::XWindowAttributes attrs{};
        ::XSizeHints        hints{};
        long              supplied{};

        XSync(display, False);
        XGetWindowAttributes(display, window, &attrs);
        XGetWMNormalHints(display, window, &hints, &supplied);

        if (hints.flags & PBaseSize) {
            setBaseSize(hints.base_width, hints.base_height);
        }

        if (hints.flags & PMinSize) {
            setMinimumSize(hints.min_width, hints.min_height);
        }

        if (hints.flags & PMaxSize) {
            setMaximumSize(hints.max_width, hints.max_height);
        }

        if ((hints.flags & PSize)) {
            pData->wasResized = true;
            resize(hints.width, hints.height);
        }

        pData->window = window;
    }
}

void ChibiEmbedWidget::resizeView(int width, int height)
{
    XResizeWindow(QX11Info::display(), pData->window, width, height);
}

QSize ChibiEmbedWidget::sizeHint() const
{
    printf("sizeHint\n");
    if (pData->window)
    {
        XWindowAttributes attrs{};
        // TODO check return value
        XGetWindowAttributes(QX11Info::display(), pData->window, &attrs);
        return {attrs.width, attrs.height};
    }

    return {};

}
QSize ChibiEmbedWidget::minimumSizeHint() const
{
    printf("minimumSizeHint\n");
    if (pData->window)
    {
        XSizeHints hints{};
        long       supplied{};

        // TODO check return value
        XGetWMNormalHints(QX11Info::display(), pData->window, &hints, &supplied);

        if (hints.flags & PMinSize)
            return {hints.min_width, hints.min_height};
    }

    return {};
}

void ChibiEmbedWidget::resizeEvent(QResizeEvent* const event)
{
    QWidget::resizeEvent(event);

    printf("ChibiEmbedWidget::resizeEvent %p %i %i\n",
           pData->window,
           event->size().width(),
           event->size().height());

    if (pData->window)
        resizeView(event->size().width(), event->size().height());
}
