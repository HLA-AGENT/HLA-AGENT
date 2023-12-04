! UIMX ascii 2.9 key: 5886                                                      

*genericPromptDialog.class: applicationShell
*genericPromptDialog.classinc:
*genericPromptDialog.classspec:
*genericPromptDialog.classmembers:
*genericPromptDialog.classconstructor:
*genericPromptDialog.classdestructor:
*genericPromptDialog.gbldecl: #include <stdio.h>\
#include <sys/types.h>\
#include <Xm/VendorS.h>\
\
#include "datatypes.h"\
\
\
extern swidget create_helpSelectionShell (swidget, ServiceID);\
extern PromptStruct  promptRecs[];\
\
void scheduleNextTick (char *);\
void nextTick (XtPointer, XtIntervalId);\
extern void do_autoTick (char **, int);
*genericPromptDialog.ispecdecl:
*genericPromptDialog.funcdecl: swidget create_genericPromptDialog(swidget UxParent, ServiceID serviceID)
*genericPromptDialog.funcname: create_genericPromptDialog
*genericPromptDialog.funcdef: "swidget", "<create_genericPromptDialog>(%)"
*genericPromptDialog.argdecl: swidget UxParent;\
ServiceID serviceID;
*genericPromptDialog.arglist: UxParent, serviceID
*genericPromptDialog.arglist.UxParent: "swidget", "%UxParent%"
*genericPromptDialog.arglist.serviceID: "ServiceID", "%serviceID%"
*genericPromptDialog.icode:
*genericPromptDialog.fcode: return(rtrn);\

*genericPromptDialog.auxdecl: void scheduleNextTick (char *intervalStr)\
{\
  XtAppAddTimeOut (UxAppContext, atoi (intervalStr),\
                   nextTick, (XtPointer)intervalStr);\
}\
\
void nextTick (XtPointer clientData, XtIntervalId id)\
{\
  char *intervalStr = (char *)clientData;\
\
  do_autoTick (&intervalStr, CONTINUE);\
}
*genericPromptDialog.static: true
*genericPromptDialog.name: genericPromptDialog
*genericPromptDialog.parent: NO_PARENT
*genericPromptDialog.parentExpression: UxParent
*genericPromptDialog.x: 385
*genericPromptDialog.y: 163
*genericPromptDialog.width: 338
*genericPromptDialog.title: "My Title"
*genericPromptDialog.allowShellResize: "true"
*genericPromptDialog.minWidth: 325

*genericPromptForm.class: form
*genericPromptForm.static: true
*genericPromptForm.name: genericPromptForm
*genericPromptForm.parent: genericPromptDialog

*instructionsLabel.class: label
*instructionsLabel.static: true
*instructionsLabel.name: instructionsLabel
*instructionsLabel.parent: genericPromptForm
*instructionsLabel.x: 9
*instructionsLabel.y: 16
*instructionsLabel.width: 294
*instructionsLabel.height: 35
*instructionsLabel.alignment: "alignment_beginning"
*instructionsLabel.labelString: "Please enter the following values:"

*separator1.class: separator
*separator1.static: true
*separator1.name: separator1
*separator1.parent: genericPromptForm
*separator1.y: 59
*separator1.rightAttachment: "attach_form"
*separator1.leftAttachment: "attach_form"

*extendedModeBtn.class: toggleButton
*extendedModeBtn.static: true
*extendedModeBtn.name: extendedModeBtn
*extendedModeBtn.parent: genericPromptForm
*extendedModeBtn.x: 5
*extendedModeBtn.y: 67
*extendedModeBtn.width: 251
*extendedModeBtn.height: 20
*extendedModeBtn.leftOffset: 0
*extendedModeBtn.topAttachment: "attach_widget"
*extendedModeBtn.topOffset: 5
*extendedModeBtn.topWidget: "separator1"
*extendedModeBtn.alignment: "alignment_beginning"
*extendedModeBtn.labelString: "Use Extended Mode"
*extendedModeBtn.marginLeft: 17
*extendedModeBtn.marginWidth: 2
*extendedModeBtn.visibleWhenOff: "true"
*extendedModeBtn.sensitive: "false"
*extendedModeBtn.valueChangedCallback: {\
XmToggleButtonCallbackStruct *btn =\
  (XmToggleButtonCallbackStruct *)UxCallbackArg;\
\
if (btn->set)\
  printf ("Button is set.\n");\
else\
  printf ("Button is unset.\n");\
}

