@echo off
set "rutaBase=%~dp0"

SETLOCAL EnableDelayedExpansion

set "carpeta1=%rutaBase%DerivedDataCache"
set "carpeta2=%rutaBase%Intermediate"
set "carpeta3=%rutaBase%Saved"
set "carpeta4=%rutaBase%Binaries"
set "carpeta5=%rutaBase%Build"


echo Carpeta 1: %carpeta1%
echo Carpeta 2: %carpeta2%
echo Carpeta 3: %carpeta3%
echo Carpeta 4: %carpeta4%
echo Carpeta 5: %carpeta5%

rd /s /q "%carpeta1%"
rd /s /q "%carpeta2%"

FOR %%a IN ("%carpeta3%\*") DO ( DEL "%%a" )
FOR /d %%a IN ("%carpeta3%\*") DO ( 
	IF /i NOT "%%~nxa"=="Collections" (
		IF /i NOT "%%~nxa"=="Config" ( 
			echo Deleting: %%~nxa
			RD /S /Q "%%a" 
		) ELSE (
			FOR /d %%b IN ("%%a\*") DO (
				FOR %%c IN ("%%b\*") DO (
					IF /i NOT "%%~nxc"=="EditorPerProjectUserSettings.ini" (
						echo Deleting file %%~nxc in folder %%~nxb
						DEL "%%c"
					) 
				)
				
				FOR /d %%c IN ("%%b\*") DO (
					echo Deleting subfolder: %%~nxc in folder %%~nxb
					RD /S /Q "%%c"
				)
			)
		)
	)
)

rd /s /q "%carpeta4%"
rd /s /q "%carpeta5%"
del /F /Q "*.sln"

pause