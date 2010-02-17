@echo off
cls

set from="Release\PrettyLightsWAP.dll"
set to="C:\Program Files\Winamp\Plugins\vis_prettylights.dll"

echo.
echo.
echo Copying %from% to %to%
copy %from% %to%
echo Done.
pause