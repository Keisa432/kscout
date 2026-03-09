#include <kscout_da.h>
#include <kscout_scouter.h>
#include <kscout_view.h>
#include <stdio.h>
#include <windows.h>

static int get_exe_dir(char *buf, size_t buflen)
{
  DWORD len = GetModuleFileNameA(NULL, buf, (DWORD)buflen);
  if (len == 0 || len == buflen)
    return -1;

  char *slash = strrchr(buf, '\\');
  if (slash)
    *slash = '\0';

  return 0;
}

//TODO make it possible to import current team -> compare players by position?

int main(int argc, char** argv)
{
  kscout_view_t *view;
  kscout_scouter_t *scouter;
  const char *role_cfg_path = ".\\data\\config";
  char path[256];
  
  if(argc < 3) {
    perror("Missing arguments\n");
    exit(1);
  }

  char exe_dir[256];
  get_exe_dir(exe_dir, 256);

  snprintf(path, sizeof(path), "%s\\%s", exe_dir, role_cfg_path);
  int rc = kscout_scouter_new(&scouter, path);
  if (rc != KSCOUT_OK) {
    perror("Could not create scouter module");
    goto l_end;
  }

  rc = kscout_view_new(&view, scouter);
  if (rc != KSCOUT_OK) {
    perror("Could not create view module");
    goto l_end;
  }

  rc = kscout_view_load_file(&view, argv[1]);
  if (rc != KSCOUT_OK) {
    perror("Could not load shortlist");
    goto l_end;
  }

  rc = kscout_view_export_to_csv(view, argv[2]);
  if(rc != KSCOUT_OK) {
    perror("Could not export shortlist to JSON");
    goto l_end;
  }

l_end:
  printf("end");
  kscout_scouter_destory(scouter);
  kscout_view_destroy(view);
  return rc;
}