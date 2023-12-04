
/******************************************************************************
 genericPromptDialog.cc

       Associated Header file: genericPromptDialog.h
******************************************************************************/

#include <stdio.h>
#include <errno.h>

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/MenuShell.h>
#endif /* MOTIF */

#include "UxXt.h"
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <X11/Shell.h>
#include <Xm/VendorS.h>


/******************************************************************************
       Includes, Defines, and Global variables from the Declarations Editor:
******************************************************************************/

#include <sys/types.h>

#include "datatypes.h"

extern  PromptStruct  promptRecs[];
static  Widget       *promptTextFields;

static  void          setFocus (XtPointer, XtIntervalId*);
extern  swidget       create_helpSelectionShell (swidget, ServiceID);

void                  scheduleNextTick (char *);
static  void          nextTick (XtPointer, XtIntervalId *);
extern  void          do_autoTick (char **, int);

extern  void          recordCommands (const ServiceID   serviceID,
          const Boolean     extendedMode,
          const int         promptCnt,
          const char* const fields[]);

typedef enum { NEW_DIALOG, OLD_DIALOG } DialogStates;
typedef enum { DEFAULT_MODE, EXTENDED_MODE } ModeStates;

#ifndef XKLOADDS
#define XKLOADDS
#endif /* XKLOADDS */

#include "genericPromptDialog.h"


/******************************************************************************
Auxiliary code from the Declarations Editor:
******************************************************************************/

void scheduleNextTick (char *intervalStr)
{
  int interval = atoi (intervalStr);

  XtAppAddTimeOut (UxAppContext, interval,
                   nextTick, (XtPointer)interval);
}

void nextTick (XtPointer clientData, XtIntervalId *id)
{
  int interval = (int)clientData;
  char **intervalStr;

  intervalStr = (char **)malloc (sizeof (char *));
  intervalStr[0] = (char *)malloc (10);
  sprintf (intervalStr[0], "%i", interval);

  do_autoTick (intervalStr, CONTINUE);

  free (intervalStr[0]);
  free (intervalStr);
}


/******************************************************************************
       The following are callback functions.
******************************************************************************/

void  _UxCgenericPromptDialog::activateCB_promptDialogOkBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
  Widget                  UxWidget = wgt;
  XtPointer               UxClientData = cd;
  XtPointer               UxCallbackArg = cb;
  {
    int       i;
    int       index;
    int       promptCnt;
    char    **prompts;
    char    **fields;
    char     *text;
    char     *a, *b;
    char      optionString[50];
    Boolean   extendedMode;
    Boolean   done;
    Boolean   found;
    XmString  selectionStr;
    Widget    selectedWidget;

    
    extendedMode = XmToggleButtonGetState (UxGetWidget (extendedModeBtn));

    if (extendedMode)
    {
      prompts   = promptRecs[serviceID].extendedForm->prompts;
      promptCnt = promptRecs[serviceID].extendedForm->promptCnt;
    }
    else if (strcmp (promptRecs[serviceID].prompts[0], "") == 0)
      promptCnt = 0;
    else
    {
      prompts   = promptRecs[serviceID].prompts;
      promptCnt = promptRecs[serviceID].promptCnt;
    }

    // Allocate the memory for the fields.
    fields = (char **)malloc (sizeof (char *) * promptCnt);

    // Retrieve the field values from the textField widgets
    for (i = 0; i < promptCnt; i++)
    {
      if (prompts[i][0] == '^')
      {     // optionMenu widget.
 XtVaGetValues (UxGetWidget (promptTextFields[i]),
         XmNmenuHistory, &selectedWidget,
         NULL);
 
 XtVaGetValues (selectedWidget,
         XmNlabelString, &selectionStr,
                NULL);
 
 XmStringGetLtoR (selectionStr,
                  XmFONTLIST_DEFAULT_TAG, &text);
 XmStringFree (selectionStr);

 // Now map the text string to the appropriate enumeration value.
 a = prompts[i] + 1;
 b = strchr (a, '^');

 for (index = 1, done = False, found = False;
      !done && !found; index++)
        {
   // Get the first (or next) menu item label.
   a = b + 1;
   b = strchr (a, ',');
   if (b != NULL)
   {
     strncpy (optionString, a, b - a);
     optionString[b - a] = '\0';
   }
   else
          {
     strcpy (optionString, a);
     done = True;
   }

   if (strcmp (optionString, text) == 0)
          {
     fields[i] = (char *)malloc (2);
     sprintf (fields[i], "%i", index);
     found = True;
   }  
 }
 XtFree (text);
      }
      else  // textField widget.
 XtVaGetValues (UxGetWidget (promptTextFields[i]),
         XmNvalue, &fields[i],
         NULL);
    }

    // Determine if command-recording is in process.  If it is,
    // the service ID, extended mode status, field count, and each
    // of the fields will be written to the command file.
    recordCommands (serviceID, extendedMode, promptCnt, fields);


    // Invoke the proper service function.
    if ((!extendedMode) && (promptRecs[serviceID].service != NULL))
    {
      promptRecs[serviceID].service (fields, promptCnt);
    }
    else if ((extendedMode) &&
      (promptRecs[serviceID].extendedForm->service != NULL))
    {
      promptRecs[serviceID].extendedForm->service(fields, promptCnt);
    }


    // Free the memory used by the fields array.
    for (i = 0; i < promptCnt; i++)
      XtFree (fields[i]);
    free (fields);

    UxDestroyInterface (genericPromptDialog);
  }
}


