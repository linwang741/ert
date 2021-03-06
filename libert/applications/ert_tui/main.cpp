/*
   Copyright (C) 2011  Equinor ASA, Norway.

   The file 'main.c' is part of ERT - Ensemble based Reservoir Tool.

   ERT is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ERT is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.

   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
   for more details.
*/
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <ert/util/util.hpp>
#include <ert/util/hash.hpp>
#include <ert/util/stringlist.hpp>

#include <ert/enkf/enkf_fs.hpp>
#include <ert/enkf/res_config.hpp>
#include <ert/enkf/enkf_main.hpp>
#include <ert/enkf/enkf_types.hpp>

#include <ert/ert/ert_version.hpp>

#include <enkf_tui_main.hpp>

#define WORKFLOW_OPTION "-wf"

void text_splash() {
  const int usleep_time = 1000;
  int i;
  {
#include "ERT.hpp"
    printf("\n\n");
    for (i = 0; i < SPLASH_LENGTH; i++) {
      printf("%s\n", splash_text[i]);
      util_usleep(usleep_time);
    }
    printf("\n\n");

    sleep(1);
#undef SPLASH_LENGTH
  }
}


/*
  GIT_COMMIT and COMPILE_TIME_STAMP are env variables set by the
  makefile. Will exit if the config file does not exist.
*/
void enkf_welcome(const char * config_file) {
  if (util_file_exists(config_file)) {
    char * abs_path              = util_alloc_realpath(config_file);
    char * config_file_msg       = util_alloc_sprintf("Configuration file...: %s \n",abs_path);


    free(config_file_msg);
    free(abs_path);
  } else util_exit(" ** Sorry: can not locate configuration file: %s \n\n", config_file);
}


void enkf_usage() {
  printf("\n");
  printf(" *********************************************************************\n");
  printf(" **                                                                 **\n");
  printf(" **                            E R T                                **\n");
  printf(" **                                                                 **\n");
  printf(" **-----------------------------------------------------------------**\n");
  printf(" ** You have successfully started the ert program developed at      **\n");
  printf(" ** Equinor. Before you can actually start using the program, you   **\n");
  printf(" ** must create a configuration file. When the configuration file   **\n");
  printf(" ** has been created, you can start the ert application with:       **\n");
  printf(" **                                                                 **\n");
  printf(" **   bash> ert config_file                                         **\n");
  printf(" **                                                                 **\n");
  printf(" ** Instructions on how to create the configuration file can be     **\n");
  printf(" ** found at: http://ert.nr.no                                      **\n");
  printf(" *********************************************************************\n");
}







void parse_workflows(int argc, char ** argv, stringlist_type * workflows) {
  /* bool workflow_on = false; */
  for (int iarg = 2; iarg < argc; iarg++) {
    stringlist_append_copy(workflows, argv[iarg]);

    /*if (strcmp(argv[iarg], WORKFLOW_OPTION) == 0)
      workflow_on = true;
      else {
      if (workflow_on)
        stringlist_append_copy(workflows, argv[iarg]);
      else
        fprintf(stderr,"**Warning - option:\'%s\' ignored\n",argv[iarg]);
    }
    */
  }
}




int main (int argc, char ** argv) {
  text_splash();
  printf("\n");
  printf("DEPRECATION WARNING: ERT tui is to be removed in ERT > 2.4.\n");
  printf("Documentation : %s \n","http://ert.nr.no");
  printf("git commit    : %s \n",ert_version_get_git_commit( ));
  printf("compile time  : %s \n",ert_version_get_build_time( ));
  printf("site config   : %s \n", site_config_get_location());

  enkf_main_install_SIGNALS();                     /* Signals common to both tui and gui. */
  signal(SIGINT, util_abort_signal);              /* Control C - tui only.               */
  if (argc < 2) {
    enkf_usage();
    exit(1);
  } else {
    const char * model_config_file = argv[1];
    stringlist_type * workflow_list = stringlist_alloc_new();

    parse_workflows(argc, argv, workflow_list);
    if (!(util_entry_readable(model_config_file) && util_is_file(model_config_file)))
      util_exit("Can not read file %s - exiting \n", model_config_file);

    {
      char * abs_config = util_alloc_realpath(model_config_file);
      printf("model config  : %s \n\n", abs_config);
      free(abs_config);
    }
    enkf_welcome(model_config_file);
    {
      res_config_type * res_config = res_config_alloc_load(model_config_file);
      util_chdir( res_config_get_config_directory( res_config ));
      {
        enkf_main_type * enkf_main = enkf_main_alloc(res_config, true, true);
        enkf_main_run_workflows(enkf_main, workflow_list);
        enkf_tui_main_menu(enkf_main);
        enkf_main_free(enkf_main);
        res_config_free(res_config);
      }
    }

    stringlist_free(workflow_list);
  }
  exit(0);
}
