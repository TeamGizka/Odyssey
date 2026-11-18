vs.1.1 
dp4   oPos.x, c[0], v0;
dp4   oPos.y, c[1], v0;
dp4   oPos.z, c[2], v0;
dp4   oPos.w, c[3], v0;
dp4   r10.x, c[8], v0;
dp4   r10.y, c[9], v0;
dp4   r10.z, c[10], v0;
dp4   r10.w, c[11], v0;
dp3   r1.w, r10, r10;
rsq   r1.w, r1.w;
rcp   r1.w, r1.w;
mov  oFog.x, -r1.w;
dp3   r11.x, c[8], v3;
dp3   r11.y, c[9], v3;
dp3   r11.z, c[10], v3;
mov   r9, c[15].wwwz;
mov   oT0, v7;
mov   oT1, v8;
