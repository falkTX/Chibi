#include "chibiembedwidget.h"

#include <QResizeEvent>

#if defined(Q_OS_MAC)
# import <Cocoa/Cocoa.h>
#elif defined(Q_OS_WINDOWS)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#else
# include <QX11Info>
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <pthread.h>
#endif

#ifdef HAVE_X11
static pthread_mutex_t gErrorMutex = PTHREAD_MUTEX_INITIALIZER;
static bool gErrorTriggered = false;
static int gErrorHandler(Display*, XErrorEvent*)
{
    gErrorTriggered = true;
    return 0;
}
#endif

struct ChibiEmbedWidget::PrivateData
{
    QWidget* const widget;
//     void* const ourWindowPtr;
    const uintptr_t ourWindowId;

   #if defined(Q_OS_MAC)
    NSView* const view;
    NSView* subview;
   #elif defined(Q_OS_WINDOWS)
    HWND pluginWindow;
   #else
    Display* const display;
    Window pluginWindow;
   #endif

    Callback* callback;
    uint xOffset, yOffset;
    bool lookingForChildren;

    PrivateData(QWidget* const w)
        : widget(w),
          ourWindowId(w->winId()),
         #if defined(Q_OS_MAC)
          view([[NSView new]retain]),
          subview(nullptr),
         #elif defined(Q_OS_WINDOWS)
          pluginWindow(nullptr),
         #else
          display(QX11Info::display()),
          pluginWindow(0),
         #endif
          callback(nullptr),
          xOffset(0),
          yOffset(0),
          lookingForChildren(false)
    {
       #ifdef Q_OS_MAC
        [view setAutoresizingMask:NSViewNotSizable];
        [view setAutoresizesSubviews:NO];
        [view setHidden:YES];
        [(NSView*)ourWindowPtr addSubview:view];
       #endif
    }

    ~PrivateData()
    {
       #ifdef Q_OS_MAC
        [view release];
       #endif
    }

    void* prepare(Callback* const cb)
    {
        callback = cb;
        lookingForChildren = true;
       #if defined(Q_OS_MAC)
        subview = nullptr;
        return view;
       #elif defined(Q_OS_WINDOWS)
        pluginWindow = nullptr;
        return ourWindowPtr;
       #else
        pluginWindow = 0;
        return (void*)ourWindowId;
       #endif
    }

    bool hide()
    {
       #if defined(Q_OS_MAC)
        [view setHidden:YES];
        [NSOpenGLContext clearCurrentContext];
        return true;
       #elif defined(Q_OS_WINDOWS)
        if (pluginWindow != nullptr)
        {
            ShowWindow(pluginWindow, SW_HIDE);
            pluginWindow = nullptr;
            return true;
        }
       #else
        if (pluginWindow != 0)
        {
            XUnmapWindow(display, pluginWindow);
            XSync(display, True);
            pluginWindow = 0;
            return true;
        }
       #endif
        return false;
    }

    void idle()
    {
        if (lookingForChildren)
        {
           #if defined(Q_OS_MAC)
            if (subview == nullptr)
            {
                for (NSView* subview2 in [view subviews])
                {
                    subview = subview2;
                    break;
                }
            }
           #elif defined(Q_OS_WINDOWS)
            if (pluginWindow == nullptr)
                pluginWindow = FindWindowExA((::HWND)ourWindowPtr, nullptr, nullptr, nullptr);
           #else
            if (pluginWindow == 0)
            {
                ::Window rootWindow, parentWindow;
                ::Window* childWindows = nullptr;
                uint numChildren = 0;

                XQueryTree(display, ourWindowId, &rootWindow, &parentWindow, &childWindows, &numChildren);

                if (numChildren > 0 && childWindows != nullptr)
                {
                    // pick last child, needed for NTK based UIs which do not delete/remove previous windows.
                    //sadly this breaks ildaeil-within-ildaeil recursion.. :(
                    pluginWindow = childWindows[numChildren - 1];
                    XFree(childWindows);
                }
            }
           #endif
        }

       #if defined(Q_OS_MAC)
        if (subview != nullptr)
        {
            const double scaleFactor = [[[view window] screen] backingScaleFactor];
            const NSSize size = [subview frame].size;
            const double width = size.width;
            const double height = size.height;

//             if (lookingForChildren)
//                 carla_stdout("child window bounds %f %f | offset %u %u", width, height, xOffset, yOffset);

            if (width > 1.0 && height > 1.0)
            {
                lookingForChildren = false;
                [view setFrameSize:size];
                [view setHidden:NO];
                [view setNeedsDisplay:YES];
                callback->pluginWindowResized(width * scaleFactor, height * scaleFactor);
            }
        }
       #elif defined(Q_OS_WINDOWS)
        if (pluginWindow != nullptr)
        {
            int width = 0;
            int height = 0;

            RECT rect;
            if (GetWindowRect(pluginWindow, &rect))
            {
                width = rect.right - rect.left;
                height = rect.bottom - rect.top;
            }

            if (lookingForChildren)
                d_stdout("child window bounds %i %i | offset %u %u", width, height, xOffset, yOffset);

            if (width > 1 && height > 1)
            {
                lookingForChildren = false;
                SetWindowPos(pluginWindow, 0, xOffset, yOffset, 0, 0,
                             SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER);
                callback->pluginWindowResized(width, height);
            }
        }
       #else
        if (pluginWindow != 0)
        {
            int width = 0;
            int height = 0;

            XWindowAttributes attrs;
            memset(&attrs, 0, sizeof(attrs));

            pthread_mutex_lock(&gErrorMutex);
            const XErrorHandler oldErrorHandler = XSetErrorHandler(gErrorHandler);
            gErrorTriggered = false;

            if (XGetWindowAttributes(display, pluginWindow, &attrs) && ! gErrorTriggered)
            {
                width = attrs.width;
                height = attrs.height;
            }

            XSetErrorHandler(oldErrorHandler);
            pthread_mutex_unlock(&gErrorMutex);

            if (width == 0 && height == 0)
            {
                XSizeHints sizeHints;
                memset(&sizeHints, 0, sizeof(sizeHints));

                if (XGetNormalHints(display, pluginWindow, &sizeHints))
                {
                    if (sizeHints.flags & PSize)
                    {
                        width = sizeHints.width;
                        height = sizeHints.height;
                    }
                    else if (sizeHints.flags & PBaseSize)
                    {
                        width = sizeHints.base_width;
                        height = sizeHints.base_height;
                    }
                }
            }

//             if (lookingForChildren)
//                 carla_stdout("child window bounds %i %i | offset %u %u", width, height, xOffset, yOffset);

            if (width > 1 && height > 1)
            {
                lookingForChildren = false;
                XMoveWindow(display, pluginWindow, xOffset, yOffset);
                XSync(display, True);
                widget->setFixedSize(width, height);
                callback->pluginWindowResized(width, height);
            }
        }
       #endif
    }

