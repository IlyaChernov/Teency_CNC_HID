float BytesToFloat(byte byte0, byte byte1, byte byte2, byte byte3){
  union
{
byte asBytes[4];
float asFloat;
} dataToFloat; 

dataToFloat.asBytes[0]=byte0;
dataToFloat.asBytes[1]=byte1;
dataToFloat.asBytes[2]=byte2;
dataToFloat.asBytes[3]=byte3;

return dataToFloat.asFloat;
}

long BytesToLong(byte byte0, byte byte1, byte byte2, byte byte3){
  union
{
byte asBytes[4];
long asLong;
} dataToLong; 

dataToLong.asBytes[0]=byte0;
dataToLong.asBytes[1]=byte1;
dataToLong.asBytes[2]=byte2;
dataToLong.asBytes[3]=byte3;

return dataToLong.asLong;
}

MoveCommand InterpolateLinear(long OldX, long NewX, long OldY, long NewY, long OldZ, long NewZ, long OldA, long NewA, long OldB, long NewB, long OldC, long NewC, float speedXYZ, float speedABC)
{ 
  MoveCommand MC;
  
  MC.X = NewX;
  MC.Y = NewY;
  MC.Z = NewZ;
  MC.A = NewA;
  MC.B = NewB;
  MC.C = NewC;

  long DistX = NewX == 2147483647 ? 0 : abs(NewX - OldX);
  long DistY = NewY == 2147483647 ? 0 : abs(NewY - OldY);
  long DistZ = NewZ == 2147483647 ? 0 : abs(NewZ - OldZ);
  long DistA = NewA == 2147483647 ? 0 : abs(NewA - OldA);
  long DistB = NewB == 2147483647 ? 0 : abs(NewB - OldB);
  long DistC = NewC == 2147483647 ? 0 : abs(NewC - OldC);

  double DistXYZ = sqrt(sq(DistX) + sq(DistY) + sq(DistZ));
  double DistABC = sqrt(sq(DistA) + sq(DistB) + sq(DistC));

  MC.SpeedX = speedXYZ /  DistXYZ * DistX;
  MC.SpeedY = speedXYZ /  DistXYZ * DistY;
  MC.SpeedZ = speedXYZ /  DistXYZ * DistZ;
  MC.SpeedA = speedABC /  DistABC * DistA;
  MC.SpeedB = speedABC /  DistABC * DistB;
  MC.SpeedC = speedABC /  DistABC * DistC;

  return MC;
}
