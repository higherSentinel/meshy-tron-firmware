// seperator LED task
#define TASK_SEPRATOR_LED_PULSE_NAME            TASK_NAME("PULSE_SEPERATOR")
#define TASK_SEPRATOR_LED_PULSE_STACK_SIZE      1024
#define TASK_SEPRATOR_LED_PULSE_PRIORITY        1


// no touchy
#define PREFIX "TSK_"
#define CONCAT(a,b) a b
#define TASK_NAME(a) CONCAT(PREFIX, a)

// random
#define HALT                while(1)
