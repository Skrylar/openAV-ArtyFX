
#ifndef OPENAV_AVTK_UI_HXX
#define OPENAV_AVTK_UI_HXX

// libs AVTK needs
#include "pugl/pugl.h"
#include <cairo/cairo.h>

// general C++ includes
#include <list>
#include <stack>
#include <vector>
#include <stdio.h>
#include <unistd.h>

// the AVTK UI is a group
#include "group.hxx"

// lv2 ui include
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

namespace Avtk
{

class Theme;
class Widget;
class Tester;


class UI : public Avtk::Group
{
public:
	UI( int w, int h, PuglNativeWindow parent = 0, const char* windowName = "Avtk" );
	virtual ~UI();

	/// tells the UI a widget has captured a mouse-down event, and
	/// wants to be notified of mouse movement events
	void wantsMotionUpdates( Avtk::Widget* w, bool notifyOfMotion )
	{
		if( notifyOfMotion )
			motionUpdateWidget = w;
		else
			motionUpdateWidget = 0;
	}

	/// overriden so we can set motion widget etc to null on removal.
	virtual void remove ( Widget* child );

	/// Initiate a drag-and-drop action, with the widget as the "origin widget",
	/// and the data is copied to the UI instance.
	void dragDropInit( Avtk::Widget* origin, size_t size, void* data );

	/// checks if the current drag-drop data-type is dropable on the "to" Widget
	bool dragDropVerify( Avtk::Widget* target );

	/// performs the drag-drop action
	void dragDropComplete( Avtk::Widget* target );

	/// Widget value callback: when a widget is added to the UI, its value
	/// callback is set to this function: it can be set to a custom function if
	/// preferred.
	///
	/// If the Widget* returned from new Widget() is stored, it can be compared
	/// against @param widget, which allows executing code based which Widget
	/// caused the event.
	virtual void widgetValueCB( Avtk::Widget* widget) = 0;

	/// this function can be overridden by a UI if it wants to function as an
	/// LV2 plugin UI. The "index" represents the control number, as defined
	/// in the plugin TTL file. If the port is a control port, casting the
	/// void* buffer to float* gives the value of that control port.
	virtual void lv2PortEvent(  uint32_t index,
	                            uint32_t buffer_size,
	                            uint32_t format,
	                            const void* buffer )
	{
		// stub implementation - not every UI will override this
	}

	// Used by LV2 UI - allows writing control/Atom messages from derived UI
	LV2UI_Write_Function write_function;
	LV2UI_Controller     controller;

	/// Static function for handling AVTK widget callbacks: is re-directed to
	/// instance-version above.
	static void staticWidgetValueCB( Avtk::Widget* widget, void* userdata)
	{
		UI* ui = (UI*)userdata;
		ui->widgetValueCB( widget );
	}

	/// draws the screen. Passing in will cause a partial redraw if possible
	/// on the current platform and rendering subsystem.
	void redraw();
	void redraw( Avtk::Widget* w );

	/// when used as a UI plugin, created by a host, this function should be
	/// called repeatedly at ~30 fps to handle events and redraw if needed.
	int idle();

	/// when UI is running standalone, call this function to run the UI. When
	/// the function returns, the main window has been closed.
	virtual int run();

	/// Handle Only make the UI only pass events to one widget, until it is
	/// called with 0x0, then normal functionality resumes.
	void handleOnly( Widget* wid );

	/// get the theme requested: the themes have ID's defined in theme file.
	/// calling this without a parameter returns the default theme.
	Theme* theme( int id = 0 );

	/// call this to recieve the LV2 widget handle
	PuglNativeWindow getNativeHandle()
	{
		// returns the X11 handle, or Win32 surface, or Quartz surface
		return puglGetNativeWindow( view );
	}

	/// sets the passed in Group* as the "parent" for any widgets created
	/// should only be called by Group widgets
	void pushParent( Avtk::Group* );
	void popParent();
	Avtk::Group* parentStackTop()
	{
		return parentStack.top();
	}

	int w()
	{
		return w_;
	}
	int h()
	{
		return h_;
	}

protected:
	PuglView* view;

	std::stack<Avtk::Group*> parentStack;

	bool quit_;
	int w_, h_;

#ifdef AVTK_TESTER
	/// for testing the UI, the Tester class can record and playback events.
	Tester* tester;
#endif
	/// The widget that should be handled modally: no other widgets are given
	/// the event
	Avtk::Widget* handleOnlyWidget;

	/// the list of widgets currently instantiated, in order of being drawn.
	std::list<Avtk::Widget*> widgets;

	/// A list of themes, loaded on startup, which widgets can request
	std::vector< Avtk::Theme* > themes;

	/// pointers that are dynamically switched to represent a widget that could
	/// have a specific action performed with it in the future. These pointers
	/// *must* be checked for 0 before *any* use.
	Avtk::Widget* dragDropOrigin;
	Avtk::Widget* motionUpdateWidget;

	bool          dragDropTargetVerified;
	Avtk::Widget* dragDropTargetVerifiedWidget;

	size_t dragDropDataSize;
	char*  dragDropDataPtr;

	void scroll( int x, int y, int dx, int dy );
	void display( cairo_t* cr );
	void motion(int x, int y);
	void reshape(int x, int y);
	void close()
	{
		quit_ = true;
	}

#ifdef AVTK_TESTER
public: // make event() public when TESTER is on to allow injecting events
#endif
	/// the main event function: it handles all event input, and distritbutes it
	/// to the widgets. See handleOnly() for details on handling Dialogs
	void event( const PuglEvent* event );
#ifdef AVTK_TESTER
protected:
#endif

	/// internalEvent handles events like ALT-F4, and ESC. Split from the main
	/// event function so dialog boxes etc can easily support handling these
	void internalEvent( const PuglEvent* event );

	// Static Functions for handling PUGL events below
	static void onMotion(PuglView* view, int x, int y)
	{
		UI* ui = (UI*)puglGetHandle( view );
		ui->motion( x, y );
	}
	static void onClose(PuglView* view)
	{
		UI* ui = (UI*)puglGetHandle( view );
		ui->close();
	}
	static void onEvent(PuglView* view, const PuglEvent* event)
	{
		UI* ui = (UI*)puglGetHandle( view );
		ui->event( event );
	}
	static void onDisplay(PuglView* view)
	{
		UI* ui = (UI*)puglGetHandle( view );
		cairo_t* cr = (cairo_t*)puglGetContext(view);
		ui->display( cr );
	}
	static void onScroll(PuglView* view, int x, int y, float dx, float dy)
	{
		UI* ui = (UI*)puglGetHandle( view );
		ui->scroll( x, y, dx, dy );
	}
	static void onReshape(PuglView* view, int w, int h)
	{
		UI* ui = (UI*)puglGetHandle( view );
		ui->reshape( w, h );
	}
};

}; // namespace Avtk

#endif // OPENAV_AVTK_UI_HXX
