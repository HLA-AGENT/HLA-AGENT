
/*******************************************************************************
 commandRecordingShell.cc

       Associated Header file: commandRecordingShell.h
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

extern swidget commandRecordingSwidget;
extern swidget fileSelectionSwidget;
extern swidget recordItem;
extern swidget replayItem;

extern void updateStatus();
extern void outputMsg (char *);

extern Boolean  commandRecording;
extern FILE    *commandFile;
char           *commandFilename;


#ifndef XKLOADDS
#define XKLOADDS
#endif /* XKLOADDS */

#include "commandRecordingShell.h"


/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

void  _UxCcommandRecordingShell::popupCB_commandRecordingShell(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 if (commandFilename != NULL)
   XtFree (commandFilename);
 
 commandFilename =
   XmTextFieldGetString (UxGetWidget (commandFileField));
 }
}

void  _UxCcommandRecordingShell::Wrap_popupCB_commandRecordingShell(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCcommandRecordingShell *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCcommandRecordingShell *) _UxCcommandRecordingShell::UxGetContext(UxWidget);
 UxContext->popupCB_commandRecordingShell(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCcommandRecordingShell::activateCB_commandRecordingSelectBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 fileSelectionSwidget = create_fileSelectionShell (NULL, commandFileField);
 UxPopupInterface (fileSelectionSwidget, no_grab);
 }
}

void  _UxCcommandRecordingShell::Wrap_activateCB_commandRecordingSelectBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCcommandRecordingShell *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCcommandRecordingShell *) _UxCcommandRecordingShell::UxGetContext(UxWidget);
 UxContext->activateCB_commandRecordingSelectBtn(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCcommandRecordingShell::activateCB_commandRecordingOkBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 XmString  recordItemLabel;
 
 /* Retrieve the filename from the Command Recording Dialog Box */
 if (commandFilename != NULL)
   XtFree (commandFilename);
 commandFilename =
   XmTextFieldGetString (UxGetWidget (commandFileField));
 
 commandFile = fopen (commandFilename, "w");
 
 /* Make sure we were able to open the file. */
 if (commandFile == NULL)
 {
   char msg[150];
 
   sprintf (msg, "Command Recording:  Unable to open file '%s'",
            commandFilename);
   outputMsg (msg); 
 }
 else
 {
   /* Ok, we're ready to go.  Perform all of the interface
      changes related to command recording, then return. */
   recordItemLabel =
     XmStringCreateLocalized ("Stop Command Recording");
   XtVaSetValues (UxGetWidget (recordItem),
                  XmNlabelString, recordItemLabel,
                  NULL);
   XmStringFree (recordItemLabel);
 
   commandRecording = True;
   updateStatus(); 
 
   /* Disable "File Replay" while command recording is in
      progress.  It will be re-enabled when the user selects
      "Stop Command Recording" from the "File" menu. */
 
   XtVaSetValues (UxGetWidget (replayItem),
                  XmNsensitive, False,
                  NULL);
 }
 
 /* Make the "Command Recording" dialog box disappear. */
 UxPopdownInterface (commandRecordingSwidget);
 
 }
}

void  _UxCcommandRecordingShell::Wrap_activateCB_commandRecordingOkBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCcommandRecordingShell *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCcommandRecordingShell *) _UxCcommandRecordingShell::UxGetContext(UxWidget);
 UxContext->activateCB_commandRecordingOkBtn(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCcommandRecordingShell::activateCB_commandRecordingCancelBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 XtVaSetValues (UxGetWidget (commandFileField),
                XmNvalue, commandFilename,
                NULL);
 
 UxPopdownInterface (commandRecordingSwidget);
 }
}

void  _UxCcommandRecordingShell::Wrap_activateCB_commandRecordingCancelBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCcommandRecordingShell *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCcommandRecordingShell *) _UxCcommandRecordingShell::UxGetContext(UxWidget);
 UxContext->activateCB_commandRecordingCancelBtn(UxWidget, UxClientData, UxCallbackArg);
}

/*******************************************************************************
       The following is the destroyContext callback function.
       It is needed to free the memory allocated by the context.
*******************************************************************************/

