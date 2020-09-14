

int modeSelect() //this is the actual start of program
{

    //start message

    displayString(0, "Please select a mode.");
    displayString(1, "Press the left button for stain-wiping mode.");
    displayString(2, "Press the right button for garbage-collector mode.");

    while (!getButtonPress(buttonLeft) && !getButtonPress(buttonRight)) //wait for button push
    {}

    if(getButtonPress(buttonLeft))
        return 1; //I'm really tempted to set these as constants but idk how without setting global constants which they hate??

    else if(getButtonPress(buttonRight))
        return 0; //Jackie: changed to 0
}

void slowTurn(int mpA, int mpD) // can implement if needed
{
	resetGyro();

	motor[motorA] = mpA;
	motor[motorD] = mpD;

	while(fabs(getGyroDegrees(S3)) < 60)
	{}

	motor[motorA] = mpA/2;
	motor[motorD] = mpD/2;

	while(abs(getGyroDegrees(S3)) < 90)
	{}

	motor[motorA] = motor[motorD] = 0;
}







void measureTable(int & lengthCount, int & widthCount, float ROBOTHEIGHT, int MPTURNA, int MPTURND)
{

    nMotorEncoder[motorA] = 0;
    motor[motorA] = motor[motorB] = 20;

    while (SensorValue[S1] <= ROBOTHEIGHT) // could change to a TOL
    {}

    motor[motorA] = motor[motorD] = 0;
    lengthCount = nMotorEncoder[motorA];

	slowTurn(MPTURNA, MPTURND);

    nMotorEncoder[motorA] = 0;
    motor[motorA] = motor[motorB] = 20;

    while (SensorValue[S1] <= ROBOTHEIGHT)
    {}

    motor[motorA] = motor[motorD] = 0;
    widthCount = nMotorEncoder[motorA];
}


void pickUpGarbage(int REDUCED_MP, int NORMAL_MP, float TABLELENGTH) //triggered by ultrasonic detecting height less than what it should be
{
    motor[motorA] = motor[motorD] = REDUCED_MP;
		nMotorEncoder[motorD] = 0;
		while(nMotorEncoder[motorD] < 360 && nMotorEncoder[motorA] < TABLELENGTH)
		motor[motorA] = motor[motorD] = 0;
		nMotorEncoder[motorB] = 0;
    motor[motorB] = 20; //how fast do you want this boy to pick things up

    while(nMotorEncoder[motorB] < 180) //when it completes a rotation, it'll have deposited garbage in compartment
    {}

    motor[motorB] = -50;

    while(nMotorEncoder[motorB] > 0) // lower shovel back to rest position
    {}

    motor[motorA] = motor[motorD] = NORMAL_MP;
} //question now is how to implement the unexpected case?



void wipeStain(int REDUCED_MP, int NORMAL_MP)
{
    motor[motorA] = motor[motorD] = REDUCED_MP;
    nMotorEncoder[motorA] = 0;
    motor[motorC] = 70; //brushes turn on
	// may need to back up robot depending ond location of coour sensor
    //while(SensorValue[S2] != (int)colorWhite)
    //{}
	time1[T1] = 0;
	while(time1[T1] < 5000)
	{}

    motor[motorC] = 0;
    motor[motorA] = motor[motorD] = NORMAL_MP;
}


bool turnRobot (float ROBOTWIDTH, bool turnRight, int mpA, int mpD)
{
	int motorPowerA, motorPowerD = 0;
	resetGyro();

	if (turnRight == false)
	{
		motorPowerA = -mpA;
		motorPowerD = -mpD;
	}
	else
	{
		motorPowerA = mpA;
		motorPowerD = mpD;
	}
	resetGyro();

	motor[motorA] = motorPowerA;
	motor[motorD] = motorPowerD;

	while (abs(getGyroDegrees(S3)) < 90)
	{}
	motor[motorA] = motor[motorD] = 0;

	wait1Msec(250);

	nMotorEncoder[motorA] = 0;

	motor[MotorA] = motor[MotorD] = 25;

	while (nMotorEncoder[motorA] < ROBOTWIDTH) // maybe make distance a constant
	{}

	motor[MotorA] = motor[MotorD] = 0;

	wait1Msec(250);

	resetGyro();

	motor[motorA] = motorPowerA;
	motor[motorD] = motorPowerD;

	while (abs(getGyroDegrees(S3)) < 90)
	{}

	motor[motorA] = motor[motorD] = 0;

	if(turnRight == false)
		turnRight = true;
	else
		turnRight = false;

	wait1Msec(250);

	return turnRight;
}





void driveRobotP1 (float TABLELENGTH, int mode, float ROBOTHEIGHT, int REDUCED_MP, int NORMAL_MP) //driving distance is the length of the table
{
	nMotorEncoder[motorA] = 0;
	motor[motorA] = motor[motorD] = NORMAL_MP; // idk is 50 enough?? // also change this to normal MP is it is not 50

	while (nMotorEncoder < TABLELENGTH && SensorValue[S1] > ROBOTHEIGHT) // maybe use tolerance here
	{
		if (SensorValue[S2] == (int)colorBlack && mode == 1) // 1 is stain cleaning
		{
			motor[motorA] = motor[motorD] = 0;
			wipeStain(REDUCED_MP, NORMAL_MP);
		}
		else if (SensorValue[S1] < ROBOTHEIGHT && mode == 0)
		{
			motor[motorA] = motor[motorD] = 0;
			pickUpGarbage(REDUCED_MP, NORMAL_MP, TABLELENGTH);// call garbage collection function
		}
	}

	motor[motorA] = motor[motorD] = 0;
}

