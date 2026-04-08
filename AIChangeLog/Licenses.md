⏺ Sign/verify round-trip works. Here's a summary of
everything that was implemented:

  ---
What was added

New files:
- Source/license/LicenseValidator.h/.cpp —
  validates a signed license key, loads/saves the
  key via juce::PropertiesFile (stored in
  ~/Library/Application Support/Degradizer.props)
- scripts/keygen.py — generate a new key pair (run
  if you ever rotate keys)
- scripts/sign_license.py — issue a license key:
  python3 scripts/sign_license.py customer@email.com

Modified files:
- CMakeLists.txt — added juce_cryptography,
  juce_data_structures, and LicenseValidator.cpp
- PluginEditor.cpp/.h — listens for submitLicense
  events from the UI, validates, saves if valid, and
  emits licenseStatus back to the UI on every check
- ui/src/bridge/juce.js — added submitLicense()
  and onLicenseStatus() bridge functions
- ui/src/App.jsx — shows a full-overlay activation
  dialog when not licensed; dismisses on valid key

How it works:
1. On startup the editor calls
   LicenseValidator::checkSaved() and emits the
   result to the UI
2. UI shows the overlay if valid === false
3. Customer pastes key → submitLicense event → C++
   validates RSA signature → emits result back →
   overlay disappears on success

To issue a license: python3
scripts/sign_license.py customer@example.com

--- Found issue that sounds gets processed before license activation

All done. The clangd errors are the same
pre-existing indexing issue (CMake not           
reconfigured yet) — ignore them. Here's what
changed:

- PluginProcessor.h — std::atomic<bool>           
  licensed{false} member + setLicensed() /          
  isLicensed() methods
- PluginProcessor.cpp — constructor checks      
  LicenseValidator::checkSaved() to initialise the  
  flag; processBlock clears the buffer and returns
  immediately if not licensed
- PluginEditor.cpp — calls                      
  processorRef.setLicensed(lic.valid) after a       
  successful activation so the audio gate opens
  without restarting the plugin

The licensed flag is std::atomic<bool> so the     
audio thread can read it lock-free while the
message thread writes to it.    