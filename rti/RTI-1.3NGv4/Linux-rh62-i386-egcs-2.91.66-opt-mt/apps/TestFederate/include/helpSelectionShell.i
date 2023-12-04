! UIMX ascii 2.9 key: 2494                                                      

*helpSelectionShell.class: applicationShell
*helpSelectionShell.classinc:
*helpSelectionShell.classspec:
*helpSelectionShell.classmembers:
*helpSelectionShell.classconstructor:
*helpSelectionShell.classdestructor:
*helpSelectionShell.gbldecl: #include <stdio.h>\
#include <errno.h>\
\
#include "datatypes.h"\
\
extern Boolean       webBrowserVisible;\
extern PromptStruct  promptRecs[];\
\
extern void    initializeHelpSystem ();\
static void    setFocus (XtPointer, XtIntervalId);\

*helpSelectionShell.ispecdecl:
*helpSelectionShell.funcdecl: swidget create_helpSelectionShell(swidget UxParent, ServiceID serviceID)
*helpSelectionShell.funcname: create_helpSelectionShell
*helpSelectionShell.funcdef: "swidget", "<create_helpSelectionShell>(%)"
*helpSelectionShell.argdecl: swidget UxParent;\
ServiceID serviceID;
*helpSelectionShell.arglist: UxParent, serviceID
*helpSelectionShell.arglist.UxParent: "swidget", "%UxParent%"
*helpSelectionShell.arglist.serviceID: "ServiceID", "%serviceID%"
*helpSelectionShell.icode:
*helpSelectionShell.fcode: /* Move the cursor focus to the Ok button */\
XtAppAddTimeOut (UxAppContext, 0, setFocus, NULL);\
\
return(rtrn);\

*helpSelectionShell.auxdecl: static void setFocus (XtPointer callData, XtIntervalId *id)\
{\
  XmProcessTraversal (UxGetWidget (helpSelectionOkBtn),\
               XmTRAVERSE_CURRENT);\
}
*helpSelectionShell.static: true
*helpSelectionShell.name: helpSelectionShell
*helpSelectionShell.parent: NO_PARENT
*helpSelectionShell.parentExpression: UxParent
*helpSelectionShell.x: 403
*helpSelectionShell.y: 218
*helpSelectionShell.width: 327
*helpSelectionShell.height: 140
*helpSelectionShell.title: "Help Selection"
*helpSelectionShell.popupCallback: {\
AvailableHelp  availHelp;\
\
availHelp = promptRecs[serviceID].availableHelp;\
\
\
/* If a particular type of help is not available for this\
   RTI service, desensitize the corresponding menu entry. */\
\
XtVaSetValues (UxGetWidget (specHelpItem),\
               XmNsensitive,\
                 (availHelp == IF_SPEC || availHelp == BOTH) ?\
                 True : False,\
               NULL);\
\
XtVaSetValues (UxGetWidget (APIhelpItem),\
               XmNsensitive,\
                 (availHelp == API || availHelp == BOTH) ?\
                 True : False,\
               NULL);\
\
XtVaSetValues (UxGetWidget (bothHelpItem),\
               XmNsensitive,\
                 (availHelp == BOTH) ? True : False,\
               NULL);\
\
\
/* If no IF_SPEC help is available, we also have to change the\
   default entry in the options menu to be the API help. */\
\
if ((availHelp != IF_SPEC) && (availHelp != BOTH))\
  XtVaSetValues (UxGetWidget (menu1),\
                 XmNmenuHistory, UxGetWidget (APIhelpItem),\
                 NULL);\
\
/* Move the 'cursor' to the Ok button */\
XmProcessTraversal (UxGetWidget (helpSelectionOkBtn),\
             XmTRAVERSE_CURRENT);\
\
}

*bulletinBoard.class: bulletinBoard
*bulletinBoard.static: true
*bulletinBoard.name: bulletinBoard
*bulletinBoard.parent: helpSelectionShell
*bulletinBoard.resizePolicy: "resize_none"
*bulletinBoard.unitType: "pixels"
*bulletinBoard.x: 62
*bulletinBoard.y: 16
*bulletinBoard.width: 327
*bulletinBoard.height: 140
*bulletinBoard.marginWidth: 0

*helpOptionsBoxLabel.class: label
*helpOptionsBoxLabel.static: true
*helpOptionsBoxLabel.name: helpOptionsBoxLabel
*helpOptionsBoxLabel.parent: bulletinBoard
*helpOptionsBoxLabel.x: 19
*helpOptionsBoxLabel.y: 23
*helpOptionsBoxLabel.width: 74
*helpOptionsBoxLabel.height: 35
*helpOptionsBoxLabel.alignment: "alignment_beginning"
*helpOptionsBoxLabel.labelString: "Help Type:"

*menu1.class: rowColumn
*menu1.static: true
*menu1.name: menu1
*menu1.parent: bulletinBoard
*menu1.rowColumnType: "menu_option"
*menu1.subMenuId: "helpTypesMenu"
*menu1.x: 149
*menu1.y: 23

*helpTypesMenu.class: rowColumn
*helpTypesMenu.static: true
*helpTypesMenu.name: helpTypesMenu
*helpTypesMenu.parent: menu1
*helpTypesMenu.rowColumnType: "menu_pulldown"
*helpTypesMenu.x: 0
*helpTypesMenu.y: -9