void finalCheck(float TABLELENGTH, float TABLEWIDTH, float ROBOTWIDTH, float ROBOTHEIGHT, int MPTURNA, int MPTURND) // consider splitting into 2 functions
{
	for (int turn = 0; turn < TABLELENGTH/ROBOTWIDTH; turn++) //can make a constant later on, ok nvm definitely make this a constatnt
	{
		bool secondTurn = false; // basically i will explain this to u in person cause it is too hard to explain here
		bool error = false;

		for (int count = 0; count < 2; count++)
		{

			nMotorEncoder[motorA] = 0;
			motor[motorA] = motor[motorD] = 50;
			while (nMotorEncoder < TABLEWIDTH && SensorValue[S1] < ROBOTHEIGHT && !error) //add tolerance
			{
				if (SensorValue[S2] == (int)colorBlack || SensorValue[S1] < ROBOTHEIGHT)
				{
					error = true;
				}
			}
			motor[motorA] = motor[motorD] = 0;
			if (!error)
			{
				slowTurn(MPTURNA, MPTURND);
			}

			if(secondTurn && !error)
			{
				nMotorEncoder[motorA] = 0;
				motor[motorA] = motor[motorD] = 50;
				while (nMotorEncoder < TABLELENGTH/2 && !error)
				{
					if (SensorValue[S2] == (int)colorBlack || SensorValue[S1] < ROBOTHEIGHT)
					{
						error = true;
					}
				}

				motor[motorA] = motor[motorD] = 0;

				if(!error)
				{
					slowTurn(MPTURNA, MPTURND);
				}

			}
			else if(!secondTurn && !error)
			{
				nMotorEncoder[motorA] = 0;
				motor[motorA] = motor[motorD] = 50;
				while (nMotorEncoder < TABLELENGTH/2 + ROBOTWIDTH && !error)
				{
					if (SensorValue[S2] == (int)colorBlack || SensorValue[S1] < ROBOTHEIGHT)
					{
						error = true;
					}
				}

				motor[motorA] = motor[motorD] = 0;
				secondTurn = true;
				if (!error)
				{
					slowTurn(MPTURNA, MPTURND);
				}
			}
			if (error)
			{
				count = 2;
				turn = 100;
				displayString (1, "Error has occured, please notify waiter");
				wait1MSec(10000);
				//make sound
			}
		}
	}
}



task main()
{
	const int MPTURNA = 50; // make these the value to turn the robot CW or Right
	const int MPTURND = -50;
	const int REDUCED_MP = 10;
	const int NORMAL_MP = 50; // can change these depending on testing
	const float HEIGHTLIMIT = // something we measure
	const float ROBOTLENGTH;
	const float ROBOTWIDTH;
	const float ROBOTHEIGHT;

	int mode = 0;
	float tempTableLength = 0;
	float tempTableWidth = 0;
	bool turnRight = true;

	SensorType[S1] = sensorEV3_Ultrasonic;
	SensorType[S2] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S2] = modeEV3Color_Color;
	wait1Msec(50);
	SensorType[S3] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S3] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);

	mode = modeSelect();

	measureTable (tempTableLength, tempTableWidth, ROBOTHEIGHT, MPTURNA, MPTURND);//measure table function with pass by reference
	const float TABLEWIDTH = tempTableWidth;
	const float TABLELENGTH = tempTableLength;
	const float NUMOFTURN = int(TABLEWIDTH/ROBOTWIDTH); // idk if this is the correct thign but i want to make it round down to the closest integer

	slowTurn (MPTURNA, MPTURND);

	for(int turn = 0; turn < NUMOFTURN; turn++)
	{
		driveRobotP1 (TABLELENGTH, mode, ROBOTHEIGHT, REDUCED_MP, NORMAL_MP);
		turnRight = turnRobot (ROBOTWIDTH, turnRight, MPTURNA, MPTURND);
	}

	if (NUMOFTURN%2 == 0) // to check if its even
	{
		slowTurn (MPTURNA, MPTURND);
		slowTurn (MPTURNA, MPTURND);
		nMotorEncoder[motorA] = 0;
		motor[motorA] = motor[motorD] = NORMAL_MP;
		while (nMotorEncoder[motorA] < TABLELENGTH && SensorValue[S1] <= ROBOTHEIGHT)
		{}
		motor[motorA] = motor[motorD] = 0;
		slowTurn (MPTURNA, MPTURND);
	}
	else // if odd, also we can make these into functions if needed
	{
		slowTurn (MPTURNA, MPTURND);
	}

	finalCheck(TABLELENGTH, TABLEWIDTH, ROBOTWIDTH, ROBOTHEIGHT, MPTURNA, MPTURND);
}
