// seperator LED task
#define TASK_DISPLAY_MODULE_NAME            TASK_NAME("DISPLAY_MODULE")
#define TASK_DISPLAY_MODULE_STACK_SIZE      4096
#define TASK_DISPLAY_MODULE_PRIORITY        1


// no touchy
#define PREFIX "TSK_"
#define CONCAT(a,b) a b
#define TASK_NAME(a) CONCAT(PREFIX, a)

// random
#define HALT                while(1)
