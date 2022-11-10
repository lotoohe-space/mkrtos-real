#include <sys/types.h>
#include "dietstdio.h"
#include <unistd.h>
#include <errno.h>

size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
  ssize_t res;
  size_t len=size*nmemb;
  size_t i,done;
  if (!__likely(stream->flags&CANWRITE) || __fflush4(stream,0)) {
kaputt:
    stream->flags|=ERRORINDICATOR;
    return 0;
  }
  if (!nmemb || len/nmemb!=size) return 0; /* check for integer overflow */
  if (__unlikely(len>stream->buflen || (stream->flags&NOBUF))) {
    if (fflush_unlocked(stream)) return 0;
    do {
      res=__libc_write(stream->fd,ptr,len);
    } while (res==-1 && errno==EINTR);
  } else
again:
         {
    /* try to make the common case fast */
    size_t todo=stream->buflen-stream->bm;
    if (todo>len) todo=len;

    if (todo) {
      if (stream->flags&BUFLINEWISE) {
	if (__unlikely((stream->flags&CHECKLINEWISE)!=0)) {
	  stream->flags&=~CHECKLINEWISE;
	  /* stdout is set to BUFLINEWISE|CHECKLINEWISE by default. */
	  /* that means we should check whether it is connected to a
	   * tty on first flush, and if not so, reset BUFLINEWISE */
	  if (!isatty(stream->fd)) {
	    stream->flags&=~BUFLINEWISE;
	    goto notlinewise;
	  }
	}
	for (i=0; i<todo; ++i) {
	  if ((stream->buf[stream->bm++]=((char*)ptr)[i])=='\n') {
	    if (fflush_unlocked(stream)) goto kaputt;
	  }
	}
      } else {
notlinewise:
	memcpy(stream->buf+stream->bm,ptr,todo);
	stream->bm+=todo;
	if (stream->bm==stream->buflen) {
	  if (fflush_unlocked(stream)) return 0;
	  /* if we are here, we should not have an empty buffer */
	  len-=todo;
	  if (!len) return nmemb;
	  ptr+=todo;
	  goto again;
	} else
	  return nmemb;
      }
      done=todo;
    } else
      done=0;
    for (i=done; i<len; ++i)
      if (fputc_unlocked(((char*)ptr)[i],stream) == EOF) {
	res=len-i;
	goto abort;
      }
    res=len;
  }
  if (res<0) {
    stream->flags|=ERRORINDICATOR;
    return 0;
  }
abort:
  return size?res/size:0;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) __attribute__((weak,alias("fwrite_unlocked")));
