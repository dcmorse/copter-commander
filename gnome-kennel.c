/* This code courtesey of:
 * BEAST - Bedevilled Audio System
 * Copyright (C) 1998, 1999, 2000 Tim Janik and Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>
#include "gnome-kennel.h"

/* --- Gtk+ Kennel --- */
struct _coco_GtkKennel
{
  coco_GtkKennelType	width_constraint : 8;
  coco_GtkKennelType	height_constraint : 8;
  guint         resize_handler;
  GtkWidget    *cwidget;
  guint         width;
  guint         height;
  guint         ref_count;
  GSList       *children;
};
typedef struct
{
  GtkWidget     *widget;
  GtkRequisition real_requisition;
} KennelChild;

coco_GtkKennel*
coco_gtk_kennel_new (coco_GtkKennelType width_constrain,
                     coco_GtkKennelType height_constrain)
{
  coco_GtkKennel *kennel = g_new0 (coco_GtkKennel, 1);

  kennel->ref_count = 1;
  coco_gtk_kennel_configure (kennel, width_constrain, height_constrain);

  return kennel;
}

coco_GtkKennel*
coco_gtk_kennel_ref (coco_GtkKennel *kennel)
{
  g_return_val_if_fail (kennel != NULL, NULL);
  g_return_val_if_fail (kennel->ref_count > 0, NULL);

  kennel->ref_count += 1;

  return kennel;
}

void
coco_gtk_kennel_unref (coco_GtkKennel *kennel)
{
  g_return_if_fail (kennel != NULL);
  g_return_if_fail (kennel->ref_count > 0);

  kennel->ref_count -= 1;
  if (!kennel->ref_count)
    {
      if (kennel->resize_handler)
	g_source_remove (kennel->resize_handler);
      if (kennel->cwidget)
	gtk_widget_unref (kennel->cwidget);
      g_free (kennel);
    }
}

void
coco_gtk_kennel_configure (coco_GtkKennel    *kennel,
		      coco_GtkKennelType width_constrain,
		      coco_GtkKennelType height_constrain)
{
  g_return_if_fail (kennel != NULL);
  g_return_if_fail (kennel->ref_count > 0);

  width_constrain = CLAMP (width_constrain, 0, COCO_GTK_KENNEL_TO_WIDGET);
  height_constrain = CLAMP (height_constrain, 0, COCO_GTK_KENNEL_TO_WIDGET);
  if (kennel->width_constraint != width_constrain ||
      kennel->height_constraint != height_constrain)
    {
      kennel->width_constraint = width_constrain;
      kennel->height_constraint = height_constrain;
      coco_gtk_kennel_resize (kennel, kennel->width, kennel->height);
    }
}

void
coco_gtk_kennel_resize (coco_GtkKennel *kennel,
		   guint      width,
		   guint      height)
{
  GSList *slist;

  g_return_if_fail (kennel != NULL);
  g_return_if_fail (kennel->ref_count > 0);

  kennel->width = width;
  kennel->height = height;
  for (slist = kennel->children; slist; slist = slist->next)
    {
      KennelChild *child = slist->data;
      GtkRequisition *requisition = &child->widget->requisition;

      if ((kennel->width_constraint && requisition->width != kennel->width) ||
	  (kennel->height_constraint && requisition->height != kennel->height))
	gtk_widget_queue_resize (child->widget);
    }
}

static void coco_kennel_widget_size_request (GtkWidget      *widget,
					GtkRequisition *requisition,
					coco_GtkKennel      *kennel);

static gboolean
coco_kennel_idle_sizer (gpointer data)
{
  coco_GtkKennel *kennel = data;

  kennel->resize_handler = 0;

  if (kennel->width_constraint == COCO_GTK_KENNEL_TO_MINIMUM ||
      kennel->width_constraint == COCO_GTK_KENNEL_TO_MAXIMUM ||
      kennel->height_constraint == COCO_GTK_KENNEL_TO_MINIMUM ||
      kennel->height_constraint == COCO_GTK_KENNEL_TO_MAXIMUM)
    {
      guint width = 0, height = 0;
      GSList *slist;
      
      for (slist = kennel->children; slist; slist = slist->next)
	{
	  KennelChild *child = slist->data;
	  GtkRequisition *requisition;

	  gtk_signal_handler_block_by_func (GTK_OBJECT (child->widget), coco_kennel_widget_size_request, kennel);
	  /* FIXME: GTKFIX - grrr, buggy GtkLabel code caches requisition */
	  if (GTK_IS_LABEL (child->widget))
	    {
	      gchar *string = g_strdup (GTK_LABEL (child->widget)->label);

	      gtk_label_set_text (GTK_LABEL (child->widget), NULL);
	      gtk_label_set_text (GTK_LABEL (child->widget), string);
	      g_free (string);
	    }
	  gtk_widget_size_request (child->widget, NULL);
	  gtk_signal_handler_unblock_by_func (GTK_OBJECT (child->widget), coco_kennel_widget_size_request, kennel);
	  requisition = &child->widget->requisition;
	  child->real_requisition = *requisition;
	  width = (kennel->width_constraint == COCO_GTK_KENNEL_TO_MINIMUM
		   ? MIN (width, requisition->width)
		   : MAX (width, requisition->width));
	  height = (kennel->height_constraint == COCO_GTK_KENNEL_TO_MINIMUM
		    ? MIN (height, requisition->height)
		    : MAX (height, requisition->height));
	}

      if (kennel->width_constraint != COCO_GTK_KENNEL_TO_MINIMUM &&
	  kennel->width_constraint != COCO_GTK_KENNEL_TO_MAXIMUM)
	width = kennel->width;
      if (kennel->height_constraint != COCO_GTK_KENNEL_TO_MINIMUM &&
	  kennel->height_constraint != COCO_GTK_KENNEL_TO_MAXIMUM)
	height = kennel->height;

      coco_gtk_kennel_resize (kennel, width, height);
    }

  return FALSE;
}

