
/*******************************************************************************
 helpSelectionShell.cc

       Associated Header file: helpSelectionShell.h
*******************************************************************************/

#include <stdio.h>
#include <unistd.h>

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/MenuShell.h>
#endif /* MOTIF */

#include "UxXt.h"
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/BulletinB.h>
#include <X11/Shell.h>

/*******************************************************************************
       Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/

#include <errno.h>

#include "datatypes.h"

extern Boolean       webBrowserVisible;
extern PromptStruct  promptRecs[];

extern void    initializeHelpSystem ();
static void    setFocus (XtPointer, XtIntervalId);


#ifndef XKLOADDS
#define XKLOADDS
#endif /* XKLOADDS */

#include "helpSelectionShell.h"

Widget helpSelectionOkBtn;

/*******************************************************************************
Auxiliary code from the Declarations Editor:
*******************************************************************************/

static void setFocus (XtPointer callData, XtIntervalId *id)
{
  XmProcessTraversal (UxGetWidget (helpSelectionOkBtn),
               XmTRAVERSE_CURRENT);
}

/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

void  _UxChelpSelectionShell::popupCB_helpSelectionShell(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 AvailableHelp  availHelp;
 
 availHelp = promptRecs[serviceID].availableHelp;
 
 
 /* If a particular type of help is not available for this
    RTI service, desensitize the corresponding menu entry. */
 
 XtVaSetValues (UxGetWidget (specHelpItem),
                XmNsensitive,
                  (availHelp == IF_SPEC || availHelp == BOTH) ?
                  True : False,
                NULL);
 
 XtVaSetValues (UxGetWidget (APIhelpItem),
                XmNsensitive,
                  (availHelp == API || availHelp == BOTH) ?
                  True : False,
                NULL);
 
 XtVaSetValues (UxGetWidget (bothHelpItem),
                XmNsensitive,
                  (availHelp == BOTH) ? True : False,
                NULL);
 
 
 /* If no IF_SPEC help is available, we also have to change the
    default entry in the options menu to be the API help. */
 
 if ((availHelp != IF_SPEC) && (availHelp != BOTH))
   XtVaSetValues (UxGetWidget (menu1),
                  XmNmenuHistory, UxGetWidget (APIhelpItem),
                  NULL);
 
 /* Move the 'cursor' to the Ok button */
 XmProcessTraversal (UxGetWidget (helpSelectionOkBtn),
              XmTRAVERSE_CURRENT);
 
 }
}

void  _UxChelpSelectionShell::Wrap_popupCB_helpSelectionShell(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxChelpSelectionShell  *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxChelpSelectionShell *) _UxChelpSelectionShell::UxGetContext(UxWidget);
 UxContext->popupCB_helpSelectionShell(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxChelpSelectionShell::activateCB_helpSelectionOkBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
#define SIZE 256
 
 FILE       *htmlFile;
 char       *text;
 char        cmdTemplate[60];
 int         size = SIZE;
 char       *ptr;
 char       *cmdString;
 Widget      selectedHelpWidget;
 XmString    helpSelectionStr;
 
 
 if (!webBrowserVisible)
   initializeHelpSystem ();
 
 XtVaGetValues (UxGetWidget (helpTypesMenu),
         XmNmenuHistory, &selectedHelpWidget,
         NULL);
 
 XtVaGetValues (selectedHelpWidget,
         XmNlabelString, &helpSelectionStr,
                NULL);
 
 XmStringGetLtoR (helpSelectionStr,
                  XmFONTLIST_DEFAULT_TAG, &text);
 XmStringFree (helpSelectionStr);
 
#ifndef DESIGN_TIME
 
 // Determine the current working directory
 while (1)
 {
   ptr = new char[size];
   if (getcwd (ptr, size - 1) != NULL)
     break;  // success
   if (errno != ERANGE)
     break;
 
   delete [] ptr;
   size += SIZE;
 } 
 
 if (strcmp (text, "I/F Specification") == 0)
 {
   strcpy (cmdTemplate,
     "netscape -remote 'openFile(%s/IFSPEC10.html#%s)'");
   cmdString = new char[strlen (cmdTemplate) + strlen (ptr) +
               strlen (promptRecs[serviceID].helpTag) - 3];
   sprintf (cmdString, cmdTemplate, ptr, promptRecs[serviceID].helpTag);
 }
 
 else if (strcmp (text, "Interface API") == 0)
 {
   strcpy (cmdTemplate,
     "netscape -remote 'openFile(%s/API.html#%s)'");
   cmdString = new char[strlen (cmdTemplate) + strlen (ptr) +
               strlen (promptRecs[serviceID].helpTag) - 3];
   sprintf (cmdString, cmdTemplate, ptr, promptRecs[serviceID].helpTag);
 }
 else  /* both I/F Spec and API */
 {
   /* For this, we have to set up a special html file */
   htmlFile = fopen ("./testFederate.html", "w");
   fprintf (htmlFile, "<head><title> HLA </title></head>\n");
   fprintf (htmlFile, "<frameset rows=\"2*,*\">\n");
   fprintf (htmlFile, "  <frame src=\"./IFSPEC10.html#%s\">\n",
                      promptRecs[serviceID].helpTag);
   fprintf (htmlFile, "  <frame src=\"./API.html#%s\">\n",
                      promptRecs[serviceID].helpTag);
   fprintf (htmlFile, "</frameset>\n");
   fclose (htmlFile);
 
   strcpy (cmdTemplate,
     "netscape -remote 'openFile(%s/testFederate.html)'"); 
   cmdString = new char[strlen (cmdTemplate) + strlen (ptr) +
               strlen (promptRecs[serviceID].helpTag) - 1];
   sprintf (cmdString, cmdTemplate, ptr);
 }  
 
 XtFree (text);
 
 system (cmdString);
  
 delete [] ptr;
 delete [] cmdString;
 
#endif
 
 UxDestroyInterface (helpSelectionShell);
 }
}

