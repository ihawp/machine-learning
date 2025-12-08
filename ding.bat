@echo off
REM Run the regression model on all data sets from 2015 -> 2020

CALL .\debug.bat

IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Bananaphone
    EXIT /B %ERRORLEVEL%
)

FOR /L %%q IN (0, 1, 100) DO (
    FOR /L %%i IN (2015, 1, 2020) DO (
        .\build\Debug\regression_model.exe world_happiness_data %%i %%q

        IF %ERRORLEVEL% NEQ 0 (
            ECHO Error: Bananaphone
            EXIT /B %ERRORLEVEL%
        )
    )
)

endlocal
pause