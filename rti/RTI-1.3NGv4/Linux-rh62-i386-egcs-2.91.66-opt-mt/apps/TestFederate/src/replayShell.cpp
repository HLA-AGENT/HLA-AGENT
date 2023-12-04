
/*******************************************************************************
 replayShell.cc

       Associated Header file: replayShell.h
*******************************************************************************/

#include <stdio.h>

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/MenuShell.h>
#endif /* MOTIF */

#include "UxXt.h"
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <Xm/BulletinB.h>
#include <X11/Shell.h>

/*******************************************************************************
       Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/


extern swidget create_fileSelectionShell(swidget, swidget);

extern swidget replaySwidget;
extern swidget fileSelectionSwidget;
extern swidget recordItem;
extern swidget replayItem;

extern void updateStatus();
extern void outputMsg (char *);
extern void replayCommands (XtPointer, XtIntervalId *);

extern Boolean  commandReplay;
extern FILE    *commandFile;
char           *replayFilename;


#ifndef XKLOADDS
#define XKLOADDS
#endif /* XKLOADDS */

#include "replayShell.h"


/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

void  _UxCreplayShell::popupCB_replayShell(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 if (replayFilename != NULL)
   XtFree (replayFilename);
 
 replayFilename =
   XmTextFieldGetString (UxGetWidget (replayFileField));
 }
}

void  _UxCreplayShell::Wrap_popupCB_replayShell(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCreplayShell         *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCreplayShell *) _UxCreplayShell::UxGetContext(UxWidget);
 UxContext->popupCB_replayShell(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCreplayShell::activateCB_replaySelectBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 fileSelectionSwidget = create_fileSelectionShell (NULL, replayFileField);
 UxPopupInterface (fileSelectionSwidget, no_grab);
 }
}

void  _UxCreplayShell::Wrap_activateCB_replaySelectBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCreplayShell         *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCreplayShell *) _UxCreplayShell::UxGetContext(UxWidget);
 UxContext->activateCB_replaySelectBtn(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCreplayShell::activateCB_replayOkBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 XmString  replayItemLabel;
 
 /* Retrieve the filename from the Command Replay Dialog Box */
 if (replayFilename != NULL)
   XtFree (replayFilename);
 replayFilename =
   XmTextFieldGetString (UxGetWidget (replayFileField));
 
 commandFile = fopen (replayFilename, "r");
 
 /* Make sure we were able to open the file. */
 if (commandFile == NULL)
 {
   char msg[150];
 
   sprintf (msg, "Command Replay:  Unable to open file '%s'",
            replayFilename);
   outputMsg (msg); 
   return;
 }
 
 /* Ok, we're ready to go.  Perform all of the interface
    changes related to command replay, then return. */
 replayItemLabel =
   XmStringCreateLocalized ("Stop Command Replay");
 XtVaSetValues (UxGetWidget (replayItem),
                XmNlabelString, replayItemLabel,
                NULL);
 XmStringFree (replayItemLabel);
 
 commandReplay = True;
 updateStatus(); 
 
 /* Disable "Command Record" while command replay is in
    progress.  It will be re-enabled when the user selects
    "Stop Command Replay" from the "File" menu. */
 
 XtVaSetValues (UxGetWidget (recordItem),
                XmNsensitive, False,
                NULL);
 
 /* Schedule the first command replay */
 XtAppAddTimeOut (UxAppContext, 0, replayCommands, NULL);
 
 /* Make the "Command Replay" dialog box disappear. */
 UxPopdownInterface (replaySwidget);
 
 }
}

void  _UxCreplayShell::Wrap_activateCB_replayOkBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCreplayShell         *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCreplayShell *) _UxCreplayShell::UxGetContext(UxWidget);
 UxContext->activateCB_replayOkBtn(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCreplayShell::activateCB_replayCancelBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 XtVaSetValues (UxGetWidget (replayFileField),
                XmNvalue, replayFilename,
                NULL);
 
 UxPopdownInterface (replaySwidget);
 }
}

void  _UxCreplayShell::Wrap_activateCB_replayCancelBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCreplayShell         *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCreplayShell *) _UxCreplayShell::UxGetContext(UxWidget);
 UxContext->activateCB_replayCancelBtn(UxWidget, UxClientData, UxCallbackArg);
}

/*******************************************************************************
       The following is the destroyContext callback function.
       It is needed to free the memory allocated by the context.
*******************************************************************************/

