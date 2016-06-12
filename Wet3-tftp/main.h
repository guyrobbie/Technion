#ifndef MAIN_H_
#define MAIN_H_

#include <fcntl.h>

/***********/
/* defines */
/***********/

/* define file flags */
#define FILE_MODES          S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define FILE_PERMISSIONS    O_WRONLY | O_CREAT | O_TRUNC

#endif /* #ifndef MAIN_H_ */