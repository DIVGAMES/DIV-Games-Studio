#!/bin/bash


echo "1:$1 2:$2 3:$3 4:$4"

echo "Creating build dir"

rm -rf android/resources
mkdir -p android/resources

echo "Copying source files"
cp -r $1/* android/resources


#cp tools/dx.png buildgcw/$3.png

#echo "renaming files to owercase"

#find . -depth -exec rename 's/(.*)\/([^\/]*)/$1\/\L$2/' {} \;

echo "Removing old div exe"
rm "android/resources/$2"

echo "Removing extra exe files"
find android/resources -iname "*.exe" | xargs rm 2>/dev/null

echo "Removing dll files"
find android/resources -iname "div*run.dll" | xargs rm 2>/dev/null

echo "Removing flic files"
find android/resources -iname "*.fl*" | xargs rm 2>/dev/null


PAK=`find android/resources -iname '*.pak' | wc -l`

echo "PAK FOUND: $PAK"

if [ $PAK == "0" ]
then

echo "Compressing resources to data.div file"

cd android/resources
zip ../data.div -9 -r *
rm -rf *
mv ../data.div .
cd -
fi

echo "Copying new exe files"


VER=`dd if="$1/$2" bs=1 count=1 skip=2 2>/dev/null`
#echo $VER

# s= div1 j=div2
if [ $VER = "s" ]
then
echo "DIV1 runtime"
cp android/div1.so android/div2-droid/lib/armeabi/libapplication.so
cp "$1/$2" "android/resources/run"

else
echo "DIV2 runtime"
cp android/div2.so android/div2-droid/lib/armeabi/libapplication.so
cp "$1/$2" "android/resources/EXEC.EXE"

fi


echo "Creating yml config"
cat << EOF > android/div2-droid/apktool.yml
version: 2.0.2
apkFileName: $3.apk
isFrameworkApk: false
usesFramework:
  ids:
  - 1
sdkInfo:
  minSdkVersion: '7'
  targetSdkVersion: '17'
packageInfo:
  rename-manifest-package: com.div.$3
  forced-package-id: '127'
versionInfo:
  versionCode: '1'
  versionName: '1.0'
sharedLibrary: false

EOF

echo "Creating strings file def"
cat << EOF > android/div2-droid/res/values/strings.xml
<?xml version="1.0" encoding="utf-8"?>
<resources>
    <string name="pref_statusbar_default">1</string>
    <string name="pref_cursorstyle_default">0</string>
    <string name="pref_cursorblink_default">0</string>
    <string name="pref_fontsize_default">10</string>
    <string name="pref_color_default">1</string>
    <string name="pref_backaction_default">0</string>
    <string name="pref_controlkey_default">5</string>
    <string name="pref_fnkey_default">4</string>
    <string name="pref_ime_default">0</string>
    <string name="pref_shell_default">/system/bin/sh -</string>
    <string name="pref_initialcommand_default">export PATH=/data/local/bin:$PATH</string>
    <string name="pref_termtype_default">screen</string>
    <string name="application_terminal">Terminal Emulator</string>
    <string name="preferences">Preferences</string>
    <string name="new_window">New window</string>
    <string name="close_window">Close window</string>
    <string name="window_list">Windows</string>
    <string name="prev_window">Prev window</string>
    <string name="next_window">Next window</string>
    <string name="reset">Restart</string>
    <string name="send_email">Email to</string>
    <string name="special_keys">Special keys</string>
    <string name="toggle_soft_keyboard">Toggle soft keyboard</string>
    <string name="enable_wakelock">Take WakeLock</string>
    <string name="disable_wakelock">Drop WakeLock</string>
    <string name="enable_wifilock">Take WifiLock</string>
    <string name="disable_wifilock">Drop WifiLock</string>
    <string name="edit_text">Edit text</string>
    <string name="select_text">Select text</string>
    <string name="copy_all">Copy all</string>
    <string name="paste">Paste</string>
    <string name="window_title">Window %1$d</string>
    <string name="service_notify_text">Terminal session is running</string>
    <string name="screen_preferences">Screen</string>
    <string name="title_statusbar_preference">Status bar</string>
    <string name="summary_statusbar_preference">Show/hide status bar.</string>
    <string name="dialog_title_statusbar_preference">Status bar</string>
    <string name="title_cursorstyle_preference">Cursor style</string>
    <string name="summary_cursorstyle_preference">Choose cursor style.</string>
    <string name="dialog_title_cursorstyle_preference">Cursor style</string>
    <string name="title_cursorblink_preference">Cursor blink</string>
    <string name="summary_cursorblink_preference">Choose cursor blink.</string>
    <string name="dialog_title_cursorblink_preference">Cursor blink</string>
    <string name="text_preferences">Text</string>
    <string name="title_utf8_by_default_preference">Default to UTF-8</string>
    <string name="summary_utf8_by_default_preference">Whether UTF-8 mode is enabled by default.</string>
    <string name="title_fontsize_preference">Font size</string>
    <string name="summary_fontsize_preference">Choose character height in points.</string>
    <string name="dialog_title_fontsize_preference">Font size</string>
    <string name="title_color_preference">Colors</string>
    <string name="summary_color_preference">Choose text color.</string>
    <string name="dialog_title_color_preference">Text color</string>
    <string name="keyboard_preferences">Keyboard</string>
    <string name="title_backaction_preference">Back button behavior</string>
    <string name="summary_backaction_preference">Choose what pressing the back button does.</string>
    <string name="dialog_title_backaction_preference">Back button behavior</string>
    <string name="title_controlkey_preference">Control key</string>
    <string name="summary_controlkey_preference">Choose control key.</string>
    <string name="dialog_title_controlkey_preference">Control key</string>
    <string name="title_fnkey_preference">Fn key</string>
    <string name="summary_fnkey_preference">Choose Fn key.</string>
    <string name="dialog_title_fnkey_preference">Fn key</string>
    <string name="title_ime_preference">Input method</string>
    <string name="summary_ime_preference">Choose input method for soft keyboard.</string>
    <string name="dialog_title_ime_preference">Input method</string>
    <string name="shell_preferences">Shell</string>
    <string name="title_shell_preference">Command line</string>
    <string name="summary_shell_preference">Specify the shell command line.</string>
    <string name="dialog_title_shell_preference">Shell</string>
    <string name="title_initialcommand_preference">Initial command</string>
    <string name="summary_initialcommand_preference">Sent to the shell when it starts.</string>
    <string name="dialog_title_initialcommand_preference">Initial Command</string>
    <string name="title_termtype_preference">Terminal type</string>
    <string name="summary_termtype_preference">What terminal type to report to the shell.</string>
    <string name="dialog_title_termtype_preference">Terminal type</string>
    <string name="title_close_window_on_process_exit_preference">Close window on exit</string>
    <string name="summary_close_window_on_process_exit_preference">Whether a window should close when its shell exits.</string>
    <string name="control_key_dialog_title">Control and Function Keys</string>
    <string name="control_key_dialog_control_text">"CTRLKEY Space : Control-@ (NUL)
CTRLKEY A..Z : Control-A..Z
CTRLKEY 5 : Control-]
CTRLKEY 6 : Control-^
CTRLKEY 7 : Control-_
CTRLKEY 9 : F11
CTRLKEY 0 : F12"</string>
    <string name="control_key_dialog_control_disabled_text">No control key set.</string>
    <string name="control_key_dialog_fn_text">"FNKEY 1..9 : F1-F9
FNKEY 0 : F10
FNKEY W : Up
FNKEY A : Left
FNKEY S : Down
FNKEY D : Right
FNKEY P : PageUp
FNKEY N : PageDown
FNKEY T : Tab
FNKEY L : | (pipe)
FNKEY U : _ (underscore)
FNKEY E : Control-[ (ESC)
FNKEY X : Delete
FNKEY I : Insert
FNKEY H : Home
FNKEY F : End
FNKEY . : Control-\\
"</string>
    <string name="control_key_dialog_fn_disabled_text">No function key set.</string>
    <string name="send_control_key">Send control key</string>
    <string name="send_fn_key">Send fn key</string>
    <string name="title_verify_path_preference">Verify PATH entries</string>
    <string name="summary_verify_path_preference">Whether inaccessible directories should be removed from the PATH.</string>
    <string name="title_do_path_extensions_preference">Allow PATH extensions</string>
    <string name="summary_do_path_extensions_preference">Whether other apps should be allowed to provide additional commands (add to PATH).</string>
    <string name="title_allow_prepend_path_preference">Allow PATH prepend</string>
    <string name="summary_allow_prepend_path_preference">Whether other apps should be allowed to override existing commands (add to the beginning of PATH).</string>
    <string name="alt_sends_esc">Alt key sends ESC</string>
    <string name="alt_sends_esc_summary_on">Alt key sends ESC.</string>
    <string name="alt_sends_esc_summary_off">Alt key does not send ESC.</string>
    <string name="app_name">$4</string>
    <string name="ministro_not_found_msg">"Can't find Ministro service.
The application can't start."</string>
    <string name="ministro_needed_msg">This application requires Ministro service. Would you like to install it?</string>
    <string name="fatal_error_msg">Your application encountered a fatal error and cannot continue.</string>
    <string name="export_mode">01</string>
    <string name="qt_libs">Qt5Core,Qt5Gui,Qt5Widgets,AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA</string>
</resources>

EOF


echo "Creating prefs file"
cat << EOF > android/div2-droid/res/xml/sdlprefs.xml
<?xml version="1.0" encoding="utf-8"?>
<PreferenceScreen
  xmlns:android="http://schemas.android.com/apk/res/android">
    <CheckBoxPreference android:title="Enable default on-screen buttons" android:key="hudenabled" android:defaultValue="true" />
    <ListPreference android:title="Button 1" android:key="btn1pref" android:defaultValue="32" />
    <ListPreference android:title="Button 2" android:key="btn2pref" android:defaultValue="304" />
    <ListPreference android:title="Button 3" android:key="btn3pref" android:defaultValue="308" />
    <ListPreference android:title="Button 4" android:key="btn4pref" android:defaultValue="306" />
    <ListPreference android:title="Button 5" android:key="btn5pref" android:defaultValue="8" />
    <ListPreference android:title="Button 6" android:key="btn6pref" android:defaultValue="9" />
</PreferenceScreen>
EOF

echo "Making resources file"

pwd

cd android
rm ./div2-droid/assets/resources.zip
zip ./div2-droid/assets/resources.zip -r -9 resources/*
#cd -

echo "packaging apk"
./apktool b div2-droid -o $3.apk

echo "Signing apk"
jarsigner –verbose –sigalg MD5withRSA –digestalg SHA1 –keystore divgames $3.apk div2
cd -

#cat buildgcw/default.gcw0.desktop



#echo "Making $4 GCW BINARY"
#cmake . -DGCW=1 -DWINDOWS=0 -DAMIGA=0 -DRPI=0 > /dev/null
#make -j5 div1run-GCW divrun-GCW > /dev/null

#echo "Copying $3 to buildgcw/$3"

#VER=`dd if=buildgcw/$3.dat bs=1 count=1 skip=2 2>/dev/null`
#echo $VER

# s= div1 j=div2
#if [ $VER = "s" ]
#then
#echo "DIV1 runtime"
#cp ./div1run-GCW "buildgcw/$3"
#else
#echo "DIV2 runtime"
#cp system/divrun-GCW "buildgcw/$3"
#fi
#
#echo "Compressing binary"
#upx -9 "buildgcw/$3" > /dev/null

#echo "Making opk file"

#mksquashfs buildgcw $3.opk -all-root -noappend -no-exports -no-xattrs > /dev/null
#rm -rf buildgcw

echo "Done! - created $3.apk"

