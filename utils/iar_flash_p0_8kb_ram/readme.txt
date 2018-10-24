1. Place all of the files in this zip (except this readme file) in your IAR Systems\Embedded Workbench.6.3_x\arm\config\flashloader\Freescale folder

2. Open your IAR workspace file and select a flash configuration for the project (you might have to create one if you don't have one already)

3. In the project options select Debugger->Download 

4. Check the "Use flash loader(s)" box

5. Check the "Override default .board file box"

6. Provide the path to the FlashK20Xxxx.board file. Should be:
	$TOOLKIT_DIR$\config\flashloader\Freescale\FlashK20Xxxx.board