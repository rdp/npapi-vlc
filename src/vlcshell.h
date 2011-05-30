/*****************************************************************************
 * vlcshell.h:
 *****************************************************************************
 * Copyright (C) 2009-2010 the VideoLAN team
 * $Id$
 *
 * Authors: Jean-Paul Saman <jpsaman@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef __VLCSHELL_H__
#define __VLCSHELL_H__

char * NPP_GetMIMEDescription( void );

NPError NPP_Initialize( void );

#ifdef OJI
jref NPP_GetJavaClass( void );
#endif
void NPP_Shutdown( void );

#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
NPError NPP_New( NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc,
                 char* argn[], char* argv[], NPSavedData* saved );
#else
NPError NPP_New( NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc,
                 char* argn[], char* argv[], NPSavedData* saved );
#endif

NPError NPP_Destroy( NPP instance, NPSavedData** save );

NPError NPP_GetValue( NPP instance, NPPVariable variable, void *value );
NPError NPP_SetValue( NPP instance, NPNVariable variable, void *value );

NPError NPP_SetWindow( NPP instance, NPWindow* window );

NPError NPP_NewStream( NPP instance, NPMIMEType type, NPStream *stream,
#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
                       NPBool seekable, uint16 *stype );
#else
                       NPBool seekable, uint16_t *stype );
#endif
NPError NPP_DestroyStream( NPP instance, NPStream *stream, NPError reason );
void NPP_StreamAsFile( NPP instance, NPStream *stream, const char* fname );

#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
int32 NPP_WriteReady( NPP instance, NPStream *stream );
int32 NPP_Write( NPP instance, NPStream *stream, int32 offset,
                 int32 len, void *buffer );
#else
int32_t NPP_WriteReady( NPP instance, NPStream *stream );
int32_t NPP_Write( NPP instance, NPStream *stream, int32_t offset,
                 int32_t len, void *buffer );
#endif

void NPP_URLNotify( NPP instance, const char* url,
                    NPReason reason, void* notifyData );
void NPP_Print( NPP instance, NPPrint* printInfo );

#ifdef XP_MACOSX
#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
int16 NPP_HandleEvent( NPP instance, void * event );
#else
int16_t NPP_HandleEvent( NPP instance, void * event );
#endif
#endif

static char mimetype[] =
    /* MPEG-1 and MPEG-2 */
    "audio/mpeg:mp2,mp3,mpga,mpega:MPEG audio;"
    "audio/x-mpeg:mp2,mp3,mpga,mpega:MPEG audio;"
    "video/mpeg:mpg,mpeg,mpe:MPEG video;"
    "video/x-mpeg:mpg,mpeg,mpe:MPEG video;"
    "video/mpeg-system:mpg,mpeg,mpe,vob:MPEG video;"
    "video/x-mpeg-system:mpg,mpeg,mpe,vob:MPEG video;"
    /* MPEG-4 */
    "audio/mp4:aac,mp4,mpg4:MPEG-4 audio;"
    "audio/x-m4a:m4a:MPEG-4 audio;"
    /* MPEG-4 ASP */
    "video/mp4:mp4,mpg4:MPEG-4 video;"
    "application/mpeg4-iod:mp4,mpg4:MPEG-4 video;"
    "application/mpeg4-muxcodetable:mp4,mpg4:MPEG-4 video;"
    "video/x-m4v:m4v:MPEG-4 video;"
    /* AVI */
    "video/x-msvideo:avi:AVI video;"
    /* QuickTime */
    /* "video/quicktime:mov,qt:QuickTime video;" */
    /* OGG */
    "application/ogg:ogg:Ogg stream;"
    "video/ogg:ogv:Ogg video;"
    "application/x-ogg:ogg:Ogg stream;"
    /* VLC */
    "application/x-vlc-plugin:vlc:VLC plug-in;"
    /* Windows Media */
    "video/x-ms-asf-plugin:asf,asx:Windows Media Video;"
    "video/x-ms-asf:asf,asx:Windows Media Video;"
    "application/x-mplayer2::Windows Media;"
    "video/x-ms-wmv:wmv:Windows Media;"
    "video/x-ms-wvx:wvx:Windows Media Video;"
    "audio/x-ms-wma:wma:Windows Media Audio;"
    /* Google VLC */
    "application/x-google-vlc-plugin::Google VLC plug-in;"
    /* WAV audio */
    "audio/wav:wav:WAV audio;"
    "audio/x-wav:wav:WAV audio;"
    /* 3GPP */
    "audio/3gpp:3gp,3gpp:3GPP audio;"
    "video/3gpp:3gp,3gpp:3GPP video;"
    /* 3GPP2 */
    "audio/3gpp2:3g2,3gpp2:3GPP2 audio;"
    "video/3gpp2:3g2,3gpp2:3GPP2 video;"
    /* DIVX */
    "video/divx:divx:DivX video;"
    /* FLV */
    "video/flv:flv:FLV video;"
    "video/x-flv:flv:FLV video;"
    /* Matroska */
    "video/x-matroska:mkv:Matroska video;"
    "audio/x-matroska:mka:Matroska audio;"
    /* XSPF */
    "application/xspf+xml:xspf:Playlist xspf;"
    /* M3U */
    "audio/x-mpegurl:m3u:MPEG audio;"
    /* Webm */
    "video/webm:webm:WebM video;"
    "audio/webm:webm:WebM audio;"
    /* Real Media */
    "application/vnd.rn-realmedia:rm:Real Media File;"
    "audio/x-realaudio:ra:Real Media Audio;"
    /* AMR */
    "audio/amr:amr:AMR audio;"
    ;


#endif