static void
coco_kennel_widget_size_request (GtkWidget      *widget,
			    GtkRequisition *requisition,
			    coco_GtkKennel      *kennel)
{
  KennelChild *child = gtk_object_get_data (GTK_OBJECT (widget), "coco_GtkKennelChild");
  GtkRequisition *real_requisition = &child->real_requisition;
  gboolean need_resize = FALSE;

  g_return_if_fail (g_slist_find (kennel->children, child));

  requisition->width = MAX (0, requisition->width);
  requisition->height = MAX (0, requisition->height);

  switch (kennel->width_constraint)
    {
    case COCO_GTK_KENNEL_TO_MINIMUM:
      if (requisition->width < kennel->width ||
	  (real_requisition->width == kennel->width &&
	   requisition->width != real_requisition->width))
	need_resize = TRUE;
      else
	requisition->width = kennel->width;
      break;
    case COCO_GTK_KENNEL_TO_MAXIMUM:
      if (requisition->width > kennel->width ||
	  (real_requisition->width == kennel->width &&
	   requisition->width != real_requisition->width))
	need_resize = TRUE;
      else
	requisition->width = kennel->width;
      break;
    case COCO_GTK_KENNEL_TO_USIZE:
    case COCO_GTK_KENNEL_TO_WIDGET:
      requisition->width = kennel->width;
      break;
    }
  switch (kennel->height_constraint)
    {
    case COCO_GTK_KENNEL_TO_MINIMUM:
      if (requisition->height < kennel->height ||
	  (real_requisition->height == kennel->height &&
	   requisition->height != real_requisition->height))
	need_resize = TRUE;
      else
	requisition->height = kennel->height;
      break;
    case COCO_GTK_KENNEL_TO_MAXIMUM:
      if (requisition->height > kennel->height ||
	  (real_requisition->height == kennel->height &&
	   requisition->height != real_requisition->height))
	need_resize = TRUE;
      else
	requisition->height = kennel->height;
      break;
    case COCO_GTK_KENNEL_TO_USIZE:
    case COCO_GTK_KENNEL_TO_WIDGET:
      requisition->height = kennel->height;
      break;
    }
  if (need_resize && !kennel->resize_handler)
    kennel->resize_handler = g_idle_add_full (GTK_PRIORITY_RESIZE - 1, coco_kennel_idle_sizer, kennel, NULL);
}

void
coco_gtk_kennel_add (coco_GtkKennel *kennel,
		GtkWidget *widget)
{
  KennelChild *child;

  g_return_if_fail (kennel != NULL);
  g_return_if_fail (kennel->ref_count > 0);
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (GTK_OBJECT_DESTROYED (widget) == FALSE);
  g_return_if_fail (gtk_object_get_data (GTK_OBJECT (widget), "coco_GtkKennelChild") == NULL);

  coco_gtk_kennel_ref (kennel);
  child = g_new0 (KennelChild, 1);
  gtk_object_set_data (GTK_OBJECT (widget), "coco_GtkKennelChild", child);
  child->widget = widget;
  kennel->children = g_slist_prepend (kennel->children, child);
  gtk_widget_set (widget,
		  "signal_after::size_request",
                  coco_kennel_widget_size_request, kennel,
		  "object_signal::destroy",  coco_gtk_kennel_remove, kennel,
		  NULL);
  gtk_widget_queue_resize (widget);
}

void
coco_gtk_kennel_remove (coco_GtkKennel *kennel,
		   GtkWidget *widget)
{
  GSList *slist, *last = NULL;

  g_return_if_fail (kennel != NULL);
  g_return_if_fail (kennel->ref_count > 0);
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (gtk_object_get_data (GTK_OBJECT (widget), "coco_GtkKennelChild") != NULL);

  gtk_object_set_data (GTK_OBJECT (widget), "coco_GtkKennelChild", NULL);
  gtk_signal_disconnect_by_func (GTK_OBJECT (widget),
				 coco_kennel_widget_size_request,
				 kennel);
  gtk_signal_disconnect_by_func (GTK_OBJECT (widget),
				 coco_gtk_kennel_remove,
				 kennel);
  for (slist = kennel->children; slist; last = slist, slist = last->next)
    {
      KennelChild *child = slist->data;

      if (child->widget == widget)
	{
	  (last ? last->next : kennel->children) = slist->next;
	  g_slist_free_1 (slist);
	  g_free (child);
	  break;
	}
    }
  if (!kennel->resize_handler)
    kennel->resize_handler = g_idle_add_full (GTK_PRIORITY_RESIZE - 1, 
                                              coco_kennel_idle_sizer,
                                              kennel, NULL);
  if (!GTK_OBJECT_DESTROYED (widget))
    gtk_widget_queue_resize (widget);
  coco_gtk_kennel_unref (kennel);
}
