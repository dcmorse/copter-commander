/*
 * Copter Commander - A 2d network helicopter game
 * Copyright (C) 2001  David Morse
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "coco.h"


const char *coco_version_string = "1.1";

const char *coco_product_name = COCO_PRODUCT_NAME;
const char *coco_run_name;      /* argv[0] */

FILE *coco_server_log;

char *coco_level_text = NULL;

const char *coco_local_server = "localhost";
char *coco_server = "localhost";
coco_bool coco_server_specified_p = COCO_FALSE;
coco_bool coco_server_malloced_p = COCO_FALSE;

int coco_blend_alpha = -1;/* not coco_bool to appease popt, and -1 means "maybe, depending on glXIsDirect" */

int coco_jamming = 1;

int coco_command_line_inhibit_double_buffer = COCO_FALSE;/* not coco_bool to appease popt */

const char *coco_product_authors[]
 = { "David Morse <copter-commander-devel@bomberlan.net>", NULL };
const char *coco_product_copyright = "Copyright 2001 David Morse.";
const char *coco_product_description
 = "A 2d network helicopter game.\n";
const char *coco_product_description_windbag
 = "A 2d network helicopter game.  \nUses some code courtesy of BEAST, Copyright 1998, 1999, 2000 Tim Janik and Red Hat.";

const char *coco_gnu_public_license =
#include "COPYING.strlit"
;

const char *coco_level_directory = COCO_LEVEL_DIRECTORY;
const char *coco_sound_directory = COCO_SOUND_DIRECTORY;

const char *coco_keybindings = 
#include "keybindings.strlit"
;

const char *coco_mini_manual =
#include "manual.strlit"
;

struct coco_version_number {
  short major;
  short minor;
};


/* versions of coco compatible with the current version */
#define COCO_NETWORK_COMPATIBLE_VERSIONS_SIZE 1
static struct coco_version_number coco_network_compatible_versions
 [COCO_NETWORK_COMPATIBLE_VERSIONS_SIZE]
 = { { 1, 0 } };
/* One day, if there is ever a compatible version, this will be replace by
   something like:
static struct coco_version_number coco_network_compatible_versions[COCO_NETWORK_COMPATIBLE_VERSIONS_SIZE] = { { 1, 1 } } */

#define COCO_LEVEL_FILE_FORMAT_COMPATIBLE_VERSIONS_SIZE 1
static struct coco_version_number coco_level_file_format_compatible_versions
 [COCO_NETWORK_COMPATIBLE_VERSIONS_SIZE]
 = { { 1, 0 } };
/* One day, if there is ever a compatible version, this will be replace by
   something like:
static struct coco_version_number coco_level_file_format_compatible_versions[COCO_LEVEL_FILE_FORMAT_COMPATIBLE_VERSIONS_SIZE] = { { 1, 1 } } */

/* see long comment by the definition of COCO_MAJOR_VERSION for a
   discussion of coco's approach to versioning.  */
coco_bool 
coco_version_compatible_p( short major, short minor, 
                           enum coco_version_domain domain )
{
  if ((COCO_MAJOR_VERSION==major) && (COCO_MINOR_VERSION==minor))
    return COCO_TRUE; /* always compatible with myself, I hope */
  else if ((COCO_MAJOR_VERSION > major)
           || ((COCO_MAJOR_VERSION == major) 
               && (COCO_MINOR_VERSION >= minor)))
    { /* trying to communicate with older version */
      int i, size;
      struct coco_version_number *versions;
      if (COCO_NETWORK==domain)
        {
          size = COCO_NETWORK_COMPATIBLE_VERSIONS_SIZE;
          versions = coco_network_compatible_versions;
        }
      else
        {
          size = COCO_LEVEL_FILE_FORMAT_COMPATIBLE_VERSIONS_SIZE;
          versions = coco_level_file_format_compatible_versions;
        }
      for (i=0; i<size; i++)
        if ((major==versions[i].major) && (minor==versions[i].minor))
          return COCO_TRUE;
      return COCO_FALSE;
    }
  else 
    /* trying to communicate with future version */
    return (COCO_MAJOR_VERSION == major);
}
