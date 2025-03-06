/* Version: 1.1
**
** Copyright (c) 2002-2016 Walter William Karas
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

/* The function in this file performs default actions if self-auditing
** finds heap corruption.  Don't rely my feeble attempt to handle the
** case where HMM is being used to implement the malloc and free standard
** library functions.  Rewrite the function if necessary to avoid using
** I/O and execution termination functions that call malloc or free.
** In Unix, for example, you would replace the fputs calls with calls
** to the write system call using file handle number 2.
*/

#include <stdio.h>
#include <stdlib.h>

static int entered = 0;

/* Print abort message, file and line.  Terminate execution.
*/
void HMM_dflt_abort(const char *file, const char *line)
  {
    /* Avoid use of printf(), which is more likely to use heap. */

    if (entered)
      /* The standard I/O functions called a heap function and caused
      ** an indirect recursive call to this function.  So we'll have
      ** to just exit without printing a message.  */
      _exit(1);

    entered = 1;

    fputs("\nABORT - Heap corruption\n" "File: ", stderr);
    fputs(file, stderr);
    fputs("  Line: ", stderr);
    fputs(line, stderr);
    fputs("\n\n", stderr);
    fflush(stderr);

    _exit(1);
  }
