// RobotBuilder Version: 1.0
//
// This file was generated by RobotBuilder. It contains sections of
// code that are automatically generated and assigned by robotbuilder.
// These sections will be updated in the future when you export to
// C++ from RobotBuilder. Do not put any code or make any change in
// the blocks indicating autogenerated code or it will be lost on an
// update. Deleting the comments indicating the section will prevent
// it from being updated in th future.
#include "Robot.h"
// BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=INITIALIZATION
DriveTrain* Robot::driveTrain = 0;
Shooter* Robot::shooter = 0;
Pickup* Robot::pickup = 0;
OI* Robot::oi = 0;
    // END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=INITIALIZATION
BSTimer* autoStepTimer = NULL;
void Robot::RobotInit() {
	RobotMap::init();
	// BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=CONSTRUCTORS
	driveTrain = new DriveTrain();
	shooter = new Shooter();
	pickup = new Pickup();
    // END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=CONSTRUCTORS
	// This MUST be here. If the OI creates Commands (which it very likely
	// will), constructing it during the construction of CommandBase (from
	// which commands extend), subsystems are not guaranteed to be
	// yet. Thus, their requires() statements may grab null pointers. Bad
	// news. Don't move it.
	oi = new OI();
	lw = LiveWindow::GetInstance();
	// instantiate the command used for the autonomous period
	prevTrigger = false;
	
	File = RAWCConstants::getInstance();
	Robot::driveTrain->SetWheelbase(20.625/2, 20.625, 20.625/2);
	FLOffset = File->getValueForKey("FLOff");
	FROffset = File->getValueForKey("FROff");
	RLOffset = File->getValueForKey("RLOff");
	RROffset = File->getValueForKey("RROff");
	Robot::driveTrain->SetOffsets(FLOffset, FROffset, RLOffset, RROffset);
		
	Robot::pickup->comp->Start();
	
	autoStep = Initiate;
	autoProgram = fire2FromCenter;
	autoStepComplete = false;
	autoStepIncrementer = 0;
	turnDirection = 0;
	
	autoChooser = new SendableChooser();
	autoChooser->AddDefault("1. Shoot 1 From Left", (void*)fire1Left);
	autoChooser->AddObject("2. Shoot 1 From Right", (void*)fire1Right);
	autoChooser->AddObject("3. Shoot 2 From Center", (void*)fire2FromCenter);
	autoChooser->AddObject("4. Shoot 3 From Center", (void*)fire3FromCenter);
	SmartDashboard::PutData("Autonomous Chooser", autoChooser);
	autoStepTimer = new BSTimer();
	autoStepTimer->Start();
	onTargetTimer = new BSTimer();
	onTargetTimer->Start();
	autonomousTimer = new BSTimer();
	autonomousTimer->Start();
	beaterBarTimer = new BSTimer();
	beaterBarTimer->Start();
	
	Robot::driveTrain->sendProcessImage->Set(1);
}	
void Robot::DisabledPeriodic() {
	if(Robot::oi->getDriverJoystickRight()->GetRawButton(7))
		SMDB();
	
	Robot::pickup->beaterBarOut->Set(false);
	Robot::pickup->wings->Set(false);
	
	Scheduler::GetInstance()->Run();
}
	
