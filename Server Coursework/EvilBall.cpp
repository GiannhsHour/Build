#include "EvilBall.h"
#include <ncltech\Scene.h>

EvilBall::EvilBall()
	: GameObject("evil")

{
	state_name = "patrol";
}

EvilBall::~EvilBall()
{
}

