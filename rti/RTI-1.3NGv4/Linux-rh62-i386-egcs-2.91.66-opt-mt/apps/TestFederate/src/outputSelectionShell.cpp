
/*******************************************************************************
 outputSelectionShell.cc

       Associated Header file: outputSelectionShell.h
*******************************************************************************/

#include <stdio.h>

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/MenuShell.h>
#endif /* MOTIF */

#include "UxXt.h"
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/BulletinB.h>
#include <X11/Shell.h>

/*******************************************************************************
       Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/


extern swidget  create_fileSelectionShell(swidget, swidget);

extern swidget  outputSelectionSwidget;
extern swidget  statusLine;

extern void updateStatus();
extern void outputMsg (char *);


extern Boolean  outputRecording;
extern Boolean  outputDisplay;
extern FILE    *outputFile;
char           *outputFilename;

swidget fileSelectionSwidget;


#ifndef XKLOADDS
#define XKLOADDS
#endif /* XKLOADDS */

#include "outputSelectionShell.h"


/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

void  _UxCoutputSelectionShell::popupCB_outputSelectionShell(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 XmToggleButtonSetState (UxGetWidget (windowOutputBtn),
                         outputDisplay, False);
 
 XmToggleButtonSetState (UxGetWidget (fileOutputBtn),
                         outputRecording, False);
 
 XmTextFieldSetString (UxGetWidget (outputFileField),
                       outputFilename);
 }
}

void  _UxCoutputSelectionShell::Wrap_popupCB_outputSelectionShell(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCoutputSelectionShell *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCoutputSelectionShell *) _UxCoutputSelectionShell::UxGetContext(UxWidget);
 UxContext->popupCB_outputSelectionShell(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCoutputSelectionShell::activateCB_outputFileSelectBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 fileSelectionSwidget = create_fileSelectionShell (NULL, outputFileField);
 UxPopupInterface (fileSelectionSwidget, no_grab);
 }
}

void  _UxCoutputSelectionShell::Wrap_activateCB_outputFileSelectBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCoutputSelectionShell *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCoutputSelectionShell *) _UxCoutputSelectionShell::UxGetContext(UxWidget);
 UxContext->activateCB_outputFileSelectBtn(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCoutputSelectionShell::activateCB_outputSelectionOkBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 /* Retrieve the new values */
 
 outputDisplay   = XmToggleButtonGetState (UxGetWidget (windowOutputBtn));
 outputRecording = XmToggleButtonGetState (UxGetWidget (fileOutputBtn));
 
 if (outputFilename != NULL)
   XtFree (outputFilename);
 
 outputFilename =
   XmTextFieldGetString (UxGetWidget (outputFileField));
 
 if ((outputRecording) && (outputFile == NULL))
 {
   outputFile = fopen (outputFilename, "w");
 
   /* Make sure we were able to open the file. */
   if (outputFile == NULL)
   {
     char msg[150];
 
     sprintf (msg, "Output Recording:  Unable to open file '%s'",
              outputFilename);
     outputMsg (msg);
 
     outputRecording = False;
 
     /* Make the "Output Display" dialog box disappear. */
     UxPopdownInterface (outputSelectionSwidget);
 
     return;
   }
 }
 else if ((!outputRecording) && (outputFile != NULL))
 {
   fclose (outputFile);
   outputFile = NULL;
 }
 
 
 updateStatus();
 
 /* Turn off the output file field and the Select button
    while recording the window output to file.  */
  
 XtVaSetValues (UxGetWidget (outputFileField),
                XmNsensitive, !outputRecording,
                NULL);
 XtVaSetValues (UxGetWidget (outputFileSelectBtn),
                XmNsensitive, !outputRecording,
                NULL);
 
 /* Make the "Output Display" dialog box disappear. */
 UxPopdownInterface (outputSelectionSwidget);
 }
}

