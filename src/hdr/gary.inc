
/*
	return UTD info
	driver version
	firmware version

*/

string GetUTDInfo(void)
{
	string res;
	string driver=Get_d8_driver();
	string firmware=utd->GetFirmwareVersion();

	res="UTD DRIVER -"+ driver + ":UTD FIRMWARE - "+firmware;
	return res;
}


string GetLockInfo(int index)
{
	char ld[20];
	string res;
	string il;
	string locktype=GetLockType(index);
	int lockdelay=GetLockDelay(index);
	string lockdriver=GetLockDriverVersion();
	string lockfirmware=GetLockFirmware(index);
	string lockvolts=GetLockVolts(index);
	string lockserial=GetLockSerial(index);
	bool islocked=GetIsLocked(index);

	sprintf(ld,"%d",lockdelay);
	if (islocked)
		il="YES";
	else
		il="NO";

	res="LOCK TYPE -" +locktype+":LOCK DELAY -" + string(ld) + ":LOCK DRIVER -"+ lockdriver + ":LOCK FIRMWARE -"+ lockfirmware +":LOCK VOLTS -"+lockvolts+":LOOCK SERIAL -"+lockserial +":ISLOCKED -"+ il +":";

	return res;
}

