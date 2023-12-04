
/*******************************************************************************
 quitQuestionDialog.cc

       Associated Header file: quitQuestionDialog.h
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

#include "quitQuestionDialog.h"


/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

/*******************************************************************************
       The following is the destroyContext callback function.
       It is needed to free the memory allocated by the context.
*******************************************************************************/

static void DelayedDelete( XtPointer obj, XtIntervalId *)
{
 delete ((_UxCquitQuestionDialog *) obj);
}
void  _UxCquitQuestionDialog::UxDestroyContextCB(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;

 ((_UxCquitQuestionDialog *) UxClientData)->UxThis = NULL;
 XtAppAddTimeOut(UxAppContext, 0, DelayedDelete, UxClientData);
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

Widget _UxCquitQuestionDialog::_build()
{
 Widget  _UxParent;


 // Creation of quitQuestionDialog
 _UxParent = UxParent;
 if ( _UxParent == NULL )
 {
  _UxParent = UxTopLevel;
 }

 _UxParent = XtVaCreatePopupShell( "quitQuestionDialog_shell",
   xmDialogShellWidgetClass, _UxParent,
   XmNx, 397,
   XmNy, 242,
   XmNwidth, 303,
   XmNheight, 115,
   XmNshellUnitType, XmPIXELS,
   XmNtitle, "quitQuestionDialog",
   NULL );

 quitQuestionDialog = XtVaCreateWidget( "quitQuestionDialog",
   xmMessageBoxWidgetClass,
   _UxParent,
   XmNdialogType, XmDIALOG_QUESTION,
   XmNunitType, XmPIXELS,
   XmNwidth, 303,
   XmNheight, 115,
   RES_CONVERT( XmNcancelLabelString, "No" ),
   RES_CONVERT( XmNdialogTitle, "Quit?" ),
   RES_CONVERT( XmNokLabelString, "Yes" ),
   RES_CONVERT( XmNmessageString, "This federate is stilled joined to a\nFederation.  Do you still want to Quit?" ),
   NULL );
 UxPutContext( quitQuestionDialog, (char *) this );
 UxThis = quitQuestionDialog;



 XtAddCallback( quitQuestionDialog, XmNdestroyCallback,
  (XtCallbackProc) &_UxCquitQuestionDialog::UxDestroyContextCB,
  (XtPointer) this);


 return ( quitQuestionDialog );
}

/*******************************************************************************
       The following function includes the code that was entered
       in the 'Initial Code' and 'Final Code' sections of the
       Declarations Editor. This function is called from the
       'Interface function' below.
*******************************************************************************/

swidget _UxCquitQuestionDialog::_create_quitQuestionDialog(void)
{
 Widget                  rtrn;
 UxThis = rtrn = _build();

 // Final Code from declarations editor
 return(rtrn);
}

/*******************************************************************************
       The following is the destructor function.
*******************************************************************************/

_UxCquitQuestionDialog::~_UxCquitQuestionDialog()
{
 if (this->UxThis)
 {
  XtRemoveCallback( UxGetWidget(this->UxThis),
   XmNdestroyCallback,
   (XtCallbackProc) &_UxCquitQuestionDialog::UxDestroyContextCB,
   (XtPointer) this);
  UxDestroyInterface(this->UxThis);
 }
}

/*******************************************************************************
       The following is the constructor function.
*******************************************************************************/

_UxCquitQuestionDialog::_UxCquitQuestionDialog( swidget UxParent )
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

swidget create_quitQuestionDialog( swidget UxParent )
{
 _UxCquitQuestionDialog *theInterface = 
   new _UxCquitQuestionDialog( UxParent );
 return (theInterface->_create_quitQuestionDialog());
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

