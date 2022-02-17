#ifndef _PID_FILTER_H
#define _PID_FILTER_H

/* defs: shift to header */
#define PS_SAMPLE 1000

/* defines for DVB Transport stream header, move to general header for TS details */
#define PID_MASK 0x1FFF
#define PUSI_MASK 0x4000
#define TEI_MASK 0x8000
#define TR_MASK 0x200

#endif
