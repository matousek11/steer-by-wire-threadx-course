#include "control.h"
#include "udp_link.h"
#include "encoder_driver.h"
#include <stdio.h>

/* Loop period in ThreadX ticks (1 tick = 10 ms, 100 ticks = 1 s). */
#define CONTROL_LOOP_PERIOD         5

VOID control_thread_entry(ULONG input)
{
    UINT  ret;
    ULONG next_wakeup;
    ULONG now;
    ULONG loop_start;
#if IS_SENDER_BOARD
    uint32_t heading;
#else
    /* last valid heading, kept when nothing new arrives */
    INT requested_heading = 0;
#endif

    TX_PARAMETER_NOT_USED(input);

    ret = udp_link_initialize();
    if (ret != NX_SUCCESS)
    {
        printf("UDP link init failed\n");
        return;
    }

#if IS_SENDER_BOARD
    ret = encoder_driver_initialize();
    if (ret != TX_SUCCESS)
    {
        printf("Encoder init failed\n");
        return;
    }
#endif
    /* Control algorithm is going to be initialized here when IS_SENDER_BOARD = 0 */

    next_wakeup = tx_time_get();

    while (1)
    {
        loop_start = tx_time_get();

#if IS_SENDER_BOARD
        ret = encoder_driver_get_degrees(&heading);
        if (ret == TX_SUCCESS)
        {
            ret = udp_link_send_heading((INT) heading);
            if (ret != NX_SUCCESS)
            {
                printf("Failed to send heading\n");
            }
        }
        else
        {
            printf("Failed to read encoder\n");
        }
#else
        ret = udp_link_receive_heading(&requested_heading);
        if (ret == NX_SUCCESS)
        {
            /* New requested heading is in requested_heading (PID goes here later). */
        }
        else if (ret != NX_NO_PACKET)
        {
            /* NX_NO_PACKET only means nothing new arrived; anything else is an error. */
            printf("Failed to receive heading\n");
        }
#endif

        /* Fixed period: wake up at a planned moment, not "period after the work",
           so the time spent above does not stretch the loop. */
        next_wakeup += CONTROL_LOOP_PERIOD;
        now = tx_time_get();

        if ((LONG) (next_wakeup - now) > 0)
        {
            tx_thread_sleep(next_wakeup - now);
        }
        else
        {
            /* The work took longer than one period: start counting again from now. */
            printf("Work in control loop took longer than expected: %lu ticks\n", now - loop_start);
            next_wakeup = now;
        }
    }
}