*specHelpItem.class: pushButton
*specHelpItem.static: true
*specHelpItem.name: specHelpItem
*specHelpItem.parent: helpTypesMenu
*specHelpItem.labelString: "I/F Specification"
*specHelpItem.x: 2
*specHelpItem.y: 37

*APIhelpItem.class: pushButton
*APIhelpItem.static: true
*APIhelpItem.name: APIhelpItem
*APIhelpItem.parent: helpTypesMenu
*APIhelpItem.labelString: "Interface API"
*APIhelpItem.x: 2
*APIhelpItem.y: 37

*bothHelpItem.class: pushButton
*bothHelpItem.static: true
*bothHelpItem.name: bothHelpItem
*bothHelpItem.parent: helpTypesMenu
*bothHelpItem.labelString: "Specification & API"
*bothHelpItem.x: 2
*bothHelpItem.y: 37
*bothHelpItem.activateCallback.source: public
*bothHelpItem.activateCallback: 

*separator.class: separator
*separator.static: true
*separator.name: separator
*separator.parent: bulletinBoard
*separator.x: 0
*separator.y: 80
*separator.width: 330

*helpSelectionOkBtn.class: pushButton
*helpSelectionOkBtn.name.source: public
*helpSelectionOkBtn.static: false
*helpSelectionOkBtn.name: helpSelectionOkBtn
*helpSelectionOkBtn.parent: bulletinBoard
*helpSelectionOkBtn.x: 58
*helpSelectionOkBtn.y: 99
*helpSelectionOkBtn.width: 70
*helpSelectionOkBtn.height: 26
*helpSelectionOkBtn.labelString: "Ok"
*helpSelectionOkBtn.activateCallback: {\
#define SIZE 256\
\
FILE       *htmlFile;\
char       *text;\
char        cmdTemplate[60];\
int         size = SIZE;\
char       *ptr;\
char       *cmdString;\
Widget      selectedHelpWidget;\
XmString    helpSelectionStr;\
\
\
if (!webBrowserVisible)\
  initializeHelpSystem ();\
\
XtVaGetValues (UxGetWidget (helpTypesMenu),\
        XmNmenuHistory, &selectedHelpWidget,\
        NULL);\
\
XtVaGetValues (selectedHelpWidget,\
        XmNlabelString, &helpSelectionStr,\
               NULL);\
\
XmStringGetLtoR (helpSelectionStr,\
                 XmFONTLIST_DEFAULT_TAG, &text);\
XmStringFree (helpSelectionStr);\
\
#ifndef DESIGN_TIME\
\
// Determine the current working directory\
while (1)\
{\
  ptr = new char[size];\
  if (getcwd (ptr, size - 1) != NULL)\
    break;  // success\
  if (errno != ERANGE)\
    break;\
\
  delete [] ptr;\
  size += SIZE;\
} \
\
if (strcmp (text, "I/F Specification") == 0)\
{\
  strcpy (cmdTemplate,\
    "netscape -remote 'openFile(%s/IFSPEC10.html#%s)'");\
  cmdString = new char[strlen (cmdTemplate) + strlen (ptr) +\
              strlen (promptRecs[serviceID].helpTag) - 3];\
  sprintf (cmdString, cmdTemplate, ptr, promptRecs[serviceID].helpTag);\
}\
\
else if (strcmp (text, "Interface API") == 0)\
{\
  strcpy (cmdTemplate,\
    "netscape -remote 'openFile(%s/API.html#%s)'");\
  cmdString = new char[strlen (cmdTemplate) + strlen (ptr) +\
              strlen (promptRecs[serviceID].helpTag) - 3];\
  sprintf (cmdString, cmdTemplate, ptr, promptRecs[serviceID].helpTag);\
}\
else  /* both I/F Spec and API */\
{\
  /* For this, we have to set up a special html file */\
  htmlFile = fopen ("./testFederate.html", "w");\
  fprintf (htmlFile, "<head><title> HLA </title></head>\n");\
  fprintf (htmlFile, "<frameset rows=\"2*,*\">\n");\
  fprintf (htmlFile, "  <frame src=\"./IFSPEC10.html#%s\">\n",\
                     promptRecs[serviceID].helpTag);\
  fprintf (htmlFile, "  <frame src=\"./API.html#%s\">\n",\
                     promptRecs[serviceID].helpTag);\
  fprintf (htmlFile, "</frameset>\n");\
  fclose (htmlFile);\
\
  strcpy (cmdTemplate,\
    "netscape -remote 'openFile(%s/testFederate.html)'"); \
  cmdString = new char[strlen (cmdTemplate) + strlen (ptr) +\
              strlen (promptRecs[serviceID].helpTag) - 1];\
  sprintf (cmdString, cmdTemplate, ptr);\
}  \
\
XtFree (text);\
\
system (cmdString);\
 \
delete [] ptr;\
delete [] cmdString;\
\
#endif\
\
UxDestroyInterface (helpSelectionShell);\
}

*helpSelectionCancelBtn.class: pushButton
*helpSelectionCancelBtn.static: true
*helpSelectionCancelBtn.name: helpSelectionCancelBtn
*helpSelectionCancelBtn.parent: bulletinBoard
*helpSelectionCancelBtn.x: 200
*helpSelectionCancelBtn.y: 99
*helpSelectionCancelBtn.width: 70
*helpSelectionCancelBtn.height: 26
*helpSelectionCancelBtn.labelString: "Cancel"
*helpSelectionCancelBtn.activateCallback: {\
UxDestroyInterface (helpSelectionShell);\
}