void Robot::AutonomousInit() {
    autoProgram = static_cast<AutoProgram>((int)(autoChooser->GetSelected()));
	autoStepTimer->Reset();
	autoStepIncrementer = 0;
	genericAutoProgram.clear();
	autonomousTimer->Reset();
	Robot::driveTrain->gyro->Reset();
	switch(autoProgram) {
	case fire2FromCenter:
		genericAutoProgram.push_back(Initiate);
		genericAutoProgram.push_back(FindTarget);
		genericAutoProgram.push_back(FirstTurn);
		genericAutoProgram.push_back(Fire);
		genericAutoProgram.push_back(CollectBall);
		genericAutoProgram.push_back(LoadBall);
		genericAutoProgram.push_back(SecondTurn);
		genericAutoProgram.push_back(DropPickup);
		genericAutoProgram.push_back(Fire);
		genericAutoProgram.push_back(Chill);
		genericAutoProgram.push_back(DriveForward);
		genericAutoProgram.push_back(End);
		break;

	case fire3FromCenter:
		genericAutoProgram.push_back(Initiate);
		genericAutoProgram.push_back(FindTarget);
		genericAutoProgram.push_back(FirstTurn);
		genericAutoProgram.push_back(Fire);
		genericAutoProgram.push_back(CollectBall);
		genericAutoProgram.push_back(LoadBall);
		genericAutoProgram.push_back(SecondTurn);
		genericAutoProgram.push_back(DropPickup);
		genericAutoProgram.push_back(Fire);		
		genericAutoProgram.push_back(CollectBall);
		genericAutoProgram.push_back(LoadBall);
		genericAutoProgram.push_back(DropPickup);
		genericAutoProgram.push_back(Fire);		
		genericAutoProgram.push_back(Chill);
		genericAutoProgram.push_back(DriveForward);
		genericAutoProgram.push_back(End);
		break;
		
	case fire1Left:
		genericAutoProgram.push_back(Initiate);
		genericAutoProgram.push_back(FindTarget);
		genericAutoProgram.push_back(WaitToFire);
		genericAutoProgram.push_back(Fire);
		genericAutoProgram.push_back(DriveForward);
		genericAutoProgram.push_back(End);
		break;
		
	case fire1Right:
		genericAutoProgram.push_back(Initiate);
		genericAutoProgram.push_back(FindTarget);
		genericAutoProgram.push_back(WaitToFire);
		genericAutoProgram.push_back(Fire);
		genericAutoProgram.push_back(DriveForward);
		genericAutoProgram.push_back(End);
		break;
	}
	autoStep = genericAutoProgram[autoStepIncrementer];
	
	Robot::driveTrain->DriveControlTwist->Enable();
}
	
