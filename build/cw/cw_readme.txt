General directions for all CodeWarrior projects:

1. Set the workspace directory to the "kinetis-sc" folder location. These projects use workspace relative paths, so in order for the project to find all files correctly the CW10.1 workspace must be set to the kinetis-sc folder (the one that contains the build and src directories).

2. From the CW10.1 bench select the File->Import... option. Expand the "General" folder and selct "Existing Projects into Workspace" and click on Next. Click on the "Browse..." button next to the select root directory option. If you select the "kinetis-sc" folder location you'll get a list of all of the CW10.1 example code projects in this archive. If you would like to see a shorter list, then you can navigate to the specific example you would like to see by looking through the list of folders found in the "kinetis-sc/build/cw" folder. Select the project or projects you would like and click on "Finish." You will get messages saying that the project refers to a remote system that does not exist. Click on "Yes" to add them to the workspace.

3. Select the configuration to bulid. The first line of the project in the project window shows the currently selected configuration for the project. Most projects will have a RAM and a flash option. If you want to change the current configuration, click on the configuration name (to the right of the project name) and a window should popup showing you the available options with a check mark next to the currently selected one. You can click on the other configuration to make it active.

4. Click on the Build button (the hammer icon). This will build the currently select configuraiton of the project. The build will have a  number of warnings mostly about multiply defined symbols. These are ok and can be ignored.

5. Click on the small arrow next to the debug button (the bug icon) and click on "Debug Configurations...". In the window that pops up expand the "CodeWarrior Download" option. Here you will see a list of connection options. These options allow you to select the target you want to debug (flash or RAM) and the connection method you are using to talk to your board (OSJTAG, P&E Universal-multilink, or J-link). Select the option appropriate for the configuration and JTAG hardware you are using and select "Debug" to download the code (programming the flash if needed) and start a debug session. NOTE: if you haven't built a given configuration the "Debug" button will remain grayed out.

Cloning projects:

The "make_new_cw_project.exe" file can be used to generate a new project. It will make a copy of the hello_world project settings, and replace all the references and paths mentioning "hello_world" to "your_project_name". 

Typically this could be used to create a CW10 project for sample code already in this repository, such as "gpio" or "rtc". Then the project will point to the source folder at \kinetis-sc\src\projects\project_name_chosen. Otherwise you will need to create that folder and files in the \src\projects\ folder to put your main() function.

Note: it can take a while for the text prompt to come up.


