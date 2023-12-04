
/*******************************************************************************
 endTestModeDialog.cc

       Associated Header file: endTestModeDialog.h
*******************************************************************************/

#include <stdio.h>

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#endif /* MOTIF */

#include "UxXt.h"
#include <Xm/MessageB.h>



#ifndef XKLOADDS
#define XKLOADDS
#endif /* XKLOADDS */

#include "endTestModeDialog.h"


/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

/*******************************************************************************
       The following is the destroyContext callback function.
       It is needed to free the memory allocated by the context.
*******************************************************************************/

static void DelayedDelete( XtPointer obj, XtIntervalId *)
{
 delete ((_UxCendTestModeDialog *) obj);
}
void  _UxCendTestModeDialog::UxDestroyContextCB(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;

 ((_UxCendTestModeDialog *) UxClientData)->UxThis = NULL;
 XtAppAddTimeOut(UxAppContext, 0, DelayedDelete, UxClientData);
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

Widget _UxCendTestModeDialog::_build()
{
 Widget  _UxParent;


 // Creation of endTestModeDialog
 _UxParent = UxParent;
 if ( _UxParent == NULL )
 {
  _UxParent = UxTopLevel;
 }

 _UxParent = XtVaCreatePopupShell( "endTestModeDialog_shell",
   xmDialogShellWidgetClass, _UxParent,
   XmNx, 404,
   XmNy, 235,
   XmNwidth, 303,
   XmNheight, 115,
   XmNshellUnitType, XmPIXELS,
   XmNtitle, "endTestModeDialog",
   NULL );

 endTestModeDialog = XtVaCreateWidget( "endTestModeDialog",
   xmMessageBoxWidgetClass,
   _UxParent,
   XmNdialogType, XmDIALOG_QUESTION,
   XmNunitType, XmPIXELS,
   XmNwidth, 303,
   XmNheight, 115,
   RES_CONVERT( XmNcancelLabelString, "No" ),
   RES_CONVERT( XmNdialogTitle, "End Test Mode?" ),
   RES_CONVERT( XmNokLabelString, "Yes" ),
   RES_CONVERT( XmNmessageString, "End Test Mode?" ),
   NULL );
 UxPutContext( endTestModeDialog, (char *) this );
 UxThis = endTestModeDialog;



 XtAddCallback( endTestModeDialog, XmNdestroyCallback,
  (XtCallbackProc) &_UxCendTestModeDialog::UxDestroyContextCB,
  (XtPointer) this);


 return ( endTestModeDialog );
}

/*******************************************************************************
       The following function includes the code that was entered
       in the 'Initial Code' and 'Final Code' sections of the
       Declarations Editor. This function is called from the
       'Interface function' below.
*******************************************************************************/

swidget _UxCendTestModeDialog::_create_endTestModeDialog(void)
{
 Widget                  rtrn;
 UxThis = rtrn = _build();

 // Final Code from declarations editor
 return(rtrn);
}

/*******************************************************************************
       The following is the destructor function.
*******************************************************************************/

_UxCendTestModeDialog::~_UxCendTestModeDialog()
{
 if (this->UxThis)
 {
  XtRemoveCallback( UxGetWidget(this->UxThis),
   XmNdestroyCallback,
   (XtCallbackProc) &_UxCendTestModeDialog::UxDestroyContextCB,
   (XtPointer) this);
  UxDestroyInterface(this->UxThis);
 }
}

/*******************************************************************************
       The following is the constructor function.
*******************************************************************************/

_UxCendTestModeDialog::_UxCendTestModeDialog( swidget UxParent )
{
 this->UxParent = UxParent;

 // User Supplied Constructor Code
}

/*******************************************************************************
       The following is the 'Interface function' which is the
       external entry point for creating this interface.
       This function should be called from your application or from
       a callback function.
*******************************************************************************/

swidget create_endTestModeDialog( swidget UxParent )
{
 _UxCendTestModeDialog *theInterface = 
   new _UxCendTestModeDialog( UxParent );
 return (theInterface->_create_endTestModeDialog());
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

