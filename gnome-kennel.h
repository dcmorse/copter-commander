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


/* --- Gtk+ Kennel --- */
typedef enum
{
  COCO_GTK_KENNEL_TO_MINIMUM	= 1,
  COCO_GTK_KENNEL_TO_MAXIMUM	= 2,
  COCO_GTK_KENNEL_TO_USIZE	= 3,
  COCO_GTK_KENNEL_TO_WIDGET	= 4
} coco_GtkKennelType;
typedef    struct _coco_GtkKennel              coco_GtkKennel;
coco_GtkKennel* coco_gtk_kennel_new (coco_GtkKennelType width_constrain,
                                     coco_GtkKennelType height_constrain);
void coco_gtk_kennel_configure (coco_GtkKennel *kennel,
                                coco_GtkKennelType width_constrain,
                                coco_GtkKennelType height_constrain);
coco_GtkKennel* coco_gtk_kennel_ref (coco_GtkKennel *kennel);
void coco_gtk_kennel_unref (coco_GtkKennel *kennel);
void coco_gtk_kennel_add (coco_GtkKennel *kennel,
                          GtkWidget *widget);
void coco_gtk_kennel_remove (coco_GtkKennel *kennel,
                             GtkWidget *widget);
void coco_gtk_kennel_resize (coco_GtkKennel *kennel,
                             guint width,
                             guint height);

