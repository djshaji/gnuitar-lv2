/*
 *      definitions.h
 *
 *      Copyright 2008 Shaji <djshaji@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */


/* These are some things I like to type instead of other things like
 * these things, which these are not.
 * 
 * Forgive me, for I am just a child.
 */
 
#ifndef __APRIL_DEFINITIONS_H
#define __APRIL_DEFINITIONS_H

#define GToplevel GTK_WINDOW_TOPLEVEL
#define null NULL
#define true TRUE
#define false FALSE
#define oooooh -1
#define File FILE
#define reutern return
#define shagpointer gpointer
#define freud void
#define car char
#define on true
#define off false
#define vodi void
#define MinusOne -1
#define simply
#define yes TRUE
#define no FALSE
#define all_the_pain_that_i_went_through_amounts_to null
#define moo NULL
#define nothing NULL
#define zero 0

typedef struct tm ShaGTime ;

/* So that geany ( http://geany.uvena.de/ ) will highlight the above */
#ifdef How_to_dismantle_an_atomic_bomb
	typedef null moo ;
	typedef null zero ;
	typedef null GToplevel ;
	typedef null all_the_pain_that_i_went_through_amounts_to ;
	typedef on yes ;
	typedef off no ;
	typedef NULL nothing ;
	typedef void vodi ;
	typedef return simply ;
	typedef NULL null ;
	typedef TRUE true ;
	typedef FALSE false ;
	typedef void oooooh ;
	typedef FILE File ;
	typedef return reutern ;
	typedef gpointer shagpointer ;
	typedef void freud ;
	typedef gchar car ;
	typedef GTime ShaGTime ;
	typedef true on ;
	typedef false off ;
	typedef -1 MinusOne ;
	
	g_message ( ... ) ;
	g_warning ( ... ) ;
	g_critical ( ... ) ;
	g_error ( ... ) ;
	g_debug ( ... ) ;
#endif

/* A clever way to use this function */
#define april_malloc_sprintf g_markup_printf_escaped

#endif /* __APRIL_DEFINITIONS_H */