void Robot::AutonomousPeriodic() {
	bool beaterBarOut = false;
	bool wingsOut = false;
	float beaterBarSpeed = 0;
	float driveX = 0;
	float driveY = 0;
	float driveTwist = 0;
	bool fire = false;
	bool drive = false;
	
	SMDB();
	
	switch(autoStep) {
	case Initiate:
		if(autoProgram == fire3FromCenter)
			beaterBarOut = false;
		else
			beaterBarOut = true;
		wingsOut = false;
		beaterBarSpeed = 0;
		drive = false;
		fire = true;
		SmartDashboard::PutString("AutoStep", "Initiate");
		if(autoStepTimer->HasPeriodPassed(.5))
			autoStepComplete = true;
		break;
		
	case FindTarget:
		beaterBarOut = Robot::pickup->beaterBarOut->Get();
		wingsOut = false;
		beaterBarSpeed = 0;
		drive = false;
		fire = false;
		if(Robot::driveTrain->targetLeft->Get())
			turnDirection = -1;
		else
			turnDirection = 1;
		SmartDashboard::PutString("AutoStep", "FindTarget");
		Robot::driveTrain->sendProcessImage->Set(1);
		autoStepComplete = true;
		break;
		
	case WaitToFire:
		beaterBarOut = true;
		wingsOut = false;
		beaterBarSpeed = 0;
		drive = false;
		fire = false;
		SmartDashboard::PutString("AutoStep", "FindTarget");
		if(turnDirection < 0 && autoProgram == fire1Left)
			autoStepComplete = true;
		else
			if(autonomousTimer->HasPeriodPassed(5.0))
				autoStepComplete = true;
		break;
	
	case FirstTurn:
		beaterBarOut = Robot::pickup->beaterBarOut->Get();
		wingsOut = false;
		beaterBarSpeed = 0;
		drive = true;
		driveX = 0;
		driveY = 0;
		driveTwist = 10*turnDirection;
		Robot::driveTrain->DriveControlTwist->SetPID(.035, 0, .01);
		fire = false;
		SmartDashboard::PutString("AutoStep", "FirstTurn");
		if(!Robot::driveTrain->DriveControlTwist->OnTarget()) {
			onTargetTimer->Reset();
		}
		if(onTargetTimer->HasPeriodPassed(.3)){
			autoStepComplete = true;
		}
		break;
	
	case SecondTurn:
		beaterBarOut = true;
		wingsOut = false;
		beaterBarSpeed = 0;
		drive = true;
		driveX = 0;
		driveY = 0;
		driveTwist = 10*-turnDirection;
		Robot::driveTrain->DriveControlTwist->SetPID(.025, 0, .01);
		fire = false;
		SmartDashboard::PutString("AutoStep", "SecondTurn");
		if(!Robot::driveTrain->DriveControlTwist->OnTarget()) {
			onTargetTimer->Reset();
		}
		if(onTargetTimer->HasPeriodPassed(.3)){
			autoStepComplete = true;
		}
		break;
		
	case Fire:
		beaterBarOut = true;
		wingsOut = false;
		beaterBarSpeed = 0;
		drive = false;
		fire = true;
		SmartDashboard::PutString("AutoStep", "Fire");
		autoStepComplete = true;
		break;
	
	case Chill:
		beaterBarOut = true;
		wingsOut = false;
		beaterBarSpeed = 0;
		drive = false;
		fire = false;
		SmartDashboard::PutString("AutoStep", "Chill");
		if(autoStepTimer->HasPeriodPassed(.5))
			autoStepComplete = true;
		break;
	
	case CollectBall:
		beaterBarOut = true;
		wingsOut = false;
		beaterBarSpeed = 1;
		drive = false;
		fire = false;
		SmartDashboard::PutString("AutoStep", "CollectBall");
		if(autoStepTimer->HasPeriodPassed(1.5))
			autoStepComplete = true;
		break;
		
		
	case LoadBall:
		beaterBarOut = false;
		wingsOut = false;
		beaterBarSpeed = 0;
		drive = false;
		fire = false;
		SmartDashboard::PutString("AutoStep", "LoadBall");
		if(autoStepTimer->HasPeriodPassed(.3))
			autoStepComplete = true;
		break;
		
	case DropPickup:
		beaterBarOut = true;
		wingsOut = false;
		beaterBarSpeed = 0;
		drive = false;
		fire = false;
		SmartDashboard::PutString("AutoStep", "DropPickup");
		if(autoStepTimer->HasPeriodPassed(0.6))
			autoStepComplete = true;
		break;
	
	case DriveForward:
		beaterBarOut = true;
		wingsOut = false;
		beaterBarSpeed = 0;
		drive = true;
		driveX = 0;
		driveY = -1;
		driveTwist = 10*-turnDirection;
		fire = false;
		SmartDashboard::PutString("AutoStep", "DriveForward");
		if(autoStepTimer->HasPeriodPassed(1.0))
			autoStepComplete = true;
		break;
	
	case End:
		beaterBarOut = true;
		wingsOut = false;
		beaterBarSpeed =0;
		drive = false;
		fire = false;
		SmartDashboard::PutString("AutoStep", "End");
		break;
	}
	
/***********************Beater Bar / Wings****************************/	
	Robot::pickup->beaterBarOut->Set(beaterBarOut);
	Robot::pickup->wings->Set(wingsOut);
	Robot::pickup->beaterBar->Set(beaterBarSpeed);
/***********************Driving****************************/	
	Robot::driveTrain->DriveControlTwist->SetSetpoint(driveTwist);
	if(drive)
		Robot::driveTrain->Crab(Robot::driveTrain->CrabSpeedTwist->Get(), driveY, driveX, true);	
	else
		Robot::driveTrain->Crab(0,0,0,false);
/***********************Shooter****************************/		
	if(fire)
		Robot::shooter->Fire(0, false);		
	
	Robot::shooter->CamChecker();
/***********************Increment Through Program****************************/	
	if (autoStepComplete) {
		autoStepTimer->Reset();
		autoStepComplete = false;
		autoStepIncrementer ++;
		onTargetTimer->Reset();
		try {
			autoStep = genericAutoProgram.at(autoStepIncrementer);
		}
		catch (const out_of_range& oor) {
			printf ("AutoProgram Vector Out of Range \n");
		}
	}
}
	
void Robot::TeleopInit() {
	// This makes sure that the autonomous stops running when
	// teleop starts running. If you want the autonomous to 
	// continue until interrupted by another command, remove
	// this line or comment it out.
	Robot::driveTrain->DriveControlTwist->Disable();
}
	
