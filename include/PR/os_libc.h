#ifndef _OS_LIBC_H_
#define _OS_LIBC_H_

#include "ultratypes.h"

 // Old deprecated functions from strings.h, replaced by memcpy/memset.
#ifndef __APPLE__
 extern void bcopy(const void *, void *, size_t);
 extern void bzero(void *, size_t);
#endif

#endif /* !_OS_LIBC_H_ */