void  _UxCoutputSelectionShell::Wrap_activateCB_outputSelectionOkBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCoutputSelectionShell *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCoutputSelectionShell *) _UxCoutputSelectionShell::UxGetContext(UxWidget);
 UxContext->activateCB_outputSelectionOkBtn(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCoutputSelectionShell::activateCB_outputSelectionCancelBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 UxPopdownInterface (outputSelectionSwidget);
 }
}

void  _UxCoutputSelectionShell::Wrap_activateCB_outputSelectionCancelBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCoutputSelectionShell *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCoutputSelectionShell *) _UxCoutputSelectionShell::UxGetContext(UxWidget);
 UxContext->activateCB_outputSelectionCancelBtn(UxWidget, UxClientData, UxCallbackArg);
}

/*******************************************************************************
       The following is the destroyContext callback function.
       It is needed to free the memory allocated by the context.
*******************************************************************************/

static void DelayedDelete( XtPointer obj, XtIntervalId *)
{
 delete ((_UxCoutputSelectionShell *) obj);
}
void  _UxCoutputSelectionShell::UxDestroyContextCB(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;

 ((_UxCoutputSelectionShell *) UxClientData)->UxThis = NULL;
 XtAppAddTimeOut(UxAppContext, 0, DelayedDelete, UxClientData);
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

Widget _UxCoutputSelectionShell::_build()
{
 Widget  _UxParent;


 // Creation of outputSelectionShell
 _UxParent = UxParent;
 if ( _UxParent == NULL )
 {
  _UxParent = UxTopLevel;
 }

 outputSelectionShell = XtVaCreatePopupShell( "outputSelectionShell",
   applicationShellWidgetClass,
   _UxParent,
   XmNx, 403,
   XmNy, 218,
   XmNwidth, 327,
   XmNheight, 140,
   XmNtitle, "Output Selection",
   NULL );
 XtAddCallback( outputSelectionShell, XmNpopupCallback,
  (XtCallbackProc) &_UxCoutputSelectionShell::Wrap_popupCB_outputSelectionShell,
  (XtPointer) NULL );

 UxPutContext( outputSelectionShell, (char *) this );
 UxThis = outputSelectionShell;



 // Creation of bulletinBoard2
 bulletinBoard2 = XtVaCreateManagedWidget( "bulletinBoard2",
   xmBulletinBoardWidgetClass,
   outputSelectionShell,
   XmNresizePolicy, XmRESIZE_NONE,
   XmNunitType, XmPIXELS,
   XmNx, 62,
   XmNy, 16,
   XmNwidth, 327,
   XmNheight, 140,
   XmNmarginWidth, 0,
   NULL );
 UxPutContext( bulletinBoard2, (char *) this );


 // Creation of windowOutputBtn
 windowOutputBtn = XtVaCreateManagedWidget( "windowOutputBtn",
   xmToggleButtonWidgetClass,
   bulletinBoard2,
   XmNx, 9,
   XmNy, 10,
   XmNwidth, 123,
   XmNheight, 25,
   RES_CONVERT( XmNlabelString, "Output to Window" ),
   XmNalignment, XmALIGNMENT_BEGINNING,
   NULL );
 UxPutContext( windowOutputBtn, (char *) this );


 // Creation of fileOutputBtn
 fileOutputBtn = XtVaCreateManagedWidget( "fileOutputBtn",
   xmToggleButtonWidgetClass,
   bulletinBoard2,
   XmNx, 9,
   XmNy, 36,
   XmNwidth, 125,
   XmNheight, 25,
   RES_CONVERT( XmNlabelString, "Output to File:" ),
   XmNalignment, XmALIGNMENT_BEGINNING,
   NULL );
 UxPutContext( fileOutputBtn, (char *) this );


 // Creation of outputFileField
 outputFileField = XtVaCreateManagedWidget( "outputFileField",
   xmTextFieldWidgetClass,
   bulletinBoard2,
   XmNx, 135,
   XmNy, 34,
   XmNwidth, 113,
   XmNheight, 31,
   NULL );
 UxPutContext( outputFileField, (char *) this );


 // Creation of outputFileSelectBtn
 outputFileSelectBtn = XtVaCreateManagedWidget( "outputFileSelectBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 256,
   XmNy, 34,
   XmNwidth, 54,
   XmNheight, 31,
   RES_CONVERT( XmNlabelString, "Select" ),
   NULL );
 XtAddCallback( outputFileSelectBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxCoutputSelectionShell::Wrap_activateCB_outputFileSelectBtn,
  (XtPointer) NULL );

 UxPutContext( outputFileSelectBtn, (char *) this );


 // Creation of separator1
 separator1 = XtVaCreateManagedWidget( "separator1",
   xmSeparatorWidgetClass,
   bulletinBoard2,
   XmNx, -6,
   XmNy, 80,
   XmNwidth, 333,
   XmNheight, 7,
   NULL );
 UxPutContext( separator1, (char *) this );


 // Creation of outputSelectionOkBtn
 outputSelectionOkBtn = XtVaCreateManagedWidget( "outputSelectionOkBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 20,
   XmNy, 100,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Ok" ),
   NULL );
 XtAddCallback( outputSelectionOkBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxCoutputSelectionShell::Wrap_activateCB_outputSelectionOkBtn,
  (XtPointer) NULL );

 UxPutContext( outputSelectionOkBtn, (char *) this );


 // Creation of outputSelectionCancelBtn
 outputSelectionCancelBtn = XtVaCreateManagedWidget( "outputSelectionCancelBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 130,
   XmNy, 100,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Cancel" ),
   NULL );
 XtAddCallback( outputSelectionCancelBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxCoutputSelectionShell::Wrap_activateCB_outputSelectionCancelBtn,
  (XtPointer) NULL );

 UxPutContext( outputSelectionCancelBtn, (char *) this );


 // Creation of outputSelectionHelpBtn
 outputSelectionHelpBtn = XtVaCreateManagedWidget( "outputSelectionHelpBtn",
   xmPushButtonWidgetClass,
   bulletinBoard2,
   XmNx, 240,
   XmNy, 100,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Help" ),
   NULL );
 UxPutContext( outputSelectionHelpBtn, (char *) this );


 XtAddCallback( outputSelectionShell, XmNdestroyCallback,
  (XtCallbackProc) &_UxCoutputSelectionShell::UxDestroyContextCB,
  (XtPointer) this);


 return ( outputSelectionShell );
}

/*******************************************************************************
       The following function includes the code that was entered
       in the 'Initial Code' and 'Final Code' sections of the
       Declarations Editor. This function is called from the
       'Interface function' below.
*******************************************************************************/

swidget _UxCoutputSelectionShell::_create_outputSelectionShell(void)
{
 Widget                  rtrn;

 // Initial code from declarations editor
 outputFilename = "./tf_output.log";
 UxThis = rtrn = _build();

 // Final Code from declarations editor
 return(rtrn);
}

/*******************************************************************************
       The following is the destructor function.
*******************************************************************************/

_UxCoutputSelectionShell::~_UxCoutputSelectionShell()
{
 if (this->UxThis)
 {
  XtRemoveCallback( UxGetWidget(this->UxThis),
   XmNdestroyCallback,
   (XtCallbackProc) &_UxCoutputSelectionShell::UxDestroyContextCB,
   (XtPointer) this);
  UxDestroyInterface(this->UxThis);
 }
}

/*******************************************************************************
       The following is the constructor function.
*******************************************************************************/

_UxCoutputSelectionShell::_UxCoutputSelectionShell( swidget UxParent )
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

swidget create_outputSelectionShell( swidget UxParent )
{
 _UxCoutputSelectionShell *theInterface = 
   new _UxCoutputSelectionShell( UxParent );
 return (theInterface->_create_outputSelectionShell());
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

