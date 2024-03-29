/*--------------------------------------------------------*/
/*                                                        */
/* SDL puzzle project - for COMIKET62                     */
/*                        Fumi2Kick/LiMo/omamori-2002     */
/*                        1st Maintaner  Rerorero@fumi.   */
/*                                                        */
/*   sound.pp                                             */
/*     簡易サウンド(SDL_mixer使用)                        */
/*                                                        */
/*--------------------------------------------------------*/
/* -- $Id: sound.pp,v 1.3 2002/08/10 03:05:14 rero2 Exp $ */

/*------------------------------------------------------------- */
/** @file
    @brief		サウンドマネージ��E    @author		K.Kunikane (rerofumi)
    @since		Sep.04.2005
    $Revision: 1.1.1.1 $
*/
/*-----------------------------------------------------
 Copyright (C) 2002,2005 rerofumi <rero2@yuumu.org>
 All Rights Reserved.
 ------------------------------------------------------*/

/*-------------------------------*/
/* include                       */
/*-------------------------------*/

#include "default.h"

/*-------------------------------*/
/* define                        */
/*-------------------------------*/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*-------------------------------*/
/* local value                   */
/*-------------------------------*/

/* ----- BGM トラック */
Mix_Music *SoundTrack;
char *BGMPool[BGMMAX];
char *BGMPoolList[BGMMAX + 1] = {
	"",
	"sound/music/gameover.ogg",
	"sound/music/title.ogg",
	"sound/music/network.ogg",
	"sound/music/stage1.ogg",
	"sound/music/stage2.ogg",
	"sound/music/stage3.ogg",
	"sound/music/stage4.ogg",
	"sound/music/stage5.ogg",
	"sound/music/stage6.ogg",
	"sound/music/freeplay.ogg"
};

/* ----- SE トラック */
Mix_Chunk *SEPool[SEMAX];
char *SEPoolList[SEMAX + 1] = {
	"",
	"sound/menu/init.wav",
	"sound/menu/start.wav",
	"sound/game/get_ready.wav",
	"sound/game/clean.wav",
	"sound/game/clear.wav",
	"sound/menu/enter.wav",
	"sound/game/fall.wav",
	"sound/game/null.wav",
	"sound/voice/title.wav",
	"sound/menu/cancel.wav",
	"sound/menu/decision.wav",
	"sound/menu/move.wav",
	"sound/voice/bvs.wav",
	"sound/voice/singleplayer.wav",
	"sound/voice/multiplayer.wav",
	"sound/voice/game_settings.wav",
	"sound/voice/game_rules.wav",
	"sound/voice/quit.wav",
	"sound/voice/ready.wav",
	"sound/voice/see_you_later.wav",
	"sound/voice/bye_bye.wav",
	"sound/voice/new_level.wav",
	"sound/voice/new_stage.wav",
	"sound/voice/gameover.wav",
	"sound/menu/connect.wav",
	"sound/game/talk.wav"
};

/* ----- サウンドチャンネ��E離肇薀奪� */
int  track;

/* ----- サウンド有効フラグ */
int  SoundEnable;

/* -------------------------------------------------------------- */
/* --- サウンド                                                   */
/* -------------------------------------------------------------- */

/* ---------------------------------------- */
/* --- サウンドの初��E肇�E好汎匹濆�み       */
/* ---------------------------------------- */

void  SoundInit(void)
{
  int  i, ret;
#ifdef LIBPAK
  unsigned int size;
  byte *buffer;
#endif

  track = 0;
  SoundEnable = TRUE;

  if (sys_sound == 0)
    return;

  /* ----- SDL_mixer のオープ��E*/
  ret = Mix_OpenAudio (SND_FREQ, SND_FORMAT, SND_CHAN, SND_SAMPLE);
  if (ret != 0) {
    SoundEnable = FALSE;
    printf ("Mix_OpenAudio failed: %s\n", Mix_GetError ());
    return;
  }

  SoundVolume ();

  /* ----- 配列ク��E� */
  for (i = 0; i < BGMMAX; i++) {
    BGMPool[i] = va ("%s/%s", BASEDIR, BGMPoolList[i]);
  }

  for (i = 0; i < SEMAX; i++) {
#ifdef LIBPAK
    buffer = extractMem(SEPoolList[i], openPak("data.pak"), &size);
    if (buffer == NULL)
      return;

    SEPool[i] = Mix_LoadWAV_RW (SDL_RWFromMem (buffer, size), 0);
#else
    SEPool[i] = Mix_LoadWAV (va ("%s/%s", BASEDIR, SEPoolList[i]));
#endif
    if (SEPool[i] == NULL)
      printf ("Mix_LoadWAV failed: Cannot open %s\n", SEPoolList[i]);
  }
}