void  _UxCgenericPromptDialog::Wrap_activateCB_promptDialogOkBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCgenericPromptDialog *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCgenericPromptDialog *) _UxCgenericPromptDialog::UxGetContext(UxWidget);
 UxContext->activateCB_promptDialogOkBtn(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCgenericPromptDialog::activateCB_promptDialogCancelBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
  Widget                  UxWidget = wgt;
  XtPointer               UxClientData = cd;
  XtPointer               UxCallbackArg = cb;
  {
    UxDestroyInterface (genericPromptDialog);
  }
}

void  _UxCgenericPromptDialog::Wrap_activateCB_promptDialogCancelBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCgenericPromptDialog *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCgenericPromptDialog *) _UxCgenericPromptDialog::UxGetContext(UxWidget);
 UxContext->activateCB_promptDialogCancelBtn(UxWidget, UxClientData, UxCallbackArg);
}


void  _UxCgenericPromptDialog::activateCB_promptDialogHelpBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
  Widget                  UxWidget = wgt;
  XtPointer               UxClientData = cd;
  XtPointer               UxCallbackArg = cb;
  {
    UxPopupInterface (create_helpSelectionShell (NULL, serviceID), no_grab);
  }
}

void  _UxCgenericPromptDialog::Wrap_activateCB_promptDialogHelpBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCgenericPromptDialog *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCgenericPromptDialog *) _UxCgenericPromptDialog::UxGetContext(UxWidget);
 UxContext->activateCB_promptDialogHelpBtn(UxWidget, UxClientData, UxCallbackArg);
}


void  _UxCgenericPromptDialog::valueChangedCB_extendedModeBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
  Widget                  UxWidget = wgt;
  XtPointer               UxClientData = cd;
  XtPointer               UxCallbackArg = cb;
  {
    XmToggleButtonCallbackStruct *btn =
      (XmToggleButtonCallbackStruct *)UxCallbackArg;
 
    XtVaSetValues (separator3,
     XmNtopAttachment, XmATTACH_NONE,
     NULL);

    XtDestroyWidget (variableForm);

    buildVariableForm (OLD_DIALOG,
         (btn->set) ? EXTENDED_MODE : DEFAULT_MODE);

    XtVaSetValues (separator3,
     XmNtopAttachment, XmATTACH_WIDGET,
     XmNtopOffset,     7,
     XmNtopWidget,     variableForm,
     NULL);

    // Move the cursor focus to the first text field.
    XtAppAddTimeOut (UxAppContext, 0, setFocus, (XtPointer)serviceID);
  }
}


void  _UxCgenericPromptDialog::Wrap_valueChangedCB_extendedModeBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCgenericPromptDialog *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCgenericPromptDialog *) _UxCgenericPromptDialog::UxGetContext(UxWidget);
 UxContext->valueChangedCB_extendedModeBtn(UxWidget, UxClientData, UxCallbackArg);
}


/******************************************************************************
       The following is the destroyContext callback function.
       It is needed to free the memory allocated by the context.
******************************************************************************/