static void DelayedDelete( XtPointer obj, XtIntervalId *)
{
 delete ((_UxCcommandRecordingShell *) obj);
}
void  _UxCcommandRecordingShell::UxDestroyContextCB(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;

 ((_UxCcommandRecordingShell *) UxClientData)->UxThis = NULL;
 XtAppAddTimeOut(UxAppContext, 0, DelayedDelete, UxClientData);
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

Widget _UxCcommandRecordingShell::_build()
{
 Widget  _UxParent;


 // Creation of commandRecordingShell
 _UxParent = UxParent;
 if ( _UxParent == NULL )
 {
  _UxParent = UxTopLevel;
 }

 commandRecordingShell = XtVaCreatePopupShell( "commandRecordingShell",
   applicationShellWidgetClass,
   _UxParent,
   XmNx, 419,
   XmNy, 251,
   XmNwidth, 327,
   XmNheight, 133,
   XmNtitle, "Command Recording",
   NULL );
 XtAddCallback( commandRecordingShell, XmNpopupCallback,
  (XtCallbackProc) &_UxCcommandRecordingShell::Wrap_popupCB_commandRecordingShell,
  (XtPointer) NULL );

 UxPutContext( commandRecordingShell, (char *) this );
 UxThis = commandRecordingShell;



 // Creation of bulletinBoard2
 bulletinBoard2 = XtVaCreateManagedWidget( "bulletinBoard2",
   xmBulletinBoardWidgetClass,
   commandRecordingShell,
   XmNresizePolicy, XmRESIZE_NONE,
   XmNunitType, XmPIXELS,
   XmNx, 62,
   XmNy, 16,
   XmNwidth, 327,
   XmNheight, 272,
   XmNmarginWidth, 0,
   NULL );
 UxPutContext( bulletinBoard2, (char *) this );


 // Creation of commandFileField
 commandFileField = XtVaCreateManagedWidget( "commandFileField",
   xmTextFieldWidgetClass,
   bulletinBoard2,
   XmNx, 117,
   XmNy, 20,
   XmNwidth, 133,
   XmNheight, 31,
   XmNvalue, "./tf_command.log",
   NULL );
 UxPutContext( commandFileField, (char *) this );


 // Creation of commandRecordingSelectBtn
 commandRecordingSelectBtn = XtVaCreateManagedWidget( "commandRecordingSelectBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 258,
   XmNy, 20,
   XmNwidth, 54,
   XmNheight, 31,
   RES_CONVERT( XmNlabelString, "Select" ),
   NULL );
 XtAddCallback( commandRecordingSelectBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxCcommandRecordingShell::Wrap_activateCB_commandRecordingSelectBtn,
  (XtPointer) NULL );

 UxPutContext( commandRecordingSelectBtn, (char *) this );


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


 // Creation of commandRecordingLabel
 commandRecordingLabel = XtVaCreateManagedWidget( "commandRecordingLabel",
   xmLabelWidgetClass,
   bulletinBoard2,
   XmNx, 16,
   XmNy, 22,
   XmNwidth, 97,
   XmNheight, 25,
   RES_CONVERT( XmNlabelString, "Output to File:" ),
   NULL );
 UxPutContext( commandRecordingLabel, (char *) this );


 // Creation of commandRecordingOkBtn
 commandRecordingOkBtn = XtVaCreateManagedWidget( "commandRecordingOkBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 17,
   XmNy, 87,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Ok" ),
   XmNwidth, 70,
   NULL );
 XtAddCallback( commandRecordingOkBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxCcommandRecordingShell::Wrap_activateCB_commandRecordingOkBtn,
  (XtPointer) NULL );

 UxPutContext( commandRecordingOkBtn, (char *) this );


 // Creation of commandRecordingCancelBtn
 commandRecordingCancelBtn = XtVaCreateManagedWidget( "commandRecordingCancelBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 127,
   XmNy, 87,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Cancel" ),
   NULL );
 XtAddCallback( commandRecordingCancelBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxCcommandRecordingShell::Wrap_activateCB_commandRecordingCancelBtn,
  (XtPointer) NULL );

 UxPutContext( commandRecordingCancelBtn, (char *) this );


 // Creation of commandRecordingHelpBtn
 commandRecordingHelpBtn = XtVaCreateManagedWidget( "commandRecordingHelpBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 237,
   XmNy, 87,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Help" ),
   NULL );
 UxPutContext( commandRecordingHelpBtn, (char *) this );


 XtAddCallback( commandRecordingShell, XmNdestroyCallback,
  (XtCallbackProc) &_UxCcommandRecordingShell::UxDestroyContextCB,
  (XtPointer) this);


 return ( commandRecordingShell );
}

/*******************************************************************************
       The following function includes the code that was entered
       in the 'Initial Code' and 'Final Code' sections of the
       Declarations Editor. This function is called from the
       'Interface function' below.
*******************************************************************************/

swidget _UxCcommandRecordingShell::_create_commandRecordingShell(void)
{
 Widget                  rtrn;

 // Initial code from declarations editor
 commandFilename = NULL;
 UxThis = rtrn = _build();

 // Final Code from declarations editor
 return(rtrn);
}

/*******************************************************************************
       The following is the destructor function.
*******************************************************************************/

_UxCcommandRecordingShell::~_UxCcommandRecordingShell()
{
 if (this->UxThis)
 {
  XtRemoveCallback( UxGetWidget(this->UxThis),
   XmNdestroyCallback,
   (XtCallbackProc) &_UxCcommandRecordingShell::UxDestroyContextCB,
   (XtPointer) this);
  UxDestroyInterface(this->UxThis);
 }
}

/*******************************************************************************
       The following is the constructor function.
*******************************************************************************/

_UxCcommandRecordingShell::_UxCcommandRecordingShell( swidget UxParent )
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

swidget create_commandRecordingShell( swidget UxParent )
{
 _UxCcommandRecordingShell *theInterface = 
   new _UxCcommandRecordingShell( UxParent );
 return (theInterface->_create_commandRecordingShell());
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

