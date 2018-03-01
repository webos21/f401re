#if defined(__GNUC__) && !defined(__MINGW32__)
/*
 * Copyright 2016 Cheolmin Jo (webos21@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ----------------------------------------------
 * Headers
 * ---------------------------------------------- */

#include <stdio.h>
#include <time.h>
#ifndef S_SPLINT_S
/* SPLINT cannot parse these headers */
#include <sys/stat.h>
#include <errno.h>
#endif /*S_SPLINT_S*/

#ifdef S_SPLINT_S
/*
 * Ignore the warnings of SPLINT : _isatty(), _lseek() ...
 * Define the type for SPLINT : caddr_t
 */
/*@-namechecks@*/
/*@-exportheader@*/
typedef char * caddr_t;
#endif /*S_SPLINT_S*/

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

/* Called before main() */
void _init(void) {
}

int _isatty(int file) {
	(void)(file);
	return 1;
}

int _lseek(int file, int ptr, int dir) {
	(void)(file);
	(void)(ptr);
	(void)(dir);
	return 0;
}

int _fstat(int file, struct stat *st) {
	(void)(file);
	(void)(st);
	return 0;
}

int _read(int file, char *ptr, int len) {
	(void)(file);
	(void)(ptr);
	(void)(len);
	return 0;
}

int _write(int file, void *ptr, int len) {
	(void)(file);
	(void)(ptr);
	(void)(len);
	return 0;
}

int _close(int file) {
	(void)(file);
	return 0;
}

int _getpid(void) {
	return 1;
}

int _kill(int signo) {
	(void)(signo);
	return 0;
}

int _exit(int status) {
	(void)(status);
	return 0;
}

/* SPLINT cannot parse these headers */
#ifndef S_SPLINT_S
/* Register name faking - works in collusion with the linker.  */
register char * stack_ptr asm ("sp");
#endif

caddr_t _sbrk_r (struct _reent *r, int incr)
{
#ifndef S_SPLINT_S
	extern char end asm ("end"); /* Defined by the linker.  */
#endif
	static char * heap_end;
	char * prev_heap_end;

	(void)(r);

	if (heap_end == NULL) {
		heap_end = & end;
	}

	prev_heap_end = heap_end;

	if (heap_end + incr > stack_ptr) {
		/* Some of the libstdc++-v3 tests rely upon detecting
		 out of memory errors, so do not abort here.  */
#if 0
		_write (1, "_sbrk: Heap and stack collision\n", 32);
		abort ();
#else /* !0 */
		errno = ENOMEM;
		return (caddr_t) -1;
#endif /* 0 */
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

#else /* !__GNUC__ || __MINGW32__ */
typedef int compiler_happy_pedantic;
#endif /* __GNUC__ && !__MINGW32__ */
