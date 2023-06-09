/* Jody Bruchon's path manipulation code library
 * See jody_paths.c for license information */

#ifndef JODY_PATHS_H
#define JODY_PATHS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PATHBUF_SIZE
 #define PATHBUF_SIZE 4096
#endif

extern int jc_collapse_dotdot(char * const path);
extern int jc_make_relative_link_name(const char * const src,
                const char * const dest, char * rel_path);

#ifdef __cplusplus
}
#endif

#endif /* JODY_PATHS_H */
