#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <stdlib.h>


#define ERROR_LVL   1
#define TRACE_LVL   ERROR_LVL + 1
#define INFO_LVL    TRACE_LVL + 1

//#define DBG INFO_LVL
//#define DBG TRACE_LVL
#define DBG 0

extern FILE* logFile;
pthread_mutex_t trace_mutex;

#define LOG(...)                do{ \
                                TRACE(__VA_ARGS__); \
                                fprintf(logFile, __VA_ARGS__); \
                                fprintf(logFile, "\r\n"); \
                                }while(0);
                                
                                

#if (DBG >= INFO_LVL)
    #define INFO(...)           do{\
                                pthread_mutex_lock(&trace_mutex); \
                                printf("Info: "); \
                                printf (__VA_ARGS__); \
                                printf("\r\n"); \
                                pthread_mutex_unlock(&trace_mutex); \
                                }while(0);
                                
    //#define COMMON_Print(...) printf (__VA_ARGS__)
#else
    #define INFO(...)
#endif


#if (DBG >= TRACE_LVL)
    #define TRACE(...)          do{\
                                pthread_mutex_lock(&trace_mutex); \
                                printf("Trace: "); \
                                printf (__VA_ARGS__); \
                                printf("\r\n"); \
                                pthread_mutex_unlock(&trace_mutex); \
                                }while(0);
                                
    //#define COMMON_Print(...) printf (__VA_ARGS__)
#else
    #define TRACE(...)
#endif

#if (DBG >= ERROR_LVL)
    #define ERROR(...)      do{ \
							pthread_mutex_lock(&trace_mutex); \
                            printf("\r\nError: "); \
                            printf (__VA_ARGS__); \
                            printf("\r\n"); \
							pthread_mutex_unlock(&trace_mutex); \
                            }while(0);
    //#define COMMON_Print(...) printf (__VA_ARGS__)
#else
    #define ERROR(...)
#endif


/*************************/
/* Function declarations */
/*************************/

void die(char *s);

#endif /* #ifndef _DEBUG_H_ */