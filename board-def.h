// pins

// hpdl1414
#define HPDL1414_A0             3       // needs to be floating on boot
#define HPDL1414_A1             42      // NO JTAG       
#define HPDL1414_D0             41      // NO JTAG
#define HPDL1414_D1             40      // NO JTAG
#define HPDL1414_D2             39      // NO JTAG
#define HPDL1414_D3             38
#define HPDL1414_D4             18      // shared with main nixie sr data pin
#define HPDL1414_D5             14
#define HPDL1414_D6             48
#define HPDL1414_WR             2


// nixie related
#define NIXIE_SR_COM_MR         21
#define NIXIE_SR_COM_DAT        18      // shared with hpdl1414 D4
#define NIXIE_SR_COM_LAT        17
#define NIXIE_SR_CLK_1          4
#define NIXIE_SR_CLK_2          5
#define NIXIE_SR_CLK_3          6
#define NIXIE_SR_CLK_4          7
#define NIXIE_PWM_1             10
#define NIXIE_PWM_2             11
#define NIXIE_PWM_3             12
#define NIXIE_PWM_4             13

// misc
#define SEPARATION_LED_PIN      47
#define RTC_INT_PIN             1