void  _UxChelpSelectionShell::Wrap_activateCB_helpSelectionOkBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxChelpSelectionShell  *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxChelpSelectionShell *) _UxChelpSelectionShell::UxGetContext(UxWidget);
 UxContext->activateCB_helpSelectionOkBtn(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxChelpSelectionShell::activateCB_helpSelectionCancelBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 UxDestroyInterface (helpSelectionShell);
 }
}

void  _UxChelpSelectionShell::Wrap_activateCB_helpSelectionCancelBtn(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxChelpSelectionShell  *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxChelpSelectionShell *) _UxChelpSelectionShell::UxGetContext(UxWidget);
 UxContext->activateCB_helpSelectionCancelBtn(UxWidget, UxClientData, UxCallbackArg);
}

/*******************************************************************************
       The following is the destroyContext callback function.
       It is needed to free the memory allocated by the context.
*******************************************************************************/

static void DelayedDelete( XtPointer obj, XtIntervalId *)
{
 delete ((_UxChelpSelectionShell *) obj);
}
void  _UxChelpSelectionShell::UxDestroyContextCB(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;

 ((_UxChelpSelectionShell *) UxClientData)->UxThis = NULL;
 XtAppAddTimeOut(UxAppContext, 0, DelayedDelete, UxClientData);
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

Widget _UxChelpSelectionShell::_build()
{
 Widget  _UxParent;
 Widget  helpTypesMenu_shell;


 // Creation of helpSelectionShell
 _UxParent = UxParent;
 if ( _UxParent == NULL )
 {
  _UxParent = UxTopLevel;
 }

 helpSelectionShell = XtVaCreatePopupShell( "helpSelectionShell",
   applicationShellWidgetClass,
   _UxParent,
   XmNx, 403,
   XmNy, 218,
   XmNwidth, 327,
   XmNheight, 140,
   XmNtitle, "Help Selection",
   NULL );
 XtAddCallback( helpSelectionShell, XmNpopupCallback,
  (XtCallbackProc) &_UxChelpSelectionShell::Wrap_popupCB_helpSelectionShell,
  (XtPointer) NULL );

 UxPutContext( helpSelectionShell, (char *) this );
 UxThis = helpSelectionShell;



 // Creation of bulletinBoard
 bulletinBoard = XtVaCreateManagedWidget( "bulletinBoard",
   xmBulletinBoardWidgetClass,
   helpSelectionShell,
   XmNresizePolicy, XmRESIZE_NONE,
   XmNunitType, XmPIXELS,
   XmNx, 62,
   XmNy, 16,
   XmNwidth, 327,
   XmNheight, 140,
   XmNmarginWidth, 0,
   NULL );
 UxPutContext( bulletinBoard, (char *) this );


 // Creation of helpOptionsBoxLabel
 helpOptionsBoxLabel = XtVaCreateManagedWidget( "helpOptionsBoxLabel",
   xmLabelWidgetClass,
   bulletinBoard,
   XmNx, 19,
   XmNy, 23,
   XmNwidth, 74,
   XmNheight, 35,
   XmNalignment, XmALIGNMENT_BEGINNING,
   RES_CONVERT( XmNlabelString, "Help Type:" ),
   NULL );
 UxPutContext( helpOptionsBoxLabel, (char *) this );


 // Creation of helpTypesMenu
 helpTypesMenu_shell = XtVaCreatePopupShell ("helpTypesMenu_shell",
   xmMenuShellWidgetClass, bulletinBoard,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 helpTypesMenu = XtVaCreateWidget( "helpTypesMenu",
   xmRowColumnWidgetClass,
   helpTypesMenu_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   XmNx, 0,
   XmNy, -9,
   NULL );
 UxPutContext( helpTypesMenu, (char *) this );


 // Creation of specHelpItem
 specHelpItem = XtVaCreateManagedWidget( "specHelpItem",
   xmPushButtonWidgetClass,
   helpTypesMenu,
   RES_CONVERT( XmNlabelString, "I/F Specification" ),
   XmNx, 2,
   XmNy, 37,
   NULL );
 UxPutContext( specHelpItem, (char *) this );


 // Creation of APIhelpItem
 APIhelpItem = XtVaCreateManagedWidget( "APIhelpItem",
   xmPushButtonWidgetClass,
   helpTypesMenu,
   RES_CONVERT( XmNlabelString, "Interface API" ),
   XmNx, 2,
   XmNy, 37,
   NULL );
 UxPutContext( APIhelpItem, (char *) this );


 // Creation of bothHelpItem
 bothHelpItem = XtVaCreateManagedWidget( "bothHelpItem",
   xmPushButtonWidgetClass,
   helpTypesMenu,
   RES_CONVERT( XmNlabelString, "Specification & API" ),
   XmNx, 2,
   XmNy, 37,
   NULL );
 UxPutContext( bothHelpItem, (char *) this );


 // Creation of menu1
 menu1 = XtVaCreateManagedWidget( "menu1",
   xmRowColumnWidgetClass,
   bulletinBoard,
   XmNrowColumnType, XmMENU_OPTION,
   XmNsubMenuId, helpTypesMenu,
   XmNx, 149,
   XmNy, 23,
   NULL );
 UxPutContext( menu1, (char *) this );


 // Creation of separator
 separator = XtVaCreateManagedWidget( "separator",
   xmSeparatorWidgetClass,
   bulletinBoard,
   XmNx, 0,
   XmNy, 80,
   XmNwidth, 330,
   NULL );
 UxPutContext( separator, (char *) this );


 // Creation of helpSelectionOkBtn
 helpSelectionOkBtn = XtVaCreateManagedWidget( "helpSelectionOkBtn",
   xmPushButtonWidgetClass,
   bulletinBoard,
   XmNx, 58,
   XmNy, 99,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Ok" ),
   NULL );
 XtAddCallback( helpSelectionOkBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxChelpSelectionShell::Wrap_activateCB_helpSelectionOkBtn,
  (XtPointer) NULL );

 UxPutContext( helpSelectionOkBtn, (char *) this );


 // Creation of helpSelectionCancelBtn
 helpSelectionCancelBtn = XtVaCreateManagedWidget( "helpSelectionCancelBtn",
   xmPushButtonWidgetClass,
   bulletinBoard,
   XmNx, 200,
   XmNy, 99,
   XmNwidth, 70,
   XmNheight, 26,
   RES_CONVERT( XmNlabelString, "Cancel" ),
   NULL );
 XtAddCallback( helpSelectionCancelBtn, XmNactivateCallback,
  (XtCallbackProc) &_UxChelpSelectionShell::Wrap_activateCB_helpSelectionCancelBtn,
  (XtPointer) NULL );

 UxPutContext( helpSelectionCancelBtn, (char *) this );


 XtAddCallback( helpSelectionShell, XmNdestroyCallback,
  (XtCallbackProc) &_UxChelpSelectionShell::UxDestroyContextCB,
  (XtPointer) this);


 return ( helpSelectionShell );
}

/*******************************************************************************
       The following function includes the code that was entered
       in the 'Initial Code' and 'Final Code' sections of the
       Declarations Editor. This function is called from the
       'Interface function' below.
*******************************************************************************/

swidget _UxChelpSelectionShell::_create_helpSelectionShell(void)
{
 Widget                  rtrn;
 UxThis = rtrn = _build();

 // Final Code from declarations editor
 /* Move the cursor focus to the Ok button */
 XtAppAddTimeOut (UxAppContext, 0, setFocus, NULL);
 
 return(rtrn);
}

/*******************************************************************************
       The following is the destructor function.
*******************************************************************************/

_UxChelpSelectionShell::~_UxChelpSelectionShell()
{
 if (this->UxThis)
 {
  XtRemoveCallback( UxGetWidget(this->UxThis),
   XmNdestroyCallback,
   (XtCallbackProc) &_UxChelpSelectionShell::UxDestroyContextCB,
   (XtPointer) this);
  UxDestroyInterface(this->UxThis);
 }
}

/*******************************************************************************
       The following is the constructor function.
*******************************************************************************/

_UxChelpSelectionShell::_UxChelpSelectionShell( swidget UxParent, ServiceID serviceID )
{
 this->UxParent = UxParent;
 this->serviceID = serviceID;

 // User Supplied Constructor Code
}

/*******************************************************************************
       The following is the 'Interface function' which is the
       external entry point for creating this interface.
       This function should be called from your application or from
       a callback function.
*******************************************************************************/

swidget create_helpSelectionShell( swidget UxParent, ServiceID serviceID )
{
 _UxChelpSelectionShell *theInterface = 
   new _UxChelpSelectionShell( UxParent, serviceID );
 return (theInterface->_create_helpSelectionShell());
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