/* ---------------------------------------- */
/* --- サウンドの解��E                      */
/* ---------------------------------------- */
void  SoundFree(void)
{
  int i = 0;

  if (SoundEnable == FALSE)
    return;

  /* ----- 保持サウンドの解��E*/
  if (SoundTrack != 0) {
    Mix_HaltMusic();
    Mix_FreeMusic(SoundTrack);
    SoundTrack = 0;
  }

  /* ----- SEチャンネ��E猟篁� */
  for(i=0; i<8; i++)
    Mix_HaltChannel(i);

  for (i = 0; i < BGMMAX; i++)
    BGMPool[i] = BGMPoolList[i] = 0;

  /* ----- SE chank の解��E*/
  for(i=0; i<SEMAX; i++) {
    if (SEPool[i] != 0) {
      Mix_FreeChunk(SEPool[i]);
      SEPool[i] = 0;
    }
  }

  /* ----- SDL_Mixer のク��充ズ */
  Mix_CloseAudio();
}


/* ---------------------------------------- */
/* --- BGM の��E�エスト                     */
/* ---------------------------------------- */
void  SoundMusic(int req)
{
#ifdef LIBPAK
  unsigned int size;
  byte *buffer;
#endif
  if (SoundEnable == FALSE) return;

  /* --- 範囲チェック */
  if (req < 0) return;
  if (req >= BGMMAX) return;
  if (BGMPool[req][0] == 0) return;

  /* ----- そ��E泙任留藾嫩篁� */
  if (SoundTrack != 0) {
    Mix_HaltMusic();
    Mix_FreeMusic(SoundTrack);
    SoundTrack = 0;
  }

  /* ----- BGM 演奏開始 */
#ifdef LIBPAK
  buffer = extractMem(BGMPool[req], openPak("data.pak"), &size);
  if (buffer == NULL)
    return;

#if defined(VS_WIN8)
  SoundTrack = Mix_LoadMUS_RW (SDL_RWFromMem (buffer, size));
#else
  SoundTrack = Mix_LoadMUS_RW (SDL_RWFromMem (buffer, size), 0);
#endif
#else
  SoundTrack = Mix_LoadMUS(BGMPool[req]);
#endif
  if (SoundTrack == NULL) {
    printf ("Mix_LoadMUS failed: Cannot open %s\n", BGMPool[req]);
    SoundTrack = 0;
    return;
  }

  Mix_PlayMusic(SoundTrack, -1);
}

/* ---------------------------------------- */
/* --- 繰��E屬気覆� BGM の��E�エスト        */
/* ---------------------------------------- */
void  SoundMusicOneshot(int  req)
{
#ifdef LIBPAK
  unsigned int size;
  byte *buffer;
#endif
  if (SoundEnable == FALSE) return;

  /* --- 範囲チェック */
  if (req < 0) return;
  if (req >= BGMMAX) return;
  if (BGMPool[req][0] == 0) return;

  /* ----- そ��E泙任留藾嫩篁� */
  if (SoundTrack != 0) {
    Mix_HaltMusic();
    Mix_FreeMusic(SoundTrack);
    SoundTrack = 0;
  }

  /* ----- BGM 演奏開始 */
#ifdef LIBPAK
  buffer = extractMem(BGMPool[req], openPak("data.pak"), &size);
  if (buffer == NULL)
    return;

#if defined(VS_WIN8)
  SoundTrack = Mix_LoadMUS_RW (SDL_RWFromMem (buffer, size));
#else
  SoundTrack = Mix_LoadMUS_RW (SDL_RWFromMem (buffer, size), 0);
#endif
#else
  SoundTrack = Mix_LoadMUS(BGMPool[req]);
#endif
  if (SoundTrack == NULL) {
    printf ("Mix_LoadMUS failed: Cannot open %s\n", BGMPool[req]);
    SoundTrack = 0;
    return;
  }

  Mix_PlayMusic(SoundTrack, 1);
}

/* ---------------------------------------- */
/* --- SE の��E�エスト                      */
/* ---------------------------------------- */
void  SoundSE(int req)
{
  if (SoundEnable == FALSE) return;

  /* --- 範囲チェック */
  if (req < 0) return;
  if (req >= BGMMAX) return;
  if (SEPool[req] == 0) return;

  /* ----- SE 発呼 */
  Mix_PlayChannel(track, SEPool[req], 0);
  track = (track + 1) % 8;
}

/* ---------------------------------------- */
/* --- BGM トラックの停止                   */
/* ---------------------------------------- */
void  SoundMusicStop(void)
{
  if (SoundEnable == FALSE) return;

  Mix_HaltMusic();
}

/* ---------------------------------------- */
/* --- SE の全停止                          */
/* ---------------------------------------- */
void  SoundSEStop(void)
{
  int i = 0;

  if (SoundEnable == FALSE)
    return;

  /* ----- SEチャンネ��E猟篁� */
  for(i=0; i<8; i++)
    Mix_HaltChannel(i);
}

/* ---------------------------------------- */
/* --- Volume値の設��E                     */
/* ---------------------------------------- */
void  SoundVolume (void)
{
  int i = 0;

  for(i=0; i < 8; i++)
    Mix_Volume(i, sys_se_volume);
  Mix_VolumeMusic(sys_bgm_volume);
}
