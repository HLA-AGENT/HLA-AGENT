/*------------------------------------------------------------------------------
 * $Date: 1998/05/06 19:11:19 $   $Revision: 2.7
 *------------------------------------------------------------------------------
 *  Copyright (c) 1991, Visual Edge Software Ltd.
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
 * This file defines the base class from which interfaces are derived.
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus

#ifdef XT_CODE
#       include "UxXt.h"
#else /* XT_CODE */
#       include "UxLib.h"
#endif /* XT_CODE */

/*------------------------------------------------------------------------------
 * NAME: childSite
 * RETURNS: NULL
 * DESCRIPTION: Default childSite method for interface base class.
 *----------------------------------------------------------------------------*/
swidget _UxCInterface::childSite (Environment * pEnv) 
{
 if (pEnv) {
  pEnv->_major = NO_EXCEPTION;
 }
 return 0;
}

/*------------------------------------------------------------------------------
 * NAME: UxChildSite
 * INPUT: swidget sw
 * DESCRIPTION: Gets the designatedChildSite by calling the childSite method.
 *  If the childSite itself is a component, we recur in it to ensure
 *  that the child is placed correctly by its rules, and so on.
 *----------------------------------------------------------------------------*/
swidget _UxCInterface::UxChildSite (swidget proposedParent)
{
 _UxCInterface *ppIface;

 if (!proposedParent) {
  return NULL;
 }

 ppIface = (_UxCInterface *) UxGetContext (proposedParent);
 if (ppIface && (ppIface != this)) {
  swidget preferredParent = ppIface->childSite(&UxEnv);
  if (preferredParent) {
   return ppIface->UxChildSite(preferredParent);
  }
 }
 return proposedParent;
}

/*------------------------------------------------------------------------------
 * Interface methods for geometry.
 * These are the default methods for normal generated components;
 * they may be overridden for components not made in UIM/X.
 *----------------------------------------------------------------------------*/

int _UxCInterface::_get_x(Environment*)
{
 Position x;
 XtVaGetValues(UxRealWidget(UxThis), XmNx, &x, NULL);
 return x;
}

void _UxCInterface::_set_x(Environment*, int x)
{
 XtVaSetValues(UxRealWidget(UxThis), XmNx, x, NULL);
}

int _UxCInterface::_get_y(Environment*)
{
 Position y;
 XtVaGetValues(UxRealWidget(UxThis), XmNy, &y, NULL);
 return y;
}

void _UxCInterface::_set_y(Environment*, int y)
{
 XtVaSetValues(UxRealWidget(UxThis), XmNy, y, NULL);
}

int _UxCInterface::_get_width(Environment*)
{
 Dimension width;
 XtVaGetValues(UxRealWidget(UxThis), XmNwidth, &width, NULL);
 return width;
}

void _UxCInterface::_set_width(Environment*, int width)
{
 XtVaSetValues(UxRealWidget(UxThis), XmNwidth, width, NULL);
}

int _UxCInterface::_get_height(Environment*)
{
 Dimension height;
 XtVaGetValues(UxRealWidget(UxThis), XmNheight, &height, NULL);
 return height;
}

void _UxCInterface::_set_height(Environment*, int height)
{
 XtVaSetValues(UxRealWidget(UxThis), XmNheight, height, NULL);
}


/*------------------------------------------------------------------------------
 * NAME: UxManage
 * DESCRIPTION: Manage this interface
 * This method is called after the interface is constructed and initialized,
 * unless the interface was defined to be created unmanaged.
 * Its makes the interface visible.
 *----------------------------------------------------------------------------*/
void _UxCInterface::UxManage(Environment*)
{
 Widget  w = UxGetWidget(UxThis);
 Widget rw = UxRealWidget(UxThis);

   if (w && XtIsComposite(XtParent(w))) {
  XtManageChild(w);
 }
 if (rw && XtIsSubclass(rw, shellWidgetClass)) {
        XtPopup(rw, XtGrabNone);
 }
}

#endif /* __cplusplus */
