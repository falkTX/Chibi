#include "chibiembedwidget.h"

#include <QResizeEvent>

#ifdef HAVE_X11
# include <QX11Info>
# include <X11/Xlib.h>
# include <X11/Xutil.h>
#endif

struct ChibiEmbedWidget::PrivateData
{
    bool wasResized = false;
   #ifdef HAVE_X11
    ::Window window = 0;
   #endif
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
#ifdef HAVE_X11
    return QX11Info::isPlatformX11();
#else
    return false;
#endif
}

bool ChibiEmbedWidget::wasResized() const
{
    return pData->wasResized;
}

void ChibiEmbedWidget::setup(void* const ptr)
{
#ifdef HAVE_X11
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
#endif
}

void ChibiEmbedWidget::resizeView(int width, int height)
{
#ifdef HAVE_X11
    if (pData->window)
    {
        XResizeWindow(QX11Info::display(), pData->window, width, height);
    }
#endif
}

QSize ChibiEmbedWidget::sizeHint() const
{
    printf("sizeHint\n");
#ifdef HAVE_X11
    if (pData->window)
    {
        XWindowAttributes attrs{};
        // TODO check return value
        XGetWindowAttributes(QX11Info::display(), pData->window, &attrs);
        return {attrs.width, attrs.height};
    }
#endif

    return {};

}
QSize ChibiEmbedWidget::minimumSizeHint() const
{
    printf("minimumSizeHint\n");
#ifdef HAVE_X11
    if (pData->window)
    {
        XSizeHints hints{};
        long       supplied{};

        // TODO check return value
        XGetWMNormalHints(QX11Info::display(), pData->window, &hints, &supplied);

        if (hints.flags & PMinSize)
            return {hints.min_width, hints.min_height};
    }
#endif

    return {};
}

void ChibiEmbedWidget::resizeEvent(QResizeEvent* const event)
{
    QWidget::resizeEvent(event);

    printf("ChibiEmbedWidget::resizeEvent %i %i\n",
           event->size().width(),
           event->size().height());

#ifdef HAVE_X11
    if (pData->window)
        resizeView(event->size().width(), event->size().height());
#endif
}
