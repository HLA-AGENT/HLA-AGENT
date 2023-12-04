
/*******************************************************************************
 fileSelectionShell.cc

       Associated Header file: fileSelectionShell.h
*******************************************************************************/

#include <stdio.h>

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/MenuShell.h>
#endif /* MOTIF */

#include "UxXt.h"
#include <Xm/FileSB.h>
#include <X11/Shell.h>

/*******************************************************************************
       Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/


extern swidget fileSelectionSwidget;


#ifndef XKLOADDS
#define XKLOADDS
#endif /* XKLOADDS */

#include "fileSelectionShell.h"


/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

void  _UxCfileSelectionShell::cancelCB_fileSelectionBoxDialog(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 UxPopdownInterface (fileSelectionSwidget);
 }
}

void  _UxCfileSelectionShell::Wrap_cancelCB_fileSelectionBoxDialog(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCfileSelectionShell  *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCfileSelectionShell *) _UxCfileSelectionShell::UxGetContext(UxWidget);
 UxContext->cancelCB_fileSelectionBoxDialog(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCfileSelectionShell::okCallback_fileSelectionBoxDialog(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 char *filename;
 XmFileSelectionBoxCallbackStruct *cbs =
   (XmFileSelectionBoxCallbackStruct *) UxCallbackArg;
 
 UxPopdownInterface (fileSelectionSwidget);
 
 if (!XmStringGetLtoR (cbs->value, XmFONTLIST_DEFAULT_TAG,
                       &filename))
   return; /* must have been an internal error */
 
 if (!*filename) /* nothing typed */
 {
   printf ("No file selected.\n");
   XtFree (filename);  /* even "" is an allocated byte */
   return;
 }
 
 XtVaSetValues (UxGetWidget (outputFileField),
                XmNvalue, filename,
                NULL);
 
 XtFree (filename);
 
 
 }
}

void  _UxCfileSelectionShell::Wrap_okCallback_fileSelectionBoxDialog(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCfileSelectionShell  *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCfileSelectionShell *) _UxCfileSelectionShell::UxGetContext(UxWidget);
 UxContext->okCallback_fileSelectionBoxDialog(UxWidget, UxClientData, UxCallbackArg);
}

/*******************************************************************************
       The following is the destroyContext callback function.
       It is needed to free the memory allocated by the context.
*******************************************************************************/

static void DelayedDelete( XtPointer obj, XtIntervalId *)
{
 delete ((_UxCfileSelectionShell *) obj);
}
void  _UxCfileSelectionShell::UxDestroyContextCB(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;

 ((_UxCfileSelectionShell *) UxClientData)->UxThis = NULL;
 XtAppAddTimeOut(UxAppContext, 0, DelayedDelete, UxClientData);
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

Widget _UxCfileSelectionShell::_build()
{
 Widget  _UxParent;


 // Creation of fileSelectionShell
 _UxParent = UxParent;
 if ( _UxParent == NULL )
 {
  _UxParent = UxTopLevel;
 }

 fileSelectionShell = XtVaCreatePopupShell( "fileSelectionShell",
   applicationShellWidgetClass,
   _UxParent,
   XmNx, 391,
   XmNy, 189,
   XmNwidth, 339,
   XmNheight, 327,
   XmNtitle, "Select Output File",
   NULL );
 UxPutContext( fileSelectionShell, (char *) this );
 UxThis = fileSelectionShell;



 // Creation of fileSelectionBoxDialog
 fileSelectionBoxDialog = XtVaCreateManagedWidget( "fileSelectionBoxDialog",
   xmFileSelectionBoxWidgetClass,
   fileSelectionShell,
   XmNresizePolicy, XmRESIZE_NONE,
   XmNunitType, XmPIXELS,
   XmNx, 82,
   XmNy, 45,
   XmNwidth, 339,
   XmNheight, 327,
   RES_CONVERT( XmNpattern, "*.log" ),
   NULL );
 XtAddCallback( fileSelectionBoxDialog, XmNcancelCallback,
  (XtCallbackProc) &_UxCfileSelectionShell::Wrap_cancelCB_fileSelectionBoxDialog,
  (XtPointer) NULL );
 XtAddCallback( fileSelectionBoxDialog, XmNokCallback,
  (XtCallbackProc) &_UxCfileSelectionShell::Wrap_okCallback_fileSelectionBoxDialog,
  (XtPointer) NULL );

 UxPutContext( fileSelectionBoxDialog, (char *) this );


 XtAddCallback( fileSelectionShell, XmNdestroyCallback,
  (XtCallbackProc) &_UxCfileSelectionShell::UxDestroyContextCB,
  (XtPointer) this);


 return ( fileSelectionShell );
}

/*******************************************************************************
       The following function includes the code that was entered
       in the 'Initial Code' and 'Final Code' sections of the
       Declarations Editor. This function is called from the
       'Interface function' below.
*******************************************************************************/

swidget _UxCfileSelectionShell::_create_fileSelectionShell(void)
{
 Widget                  rtrn;
 UxThis = rtrn = _build();

 // Final Code from declarations editor
 return(rtrn);
}

/*******************************************************************************
       The following is the destructor function.
*******************************************************************************/

_UxCfileSelectionShell::~_UxCfileSelectionShell()
{
 if (this->UxThis)
 {
  XtRemoveCallback( UxGetWidget(this->UxThis),
   XmNdestroyCallback,
   (XtCallbackProc) &_UxCfileSelectionShell::UxDestroyContextCB,
   (XtPointer) this);
  UxDestroyInterface(this->UxThis);
 }
}

/*******************************************************************************
       The following is the constructor function.
*******************************************************************************/

_UxCfileSelectionShell::_UxCfileSelectionShell( swidget UxParent, swidget outputFileField )
{
 this->UxParent = UxParent;
 this->outputFileField = outputFileField;

 // User Supplied Constructor Code
}

/*******************************************************************************
       The following is the 'Interface function' which is the
       external entry point for creating this interface.
       This function should be called from your application or from
       a callback function.
*******************************************************************************/

swidget create_fileSelectionShell( swidget UxParent, swidget outputFileField )
{
 _UxCfileSelectionShell *theInterface = 
   new _UxCfileSelectionShell( UxParent, outputFileField );
 return (theInterface->_create_fileSelectionShell());
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

