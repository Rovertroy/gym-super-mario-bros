/// \file
/// \brief Handles emulation speed throttling using the SDL timing functions.

#include "sdl.h"
#include "throttle.h"

static const double Slowest = 0.015625; // 1/64x speed (around 1 fps on NTSC)
static const double Fastest = 1440;       // 32x speed   (around 1920 fps on NTSC)
static const double Normal  = 1.0;      // 1x speed    (around 60 fps on NTSC)

static uint64 Lasttime, Nexttime;
static long double desired_frametime;
static int InFrame;
double g_fpsScale = Fastest; // used by sdl.cpp
bool MaxSpeed = false;

/* LOGMUL = exp(log(2) / 3)
 *
 * This gives us a value such that if we do x*=LOGMUL three times,
 * then after that, x is twice the value it was before.
 *
 * This gives us three speed steps per order of magnitude.
 *
 */
#define LOGMUL 1.259921049894873

/**
 * Refreshes the FPS throttling variables.
 */
void
RefreshThrottleFPS()
{
	uint64 fps = FCEUI_GetDesiredFPS(); // Do >> 24 to get in Hz
	desired_frametime = 16777216.0l / (fps * g_fpsScale);

	Lasttime=0;
	Nexttime=0;
	InFrame=0;
}

/**
 * Perform FPS speed throttling by delaying until the next time slot.
 */
int
SpeedThrottle()
{
	return 0;
}

/**
 * Set the emulation speed throttling to the next entry in the speed table.
 */
void IncreaseEmulationSpeed(void)
{
	g_fpsScale *= LOGMUL;

	if(g_fpsScale > Fastest) g_fpsScale = Fastest;

	RefreshThrottleFPS();

	FCEU_DispMessage("Emulation speed %.1f%%",0, g_fpsScale*100.0);
}

/**
 * Set the emulation speed throttling to the previous entry in the speed table.
 */
void DecreaseEmulationSpeed(void)
{
	g_fpsScale /= LOGMUL;
	if(g_fpsScale < Slowest)
		g_fpsScale = Slowest;

	RefreshThrottleFPS();

	FCEU_DispMessage("Emulation speed %.1f%%",0, g_fpsScale*100.0);
}

/**
 * Set the emulation speed throttling to a specific value.
 */
void
FCEUD_SetEmulationSpeed(int cmd)
{
	MaxSpeed = false;

	switch(cmd) {
	case EMUSPEED_SLOWEST:
		g_fpsScale = Slowest;
		break;
	case EMUSPEED_SLOWER:
		DecreaseEmulationSpeed();
		break;
	case EMUSPEED_NORMAL:
		g_fpsScale = Normal;
		break;
	case EMUSPEED_FASTER:
		IncreaseEmulationSpeed();
		break;
	case EMUSPEED_FASTEST:
		g_fpsScale = Fastest;
		MaxSpeed = true;
		break;
	default:
		return;
	}

	RefreshThrottleFPS();

	FCEU_DispMessage("Emulation speed %.1f%%",0, g_fpsScale*100.0);
}