static void DelayedDelete( XtPointer obj, XtIntervalId *)
{
 delete ((_UxCgenericPromptDialog *) obj);
}
void  _UxCgenericPromptDialog::UxDestroyContextCB(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;

 ((_UxCgenericPromptDialog *) UxClientData)->UxThis = NULL;
 XtAppAddTimeOut(UxAppContext, 0, DelayedDelete, UxClientData);
}

/******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
******************************************************************************/

Widget _UxCgenericPromptDialog::_build()
{
 Widget  _UxParent;
 XmString        PBserviceName;
 char           *serviceName;

 // Creation of genericPromptDialog
 _UxParent = UxParent;
 if ( _UxParent == NULL )
 {
   _UxParent = UxTopLevel;
 }


 /* Get the label of the menu button that got us here.
    It will be used as the service name for the dialog box. */
 XtVaGetValues (UxGetWidget (_UxParent),
         XmNlabelString, &PBserviceName,
         NULL);
 XmStringGetLtoR (PBserviceName, XmFONTLIST_DEFAULT_TAG,
    &serviceName);
 XmStringFree (PBserviceName);
 serviceName[strlen (serviceName) - 3] = '\0';

 genericPromptDialog = XtVaCreatePopupShell( "genericPromptDialog",
   applicationShellWidgetClass,
   _UxParent,
   XmNx,                385,
   XmNy,                163,
   XmNwidth,            433,
            XmNminWidth,         433,
   XmNallowShellResize, TRUE,
   XmNmwmDecorations,
                          MWM_DECOR_TITLE   | MWM_DECOR_BORDER   | 
                          MWM_DECOR_RESIZEH | MWM_DECOR_MINIMIZE,
   XmNtitle,            serviceName,
   NULL );
 UxPutContext( genericPromptDialog, (char *) this );
 UxThis = genericPromptDialog;
 serviceName[strlen (serviceName) - 3] = '.';
 XtFree (serviceName);


 // Creation of genericPromptForm
 genericPromptForm = XtVaCreateManagedWidget( "genericPromptForm",
   xmFormWidgetClass,
   genericPromptDialog,
   NULL );
 UxPutContext( genericPromptForm, (char *) this );


 // Creation of instructionsLabel
 instructionsLabel = XtVaCreateManagedWidget( "instructionsLabel",
   xmLabelWidgetClass,
   genericPromptForm,
   XmNx, 9,
   XmNy, 16,
   XmNwidth, 320,
   XmNheight, 35,
   XmNalignment, XmALIGNMENT_BEGINNING,
   RES_CONVERT( XmNlabelString,
        "Please enter the following values:" ),
   NULL );
 UxPutContext( instructionsLabel, (char *) this );


 // Creation of separator1
 separator1 = XtVaCreateManagedWidget( "separator1",
   xmSeparatorWidgetClass,
   genericPromptForm,
   XmNy, 59,
   XmNleftAttachment,  XmATTACH_FORM,
   XmNrightAttachment, XmATTACH_FORM,
   NULL );
 UxPutContext( separator1, (char *) this );


 // Creation of extendedModeBtn
 extendedModeBtn = XtVaCreateManagedWidget( "extendedModeBtn",
   xmToggleButtonWidgetClass,
   genericPromptForm,
   XmNx, 5,
   XmNwidth, 320,
   XmNheight, 20,
   XmNleftOffset, 0,
   XmNtopAttachment, XmATTACH_WIDGET,
   XmNtopOffset, 5,
   XmNtopWidget, separator1,
   XmNalignment, XmALIGNMENT_BEGINNING,
   RES_CONVERT( XmNlabelString,
         "Use the extended form of this command" ),
   XmNmarginLeft, 17,
   XmNmarginWidth, 2,
   XmNvisibleWhenOff, TRUE,
                        XmNsensitive,
     (promptRecs[serviceID].extendedForm != NULL) ?
       TRUE : FALSE,
   NULL );
 XtAddCallback( extendedModeBtn, XmNvalueChangedCallback,
                      (XtCallbackProc)&_UxCgenericPromptDialog::
          Wrap_valueChangedCB_extendedModeBtn,
        (XtPointer) NULL );
 UxPutContext( extendedModeBtn, (char *) this );

 buildVariableForm (NEW_DIALOG, DEFAULT_MODE);

 // Creation of separator3
 separator3 = XtVaCreateManagedWidget( "separator3",
   xmSeparatorWidgetClass,
   genericPromptForm,
   XmNtopAttachment,   XmATTACH_WIDGET,
   XmNtopOffset,       7,
   XmNtopWidget,       variableForm,
   XmNleftAttachment,  XmATTACH_FORM,
   XmNrightAttachment, XmATTACH_FORM,
   NULL );
 UxPutContext( separator3, (char *) this );


 // Creation of promptDialogOkBtn
 promptDialogOkBtn = XtVaCreateManagedWidget( "promptDialogOkBtn",
   xmPushButtonWidgetClass,
   genericPromptForm,
   XmNx, 45,
   XmNy, 281,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Ok" ),
   XmNtopAttachment, XmATTACH_WIDGET,
   XmNtopOffset, 20,
   XmNtopWidget, separator3,
   NULL );
 XtAddCallback( promptDialogOkBtn, XmNactivateCallback,
        (XtCallbackProc) &_UxCgenericPromptDialog::
        Wrap_activateCB_promptDialogOkBtn,
        (XtPointer) NULL );

 UxPutContext( promptDialogOkBtn, (char *) this );


 // Creation of promptDialogCancelBtn
 promptDialogCancelBtn =
   XtVaCreateManagedWidget( "promptDialogCancelBtn",
   xmPushButtonWidgetClass,
   genericPromptForm,
   XmNx, 180,
   XmNy, 281,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Cancel" ),
   XmNtopAttachment, XmATTACH_WIDGET,
   XmNtopOffset, 20,
   XmNtopWidget, separator3,
   NULL );
 XtAddCallback( promptDialogCancelBtn, XmNactivateCallback,
        (XtCallbackProc) &_UxCgenericPromptDialog::
          Wrap_activateCB_promptDialogCancelBtn,
        (XtPointer) NULL );

 UxPutContext( promptDialogCancelBtn, (char *) this );


 // Creation of promptDialogHelpBtn
 promptDialogHelpBtn = XtVaCreateManagedWidget( "promptDialogHelpBtn",
   xmPushButtonWidgetClass,
   genericPromptForm,
   XmNx, 320,
   XmNy, 281,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Help" ),
   XmNtopAttachment, XmATTACH_WIDGET,
   XmNtopOffset, 20,
   XmNtopWidget, separator3,
   XmNsensitive,
     (promptRecs[serviceID].availableHelp != NONE) ?
     True : False,
   NULL );
 XtAddCallback( promptDialogHelpBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxCgenericPromptDialog::Wrap_activateCB_promptDialogHelpBtn,
  (XtPointer) NULL );

 UxPutContext( promptDialogHelpBtn, (char *) this );


 XtAddCallback( genericPromptDialog, XmNdestroyCallback,
  (XtCallbackProc) &_UxCgenericPromptDialog::UxDestroyContextCB,
  (XtPointer) this);


 // Creation of placeHolder
 placeHolder2 = XtVaCreateManagedWidget( "placeHolder2",
   xmLabelWidgetClass,
   genericPromptForm,
   XmNwidth, 1,
   XmNheight, 1,
   RES_CONVERT( XmNlabelString, "" ),
   XmNtopAttachment,  XmATTACH_WIDGET,
   XmNtopWidget,      promptDialogOkBtn,
   XmNtopOffset,      20,
   XmNleftAttachment, XmATTACH_FORM,
   NULL );
 UxPutContext( placeHolder2, (char *) this );


 // Move the cursor focus to the first text field.
 XtAppAddTimeOut (UxAppContext, 0, setFocus, (XtPointer)serviceID);

 return ( genericPromptDialog );
}

