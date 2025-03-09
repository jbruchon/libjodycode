#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "libjodycode.h"
#include "likely_unlikely.h"

/* Apple clonefile() is basically a hard link */
#ifdef __APPLE__
 #include <sys/attr.h>
 #include <copyfile.h>
 #ifndef NO_CLONEFILE
  #include <sys/clonefile.h>
  #define ENABLE_CLONEFILE 1
 #endif /* NO_CLONEFILE */
#endif /* __APPLE__ */

#ifdef __linux__
 #include <linux/fs.h>
 #ifndef FICLONE
  #define FICLONE _IOW(0x94, 9, int)
 #endif
 #include <sys/ioctl.h>
#endif /* __linux__ */


#define TEMPORARY_SUFFIX "._link_.tmp";
static const char *tempsuffix = TEMPORARY_SUFFIX;
static const size_t templen = sizeof TEMPORARY_SUFFIX;


static char *alloc_tempname(const struct JC_DIRENT * const restrict dirent)
{
	char *tempname = NULL;
	size_t len;

	len = jc_get_d_namlen(dirent);
	if (len == 0) goto error_params;
	tempname = (char *)malloc(len + templen);
	if (tempname == NULL) goto error_oom;
	memcpy(tempname, dirent->d_name, len);
	memcpy(tempname + len, tempsuffix, templen);
	return tempname;

error_params:
	jc_errno = EFAULT;
	return NULL;
error_oom:
	jc_errno = ENOMEM;
	return NULL;
}


/* Rename a file to a temporary name for safe linking */
static int safe_rename_for_link(const struct jc_fileinfo * const restrict file, const char **newname)
{
	char *tempname = NULL;

	if (unlikely(file == NULL)) goto error_params;

	tempname = alloc_tempname(file->dirent);
	if (tempname == NULL) goto error_params;
	errno = 0;
	if (unlikely(jc_rename(file->dirent->d_name, tempname) == -1)) goto error_with_errno;

	if (newname == NULL) free(tempname);
	else *newname = tempname;

	return 0;

error_params:
	jc_errno = EFAULT;
	return -1;
error_with_errno:
	if (tempname != NULL) free(tempname);
	jc_errno = errno;
	return -1;
}


static int safe_rename_revert(const struct jc_fileinfo * const restrict file, char *newname)
{
	char *tempname;

	if (unlikely(file == NULL)) goto error_params;

	if (newname == NULL) {
		tempname = alloc_tempname(file->dirent);
		if (tempname == NULL) goto error_params;
	} else tempname = newname;
	errno = 0;
	if (unlikely(jc_rename(tempname, file->dirent->d_name) == -1)) goto error_with_errno;
	free(tempname);

	return 0;

error_params:
	jc_errno = EFAULT;
	return -1;
error_with_errno:
	if (tempname != NULL) free(tempname);
	jc_errno = errno;
	return -1;
}


