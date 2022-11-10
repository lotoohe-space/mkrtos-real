#include <dietstdio.h>

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t tmp;
  pthread_mutex_lock(&stream->m);
  tmp=fwrite_unlocked(ptr,size,nmemb,stream);
  pthread_mutex_unlock(&stream->m);
  return tmp;
}