/******************************************************************************
       The following function includes the code that was entered
       in the 'Initial Code' and 'Final Code' sections of the
       Declarations Editor. This function is called from the
       'Interface function' below.
******************************************************************************/

swidget _UxCgenericPromptDialog::_create_genericPromptDialog(void)
{
 Widget                  rtrn;
 UxThis = rtrn = _build();

 // Final Code from declarations editor
 return(rtrn);
}

/******************************************************************************
       The following is the destructor function.
******************************************************************************/

_UxCgenericPromptDialog::~_UxCgenericPromptDialog()
{
 if (this->UxThis)
 {
  XtRemoveCallback( UxGetWidget(this->UxThis),
   XmNdestroyCallback,
   (XtCallbackProc) &_UxCgenericPromptDialog::UxDestroyContextCB,
   (XtPointer) this);
  UxDestroyInterface(this->UxThis);
 }
}

/******************************************************************************
       The following is the constructor function.
******************************************************************************/

_UxCgenericPromptDialog::_UxCgenericPromptDialog( swidget UxParent, ServiceID serviceID )
{
 this->UxParent = UxParent;
 this->serviceID = serviceID;

 // User Supplied Constructor Code
}

/******************************************************************************
       The following is the 'Interface function' which is the
       external entry point for creating this interface.
       This function should be called from your application or from
       a callback function.
******************************************************************************/

