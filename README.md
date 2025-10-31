# Gesture Reader

> An arduino program that constantly polls the potentiometer to check the arduino's orientation. It then transfers the orientation into states which it uses to track motions.
>
> The known motions are:
> - Reset - REST -> rotate left -> rotate back
> - Curl - REST -> rotate up -> rotate back
> - Flip - REST -> flip -> flip
> - Fall - Rotated up -> back to rest -> maintain rest