int jc_linkfiles(struct jc_fileinfo_batch * const restrict batch, const enum jc_e_link linktype)
{
	int i, retval = 0;

	if (unlikely(batch == NULL || batch->count < 2 || linktype > 2 || linktype < 0)) goto error_bad_params;

	/* All batch statuses default to general failure */
	for (i = 1; i < batch->count; i++) {
		batch->files[i].status = ECANCELED;
		if (unlikely(batch->files[i].dirent == NULL)) goto error_bad_params;
	}

#ifdef __linux__
	if (unlikely(jc_get_kernel_version() < 3012000)) goto error_kernel_version;
#endif /* __linux__ */

	/* TODO: hard link + clonefile link, symlink */

	switch (linktype) {
	/* "Soft" (symbolic) link */
	case 0:
		if (unlikely(batch->files[0].stat->st_mode == JC_DT_LNK)) goto error_bad_params;
		if (unlikely(batch->files[0].dirent->d_type == JC_DT_LNK)) goto error_bad_params;
		break;

	/* Hard link */
	default:
	case 1:
		break;

	/* Clone link or reflink */
	case 2:
#if defined __linux__ && defined FICLONE
		/* Linux FICLONE (reflink copy) */
		int src_fd, dest_fd;
		if (batch->files[0].dirent == NULL) goto error_bad_params;
		errno = 0;
		src_fd = open(batch->files[0].dirent->d_name, O_RDONLY);
		if (unlikely(src_fd < 0)) {
			batch->files[0].status = errno;
			jc_errno = EIO;
			return -1;
		}
		for (i = 1; i < batch->count; i++) {
			errno = 0;
			dest_fd = open(batch->files[i].dirent->d_name, O_RDWR);
			if (unlikely(dest_fd < 0)) goto error_ficlone;
			errno = 0;
			if (unlikely(ioctl(dest_fd, FICLONE, src_fd) == -1)) goto error_ficlone;
			batch->files[i].status = errno;
			close(dest_fd);
			continue;

error_ficlone:
			batch->files[i].status = errno;
			if (errno != 0) jc_errno = EIO;
			retval = -1;
			continue;
		}
		close(src_fd);
#else /* not __linux__ */
		jc_errno = ENOTSUP;
		return -1;
#endif /* __linux__ */
		break;
	}

	return retval;

#ifdef __linux__
error_kernel_version:
	jc_errno = JC_EKERNVER;
	return -1;
#endif  /* __linux__ */
error_bad_params:
	jc_errno = EFAULT;
	return -1;
}

