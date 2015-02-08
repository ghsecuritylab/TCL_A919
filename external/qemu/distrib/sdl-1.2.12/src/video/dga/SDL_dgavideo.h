
#include "SDL_config.h"

#ifndef _SDL_dgavideo_h
#define _SDL_dgavideo_h

#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xproto.h>

#include "SDL_mouse.h"
#include "SDL_mutex.h"
#include "../SDL_sysvideo.h"

#if SDL_VIDEO_DRIVER_X11_DPMS
#include <X11/extensions/dpms.h>
#endif

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_VideoDevice *this

/* Define this if you need the DGA driver to be thread-safe */
#define LOCK_DGA_DISPLAY
#ifdef LOCK_DGA_DISPLAY
#define LOCK_DISPLAY()		SDL_mutexP(event_lock)
#define UNLOCK_DISPLAY()	SDL_mutexV(event_lock)
#else
#define LOCK_DISPLAY()
#define UNLOCK_DISPLAY()
#endif


/* This is the structure we use to keep track of video memory */
typedef struct vidmem_bucket {
	struct vidmem_bucket *prev;
	int used;
	int dirty;
	Uint8 *base;
	unsigned int size;
	struct vidmem_bucket *next;
} vidmem_bucket;

/* Private display data */
struct SDL_PrivateVideoData {
	Display *DGA_Display;
	Colormap DGA_colormap;
	int visualClass;

#define NUM_MODELISTS	4		/* 8, 16, 24, and 32 bits-per-pixel */
	int SDL_nummodes[NUM_MODELISTS];
	SDL_Rect **SDL_modelist[NUM_MODELISTS];

	/* Information for the video surface */
	Uint8 *memory_base;
	int memory_pitch;
	SDL_mutex *hw_lock;
	int sync_needed;
	int was_flipped;

	/* Information for hardware surfaces */
	vidmem_bucket surfaces;
	int surfaces_memtotal;
	int surfaces_memleft;

	/* Information for double-buffering */
	int flip_page;
	int flip_yoffset[2];
	Uint8 *flip_address[2];

	/* Used to handle DGA events */
	int event_base;
#ifdef LOCK_DGA_DISPLAY
	SDL_mutex *event_lock;
#endif

	/* Screensaver settings */
	int screensaver_timeout;
	BOOL dpms_enabled;
};
/* Old variable names */
#define DGA_Display		(this->hidden->DGA_Display)
#define DGA_Screen		DefaultScreen(DGA_Display)
#define DGA_colormap		(this->hidden->DGA_colormap)
#define DGA_visualClass		(this->hidden->visualClass)
#define memory_base		(this->hidden->memory_base)
#define memory_pitch		(this->hidden->memory_pitch)
#define flip_page		(this->hidden->flip_page)
#define flip_yoffset		(this->hidden->flip_yoffset)
#define flip_address		(this->hidden->flip_address)
#define sync_needed		(this->hidden->sync_needed)
#define was_flipped		(this->hidden->was_flipped)
#define SDL_nummodes		(this->hidden->SDL_nummodes)
#define SDL_modelist		(this->hidden->SDL_modelist)
#define surfaces		(this->hidden->surfaces)
#define surfaces_memtotal	(this->hidden->surfaces_memtotal)
#define surfaces_memleft	(this->hidden->surfaces_memleft)
#define hw_lock			(this->hidden->hw_lock)
#define DGA_event_base		(this->hidden->event_base)
#define event_lock		(this->hidden->event_lock)
#define screensaver_timeout	(this->hidden->screensaver_timeout)
#define dpms_enabled		(this->hidden->dpms_enabled)

#endif /* _SDL_dgavideo_h */