    void setPositionAndSize(const uint x, const uint y, const uint width, const uint height)
    {
       #if defined(Q_OS_MAC)
        const double scaleFactor = [[[view window] screen] backingScaleFactor];
        [view setFrame:NSMakeRect(x / scaleFactor, y / scaleFactor, width / scaleFactor, height / scaleFactor)];
       #else
        // unused
        (void)width;
        (void)height;
       #endif

        xOffset = x;
        yOffset = y;
    }
};

static QWidget* widgetSetup(QWidget* const w)
{
    w->setAttribute(Qt::WA_OpaquePaintEvent);
    return w;
}

ChibiEmbedWidget::ChibiEmbedWidget(QWidget*)
    : QWidget(nullptr, Qt::Window)
    , pData(new PrivateData(widgetSetup(this)))
{
}

ChibiEmbedWidget::~ChibiEmbedWidget()
{
    delete pData;
}

void* ChibiEmbedWidget::prepare(Callback* const callback)
{
    return pData->prepare(callback);
}

void ChibiEmbedWidget::idle()
{
    return pData->idle();
}

void ChibiEmbedWidget::setup(void* const ptr)
{
    idle();

// #ifdef HAVE_X11
//     if (const ::Window window = reinterpret_cast<::Window>(ptr))
//     {
//         ::Display* const display = QX11Info::display();
//
//         ::XWindowAttributes attrs{};
//         ::XSizeHints        hints{};
//         long              supplied{};
//
//         XSync(display, False);
//         XGetWindowAttributes(display, window, &attrs);
//         XGetWMNormalHints(display, window, &hints, &supplied);
//
//         if (hints.flags & PBaseSize) {
//             setBaseSize(hints.base_width, hints.base_height);
//         }
//
//         if (hints.flags & PMinSize) {
//             setMinimumSize(hints.min_width, hints.min_height);
//         }
//
//         if (hints.flags & PMaxSize) {
//             setMaximumSize(hints.max_width, hints.max_height);
//         }
//
//         if ((hints.flags & PSize)) {
//             // pData->wasResized = true;
//             resize(hints.width, hints.height);
//         }
//
//         pData->window = window;
//     }
// #endif
}

void ChibiEmbedWidget::resizeView(int width, int height)
{
// #ifdef HAVE_X11
//     if (pData->window)
//     {
//         XResizeWindow(QX11Info::display(), pData->window, width, height);
//     }
// #endif
}

QSize ChibiEmbedWidget::sizeHint() const
{
    printf("sizeHint\n");
// #ifdef HAVE_X11
//     if (pData->pluginWindow != 0)
//     {
//         XWindowAttributes attrs{};
//         // TODO check return value
//         XGetWindowAttributes(QX11Info::display(), pData->pluginWindow, &attrs);
//         return {attrs.width, attrs.height};
//     }
// #endif

    return size();

}
QSize ChibiEmbedWidget::minimumSizeHint() const
{
    printf("minimumSizeHint\n");
// #ifdef HAVE_X11
//     if (pData->pluginWindow != 0)
//     {
//         XSizeHints hints{};
//         long       supplied{};
//
//         // TODO check return value
//         XGetWMNormalHints(QX11Info::display(), pData->pluginWindow, &hints, &supplied);
//
//         if (hints.flags & PMinSize)
//             return {hints.min_width, hints.min_height};
//     }
// #endif

    return sizeHint();
}

void ChibiEmbedWidget::resizeEvent(QResizeEvent* const event)
{
    QWidget::resizeEvent(event);

    printf("ChibiEmbedWidget::resizeEvent %i %i\n",
           event->size().width(),
           event->size().height());

// #ifdef HAVE_X11
//     if (pData->window)
//         resizeView(event->size().width(), event->size().height());
// #endif
}
