set xgfpath=%~1
set basepath="%xgfpath%Samples\resource\*"
set basepathfx="%xgfpath%XGF\Res\fx\*"
set targetpath=%2

xcopy %basepath% %targetpath% /D/Y
xcopy %basepathfx% %targetpath% /D/Y
xcopy "%xgfpath%logo.png" %targetpath% /D/Y

echo copy end