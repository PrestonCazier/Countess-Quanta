The file structure CountessQuantaControl houses the Visual Studio Solution that is run to operate the Countess Quanta Robot from Windows and uses the Pololu Mini Maestro 24 Servo Controller Board and a Microsoft Kinect.  The software is only verified to work with the 1414 Microsoft Kinect.

Inside the folder Modified-Code are four files that were modified to extend the speech recognition part of the software, so that it contained more recognized speech paterns and animations.  Below is listed the four files and their destinations inside of the CountessQuantaControl project:

KinectManager.cs			CountessQuantaControl/CountessQuantaControl/
SequenceFile.xml			CountessQuantaControl/CountessQuantaControl/bin/Debug
SequenceProcessor.cs		CountessQuantaControl/CountessQuantaControl/
SpeechGrammer.xml			CountessQuantaControl/CountessQuantaControl/
