@echo off
rem Force la console en UTF-8 (et masque le message)
chcp 65001 >nul

rem S'assure que nous sommes a la racine du projet
cd /d "%~dp0.."

rem Compile le projet (mingw32-make utilise le Makefile)
mingw32-make || goto :error

echo.
echo Compilation reussie, lancement du jeu...
projet_oceandepths
goto :eof

:error
echo.
echo Echec de la compilation (voir messages ci-dessus)
exit /b 1
