/*
 * vos_main.c
 *
 *  Created on: 2014-1-25
 *      Author: kevin
 * 
 */

#include "vos_kernel.h"

// headers

int main(int argc, char *argv[])
{
#ifdef RUN_DAEMON
    daemon(1,0);
#endif
    // init syslog
    openlog(argv[0], LOG_CONS | LOG_PID, LOG_USER);
    
    // read config

    // load modules

    // event loop

    // end
    closelog();
}


