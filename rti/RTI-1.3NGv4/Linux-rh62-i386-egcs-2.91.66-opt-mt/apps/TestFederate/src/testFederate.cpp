/*------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 *  Copyright 1991, Visual Edge Software Ltd.
 *
 * ALL  RIGHTS  RESERVED.  Permission  to  use,  copy,  modify,  and
 * distribute  this  software  and its documentation for any purpose
 * and  without  fee  is  hereby  granted,  provided  that the above
 * copyright  notice  appear  in  all  copies  and  that  both  that
 * copyright  notice and this permission notice appear in supporting
 * documentation,  and that  the name of Visual Edge Software not be
 * used  in advertising  or publicity  pertaining to distribution of
 * the software without specific, written prior permission. The year
 * included in the notice is the year of the creation of the work.
 *------------------------------------------------------------------------------
 * This is the project main program file for Xt generated code.
 * You may add application dependent source code at the appropriate places.
 *
 * Do not modify the statements preceded by the dollar sign ($), these
 * statements will be replaced with the appropriate source code when the
 * main program is automatically generated.
 *----------------------------------------------------------------------------*/

#ifdef XOPEN_CATALOG
#include <locale.h>
#endif

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>

#include <signal.h>
/*-------------------------------------------------- 
 * UxXt.h needs to be included only when compiling
 * a stand-alone application. 
 *-------------------------------------------------*/
#ifndef DESIGN_TIME
#include "UxXt.h"
#endif /* DESIGN_TIME */

XtAppContext UxAppContext;
Widget  UxTopLevel;
Display  *UxDisplay;
int  UxScreen;
#ifdef UX_CATALOG
nl_catd  UxMsgCatalog;
#endif /* UX_CATALOG */

/*----------------------------------------------
 * Insert application global declarations here
 *---------------------------------------------*/
extern void handleWebBrowserShutdownSignal (int sig);


#ifdef _NO_PROTO
int main(argc,argv)
 int argc;
 char *argv[];
#else
int main(int argc, char *argv[])
#endif /* _NO_PROTO */
{
 /*-----------------------------------------------------------
  * Declarations.
  * The default identifier - mainIface will only be declared
  * if the interface function is global and of type swidget.
  * To change the identifier to a different name, modify the
  * string mainIface in the file "xtmain.dat". If "mainIface"
  * is declared, it will be used below where the return value
  * of PJ_INTERFACE_FUNCTION_CALL will be assigned to it.
  *----------------------------------------------------------*/

 Widget mainIface;

 /*---------------------------------
  * Interface function declaration
  *--------------------------------*/ 

 Widget  create_testFederateShell(swidget);

 swidget UxParent = NULL;


 /*---------------------
  * Initialize program
  *--------------------*/

#ifdef XOPEN_CATALOG
 setlocale(LC_ALL, "");
 if (XSupportsLocale())
 {
  XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);
 }
#endif

#ifdef UX_CATALOG

#if defined(SOLARIS)
 /* Ensure NLSPATH has default value if not set. */
 if (!getenv("NLSPATH"))
 {
  putenv("NLSPATH=./%N");
 }
#endif
 UxMsgCatalog = UxCATOPEN(UX_CATALOG_NAME, 0);
#endif

        /* Added by JCH to deal with web browser shutdown */
        signal (SIGCHLD, handleWebBrowserShutdownSignal);

 UxTopLevel = XtAppInitialize(&UxAppContext, "testFederate",
         NULL, 0, &argc, argv, NULL, NULL, 0);

 UxDisplay = XtDisplay(UxTopLevel);
 UxScreen = XDefaultScreen(UxDisplay);

 /* We set the geometry of UxTopLevel so that dialogShells
    that are parented on it will get centered on the screen
    (if defaultPosition is true). */

 XtVaSetValues(UxTopLevel,
   XtNx, 0,
   XtNy, 0,
   XtNwidth, DisplayWidth(UxDisplay, UxScreen),
   XtNheight, DisplayHeight(UxDisplay, UxScreen),
   NULL);

 /*-------------------------------------------------------
  * Insert initialization code for your application here
  *------------------------------------------------------*/
 

 /*----------------------------------------------------------------
  * Create and popup the first window of the interface. The
  * return value can be used in the popdown or destroy functions.
  * The Widget return value of PJ_INTERFACE_FUNCTION_CALL will
  * be assigned to "mainIface" from PJ_INTERFACE_RETVAL_TYPE.
  *---------------------------------------------------------------*/

 mainIface = create_testFederateShell(UxParent);

 Interface_UxManage(mainIface, &UxEnv);

 /*-----------------------
  * Enter the event loop 
  *----------------------*/

 XtAppMainLoop(UxAppContext);

}
