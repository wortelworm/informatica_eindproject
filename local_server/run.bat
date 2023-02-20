@echo off

IF EXIST node_modules/ GOTO RUN
REM install node modules
npm install


:RUN
cd ..
node ./local_server/
PAUSE >NUL
EXIT