/****************************************************************************
*
*   Copyright (c) 2006 Carrick Detweiler
*                      and Massachusetts Institute of Technology
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*
*
*   $Id: queue.h 4922 2014-04-01 18:55:36Z carrick $
****************************************************************************/

/*****************USAGE********************
 * 
 * WARNING: Not interrupt safe!!! Enclose calls with
 *   oldIRQ = disableIRQ();
 *   oldFIQ = disableFIQ();
 *   QUEUE_...
 *   restoreFIQ(oldFIQ);
 *   restoreIRQ(oldIRQ);
 * if important
 *
 * to create a new queue:
 * QUEUE_NEW(RxQueue,size);
 *
 */

#if !defined( QUEUE_H )
#define QUEUE_H

#include <avr/io.h>

volatile typedef struct {
  volatile uint8_t putIdx;
  volatile uint8_t size;
  volatile uint8_t maxSize;
  char *queue;
} queue_t;

#define QUEUE_NEW( name , size)                 \
  char name##_queue_real[(size)];            \
  queue_t name = {0,0,(size),name##_queue_real};

#define QUEUE_REINIT(q)   \
  {(q).putIdx=(q).size=0;}

#define DEQUEUE( q ) \
  (((q).queue)[ ((q).putIdx - --(q).size + (q).maxSize) % ((q).maxSize) ])

///Gets the nth element without removing it
#define QUEUE_PEAK( q , n )                                             \
  (((q).queue)[ (((q).putIdx + (n) - ((q).size-1)) + (q).maxSize) % ((q).maxSize) ])

#define ENQUEUE( q , v)            \
  {((q).queue)[ (q).putIdx = (++(q).putIdx % ((q).maxSize)) ] = (v);(q).size++;}

#define QUEUE_SIZE( q ) \
  ((q).size)

#define QUEUE_MAX_SIZE( q ) \
  ((q).maxSize)

#define QUEUE_FREE( q ) \
  (((q).maxSize) - ((q).size))

#define QUEUE_ISEMPTY( q ) \
  ((q).size <= 0)

#define QUEUE_ISFULL( q ) \
  ((q).size >= (q).maxSize)

#endif /* QUEUE_H */
