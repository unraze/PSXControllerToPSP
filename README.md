# PSXControllerToPSP
This project was originally created by TokyoDrift on the ACIDMOD forums. 
I have uploaded it here to enable future development of the project. 

This project enables communication between an Atmega Controller and the PSP via the PSP's Tx/Rx lines included with the PSP's AV Jack.  



OLD README
PS2 Controller for PSP - Copyright (C) TokyoDrift 2010

-= THE SOFTWARE PROVIDED WITH THIS LICENCE IS FOR NON COMMERCIAL USE ONLY!!! =-

This software is for research purposes only.

That means you may use and modify the information given in all of these documents
for your own use only. You may redistribute a modified version of this information
as long as you give credits to everyone listed in the CREDITS file and including this
licence.

You may NOT redistribute your modified version as a binary only; you have to include
ALL sources used for your modified version. Using the information for commercial
purposes is HIGHLY FORBIDDEN! Redistributing anything of the files without giving
credits is FORBIDDEN. Showing anything u built using the information given here
without giving credits to anyone listed in the CREDITS file is FORBIDDEN.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is an attempt of connecting a PS2 controller to a PSP with neithermodifing the controller nor the PSP.
It works by converting the PS2 controller (SPI) signal to a UART signal. The code is very bad because
I'm a bad developer and I didn't care about it as long as it was working. It's your turn to improve the code.

The PSP plugin has a single known bug. It freezes when suspending and resuming the main thread. That means
you can not suspend and resume the PSP and contine playing then. It needs to completely re-load the plugin.
Getting it fixed is your turn either.

Every file provided in this archive is under the licence found in LICENCE. You may _NOT_ repack this without
including both licence and credits file. See LICENCE for more information. If you have any questions, feel
free to eMail me: TokyoDriftPSP@googlemail.com .

visit tokyodriftpsp.wordpress.com

- TokyoDrift, the 15 years old, very bad developer.



-= CREDITS GO TO: =-

TokoyDrift	- Schematics, AVR code, PSP code (apart from SIO driver,
		  syscon debug handler, power callbacks)
Jean    	- SIO Driver based on TyRaNiD's
TyRaNiD 	- SIO Driver, Power Callbacks
Booster 	- SysCon debug handler
Blizzard 	- Doing this mod before (a little different though)
PSPSDK Makers	- PSPSDK
Dark_AleX	- Making the PSP scene big
ATMEL		- Great Microcontrollers, AVR Studio
