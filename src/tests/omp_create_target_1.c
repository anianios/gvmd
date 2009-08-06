/* Test 1 of OMP CREATE_TARGET.
 * $Id$
 * Description: Test OMP CREATE_TARGET, passing an existing target.
 *
 * Authors:
 * Matthew Mundell <mmundell@intevation.de>
 *
 * Copyright:
 * Copyright (C) 2009 Greenbone Networks GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2,
 * or, at your option, any later version as published by the Free
 * Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define TRACE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "../tracef.h"

int
main ()
{
  int socket, ret;
  gnutls_session_t session;
  entity_t entity, expected;

  setup_test ();

  socket = connect_to_manager (&session);
  if (socket == -1) return EXIT_FAILURE;

  /* Send request. */

  if (env_authenticate (&session))
    {
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  if (send_to_manager (&session, "<create_target>"
                                 "<name>omp_create_target_1</name>"
                                 "<hosts>localhost,127.0.0.1</hosts>"
                                 "</create_target>")
      == -1)
    {
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  /* Read the response. */

  entity = NULL;
  if (read_entity (&session, &entity))
    {
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  /* Compare. */

  expected = add_entity (NULL, "create_target_response", NULL);
  add_attribute (expected, "status", "201");

  if (compare_entities (entity, expected))
    {
      send_to_manager (&session, "<delete_target name=\"omp_create_target_1\"/>");
      close_manager_connection (socket, session);
      free_entity (entity);
      free_entity (expected);
      return EXIT_FAILURE;
    }
  free_entity (entity);
  free_entity (expected);

  /* Send request again. */

  if (send_to_manager (&session, "<create_target>"
                                 "<name>omp_create_target_1</name>"
                                 "<hosts>localhost,127.0.0.1</hosts>"
                                 "</create_target>")
      == -1)
    {
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  /* Read the response. */

  entity = NULL;
  if (read_entity (&session, &entity))
    {
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  /* Compare. */

  expected = add_entity (NULL, "create_target_response", NULL);
  add_attribute (expected, "status", "400");

  if (compare_entities (entity, expected))
    ret = EXIT_FAILURE;
  else
    ret = EXIT_SUCCESS;

  /* Cleanup. */

  omp_delete_target (&session, "omp_create_target_1");
  close_manager_connection (socket, session);
  free_entity (entity);
  free_entity (expected);

  return ret;
}