static void DelayedDelete( XtPointer obj, XtIntervalId *)
{
 delete ((_UxCreplayShell *) obj);
}
void  _UxCreplayShell::UxDestroyContextCB(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;

 ((_UxCreplayShell *) UxClientData)->UxThis = NULL;
 XtAppAddTimeOut(UxAppContext, 0, DelayedDelete, UxClientData);
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

Widget _UxCreplayShell::_build()
{
 Widget  _UxParent;


 // Creation of replayShell
 _UxParent = UxParent;
 if ( _UxParent == NULL )
 {
  _UxParent = UxTopLevel;
 }

 replayShell = XtVaCreatePopupShell( "replayShell",
   applicationShellWidgetClass,
   _UxParent,
   XmNx, 392,
   XmNy, 204,
   XmNwidth, 327,
   XmNheight, 133,
   XmNtitle, "Replay File",
   NULL );
 XtAddCallback( replayShell, XmNpopupCallback,
  (XtCallbackProc) &_UxCreplayShell::Wrap_popupCB_replayShell,
  (XtPointer) NULL );

 UxPutContext( replayShell, (char *) this );
 UxThis = replayShell;



 // Creation of bulletinBoard2
 bulletinBoard2 = XtVaCreateManagedWidget( "bulletinBoard2",
   xmBulletinBoardWidgetClass,
   replayShell,
   XmNresizePolicy, XmRESIZE_NONE,
   XmNunitType, XmPIXELS,
   XmNx, 62,
   XmNy, 16,
   XmNwidth, 327,
   XmNheight, 272,
   XmNmarginWidth, 0,
   NULL );
 UxPutContext( bulletinBoard2, (char *) this );


 // Creation of replayFileField
 replayFileField = XtVaCreateManagedWidget( "replayFileField",
   xmTextFieldWidgetClass,
   bulletinBoard2,
   XmNx, 117,
   XmNy, 20,
   XmNwidth, 133,
   XmNheight, 31,
   XmNvalue, "./tf_command.log",
   NULL );
 UxPutContext( replayFileField, (char *) this );


 // Creation of replaySelectBtn
 replaySelectBtn = XtVaCreateManagedWidget( "replaySelectBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 258,
   XmNy, 20,
   XmNwidth, 54,
   XmNheight, 31,
   RES_CONVERT( XmNlabelString, "Select" ),
   NULL );
 XtAddCallback( replaySelectBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxCreplayShell::Wrap_activateCB_replaySelectBtn,
  (XtPointer) NULL );

 UxPutContext( replaySelectBtn, (char *) this );


 // Creation of separator1
 separator1 = XtVaCreateManagedWidget( "separator1",
   xmSeparatorWidgetClass,
   bulletinBoard2,
   XmNx, -4,
   XmNy, 66,
   XmNwidth, 330,
   XmNheight, 7,
   NULL );
 UxPutContext( separator1, (char *) this );


 // Creation of replayOkBtn
 replayOkBtn = XtVaCreateManagedWidget( "replayOkBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 22,
   XmNy, 86,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Ok" ),
   NULL );
 XtAddCallback( replayOkBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxCreplayShell::Wrap_activateCB_replayOkBtn,
  (XtPointer) NULL );

 UxPutContext( replayOkBtn, (char *) this );


 // Creation of replayCancelBtn
 replayCancelBtn = XtVaCreateManagedWidget( "replayCancelBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 132,
   XmNy, 86,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Cancel" ),
   NULL );
 XtAddCallback( replayCancelBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxCreplayShell::Wrap_activateCB_replayCancelBtn,
  (XtPointer) NULL );

 UxPutContext( replayCancelBtn, (char *) this );


 // Creation of reaplyHelpBtn
 reaplyHelpBtn = XtVaCreateManagedWidget( "reaplyHelpBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 242,
   XmNy, 86,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Help" ),
   NULL );
 UxPutContext( reaplyHelpBtn, (char *) this );


 // Creation of replayLabel
 replayLabel = XtVaCreateManagedWidget( "replayLabel",
   xmLabelWidgetClass,
   bulletinBoard2,
   XmNx, 16,
   XmNy, 22,
   XmNwidth, 97,
   XmNheight, 25,
   RES_CONVERT( XmNlabelString, "File to Replay:" ),
   NULL );
 UxPutContext( replayLabel, (char *) this );


 XtAddCallback( replayShell, XmNdestroyCallback,
  (XtCallbackProc) &_UxCreplayShell::UxDestroyContextCB,
  (XtPointer) this);


 return ( replayShell );
}

/*******************************************************************************
       The following function includes the code that was entered
       in the 'Initial Code' and 'Final Code' sections of the
       Declarations Editor. This function is called from the
       'Interface function' below.
*******************************************************************************/

swidget _UxCreplayShell::_create_replayShell(void)
{
 Widget                  rtrn;

 // Initial code from declarations editor
 replayFilename = NULL;
 UxThis = rtrn = _build();

 // Final Code from declarations editor
 return(rtrn);
}

/*******************************************************************************
       The following is the destructor function.
*******************************************************************************/

_UxCreplayShell::~_UxCreplayShell()
{
 if (this->UxThis)
 {
  XtRemoveCallback( UxGetWidget(this->UxThis),
   XmNdestroyCallback,
   (XtCallbackProc) &_UxCreplayShell::UxDestroyContextCB,
   (XtPointer) this);
  UxDestroyInterface(this->UxThis);
 }
}

/*******************************************************************************
       The following is the constructor function.
*******************************************************************************/

_UxCreplayShell::_UxCreplayShell( swidget UxParent )
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

swidget create_replayShell( swidget UxParent )
{
 _UxCreplayShell *theInterface = 
   new _UxCreplayShell( UxParent );
 return (theInterface->_create_replayShell());
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

