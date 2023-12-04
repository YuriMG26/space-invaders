#include "Animator.h"

Animator CreateEmptyAnimator(void)
{
  Animator result = {0};

  return result;
}

Animator CreateAnimatorWithFrames(FrameArray frames)
{
  Animator result = {0};

  result.frame_array = frames;

  return result;
}