#if 0
/* linktype: 0=soft (symbolic), 1=hard, 2=clone/reflink */
int jc_linkfiles(struct jc_fileinfo_batch *batch, const int linktype)
{
	int *srcfile;
	unsigned int x = 0;
	size_t name_len = 0;
	int i, success;
#ifndef NO_SYMLINKS
	unsigned int symsrc;
	char rel_path[JC_PATHBUF_SIZE + 4];
#endif
#if defined ON_WINDOWS || defined ENABLE_CLONEFILE_LINK
	struct JC_STAT s;
#endif
#ifdef ENABLE_CLONEFILE_LINK
	unsigned int srcfile_preserved_flags = 0;
	unsigned int dupfile_preserved_flags = 0;
	unsigned int dupfile_original_flags = 0;
	struct timeval dupfile_original_tval[2];
#endif


	for (i = 1; i < count; count++) {
		/* Link every file to the first file */
		if (linktype != 0) {
#ifndef NO_HARDLINKS
			srcfile = 0;
			x = 2;
#else
			return -1;
#endif
		} else {
#ifndef NO_SYMLINKS
			x = 1;
			/* Symlinks should target a normal file if one exists */
			srcfile = NULL;
			for (symsrc = 0; symsrc <= count; symsrc++) {
				if (!ISFLAG(dupelist[symsrc]->flags, FF_IS_SYMLINK)) {
					srcfile = symsrc;
					break;
				}
			}
			/* If no normal file exists, abort */
			if (srcfile == NULL) goto linkfile_loop;
#else
			linkfiles_nosupport("soft", "symlink");
#endif
		}
		if (linktype == 2) {
#ifdef ENABLE_CLONEFILE_LINK
			if (jc_stat(srcfile->d_name, &s) != 0) {
				fprintf(stderr, "warning: stat() on source file failed, skipping:\n[SRC] ");
				jc_fwprint(stderr, srcfile->d_name, 1);
				exit_status = EXIT_FAILURE;
				goto linkfile_loop;
			}

			/* macOS unexpectedly copies the compressed flag when copying metadata
			 * (which can result in files being unreadable), so we want to retain
			 * the compression flag of srcfile */
			srcfile_preserved_flags = s.st_flags & UF_COMPRESSED;
#else
			linkfiles_nosupport("clone", "clonefile");
#endif
		}
		for (; x <= count; x++) {
			if (linktype == 1 || linktype == 2) {
				/* Can't hard link files on different devices */
				if (srcfile->device != dupelist[x]->device) {
					fprintf(stderr, "warning: hard link target on different device, not linking:\n-//-> ");
					jc_fwprint(stderr, dupelist[x]->d_name, 1);
					exit_status = EXIT_FAILURE;
					continue;
				} else {
					/* The devices for the files are the same, but we still need to skip
					 * anything that is already hard linked (-L and -H both set) */
					if (srcfile->inode == dupelist[x]->inode) {
						continue;
					}
				}
			} else {
				/* Symlink prerequisite check code can go here */
				/* Do not attempt to symlink a file to itself or to another symlink */
#ifndef NO_SYMLINKS
				if (ISFLAG(dupelist[x]->flags, FF_IS_SYMLINK) &&
						ISFLAG(dupelist[symsrc]->flags, FF_IS_SYMLINK)) continue;
				if (x == symsrc) continue;
#endif
			}

			/* Do not attempt to hard link files for which we don't have write access */
			if (
#ifdef ON_WINDOWS
				!JC_S_ISRO(dupelist[x]->mode) &&
#endif
				(jc_access(dupelist[x]->d_name, JC_W_OK) != 0))
			{
				fprintf(stderr, "warning: link target is a read-only file, not linking:\n-//-> ");
				jc_fwprint(stderr, dupelist[x]->d_name, 1);
				exit_status = EXIT_FAILURE;
				continue;
			}
			/* Check file pairs for modification before linking */
			/* Safe linking: don't actually delete until the link succeeds */
			i = file_has_changed(srcfile);
			if (i) {
				fprintf(stderr, "warning: source file modified since scanned; changing source file:\n[SRC] ");
				jc_fwprint(stderr, dupelist[x]->d_name, 1);
				srcfile = dupelist[x];
				exit_status = EXIT_FAILURE;
				continue;
			}
			if (file_has_changed(dupelist[x])) {
				fprintf(stderr, "warning: target file modified since scanned, not linking:\n-//-> ");
				jc_fwprint(stderr, dupelist[x]->d_name, 1);
				exit_status = EXIT_FAILURE;
				continue;
			}
#ifdef ON_WINDOWS
			/* For Windows, the hard link count maximum is 1023 (+1); work around
			 * by skipping linking or changing the link source file as needed */
			if (jc_stat(srcfile->d_name, &s) != 0) {
				fprintf(stderr, "warning: win_stat() on source file failed, changing source file:\n[SRC] ");
				jc_fwprint(stderr, dupelist[x]->d_name, 1);
				srcfile = dupelist[x];
				exit_status = EXIT_FAILURE;
				continue;
			}
			if (s.st_nlink >= 1024) {
				fprintf(stderr, "warning: maximum source link count reached, changing source file:\n[SRC] ");
				srcfile = dupelist[x];
				exit_status = EXIT_FAILURE;
				continue;
			}
			if (jc_stat(dupelist[x]->d_name, &s) != 0) continue;
			if (s.st_nlink >= 1024) {
				fprintf(stderr, "warning: maximum destination link count reached, skipping:\n-//-> ");
				jc_fwprint(stderr, dupelist[x]->d_name, 1);
				exit_status = EXIT_FAILURE;
				continue;
			}
#endif
#ifdef ENABLE_CLONEFILE_LINK
			if (linktype == 2) {
				if (jc_stat(dupelist[x]->d_name, &s) != 0) {
					fprintf(stderr, "warning: stat() on destination file failed, skipping:\n-##-> ");
					jc_fwprint(stderr, dupelist[x]->d_name, 1);
					exit_status = EXIT_FAILURE;
					continue;
				}

				/* macOS unexpectedly copies the compressed flag when copying metadata
				 * (which can result in files being unreadable), so we want to ignore
				 * the compression flag on dstfile in favor of the one from srcfile */
				dupfile_preserved_flags = s.st_flags & ~(unsigned int)UF_COMPRESSED;
				dupfile_original_flags = s.st_flags;
				dupfile_original_tval[0].tv_sec = s.st_atim.tv_sec;
				dupfile_original_tval[0].tv_usec = s.st_atim.tv_usec;
				dupfile_original_tval[1].tv_sec = s.st_mtim.tv_sec;
				dupfile_original_tval[1].tv_usec = s.st_mtim.tv_usec;
			}
#endif

			/* Make sure the name will fit in the buffer before trying */
			name_len = strlen(dupelist[x]->d_name) + 11;
			if (name_len > JC_PATHBUF_SIZE) continue;
			/* Assemble a temporary file name */
			if (i != 0) {
				fprintf(stderr, "warning: cannot move link target to a temporary name, not linking:\n-//-> ");
				jc_fwprint(stderr, dupelist[x]->d_name, 1);
				exit_status = EXIT_FAILURE;
				/* Just in case the rename succeeded yet still returned an error, roll back the rename */
				jc_rename(tempname, dupelist[x]->d_name);
				continue;
			}

			/* Create the desired hard link with the original file's name */
			errno = 0;
			success = 0;
			if (linktype == 1) {
				if (jc_link(srcfile->d_name, dupelist[x]->d_name) == 0) success = 1;
#ifdef ENABLE_CLONEFILE_LINK
			} else if (linktype == 2) {
				if (clonefile(srcfile->d_name, dupelist[x]->d_name, 0) == 0) {
					if (copyfile(tempname, dupelist[x]->d_name, NULL, COPYFILE_METADATA) == 0) {
						/* If the preserved flags match what we just copied from the original dupfile, we're done.
						 * Otherwise, we need to update the flags to avoid data loss due to differing compression flags */
						if (dupfile_original_flags == (srcfile_preserved_flags | dupfile_preserved_flags)) {
							success = 1;
						} else if (chflags(dupelist[x]->d_name, srcfile_preserved_flags | dupfile_preserved_flags) == 0) {
							/* chflags overrides the timestamps that were restored by copyfile, so we need to reapply those as well */
							if (utimes(dupelist[x]->d_name, dupfile_original_tval) == 0) {
								success = 1;
							} else clonefile_error("utimes", dupelist[x]->d_name);
						} else clonefile_error("chflags", dupelist[x]->d_name);
					} else clonefile_error("copyfile", dupelist[x]->d_name);
				} else clonefile_error("clonefile", dupelist[x]->d_name);
#endif /* ENABLE_CLONEFILE_LINK */
			}
#ifndef NO_SYMLINKS
			else {
				i = jc_make_relative_link_name(srcfile->d_name, dupelist[x]->d_name, rel_path);
				if (i < 0) {
					fprintf(stderr, "warning: make_relative_link_name() failed (%d)\n", i);
				} else if (i == 1) {
					fprintf(stderr, "warning: files to be linked have the same canonical path; not linking\n");
				} else if (symlink(rel_path, dupelist[x]->d_name) == 0) success = 1;
			}
#endif /* NO_SYMLINKS */
			if (!success) {
				/* The link failed. Warn the user and put the link target back */
				exit_status = EXIT_FAILURE;
				i = jc_rename(tempname, dupelist[x]->d_name);
				if (i != 0) revert_failed(dupelist[x]->d_name, tempname);
				continue;
			}

			/* Remove temporary file to clean up; if we can't, reverse the linking */
			i = jc_remove(tempname);
			if (i != 0) {
				/* If the temp file can't be deleted, there may be a permissions problem
				 * so reverse the process and warn the user */
				fprintf(stderr, "\nwarning: can't delete temp file, reverting: ");
				jc_fwprint(stderr, tempname, 1);
				exit_status = EXIT_FAILURE;
				i = jc_remove(dupelist[x]->d_name);
				/* This last error really should not happen, but we can't assume it won't */
				if (i != 0) fprintf(stderr, "\nwarning: couldn't remove link to restore original file\n");
				else {
					i = jc_rename(tempname, dupelist[x]->d_name);
					if (i != 0) revert_failed(dupelist[x]->d_name, tempname);
				}
			}
		}
#if !defined NO_SYMLINKS || defined ENABLE_CLONEFILE_LINK
linkfile_loop:
#endif
	}

	return;
}

#endif // 0
