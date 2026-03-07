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

// TODO display data?
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

  rc = kscout_view_export_to_json(view, argv[2]);
  if(rc != KSCOUT_OK) {
    perror("Could not export shortlist to JSON");
    goto l_end;
  }
  // kscout_view_iter_t *iter;
  // rc = kscout_view_iter_init(view, &iter);
  // if (rc != KSCOUT_OK) {
  //   perror("Could not initialize view iterator");
  //   goto l_end;
  // }
  // kscout_player_t p = {0};

  // while (kscout_view_iter_next(iter, &p) == KSCOUT_OK) {
  //   char line_buf[512];
  //   int offset = 0;

  //   offset+= snprintf(line_buf, sizeof(line_buf), "player:%s    ", p.name);
  //   kscout_da_foreach(kscout_role_score_t, rating, &p.role_rating) {
  //     offset+= snprintf(line_buf + offset, sizeof(line_buf) - offset,
  //                       "role: %s score %f    ", rating->def->name,
  //                       rating->score);
  //   }
  //   offset+= snprintf(line_buf + offset, sizeof(line_buf) - offset, "\n");
  //   printf("%s", line_buf);
  // }

  // kscout_view_iter_destroy(iter);

l_end:
  printf("end");
  kscout_scouter_destory(scouter);
  kscout_view_destroy(view);
  return rc;
}