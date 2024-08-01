#ifndef MACROS_H
#define MACROS_H

#define STR_BOOL(b) b ? "true" : "false"
#define SAFE_FREE(ptr) if (ptr) {free(ptr); ptr = NULL;}
#define SAFE_FCLOSE(fp) if (fp) fclose(fp);

#define forever for(;;)

#endif //MACROS_H