void Robot::TeleopPeriodic() {
	if(Robot::oi->getDriverJoystickRight()->GetRawButton(7))
	SMDB();
		
/******************DRIVETRAIN**************************************/	
	
	//Resets gyro to zero when crab starts
	if (!prevTrigger && Robot::oi->getDriverJoystickRight()->GetRawButton(1))
		Robot::driveTrain->gyro->Reset();
	
	prevTrigger = Robot::oi->getDriverJoystickRight()->GetRawButton(1);
	if(Robot::oi->getDriverJoystickRight()->GetRawButton(1))
	{//Crab	
		Robot::driveTrain->Crab(Robot::oi->getJoystickTwist(),-Robot::oi->getJoystickY(),Robot::oi->getJoystickX(),true);	
	}
	else 
	{//Steering
		//Robot::driveTrain->Steer(Robot::oi->getScaledJoystickRadians(),Robot::oi->getJoystickMagnitude(),0.5);
		Robot::driveTrain->Steer(Robot::oi->getLeftJoystickXRadians(),Robot::oi->getJoystickY(),0.5);
	}
/******************SHOOTER**************************************/	
	if(Robot::oi->getGamePad()->GetRawButton(10)){
		Robot::shooter->RunCams(-Robot::oi->getGamePad()->GetRawAxis(2));
	}
	else {
		Robot::shooter->CamChecker(); //Runs every cycle to control cam postion
		if((Robot::oi->getGamePad()->GetRawButton(4) || Robot::oi->getDriverJoystickRight()->GetRawButton(3)) && !Robot::shooter->GetFiring()) {
			if(!Robot::pickup->beaterBarOut->Get())
				beaterBarTimer->Reset();
			Robot::pickup->beaterBarOut->Set(true);			
			Robot::shooter->Fire(0.6 - beaterBarTimer->Get(), true);
		}
	}
	
/******************BEATERBAR**************************************/	
	
	Robot::pickup->beaterBar->Set((Robot::oi->getGamePad()->GetRawAxis(4)));
	
	Robot::shooter->fingers->Set(Robot::oi->getGamePad()->GetRawButton(2));
	
	if(Robot::oi->getGamePad()->GetRawButton(7))
		Robot::pickup->wings->Set(true);
	if(Robot::oi->getGamePad()->GetRawButton(5))
		Robot::pickup->wings->Set(false);
	
	if (Robot::oi->getGamePad()->GetRawButton(8)) {
		Robot::pickup->beaterBarOut->Set(true);
		beaterBarTimer->Reset();
	}
	if (Robot::oi->getGamePad()->GetRawButton(6)) {
			Robot::pickup->beaterBarOut->Set(false);
	}
}
void Robot::TestPeriodic() {
	lw->Run();
}
void Robot::SMDB() {
	//Joystick Variables
	SmartDashboard::PutNumber("StickMagnitude",Robot::oi->getDriverJoystickRight()->GetMagnitude());
	SmartDashboard::PutNumber("StickDirection",Robot::oi->getDriverJoystickRight()->GetDirectionRadians());
//	SmartDashboard::PutNumber("LeftStickX", Robot::oi->getDriverJoystickLeft()->GetX());
	SmartDashboard::PutNumber("StickTwist",Robot::oi->getDriverJoystickRight()->GetTwist());
	SmartDashboard::PutNumber("ScaledRadians",Robot::oi->getScaledJoystickRadians());
	SmartDashboard::PutNumber("ScalingFactor",Robot::oi->getJoystickTwist());
	//Gyro Variables
	SmartDashboard::PutNumber("GyroAngle", Robot::driveTrain->gyro->GetAngle());
	SmartDashboard::PutNumber("GyroCenter", Robot::driveTrain->gyro->GetCenter());
	SmartDashboard::PutNumber("GyroOffset", Robot::driveTrain->gyro->GetOffset());
	
	//Wheel Module Voltages
	SmartDashboard::PutNumber("FrontLeftVol",Robot::driveTrain->frontLeftPos->GetAverageVoltage());
	SmartDashboard::PutNumber("FrontRightVol",Robot::driveTrain->frontRightPos->GetAverageVoltage());
	SmartDashboard::PutNumber("RearLeftVol",Robot::driveTrain->rearLeftPos->GetAverageVoltage());
	SmartDashboard::PutNumber("RearRightVol",Robot::driveTrain->rearRightPos->GetAverageVoltage());	
	//Wheel Module Errors
	SmartDashboard::PutNumber("FLError", Robot::driveTrain->frontLeft->GetError());
	SmartDashboard::PutNumber("FRError", Robot::driveTrain->frontRight->GetError());
	SmartDashboard::PutNumber("RLError", Robot::driveTrain->rearLeft->GetError());
	SmartDashboard::PutNumber("RRError", Robot::driveTrain->rearRight->GetError());
	//Wheel Module Setpoints
	SmartDashboard::PutNumber("FLSetPoint", Robot::driveTrain->frontLeft->GetSetpoint());
	SmartDashboard::PutNumber("FRSetPoint", Robot::driveTrain->frontRight->GetSetpoint());
	SmartDashboard::PutNumber("RLSetPoint", Robot::driveTrain->rearLeft->GetSetpoint());
	SmartDashboard::PutNumber("RRSetPoint", Robot::driveTrain->rearRight->GetSetpoint());
	
	//ShooterValues
	SmartDashboard::PutNumber("CorrectedCamPostion",Robot::shooter->GetCorrectedCamPos());
	SmartDashboard::PutNumber("RawCamPostion",Robot::shooter->camPos->GetAverageVoltage());
//	SmartDashboard::PutNumber("WindowMotorOutput", Robot::shooter->windowMotors->Get());
//	SmartDashboard::PutNumber("CamLeftOutput", Robot::shooter->camLeft->Get());
//	SmartDashboard::PutNumber("CamRightOutput", Robot::shooter->camRight->Get());
	SmartDashboard::PutNumber("FireTimer", Robot::shooter->fireTimer->Get());
	SmartDashboard::PutNumber("WindowMotorCurrent", Robot::shooter->windowMotors->GetOutputCurrent());
	SmartDashboard::PutNumber("CamLeftCurrent", Robot::shooter->camLeft->GetOutputCurrent());
	SmartDashboard::PutNumber("CamRightCurrent", Robot::shooter->camRight->GetOutputCurrent());
	SmartDashboard::PutBoolean("CamPositionSensorStatus", Robot::shooter->GetCamPosStatus());
	SmartDashboard::PutBoolean("BackupCamPositionSensorStatus", Robot::shooter->GetBackupCamPosStatus());
	
	//BeaterBar
//	SmartDashboard::PutNumber("BeaterBarCurrent", Robot::pickup->beaterBar->GetOutputCurrent());
	
	//Jaguar Stauses
	SmartDashboard::PutBoolean("06-FLSteerJagAlive",Robot::driveTrain->frontLeftSteer->IsAlive());
	SmartDashboard::PutBoolean("07-FRSteerJagAlive",Robot::driveTrain->frontRightSteer->IsAlive());
	SmartDashboard::PutBoolean("08-RLSteerJagAlive",Robot::driveTrain->rearLeftSteer->IsAlive());
	SmartDashboard::PutBoolean("09-RRSteerJagAlive",Robot::driveTrain->rearRightSteer->IsAlive());
	SmartDashboard::PutBoolean("10-WindowMotorJagAlive",Robot::shooter->windowMotors->IsAlive());
	SmartDashboard::PutBoolean("11-CamLeftJagAlive",Robot::shooter->camLeft->IsAlive());
	SmartDashboard::PutBoolean("12-CamRightJagAlive",Robot::shooter->camRight->IsAlive());
//	SmartDashboard::PutBoolean("13-BeaterBarJagAlive",Robot::pickup->beaterBar->IsAlive());
	
	SmartDashboard::PutNumber("PIDTwistOutput", Robot::driveTrain->DriveControlTwist->Get());
	SmartDashboard::PutNumber("PIDTwistError", Robot::driveTrain->DriveControlTwist->GetError());
	SmartDashboard::PutNumber("PIDTwistSetpoint", Robot::driveTrain->DriveControlTwist->GetSetpoint());
	
}
START_ROBOT_CLASS(Robot);
