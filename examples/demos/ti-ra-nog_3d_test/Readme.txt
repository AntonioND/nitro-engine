 _   _ _ _               ______             _              _____   ____   _____ 
| \ | (_) |  AntonioND  |  ____|           (_)            |  __ \ / __ \ / ____|
|  \| |_| |_ _ __ ___   | |__   _ __   __ _ _ _ __   ___  | |__) | |  | | |     
| . ` | | __| '__/ _ \  |  __| | '_ \ / _` | | '_ \ / _ \ |  ___/| |  | | |     
| |\  | | |_| | | (_) | | |____| | | | (_| | | | | |  __/ | |    | |__| | |____ 
|_| \_|_|\__|_|  \___/  |______|_| |_|\__, |_|_| |_|\___| |_|     \____/ \_____|
                                       __/ |                                    
                                      |___/                                     
   _                ___ ____  _____    _______        _  ___                _ 
  | |        ______|  _|___ \|  __ \  |__   __|      | ||_  |______        | |
  | |  _____|______| |   __) | |  | |    | | ___  ___| |_ | |______|_____  | |
  | | |______|_____| |  |__ <| |  | |    | |/ _ \/ __| __|| |_____|______| | |
  | |       |______| |  ___) | |__| |    | |  __/\__ \ |_ | |______|       | |
  |_|              | |_|____/|_____/     |_|\___||___/\__|| |              |_|
                   |___|                                |___|                 
________             ____________      ________              _____   __              
___  __ )____  __    ___  __/__(_)     ___  __ \_____ _      ___  | / /____________ _
__  __  |_  / / /    __  /  __  /________  /_/ /  __ `/________   |/ /_  __ \_  __ `/
_  /_/ /_  /_/ /     _  /   _  /_/_____/  _, _// /_/ /_/_____/  /|  / / /_/ /  /_/ / 
/_____/ _\__, /      /_/    /_/        /_/ |_| \__,_/        /_/ |_/  \____/_\__, /  
        /____/                                                              /____/   


If you want to enable the FAT mesh and/or texture reading, first uncomment the line: 
"#define FAT_MESH_TEXT" and then compile it.

After you have the binary file with the FAT texture/mesh reading enabled, copy this 
files to the root of your card or put them in the same folder where the nds is for 
test it on an emulator (with fat support).

In orther to use the screenshoot or "video" recording function you need to enable the
FAT access to your card, if you have a new card reader for nds you may no need to patch
it, but if you have an older one may you need to patch the binary file with dldi patch
for you card reader. 

In the binary folder there are three files, one no patched binary and two patched for
R4 and Super card SD.

For making screenshots you need to create the "/screen" folder in the root directory of
your card and then the screens should be there.

The "video" recording function make one screenshot per VBL and save it in to the video
flother that you need to create in the root of the memory (i whasn't able to create 
the directory if it doesn't exist). When you push the "R" button the ds will make one 
screnshot for every vbl and you'll see the screen blinking and making some strange
with the scenes, don't worry, the screenshots will be ok. For some reason the fat
system can't make more than 98 files for folder so the first 98 screenshots will
be in the "/video/vid1" folder and the next 98 files to the "/video/vid2" folder, by 
the moment I only put support for 197 files convining both folders because the "video"
recording is toooo slooow and i think it's sufficient for a 10-20 seconds video.

When you have finished the the "video" recording you'll need to make a animated gif
or avi video with some software, I use "Active gif creator" what's very simple and 
useful but it's shareware and if you don't buy it you'll see a beautyfull "UNREGISTRED
VERSION" in the top left corner of you gif or avi :O.

So, that's all, I think that you'll love this Engine :D