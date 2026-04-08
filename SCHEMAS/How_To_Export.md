1. Build in Release mode first

cmake -S . -B cmake-build-release                 
-DCMAKE_BUILD_TYPE=Release                        
cmake --build cmake-build-release

Artifacts land in                                 
cmake-build-release/MyPlugin_artefacts/Release/.
                                                    
---                                             
2. Create the DMG

Simplest — create-dmg (recommended):
brew install create-dmg

create-dmg \                                      
--volname "Degradizer" \
--window-size 600 400 \                         
--icon-size 128 \                             
"Degradizer.dmg" \

"cmake-build-release/MyPlugin_artefacts/Release/"

This packages everything in the artefacts folder
(VST3 + Standalone app) into a nice DMG with a    
background and icon layout.

Bare minimum — hdiutil (no dependencies):         
hdiutil create -volname "Degradizer" \
-srcfolder                                      
"cmake-build-release/MyPlugin_artefacts/Release/"
\                                                 
-ov -format UDZO \
Degradizer.dmg
                                                  
---
3. Code signing + notarization (required for
   distribution)

Without this, macOS will block the plugin for     
other users ("damaged, can't be opened").

# Sign everything
codesign --deep --force --options runtime \     
--sign "Developer ID Application: Your Name     
(TEAMID)" \                                     
"cmake-build-release/MyPlugin_artefacts/Release/
VST3/MyPlugin.vst3"

codesign --deep --force --options runtime \       
--sign "Developer ID Application: Your Name   
(TEAMID)" \                                       
"cmake-build-release/MyPlugin_artefacts/Release/
Standalone/MyPlugin.app"

# Create DMG, then sign it too
codesign --sign "Developer ID Application: Your
Name (TEAMID)" Degradizer.dmg

# Notarize
xcrun notarytool submit Degradizer.dmg \        
--apple-id "you@email.com" \
--team-id "TEAMID" \
--password "app-specific-password" \
--wait

# Staple the notarization ticket
xcrun stapler staple Degradizer.dmg

You need an Apple Developer account ($99/yr) and a
Developer ID Application certificate from Xcode →
Settings → Accounts.
                                                  
---
Summary

┌─────────────────────┬───────────────────────┐
│        Goal         │     What you need     │
├─────────────────────┼───────────────────────┤
│ Test on your own    │ Just hdiutil, no      │
│ machine             │ signing               │
├─────────────────────┼───────────────────────┤   
│ Share with a few    │ create-dmg + ad-hoc   │
│ people              │ signing               │   
├─────────────────────┼───────────────────────┤
│ Public distribution │ Full signing +        │   
│                     │ notarization          │
└─────────────────────┴───────────────────────┘

❯ what about exe?

⏺ Same idea but Windows tooling. You need to build  
on Windows (or cross-compile, which is painful   
with JUCE — just use a Windows machine or VM).

1. Build in Release

cmake -S . -B cmake-build-release                 
-DCMAKE_BUILD_TYPE=Release                      
cmake --build cmake-build-release --config Release

VST3 lands in cmake-build-release/MyPlugin_artefac
ts/Release/VST3/.
Standalone exe in cmake-build-release/MyPlugin_art
efacts/Release/Standalone/.
   
---                                               
2. Create an installer

NSIS (free, most common for plugins):

Install https://nsis.sourceforge.io/, write a     
simple script:

; installer.nsi                                 
Name "Degradizer"                                 
OutFile "Degradizer-Setup.exe"
InstallDir "$PROGRAMFILES64\Degradizer"

Section "VST3"
SetOutPath "$COMMONFILES64\VST3"                
File "cmake-build-release\MyPlugin_artefacts\Rel
ease\VST3\MyPlugin.vst3"
SectionEnd

Section "Standalone"
SetOutPath "$INSTDIR"                           
File "cmake-build-release\MyPlugin_artefacts\Rel
ease\Standalone\MyPlugin.exe"
CreateShortcut "$DESKTOP\Degradizer.lnk"
"$INSTDIR\MyPlugin.exe"                           
SectionEnd

makensis installer.nsi

Outputs Degradizer-Setup.exe.

Inno Setup (easier GUI + scripting):              
Free, more modern-looking installers, widely used
for indie software.                               
https://jrsoftware.org/isinfo.php
                                                    
---                                             
3. Code signing (optional but recommended)

Without it, Windows SmartScreen will warn users
"Unknown publisher".

signtool sign /fd SHA256 /tr                      
http://timestamp.digicert.com /td SHA256 ^        
/f "your-cert.pfx" /p "your-password" ^
Degradizer-Setup.exe

You need a Code Signing Certificate (~$70–200/yr  
from DigiCert, Sectigo, etc.). Unlike Apple,    
there's no mandatory notarization — signing alone
removes the SmartScreen warning.

  ---
Summary

┌─────────────────┬───────────────────────────┐
│      Goal       │       What you need       │   
├─────────────────┼───────────────────────────┤
│ Test on your    │ Just copy the VST3 to     │
│ own machine     │ C:\Program Files\Common   │
│                 │ Files\VST3\               │   
├─────────────────┼───────────────────────────┤
│ Share with      │ NSIS or Inno Setup        │   
│ others          │ installer                 │
├─────────────────┼───────────────────────────┤
│ Remove          │                           │
│ SmartScreen     │ Code signing certificate  │   
│ warning         │                           │
└─────────────────┴───────────────────────────┘  