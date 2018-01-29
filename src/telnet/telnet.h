#define IAC_SE               0xF0
#define IAC_SB               0xFA
#define IAC_WILL             0xFB
#define IAC_WONT             0xFC
#define IAC_DO               0xFD
#define IAC_DONT             0xFE
#define IAC                  0xFF

#define BINARY              0
#define ECHO                1
#define SUPPRESSGOAHEAD     3
#define STATUS              5           /* RFC859 */
#define LOCATION            23
#define TERMINALTYPE        24          /* RFC1091 & RFC1010 */
#define WINDOWSIZE          31          /* RFC1073 */
#define TERMINALSPEED       32          /* RFC1079 */
#define TOGGLEFLOWCONTROL   33          /* RFC1372 */
#define LINEMODE            34          /* RFC1184 */
#define XDISPLAYLOCATION    35          /* RFC1096 */
#define NEWENVIRON          39          /* RFC1572 */
#define CHARSET             42          /* RFC2066 */

#define MSG_IS      0
#define MSG_SEND    1
#define MSG_RECV    2