*variableForm.class: form
*variableForm.static: true
*variableForm.name: variableForm
*variableForm.parent: genericPromptForm
*variableForm.topAttachment: "attach_widget"
*variableForm.topOffset: 7
*variableForm.topWidget: "extendedModeBtn"
*variableForm.rightAttachment: "attach_form"
*variableForm.leftAttachment: "attach_form"
*variableForm.leftOffset: 1
*variableForm.rightOffset: 1

*placeHolder.class: label
*placeHolder.static: true
*placeHolder.name: placeHolder
*placeHolder.parent: variableForm
*placeHolder.width: 1
*placeHolder.height: 1
*placeHolder.topAttachment: "attach_form"
*placeHolder.leftAttachment: "attach_form"
*placeHolder.labelString: ""

*promptLabel.class: label
*promptLabel.static: true
*promptLabel.name: promptLabel
*promptLabel.parent: variableForm
*promptLabel.x: 9
*promptLabel.width: 219
*promptLabel.height: 31
*promptLabel.labelString: "Interaction Class Handle (or Name):"
*promptLabel.alignment: "alignment_beginning"
*promptLabel.topOffset: 5
*promptLabel.topAttachment: "attach_widget"
*promptLabel.topWidget: "placeHolder"

*promptTextField.class: textField
*promptTextField.static: true
*promptTextField.name: promptTextField
*promptTextField.parent: variableForm
*promptTextField.x: 232
*promptTextField.width: 92
*promptTextField.height: 31
*promptTextField.topAttachment: "attach_widget"
*promptTextField.topOffset: 5
*promptTextField.topWidget: "placeHolder"
*promptTextField.y: 6

*separator3.class: separator
*separator3.static: true
*separator3.name: separator3
*separator3.parent: genericPromptForm
*separator3.rightAttachment: "attach_form"
*separator3.leftAttachment: "attach_form"

*promptDialogOkBtn.class: pushButton
*promptDialogOkBtn.static: true
*promptDialogOkBtn.name: promptDialogOkBtn
*promptDialogOkBtn.parent: genericPromptForm
*promptDialogOkBtn.x: 17
*promptDialogOkBtn.y: 281
*promptDialogOkBtn.width: 70
*promptDialogOkBtn.height: 26
*promptDialogOkBtn.labelString: "Ok"

*promptDialogCancelBtn.class: pushButton
*promptDialogCancelBtn.static: true
*promptDialogCancelBtn.name: promptDialogCancelBtn
*promptDialogCancelBtn.parent: genericPromptForm
*promptDialogCancelBtn.x: 129
*promptDialogCancelBtn.y: 281
*promptDialogCancelBtn.width: 70
*promptDialogCancelBtn.height: 26
*promptDialogCancelBtn.labelString: "Cancel"
*promptDialogCancelBtn.activateCallback: {\
UxDestroyInterface (genericPromptDialog);\
}
*promptDialogCancelBtn.bottomAttachment: "attach_form"
*promptDialogCancelBtn.bottomOffset: 20
*promptDialogCancelBtn.topAttachment: "attach_none"

*promptDialogHelpBtn.class: pushButton
*promptDialogHelpBtn.static: true
*promptDialogHelpBtn.name: promptDialogHelpBtn
*promptDialogHelpBtn.parent: genericPromptForm
*promptDialogHelpBtn.x: 245
*promptDialogHelpBtn.y: 281
*promptDialogHelpBtn.width: 70
*promptDialogHelpBtn.height: 26
*promptDialogHelpBtn.labelString: "Help"
*promptDialogHelpBtn.bottomAttachment: "attach_form"
*promptDialogHelpBtn.bottomOffset: 20
*promptDialogHelpBtn.topAttachment: "attach_none"
*promptDialogHelpBtn.activateCallback: {\
UxPopupInterface (create_helpSelectionShell (NULL, serviceID), no_grab);\
\
}
*promptDialogHelpBtn.sensitive: (promptRecs[serviceID].availableHelp != NONE) ? "true" : "false"

*placeHolder2.class: label
*placeHolder2.static: true
*placeHolder2.name: placeHolder2
*placeHolder2.parent: genericPromptForm
*placeHolder2.width: 1
*placeHolder2.height: 1
*placeHolder2.labelString: ""
*placeHolder2.topAttachment: "attach_widget"
*placeHolder2.topOffset: 20
*placeHolder2.topWidget: "promptDialogOkBtn"