swidget create_genericPromptDialog( swidget UxParent, ServiceID serviceID )
{
 _UxCgenericPromptDialog *theInterface = 
   new _UxCgenericPromptDialog( UxParent, serviceID );
 return (theInterface->_create_genericPromptDialog());
}


/*****************************************************************************
      This is a interface-specific function.
*****************************************************************************/

static void setFocus (XtPointer callData, XtIntervalId *id)
{
  int serviceID = (int)callData;

  if (strcmp (promptRecs[serviceID].prompts[0], "") != 0)
    XmProcessTraversal(UxGetWidget (promptTextFields[0]),
         XmTRAVERSE_CURRENT);
}


void _UxCgenericPromptDialog::buildVariableForm (DialogStates dialogState,
       ModeStates   modeState)
{
  Widget          optionMenu_shell;
  Widget          menu1;
  Widget         *topLabelWidget;
  Widget         *topFieldWidget;
  XmString        PBserviceName;
  char           *serviceName;
  char           *a, *b;
  char            promptString[50];
  char            optionString[50];
  char          **prompts;
  int             promptCnt;
  int             i;
  Boolean         optMenu;
  Boolean         done;
  static Boolean  first;


  if (modeState == EXTENDED_MODE)
  {
    prompts   = promptRecs[serviceID].extendedForm->prompts;
    promptCnt = promptRecs[serviceID].extendedForm->promptCnt;
  }
  else
  {
    prompts   = promptRecs[serviceID].prompts;
    promptCnt = promptRecs[serviceID].promptCnt;
  }
  
  // Creation of variableForm.  If we got here from _build
  // (reset == True), then the form must be created managed.
  // Otherwise, leave it unmanaged.  In this case it will be
  // managed at the end of this subroutine.
  if (dialogState == NEW_DIALOG)
    variableForm = XtVaCreateManagedWidget( "variableForm",
       xmFormWidgetClass,
       genericPromptForm,
       XmNtopAttachment, XmATTACH_WIDGET,
       XmNtopOffset, 7,
       XmNtopWidget, extendedModeBtn,
       XmNrightAttachment, XmATTACH_FORM,
       XmNrightOffset, 1,
       XmNleftAttachment, XmATTACH_FORM,
       XmNleftOffset, 1,
       NULL);
  else
    variableForm = XtVaCreateWidget( "variableForm",
       xmFormWidgetClass,
       genericPromptForm,
       XmNtopAttachment, XmATTACH_WIDGET,
       XmNtopOffset, 7,
       XmNtopWidget, extendedModeBtn,
       XmNrightAttachment, XmATTACH_FORM,
       XmNrightOffset, 1,
       XmNleftAttachment, XmATTACH_FORM,
       XmNleftOffset, 1,
       NULL);
  UxPutContext( variableForm, (char *) this);
  
  // Creation of placeHolder
  placeHolder = XtVaCreateManagedWidget( "placeHolder",
      xmLabelWidgetClass,
      variableForm,
      XmNwidth, 1,
      XmNheight, 1,
      RES_CONVERT( XmNlabelString, "" ),
      XmNtopAttachment,  XmATTACH_FORM,
      XmNleftAttachment, XmATTACH_FORM,
      NULL);
  UxPutContext( placeHolder, (char *) this);
  
  // Don't bother printing the labels and text fields if the first
  // prompt is blank.  This can only occur on the default form of
  // the command (when set == FALSE).
  
  if (strcmp (prompts[0], "") != 0)
  {
    // Allocate the required number of text entry fields.
    promptTextFields = (Widget *) malloc (sizeof (Widget) * promptCnt);
      
    topLabelWidget = &placeHolder;
    topFieldWidget = &placeHolder;
      
    for (i = 0; i < promptCnt; i++)
    {
      if (prompts[i][0] == '^')
      {
 // If we get in here, then we are dealing with an option menu.
       
 // Extract the prompt label.
 a = prompts[i] + 1;
 b = strchr (a, '^');
 strncpy (promptString, a, b - a);
 promptString[b - a] = '\0';
 
 optMenu = True;
      }
      else
      {
 strcpy (promptString, prompts[i]);
 optMenu = False;
      }
   
      // Creation of promptLabel
      promptLabel =
 XtVaCreateManagedWidget ("promptLabel",
     xmLabelWidgetClass,
     variableForm,
     XmNx,       9,
     XmNwidth, 283,
     XmNheight, optMenu ? 35 : 31,
     RES_CONVERT( XmNlabelString, promptString ),
     XmNalignment, XmALIGNMENT_BEGINNING,
     XmNtopAttachment,  XmATTACH_WIDGET,
     XmNtopWidget,     *topLabelWidget,
     XmNtopOffset,      5,
     NULL);
      UxPutContext( promptLabel, (char *) this);
   
      if (optMenu)
      {
 // If we get in here, then we are dealing with an option menu.
       
 // Creation of an optionMenu
 optionMenu_shell =
   XtVaCreatePopupShell ("optionMenu_shell",
    xmMenuShellWidgetClass, variableForm,
    XmNwidth, 1,
    XmNheight, 1,
    XmNallowShellResize, TRUE,
    XmNoverrideRedirect, TRUE,
    NULL);
       
 promptTextFields[i] =
   XtVaCreateWidget ("optionMenu",
       xmRowColumnWidgetClass,
       optionMenu_shell,
       XmNrowColumnType, XmMENU_PULLDOWN,
       XmNx, 0,
       XmNy, -9,
       NULL);
       
 for (done = False; !done;)
 {
   // Extract the option menu items
   a = b + 1;
   b = strchr (a, ',');
   if (b != NULL)
   {
     strncpy (optionString, a, b - a);
     optionString[b - a] = '\0';
   }
   else  // Last entry in list.
   {
     strcpy (optionString, a);
     done = True;
   }
    
   // Creation of menu item
   XtVaCreateManagedWidget ("menuItem",
       xmPushButtonWidgetClass,
       promptTextFields[i],
       RES_CONVERT (XmNlabelString, optionString ),
       XmNx, 2,
       XmNy, 37,
       NULL);
 }
       
 // Creation of menu1
 menu1 = XtVaCreateManagedWidget ("menu1",
   xmRowColumnWidgetClass,
   variableForm,
   XmNrowColumnType,   XmMENU_OPTION,
   XmNsubMenuId,       promptTextFields[i],
//   XmNx,               220,
   XmNtopAttachment,   XmATTACH_WIDGET,
   XmNtopWidget,      *topFieldWidget,
   XmNtopOffset,       5,
            XmNleftAttachment,  XmATTACH_WIDGET,
            XmNleftWidget,      promptLabel,
            XmNleftOffset,      5,
   NULL);
 UxPutContext( menu1, (char *) this);
       
 topFieldWidget = &menu1;
      }
      else
      {
 // Creation of promptTextField.  This will be a textField Widget.
 promptTextFields[i] =
   XtVaCreateManagedWidget ("promptTextField",
       xmTextFieldWidgetClass,
       variableForm,
       XmNheight,          31,
       XmNtopAttachment,   XmATTACH_WIDGET,
       XmNtopWidget,      *topFieldWidget,
       XmNtopOffset,       5,
       XmNleftAttachment,  XmATTACH_WIDGET,
                   XmNleftWidget,      promptLabel,
                   XmNleftOffset,      5,
       XmNrightAttachment, XmATTACH_FORM,
       XmNrightOffset,     10,
       NULL);
 topFieldWidget = &promptTextFields[i];
      }

      UxPutContext( promptTextFields[i], (char *) this);
      topLabelWidget = &promptLabel;
    } // for each prompt
  }   // end of if prompt[0] is defined.

  if (dialogState == OLD_DIALOG)
  {
    XtRealizeWidget (variableForm);
    XtManageChild (variableForm);
  }
}

/******************************************************************************
       END OF FILE
******************************************************************************/
